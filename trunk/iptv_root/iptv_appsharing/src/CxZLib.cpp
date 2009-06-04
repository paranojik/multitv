/* CxZLib.cpp
** ----------
** ZLib Encode/Decode
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Tue Mar 11 23:06:50 BRT 2008
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "CxZLib.h"
///////////////////////////////////////////////////////////////////////////////
CxZLib::CxZLib( long nMode )
{
    if( nMode == CXZLIB_DECODE || nMode == CXZLIB_ENCODE )
        m_nMode = nMode;
    else
        m_nMode = CXZLIB_NULL;

    fl_bInit = false;
}

CxZLib::~CxZLib( void )
{
    End( );
}

bool CxZLib::Initialize( void )
{
    if( fl_bInit )
        return true;

    if( m_nMode == CXZLIB_ENCODE )
    {
        // zlib init - deflate
        m_compStream.total_in  = 0;
        m_compStream.total_out = 0;
        m_compStream.zalloc    = Z_NULL;
        m_compStream.zfree     = Z_NULL;
        m_compStream.opaque    = Z_NULL;
        m_compStream.data_type = Z_BINARY;

        //if( deflateInit2( &m_compStream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY ) != Z_OK )
        if( deflateInit( &m_compStream, Z_DEFAULT_COMPRESSION ) != Z_OK )
        {
            return false;
        }
        fl_bInit = true;

    }
    else if( m_nMode == CXZLIB_DECODE )
    {
        m_compStream.total_in  = 0;
        m_compStream.total_out = 0;
        m_compStream.zalloc    = Z_NULL;
        m_compStream.zfree     = Z_NULL;
        m_compStream.opaque    = Z_NULL;

        m_compStream.data_type = Z_BINARY;

        if( inflateInit( &m_compStream ) != Z_OK )
        {
            return false;
        }
        fl_bInit = true;
    }
    else
    {
        return false;
    }

    return true;
}

bool CxZLib::End( void  )
{
    if( !fl_bInit )
        return false;

    if( m_nMode == CXZLIB_ENCODE )
    {
        deflateEnd( &m_compStream );
    }
    else if( m_nMode == CXZLIB_DECODE )
    {
        inflateEnd( &m_compStream );
    }
    else
    {
        return false;
    }
    fl_bInit = false;
    return true;
}

bool CxZLib::Reset( void  )
{
    if( m_nMode == CXZLIB_ENCODE )
    {
        deflateReset( &m_compStream );
    }
    else if( m_nMode == CXZLIB_DECODE )
    {
        inflateReset( &m_compStream );
    }
    else
    {
        return false;
    }
    return true;
//    if( End( ) )
//        return Initialize( );
//    else
//        return false;
}

bool CxZLib::Encode( BYTE *pIN, unsigned long nSizeIn, BYTE *pOUT, unsigned long nSizeOut, unsigned long *nSize )
{
    unsigned long  nPrevTotalOut;

    if( !fl_bInit )
        return false;

    nPrevTotalOut          = m_compStream.total_out;
    m_compStream.avail_in  = nSizeIn;
    m_compStream.avail_out = nSizeOut;
    m_compStream.next_in   = pIN;
    m_compStream.next_out  = pOUT;
	m_compStream.data_type = Z_BINARY;

    if( deflate( &m_compStream,  Z_SYNC_FLUSH ) != Z_OK )
    {
        *nSize = 0;
        return false;
    }

    *nSize = m_compStream.total_out - nPrevTotalOut;

    return true;
}

bool CxZLib::Decode( BYTE *pIN, unsigned long nSizeIn, BYTE *pOUT, unsigned long nSizeOut, unsigned long *nSize )
{
    unsigned long  nPrevTotalOut;

    if( !fl_bInit )
        return false;

    nPrevTotalOut          = m_compStream.total_out;
    m_compStream.avail_in  = nSizeIn;
    m_compStream.avail_out = nSizeOut;
    m_compStream.next_in   = pIN;
    m_compStream.next_out  = pOUT;
    m_compStream.data_type = Z_BINARY;

    if( inflate( &m_compStream, Z_SYNC_FLUSH ) < 0 )
    {
        *nSize = 0;
        return false;
    }
    else
    {
        *nSize = m_compStream.total_out - nPrevTotalOut;
    }

    return true;
}
///////////////////////////////////////////////////////////////////////////////

