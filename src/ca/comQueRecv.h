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
 *  $Id$
 *
 *                              
 *                    L O S  A L A M O S
 *              Los Alamos National Laboratory
 *               Los Alamos, New Mexico 87545
 *                                  
 *  Copyright, 1986, The Regents of the University of California.
 *                                  
 *           
 *	Author Jeffrey O. Hill
 *	johill@lanl.gov
 *	505 665 1831
 */

#ifndef comQueRecvh  
#define comQueRecvh

#include "comBuf.h"

class comQueRecv {
public:
    comQueRecv ( comBufMemoryManager & ) epicsThrows (());
    ~comQueRecv () epicsThrows (());
    unsigned occupiedBytes () const epicsThrows (());
    unsigned copyOutBytes ( epicsInt8 *pBuf, unsigned nBytes ) epicsThrows (());
    unsigned removeBytes ( unsigned nBytes ) epicsThrows (());
    void pushLastComBufReceived ( comBuf & ) epicsThrows (());
    void clear () epicsThrows (());
    epicsInt8 popInt8 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsUInt8 popUInt8 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsInt16 popInt16 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsUInt16 popUInt16 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsInt32 popInt32 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsUInt32 popUInt32 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsFloat32 popFloat32 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsFloat64 popFloat64 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    void popString ( epicsOldString * ) epicsThrows (( comBuf::insufficentBytesAvailable ));
private:
    tsDLList < comBuf > bufs;
    comBufMemoryManager & comBufMemMgr;
    unsigned nBytesPending;
    epicsUInt16 multiBufferPopUInt16 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    epicsUInt32 multiBufferPopUInt32 () epicsThrows (( comBuf::insufficentBytesAvailable ));
    void removeAndDestroyBuf ( comBuf & ) epicsThrows (());
	comQueRecv ( const comQueRecv & ) epicsThrows (());
	comQueRecv & operator = ( const comQueRecv & ) epicsThrows (());
};

inline unsigned comQueRecv::occupiedBytes () const
    epicsThrows (())
{
    return this->nBytesPending;
}

inline epicsInt8 comQueRecv::popInt8 ()
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    return static_cast < epicsInt8 > ( this->popUInt8() );
}

inline epicsInt16 comQueRecv::popInt16 ()
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    return static_cast < epicsInt16 > ( ( this->popInt8() << 8u )
                                       | ( this->popInt8() << 0u ) );
}

inline epicsInt32 comQueRecv::popInt32 ()
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    epicsInt32 tmp ;
    tmp  = this->popInt8() << 24u;
    tmp |= this->popInt8() << 16u;
    tmp |= this->popInt8() << 8u;
    tmp |= this->popInt8() << 0u;
    return tmp;
}

inline epicsFloat32 comQueRecv::popFloat32 ()
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    epicsFloat32 tmp;
    epicsUInt8 wire[ sizeof ( tmp ) ];
    // optimizer will unroll this loop
    for ( unsigned i = 0u; i < sizeof ( tmp ); i++ ) {
        wire[i] = this->popUInt8 ();
    }
    osiConvertFromWireFormat ( tmp, wire );
    return tmp;
}

inline epicsFloat64 comQueRecv::popFloat64 ()
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    epicsFloat64 tmp;
    epicsUInt8 wire[ sizeof ( tmp ) ];
    // optimizer will unroll this loop
    for ( unsigned i = 0u; i < sizeof ( tmp ); i++ ) {
        wire[i] = this->popUInt8 ();
    }
    osiConvertFromWireFormat ( tmp, wire );
    return tmp;
}

inline epicsUInt8 comQueRecv::popUInt8 () 
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    comBuf * pComBuf = this->bufs.first ();
    if ( ! pComBuf ) {
        comBuf::throwInsufficentBytesException ();
    }
    epicsUInt8 tmp = pComBuf->popUInt8 ();
    if ( pComBuf->occupiedBytes() == 0u ) {
        this->removeAndDestroyBuf ( *pComBuf );
    }
    this->nBytesPending--;
    return tmp;
}

// optimization here complicates this function somewhat
inline epicsUInt16 comQueRecv::popUInt16 ()
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    comBuf *pComBuf = this->bufs.first ();
    if ( ! pComBuf ) {
        comBuf::throwInsufficentBytesException ();
    }
    // try first for all in one buffer efficent version
    // (double check here avoids slow C++ exception)
    // (hopefully optimizer removes inside check)
    epicsUInt16 tmp;
    unsigned bytesAvailable = pComBuf->occupiedBytes();
    if ( bytesAvailable >= sizeof (tmp) ) {
        tmp = pComBuf->popUInt16 ();
        if ( bytesAvailable == sizeof (tmp) ) {
            this->removeAndDestroyBuf ( *pComBuf );
        }
        this->nBytesPending -= sizeof( tmp );
    }
    else {
        tmp = this->multiBufferPopUInt16 ();
    }
    return tmp;
}

// optimization here complicates this function somewhat
inline epicsUInt32 comQueRecv::popUInt32 ()
    epicsThrows (( comBuf::insufficentBytesAvailable ))
{
    comBuf *pComBuf = this->bufs.first ();
    if ( ! pComBuf ) {
        comBuf::throwInsufficentBytesException ();
    }
    epicsUInt32 tmp;
    // try first for all in one buffer efficent version
    // (double check here avoids slow C++ exception)
    // (hopefully optimizer removes inside check)
    unsigned bytesAvailable = pComBuf->occupiedBytes();
    if ( pComBuf->occupiedBytes() >= sizeof (tmp) ) {
        tmp = pComBuf->popUInt32 ();
        if ( bytesAvailable == sizeof (tmp) ) {
            this->removeAndDestroyBuf ( *pComBuf );
        }
        this->nBytesPending -= sizeof ( tmp );
    }
    else {
        tmp = this->multiBufferPopUInt32 ();
    }
    return tmp;
}

#endif // ifndef comQueRecvh
