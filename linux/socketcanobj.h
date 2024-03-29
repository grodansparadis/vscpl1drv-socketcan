///////////////////////////////////////////////////////////////////////////////
// CAN232Obj.h: interface for the CSocketcanObj class.
//
// This file is part is part of CANAL (CAN Abstraction Layer)
// http://www.vscp.org)
//
// Copyright (C) 2000-2022 Ake Hedman,
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

#if !defined(SOCKETCANDRV_H__16828641_5EDF_4115_9522_97BD178F566B__INCLUDED_)
#define SOCKETCANDRV_H__16828641_5EDF_4115_9522_97BD178F566B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _POSIX
#include "limits.h"
#include "stdlib.h"
#include "syslog.h"
#include "unistd.h"
#include <net/if.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

// Different on Kernel 2.6 and cansocket examples
// currently using locally from can-utils
// TODO remove include form makefile when they are in sync
#include <linux/can.h>
#include <linux/can/raw.h>

#include <canal.h>
#include <canal_macro.h>
#include <vscp.h>
//#include <com.h>
#include <dllist.h>

#define SOCKETCAN_BUF_SIZE      80 // Size for one message buffer

#define SOCKETCAN_MAX_RCVMSG    512 // Maximum number of received messages
#define SOCKETCAN_MAX_SNDMSG    512 // Maximum number of received messages

#define SOCKETCAN_RX_MUTEX      "___SOCKETCAN_LEVEL1_RX_MUTEX___"
#define SOCKETCAN_TX_MUTEX      "___SOCKETCAN_LEVEL1_TX_MUTEX___"

// Flags
#define SOCKETCAN_FLAG_DEBUG    0x80000000

////////////////////////////////////////////////////////////////////////////////////////
// _socketcanobj
//

struct _socketcanobj {
    /*!
        Work flag for thread
     */
    bool m_bRun;

    /*!
     *  Device name
     */
    char m_devname[IFNAMSIZ + 1];

    /*!
     *  Socket to socketcan
     */
    int m_sock;

    /*!
        Send/Receive Statistics
     */
    canalStatistics m_stat;

    DoubleLinkedList m_rcvList; // List for incoming messages
    DoubleLinkedList m_sndList; // List for outgoing messages
};

class CSocketcanObj {
  public:
    // Constructor
    CSocketcanObj();

    // Destructor
    virtual ~CSocketcanObj();

    /*!
        Open the device
        Return True on success
     */
    int open(const char* pDevice, unsigned long flags);

    /*!
        Close the device
     */
    int close(void);

    /*!
        Send frame
     */
    int writeMsg(bool bExtended,
                 unsigned long id,
                 unsigned char dlc,
                 unsigned char* pdata);

    /*!
        Send frame
     */
    int writeMsg(PCANALMSG pCanalMsg);

    /*!
        Send frame blcoking version
     */
    int writeMsgBlocking(PCANALMSG pCanalMsg, uint32_t timeout);

    /*!
        Receive frame
     */
    int readMsg(canalMsg* pMsg);

    /*!
        Read frame blocking version
     */
    int readMsgBlocking(PCANALMSG pCanalMsg, uint32_t timeout);

    /*!
        Set filter(code) and mask
     */
    int setFilter(unsigned long filter, unsigned long mask);

    /*!
        Set filter (acceptance code)
     */
    int setFilter(unsigned long filter);

    /*!
        Set mask
     */
    int setMask(unsigned long mask);

    /*!
        Get statistics
     */
    int getStatistics(PCANALSTATISTICS& pCanalStatistics);

    /*
        Get number of frames availabel in the input queue
     */
    int dataAvailable(void);

    /*!
        Get the status code
     */
    int getStatus(PCANALSTATUS pCanalStatus);

    // Debug flag
    bool m_bDebug;

    // Flag for open connection
    bool m_bOpen;

    struct _socketcanobj m_socketcanobj;

    /*!
        The socketcan read/write mutexes
    */
    pthread_mutex_t m_socketcanRcvMutex;
    pthread_mutex_t m_socketcanSndMutex;

    sem_t m_receiveDataSem;
    sem_t m_transmitDataPutSem;
    sem_t m_transmitDataGetSem;

    /*!
        id for worker thread
     */
    pthread_t m_threadId;
};

#endif // !defined(SOCKETCANDRV_H__16828641_5EDF_4115_9522_97BD178F566B__INCLUDED_)
