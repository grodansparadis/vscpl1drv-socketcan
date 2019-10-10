// can232drv.h : main header file for the can232drv.dll
// Linux version
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
// 
// This file is part of the VSCP (http://www.vscp.org) 
//
// Copyright (C) 2000-2019 Ake Hedman,
// Ake Hedman, Grodans Paradis AB, <akhe@grodansparadis.com>
// 
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this file see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//

#if !defined(LOGGERDLL_H__A388C093_AD35_4672_8BF7_DBC702C6B0C8__INCLUDED_)
#define LOGGERDLL_H__A388C093_AD35_4672_8BF7_DBC702C6B0C8__INCLUDED_

#include "socketcanobj.h"

// This is the version info for this DLL - Change to your own value
#define DLL_VERSION		1

// This is the vendor string - Change to your own value
#define CANAL_DLL_VENDOR "Grodans Paradis AB, Sweden, https://www.grodansparadis.com"

// Max number of open connections
#define CANAL_SOCKETCAN_DRIVER_MAX_OPEN	256

/////////////////////////////////////////////////////////////////////////////
// CSocketcanApp
// 

class CSocketcanApp {
public:

    /// Constructor
    CSocketcanApp();

    /// Destructor
    ~CSocketcanApp();

    /*!
            Add a driver object

            @parm plog Object to add
            @return handle or 0 for error
     */
    long addDriverObject(CSocketcanObj *plog);

    /*!
        Get a driver object from its handle

        @param handle for object
        @return pointer to object or NULL if invalid
                handle.
     */
    CSocketcanObj *getDriverObject(long h);

    /*!
        Remove a driver object

        @parm handle for object.
     */
    void removeDriverObject(long h);

    /*!
        The log file object
        This is the array with driver objects (max 256 objects
     */
    CSocketcanObj *m_socketcanArray[ CANAL_SOCKETCAN_DRIVER_MAX_OPEN ];

    /// Mutex for driver
    pthread_mutex_t m_drvobjMutex;

    /// Counter for users of the interface
    unsigned long m_instanceCounter;

public:
    BOOL InitInstance();

};

///////////////////////////////////////////////////////////////////////////////
// CreateObject
//

extern "C" {
    CSocketcanApp *CreateObject(void);
}

#endif // !defined(LOGGERDLL_H__A388C093_AD35_4672_8BF7_DBC702C6B0C8__INCLUDED_)
