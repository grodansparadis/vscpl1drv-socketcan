///////////////////////////////////////////////////////////////////////////////
// socketcanobj.cpp: implementation of the CSocketcanObj class.
//
// This file is part is part of CANAL (CAN Abstraction Layer)
// http://www.vscp.org)
//
// Copyright (C) 2000-2021 Ake Hedman,
// Ake Hedman, Grodans Paradis AB, <akhe@grodansparadis.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <errno.h>
#include <limits.h>
#include <net/if.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

// Different on Kernel 2.6 and socketcan examples
// currently using locally from can-utils
// TODO remove include form makefile when they are in sync
#include <linux/can.h>
#include <linux/can/raw.h>

#include "socketcanobj.h"
#include <canal_macro.h>
#include <ctype.h>
#include <libgen.h>
#include <net/if.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>

// Prototypes
void*
workThread(void* p);
bool
socketcanToCanal(char* p, PCANALMSG pMsg);

////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////////////////

CSocketcanObj::CSocketcanObj()
{
    m_bDebug     = false;
    m_bOpen      = false;

    pthread_mutex_init(&m_socketcanRcvMutex, NULL);
    pthread_mutex_init(&m_socketcanSndMutex, NULL);

    sem_init(&m_receiveDataSem, 0, 0);
    sem_init(&m_transmitDataPutSem, 0, 0);
    sem_init(&m_transmitDataGetSem, 0, 0);

    strcpy(m_socketcanobj.m_devname, "vcan0");
    dll_init(&m_socketcanobj.m_rcvList, SORT_NONE);
    dll_init(&m_socketcanobj.m_sndList, SORT_NONE);
}

CSocketcanObj::~CSocketcanObj()
{
    close(); // Close comm channel in case its open

    dll_removeAllNodes(&m_socketcanobj.m_rcvList);
    dll_removeAllNodes(&m_socketcanobj.m_sndList);

    sem_destroy(&m_receiveDataSem);
    sem_destroy(&m_transmitDataPutSem);
    sem_destroy(&m_transmitDataGetSem);

    pthread_mutex_destroy(&m_socketcanRcvMutex);
    pthread_mutex_destroy(&m_socketcanSndMutex);
}

//------------------------------------------------------------------------------
// Open Socketcan
//
// Parameters are as follows
//
// interface;mask;filter
//
//------------------------------------------------------------------------------

int
CSocketcanObj::open(const char* pDevice, unsigned long flags)
{
    int rv = CANAL_ERROR_SUCCESS;

    if (flags & SOCKETCAN_FLAG_DEBUG) {
        m_bDebug = true;
        fprintf(stderr, "CSocketcanObj: Debugging is enabled.\n");
        syslog(LOG_DEBUG, "CSocketcanObj: Debugging is enabled.");
    }

    // Clear device name
    memset(m_socketcanobj.m_devname, 0, sizeof(m_socketcanobj.m_devname));

    //----------------------------------------------------------------------
    //	Set default parameters
    //----------------------------------------------------------------------
    strcpy(m_socketcanobj.m_devname, "vcan0");
    unsigned long nMask   = 0;
    unsigned long nFilter = 0;

    m_socketcanobj.m_bRun = true;

    //----------------------------------------------------------------------
    //	Parse given parameters
    //----------------------------------------------------------------------
    // Interface
    char* p = strtok((char*)pDevice, ";");
    if (NULL != p) {
        memset(m_socketcanobj.m_devname, 0, sizeof(m_socketcanobj.m_devname));
        strncpy(m_socketcanobj.m_devname, p, sizeof(m_socketcanobj.m_devname));
        if (m_bDebug) {
            syslog(LOG_DEBUG, "CSocketcanObj: Devicename = %s", p);
        }
    }

    // Mask
    p = strtok(NULL, ";");
    if (NULL != p) {
        if ((NULL != strstr(p, "0x")) || (NULL != strstr(p, "0X"))) {
            sscanf(p + 2, "%lx", &nMask);
        }
        else {
            nMask = atol(p);
        }
    }

    if (m_bDebug) {
        syslog(LOG_DEBUG, "CSocketcanObj: mask = %lX", nMask);
    }

    // Filter
    p = strtok(NULL, ";");
    if (NULL != p) {
        if ((NULL != strstr(p, "0x")) || (NULL != strstr(p, "0X"))) {
            sscanf(p + 2, "%lx", &nFilter);
        }
        else {
            nFilter = atol(p);
        }
    }

    if (m_bDebug) {
        syslog(LOG_DEBUG, "CSocketcanObj: mask = %lX", nFilter);
    }

    //----------------------------------------------------------------------
    //
    //----------------------------------------------------------------------

    // Initiate statistics
    m_socketcanobj.m_stat.cntReceiveData    = 0;
    m_socketcanobj.m_stat.cntReceiveFrames  = 0;
    m_socketcanobj.m_stat.cntTransmitData   = 0;
    m_socketcanobj.m_stat.cntTransmitFrames = 0;

    m_socketcanobj.m_stat.cntBusOff      = 0;
    m_socketcanobj.m_stat.cntBusWarnings = 0;
    m_socketcanobj.m_stat.cntOverruns    = 0;

    //----------------------------------------------------------------------
    // Start thread
    //----------------------------------------------------------------------

    if (pthread_create(&m_threadId, NULL, workThread, this)) {
        rv = CANAL_ERROR_INIT_FAIL;
        close();
    }

    //----------------------------------------------------------------------
    // Release the mutex for other threads to use
    //----------------------------------------------------------------------
    pthread_mutex_unlock(&m_socketcanRcvMutex);
    pthread_mutex_unlock(&m_socketcanSndMutex);

    m_bOpen = true;

    return rv;
}

//------------------------------------------------------------------------------
//  close
//------------------------------------------------------------------------------

int
CSocketcanObj::close(void)
{
    int rv = CANAL_ERROR_SUCCESS;

    // Do nothing if already terminated
    if (!m_socketcanobj.m_bRun)
        return CANAL_ERROR_SUCCESS;

    // Terminate the thread
    m_socketcanobj.m_bRun = false;

    // Give the worker thread some time to terminate
    int* trv;
    pthread_join(m_threadId, (void**)&trv);

    ::close(m_socketcanobj.m_sock);

    m_bOpen = false;

    return rv;
}

//------------------------------------------------------------------------------
//  writeMsg
//------------------------------------------------------------------------------

int
CSocketcanObj::writeMsg(PCANALMSG pCanalMsg)
{
    int rv = CANAL_ERROR_SUCCESS;

    if (m_socketcanobj.m_sndList.nCount > SOCKETCAN_MAX_SNDMSG) {
        syslog(LOG_DEBUG,
               "CSocketcanObj: [writeMsgBlocking] FIFO is full [%ld]",
               m_socketcanobj.m_sndList.nCount);
        return CANAL_ERROR_FIFO_FULL;
    }

    if (NULL != pCanalMsg) {
        dllnode* pNode = new dllnode;
        if (NULL != pNode) {
            canalMsg* pnewMsg = new canalMsg;
            pNode->pObject    = pnewMsg;
            pNode->pKey       = NULL;
            pNode->pstrKey    = NULL;
            if (NULL != pnewMsg) {
                memcpy(pnewMsg, pCanalMsg, sizeof(canalMsg));
            }

            LOCK_MUTEX(m_socketcanSndMutex);
            dll_addNode(&m_socketcanobj.m_sndList, pNode);
            sem_post(&m_transmitDataGetSem);
            UNLOCK_MUTEX(m_socketcanSndMutex);

            rv = CANAL_ERROR_SUCCESS;
        }
        else {
            rv = CANAL_ERROR_MEMORY;
        }
    }

    return rv;
}

//------------------------------------------------------------------------------
//  writeMsg
//------------------------------------------------------------------------------

int
CSocketcanObj::writeMsgBlocking(PCANALMSG pMsg, uint32_t timeout)
{
    // int res;
    struct timespec to = { 0, 0 };
    clock_gettime(CLOCK_REALTIME, &to);
    to.tv_sec += timeout / 1000;

    // Must be a message pointer
    if (NULL == pMsg) {
        syslog(LOG_DEBUG,
               "CSocketcanObj: [writeMsgBlocking] Invalid message object");
        return CANAL_ERROR_PARAMETER;
    }

    // Must be open
    if (!m_bOpen) {
        syslog(LOG_DEBUG, "CSocketcanObj: [writeMsgBlocking] Not open");
        return CANAL_ERROR_NOT_OPEN;
    }

    if (m_socketcanobj.m_sndList.nCount > SOCKETCAN_MAX_SNDMSG) {
        syslog(LOG_DEBUG,
               "CSocketcanObj: [writeMsgBlocking] FIFO is full [%ld]",
               m_socketcanobj.m_sndList.nCount);
        return CANAL_ERROR_FIFO_FULL;
    }

    // if (-1 == (res = sem_timedwait(&m_transmitDataPutSem, &to))) {
    //     if (errno == ETIMEDOUT) {
    //         if (m_bDebug) {
    //             // Will give to much logging if used in non debug mode
    //             syslog(LOG_DEBUG, "CSocketcanObj: [writeMsgBlocking]
    //             Timeout");
    //         }
    //         return CANAL_ERROR_TIMEOUT;
    //     }
    //     else if (errno == EINTR) {
    //         syslog(LOG_DEBUG, "CSocketcanObj: [writeMsgBlocking]
    //         Interupted"); return CANAL_ERROR_INTERUPTED;
    //     }
    //     else if (errno == EINVAL) {
    //         syslog(LOG_DEBUG, "CSocketcanObj: [writeMsgBlocking] Not a valid
    //         semaphore"); return CANAL_ERROR_PARAMETER;
    //     }
    //     else {
    //         syslog(LOG_DEBUG,
    //                "CSocketcanObj: [writeMsgBlocking] General error on wait "
    //                "fro package");
    //         return CANAL_ERROR_GENERIC;
    //     }
    // }

    dllnode* pNode = new dllnode;
    if (NULL == pNode) {
        syslog(LOG_DEBUG,
               "CSocketcanObj: [writeMsgBlocking] Node allocation error");
        return CANAL_ERROR_MEMORY;
    }

    canalMsg* pCanalMsg = new canalMsg;
    if (NULL == pCanalMsg) {
        delete pNode;
        syslog(LOG_DEBUG,
               "CSocketcanObj: [writeMsgBlocking] Message allocation error");
        return CANAL_ERROR_MEMORY;
    }

    pNode->pObject = pCanalMsg;
    pNode->pKey    = NULL;
    pNode->pstrKey = NULL;

    memcpy(pCanalMsg, pMsg, sizeof(canalMsg));

    LOCK_MUTEX(m_socketcanSndMutex);
    dll_addNode(&m_socketcanobj.m_sndList, pNode);
    sem_post(&m_transmitDataGetSem);
    UNLOCK_MUTEX(m_socketcanSndMutex);

    return CANAL_ERROR_SUCCESS;
}

//------------------------------------------------------------------------------
//	dataAvailable
//------------------------------------------------------------------------------

int
CSocketcanObj::dataAvailable(void)
{
    int cnt;

    LOCK_MUTEX(m_socketcanRcvMutex);
    cnt = dll_getNodeCount(&m_socketcanobj.m_rcvList);
    UNLOCK_MUTEX(m_socketcanRcvMutex);

    return cnt;
}

//------------------------------------------------------------------------------
//  readMsg
//------------------------------------------------------------------------------

int
CSocketcanObj::readMsg(canalMsg* pMsg)
{
    int rv = CANAL_ERROR_SUCCESS;

    if (m_socketcanobj.m_rcvList.nCount > 0) {
        LOCK_MUTEX(m_socketcanRcvMutex);
        memcpy(pMsg, m_socketcanobj.m_rcvList.pHead->pObject, sizeof(canalMsg));
        dll_removeNode(&m_socketcanobj.m_rcvList,
                       m_socketcanobj.m_rcvList.pHead);
        UNLOCK_MUTEX(m_socketcanRcvMutex);
        rv = CANAL_ERROR_SUCCESS;

    }
    else {
        rv = CANAL_ERROR_FIFO_EMPTY;
    }

    return rv;
}

//------------------------------------------------------------------------------
//  readMsgBlocking
//------------------------------------------------------------------------------

int
CSocketcanObj::readMsgBlocking(canalMsg* pMsg, uint32_t timeout)
{
    // static int count = 0;
    int rv = CANAL_ERROR_TIMEOUT;
    int res;
    struct timespec ts = { 0, 0 };
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_sec += timeout / 1000;
    uint32_t remain = timeout % 1000;
    ts.tv_nsec += remain * 1000000;
    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec %= 1000000000;

    // Must be a message pointer
    if (NULL == pMsg) {
        return CANAL_ERROR_PARAMETER;
    }

    // Must be open
    if (!m_bOpen) {
        return CANAL_ERROR_NOT_OPEN;
    }

    // Yes we block if in queue is empty
    if (0 == m_socketcanobj.m_rcvList.nCount) {
        if (timeout) {
            while ((res = sem_timedwait(&m_receiveDataSem, &ts)) == -1 &&
                   errno == EINTR) {
                continue; /* Restart if interrupted by handler */
            }
        }
        else {
            while ((res = sem_wait(&m_receiveDataSem)) == -1 &&
                   errno == EINTR) {
                continue; /* Restart if interrupted by handler */
            }
        }

        if (res == EAGAIN) {
            fprintf(stderr, "readMsgBlocking: CANAL_ERROR_TIMEOUT\n");
            return CANAL_ERROR_TIMEOUT;
        }
    }

    if (m_socketcanobj.m_rcvList.nCount > 0) {
        LOCK_MUTEX(m_socketcanRcvMutex);
        memcpy(pMsg, m_socketcanobj.m_rcvList.pHead->pObject, sizeof(canalMsg));
        dll_removeNode(&m_socketcanobj.m_rcvList,
                       m_socketcanobj.m_rcvList.pHead);
        rv = CANAL_ERROR_SUCCESS;
        UNLOCK_MUTEX(m_socketcanRcvMutex);

    }
    else {
        rv = CANAL_ERROR_FIFO_EMPTY;
    }

    return rv;
}

//------------------------------------------------------------------------------
//	setFilter
//------------------------------------------------------------------------------

int
CSocketcanObj::setFilter(unsigned long filter, unsigned long mask)
{
    return CANAL_ERROR_SUCCESS;
}

//------------------------------------------------------------------------------
//	setFilter
//------------------------------------------------------------------------------

int
CSocketcanObj::setFilter(unsigned long filter)
{
    return CANAL_ERROR_SUCCESS;
}

//------------------------------------------------------------------------------
//	setMask
//------------------------------------------------------------------------------

int
CSocketcanObj::setMask(unsigned long mask)
{
    return CANAL_ERROR_SUCCESS;
}

//------------------------------------------------------------------------------
//	getStatistics
//------------------------------------------------------------------------------

int
CSocketcanObj::getStatistics(PCANALSTATISTICS& pCanalStatistics)
{
    pCanalStatistics = &m_socketcanobj.m_stat;
    return CANAL_ERROR_SUCCESS;
}

//------------------------------------------------------------------------------
//	getStatus
//------------------------------------------------------------------------------

int
CSocketcanObj::getStatus(PCANALSTATUS pCanalStatus)
{
    memset(pCanalStatus, 0, sizeof(canalStatus));
    pCanalStatus->channel_status   = 0;
    pCanalStatus->lasterrorcode    = 0;
    pCanalStatus->lasterrorsubcode = 0;
    strcpy(pCanalStatus->lasterrorstr, "All is OK!");
    return CANAL_ERROR_SUCCESS;
}

//------------------------------------------------------------------------------
// workThread
//
// The workThread do most of the actual work such as send and receive.
//------------------------------------------------------------------------------

void*
workThread(void* pObject)
{
    int rv = 0;
    int sock;
    fd_set rdfs;
    struct timeval tv;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    CSocketcanObj* psocketcanobj = (CSocketcanObj*)pObject;
    if (NULL == psocketcanobj) {
        pthread_exit(&rv);
    }

    // Create socket
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        syslog(LOG_ERR,
               "%s",
               (const char*)"CReadSocketCanTread: Error while opening socket. "
                            "Terminating!");
        return NULL;
    }

    strcpy(ifr.ifr_name, psocketcanobj->m_socketcanobj.m_devname);
    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

#ifdef DEBUG
    printf("using interface name '%s'.\n", ifr.ifr_name);
#endif

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        syslog(LOG_ERR,
               "CReadSocketCanTread: Error in socket bind. errno=%d "
               "Terminating!",
               errno);
        close(sock);
        return NULL;
    }

    while (psocketcanobj->m_socketcanobj.m_bRun) {

        ///////////////////////////////////////////////////////////////////////
        //                        Receive
        ///////////////////////////////////////////////////////////////////////

        // LOCK_MUTEX(psocketcanobj->m_socketcanObjMutex);

        // Noting to do if we should end...
        if (!psocketcanobj->m_socketcanobj.m_bRun) {
            continue;
        }

        FD_ZERO(&rdfs);
        FD_SET(sock, &rdfs);

        tv.tv_sec  = 0;
        tv.tv_usec = 5000; // 5ms timeout

        int ret;
        if ((ret = select(sock + 1, &rdfs, NULL, NULL, &tv)) < 0) {
            // Error
            syslog(LOG_ERR, "Receive 'select' error (%d)\n", errno);
            psocketcanobj->m_socketcanobj.m_bRun = false;
            continue;
        }

        if (ret) {

            // There is data to read
            ret = read(sock, &frame, sizeof(struct can_frame));
            if (ret < 0) {
                psocketcanobj->m_socketcanobj.m_bRun = false;
                continue;
            }

            // fprintf(stderr,"Receive thread: Event received %d (%zd) %x\n",
            // ret, sizeof(struct can_frame), frame.can_id);

            if (psocketcanobj->m_socketcanobj.m_rcvList.nCount <
                SOCKETCAN_MAX_RCVMSG) {

                PCANALMSG pMsg = new canalMsg;

                if (NULL != pMsg) {

                    dllnode* pNode = new dllnode;
                    if (NULL != pNode) {

                        pNode->pObject = pMsg;

                        // Assign the message
                        pMsg->flags = 0;
                        if (frame.can_id & CAN_RTR_FLAG)
                            pMsg->flags |= CANAL_IDFLAG_RTR;

                        if (frame.can_id & CAN_EFF_FLAG) {
                            pMsg->flags |= CANAL_IDFLAG_EXTENDED;
                        }

                        // Mask of control bits
                        frame.can_id &= CAN_EFF_MASK;
                        pMsg->id = frame.can_id;

                        pMsg->sizeData = frame.can_dlc;
                        memcpy(pMsg->data, frame.data, frame.can_dlc);

                        LOCK_MUTEX(psocketcanobj->m_socketcanRcvMutex);
                        dll_addNode(&psocketcanobj->m_socketcanobj.m_rcvList,
                                    pNode);
                        sem_post(
                          &psocketcanobj
                             ->m_receiveDataSem); // Signal frame in queue
                        UNLOCK_MUTEX(psocketcanobj->m_socketcanRcvMutex);

                        // Update statistics
                        psocketcanobj->m_socketcanobj.m_stat.cntReceiveData +=
                          pMsg->sizeData;
                        psocketcanobj->m_socketcanobj.m_stat.cntReceiveFrames +=
                          1;
                    }
                    else {
                        delete pMsg;
                    }
                }
            } // room in queue
        }

        // UNLOCK_MUTEX(psocketcanobj->m_socketcanObjMutex);

        ///////////////////////////////////////////////////////////////////////
        //                          Transmit
        ///////////////////////////////////////////////////////////////////////

        // If there is noting in the queue we wait until there is
        // something there
        // if ( 0 == pobj->m_transmitList.nCount ) {
        // clock_gettime( CLOCK_REALTIME, &to );
        // to.tv_nsec += 100000;
        // res = sem_timedwait( &pobj->m_transmitDataGetSem, &to );
        // if ( res == EAGAIN ) {
        //     continue;
        // }

        if ((NULL != psocketcanobj->m_socketcanobj.m_sndList.pHead) &&
            (NULL != psocketcanobj->m_socketcanobj.m_sndList.pHead->pObject)) {

            canalMsg msg;

            memcpy(&msg,
                   psocketcanobj->m_socketcanobj.m_sndList.pHead->pObject,
                   sizeof(canalMsg));

            LOCK_MUTEX(psocketcanobj->m_socketcanSndMutex);
            dll_removeNode(&psocketcanobj->m_socketcanobj.m_sndList,
                           psocketcanobj->m_socketcanobj.m_sndList.pHead);
            UNLOCK_MUTEX(psocketcanobj->m_socketcanSndMutex);

            memset(&frame, 0, sizeof(struct can_frame));
            frame.can_id = msg.id;
            if (msg.flags & CANAL_IDFLAG_EXTENDED) {
                frame.can_id |= CAN_EFF_FLAG;
            }
            frame.can_dlc = msg.sizeData;

            memcpy(frame.data, msg.data, msg.sizeData);

            // Write the data
            write(sock, &frame, sizeof(struct can_frame));

            // Update statistics
            psocketcanobj->m_socketcanobj.m_stat.cntTransmitData +=
              msg.sizeData;
            psocketcanobj->m_socketcanobj.m_stat.cntTransmitFrames += 1;

        } // if there is something to transmit

    } // while( psocketcanobj->m_socketcanobj.m_bRun )

    pthread_exit(&rv);
}
