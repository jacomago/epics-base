/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/

/*  
 * $Id$
 *
 *                    L O S  A L A M O S
 *              Los Alamos National Laboratory
 *               Los Alamos, New Mexico 87545
 *
 *  Copyright, 1986, The Regents of the University of California.
 *
 *  Author: Jeff Hill
 */

#ifndef bheh
#define bheh

#ifdef epicsExportSharedSymbols
#   define bhehEpicsExportSharedSymbols
#   undef epicsExportSharedSymbols
#endif

#include "tsDLList.h"
#include "tsFreeList.h"
#include "epicsTime.h"
#include "cxxCompilerDependencies.h"

#ifdef bhehEpicsExportSharedSymbols
#   define epicsExportSharedSymbols
#   include "shareLib.h"
#endif

#include "inetAddrID.h"
#include "caProto.h"

class tcpiiu;
class bheMemoryManager;

// using a pure abstract wrapper class around the free list avoids
// Tornado 2.0.1 GNU compiler bugs
class bheMemoryManager {
public:
    epicsShareFunc virtual ~bheMemoryManager ();
    virtual void * allocate ( size_t ) = 0;
    virtual void release ( void * ) = 0;
};

class bhe : public tsSLNode < bhe >, public inetAddrID {
public:
    epicsShareFunc bhe ( const epicsTime & initialTimeStamp, 
        unsigned initialBeaconNumber, const inetAddrID & addr ) epicsThrows (());
    epicsShareFunc ~bhe (); 
    epicsShareFunc bool updatePeriod ( 
        const epicsTime & programBeginTime, 
        const epicsTime & currentTime, ca_uint32_t beaconNumber, 
        unsigned protocolRevision );
    epicsShareFunc double period () const epicsThrows (());
    epicsShareFunc epicsTime updateTime () const;
    epicsShareFunc void show ( unsigned level) const;
    epicsShareFunc void registerIIU ( tcpiiu & );
    epicsShareFunc void unregisterIIU ( tcpiiu & );
    epicsShareFunc void * operator new ( size_t size, bheMemoryManager & );
#ifdef CXX_PLACEMENT_DELETE
    epicsShareFunc void operator delete ( void *, bheMemoryManager & ) epicsThrows (());
#endif
private:
    tsDLList < tcpiiu > iiuList;
    epicsTime timeStamp;
    double averagePeriod;
    ca_uint32_t lastBeaconNumber;
    void beaconAnomalyNotify ();
	bhe ( const bhe & );
	bhe & operator = ( const bhe & );
    void * operator new ( size_t size );
    epicsShareFunc void operator delete ( void * );
};

// using a wrapper class around the free list avoids
// Tornado 2.0.1 GNU compiler bugs
class bheFreeStore : public bheMemoryManager {
public:
    void * allocate ( size_t );
    void release ( void * );
private:
    tsFreeList < class bhe, 0x100 > freeList;
};

inline void * bhe::operator new ( size_t size, 
        bheMemoryManager & mgr )
{ 
    return mgr.allocate ( size );
}

#ifdef CXX_PLACEMENT_DELETE
inline void bhe::operator delete ( void * pCadaver, 
        bheMemoryManager & mgr )
{ 
    mgr.release ( pCadaver );
}
#endif

#endif // ifdef bheh


