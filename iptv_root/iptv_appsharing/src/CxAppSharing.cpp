/* AsClient.cpp
** ------------
** Application Sharing Main
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Updated: Wed Dec  6 17:25:34 BRST 2006
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "CxAppSharing.h"

///////////////////////////////////////////////////////////////////////////////
CxAppSharing::CxAppSharing( int flMode )
{
    m_flMode        = flMode;
	m_pZLib         = NULL;
    m_pSockBuff     = NULL;
    m_pListener     = NULL;

#ifdef NCT_VERSION
	m_hWnd          = NULL;
	m_hMemBitmap    = NULL;
    m_hOldMemBitmap = NULL;
	m_pDebug        = NULL;
#endif /* NCT_VERSION */
}

CxAppSharing::~CxAppSharing( void )
{
}

bool CxAppSharing::End( void )
{
    //OutputDebugString( _T( "CxAppSharing::End( )" ) );
    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxAppSharing::ASEvent( int nEvent, void *pParams, unsigned int nSize )
{
    if( !m_pListener )
        return false;

    m_pListener->OnASEvent( nEvent, pParams, nSize );

    return true;
}
/*
*/
/*
///////////////////////////////////////////////////////////////////////////////
// fill the RecordData structure
// encode zlib data, calculate checksum
// send to network layer
bool CxAppSharingBase::SendData( int nX, int nY )
{
    unsigned int  nPrevTotalOut, nTotalOut;
    unsigned long nBytes=0, x, xWidth;

    if( !m_flInitCapture )
        return false;

    m_prdBits->id      = m_nMediaID;
    m_prdBits->seq     = m_nSeq++;

    m_prdBits->rd_point.x = nX;
    m_prdBits->rd_point.y = nY;

    x      = nX * AS_CHUNKSIZE_X;
    xWidth = ((((x * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);
    // temporary here - it should be at GetData(..)
    for( unsigned long kk = 0; kk < AS_CHUNKSIZE_Y; kk++ )
        memcpy( (BYTE *)m_pChunkBits + kk * m_nChunkWidthSize,
                 (BYTE *)m_pTempBits + ( kk * m_nWidth + xWidth ),
                 m_nChunkWidthSize );

    // DEBUG
    char pszFilename[256];
    RECTL rc;
    sprintf( pszFilename, "c:\\img%04d.bmp", m_nSeq );
    rc.left   = x;
    rc.top    = nY * AS_CHUNKSIZE_Y;
    rc.right  = rc.left + AS_CHUNKSIZE_X;
    rc.bottom = rc.top  + AS_CHUNKSIZE_Y;
    SaveBitmapFile( pszFilename, &rc );
    // DEBUG
    
    nPrevTotalOut        = compStream.total_out;
    compStream.avail_in  = m_nzLib_avail_in;
    compStream.avail_out = m_nzLib_avail_out;
    compStream.next_in   = (Bytef *)m_pChunkBits;
    compStream.next_out  = m_prdBits->rd_pData;
	compStream.data_type = Z_BINARY;

    deflate( &compStream, Z_SYNC_FLUSH );

    nTotalOut = compStream.total_out - nPrevTotalOut;

    m_prdBits->rd_size = sizeof( RecordData ) - sizeof( BYTE ) + nTotalOut;
    m_prdBits->datalen = (unsigned long) m_prdBits->rd_size - sizeof( LMediaPkt );

    m_prdBits->rd_CheckSum = adler32( 0, m_prdBits->rd_pData, nTotalOut );

    m_pSockBuff->Write( m_prdBits, m_prdBits->rd_size, &nBytes );

    return( nBytes > 0 );
}
// get data and copy to temporary buffer
// mark the chunk as free
bool CxAppSharingBase::GetData( int nX, int nY )
{
    long y;
    unsigned int uStartScan,   // first scan line to set
                 cScanLines;   // number of scan lines to copy


    y = nY * AS_CHUNKSIZE_Y;

    for( int k = 0; k < m_nMaxChunksX; k++ )
        m_ChunkList[nY][k] = 0;

    BitBlt( m_hDCMem,     0, y, m_SizeCapture.cx, AS_CHUNKSIZE_Y,
            m_hDCCapture, 0, y, SRCCOPY );

    SelectObject( m_hDCMem, m_hOldMemBitmap );

    // first scan line to set
    uStartScan = max( m_SizeCapture.cy - y - AS_CHUNKSIZE_Y, 0 );
    // number of scan lines to copy
    cScanLines = AS_CHUNKSIZE_Y;

    GetDIBits( m_hDCMem, m_hMemBitmap, uStartScan, cScanLines,
               m_pTempBits, (BITMAPINFO *)&m_MemBitmapInfo, DIB_RGB_COLORS );

    SelectObject( m_hDCMem, m_hMemBitmap );

    for( int k = 0; k < m_nMaxChunksX; k++ )
    {
        SendData( k, nY );
    }

    return true;
}

bool CxAppSharingBase::GetChunkList( BYTE **pChuckList, int *nSizeX, int *nSizeY )
{
    *pChuckList = (BYTE *)&m_ChunkList;

    *nSizeX = m_nMaxChunksX;
    *nSizeY = m_nMaxChunksY;

    return true;
}
*/
// should be used: a long time 'idle' (without updates), timeout notifications, application calls (requests)
///////////////////////////////////////////////////////////////////////////////
bool CxDump( RecordData *prd, unsigned long nSize )
{
#ifdef _DEBUG

    FILE *fd;
    static int i = 0;

    fd = fopen( "asdump.log", "a" );

    if( !fd )
        return false;

    fprintf( fd, "Seq: %06d - datalen: %08d - nBytes: %08d nLine: %d nChunk: %d nCount: %d\n", prd->seq, prd->datalen, nSize,
        prd->rd_nLine, prd->rd_nChunk, prd->rd_ChunkCount );

    fclose( fd );

/*
    FILE *fd2;
    fd2 = fopen( "asdump-data.log", "ab" );

    if( !fd2 )
        return false;

    fprintf( fd2, "Pacote: %04d - rd_size: %d - nBytes: %d", i++, prd->rd_size, nSize );
    fwrite( prd->rd_pData, prd->rd_size - sizeof( RecordData ) + sizeof( BYTE ), 1, fd2 );

    fclose( fd2 );
*/
#endif

    return true;
}

bool CxDump( char *pszStr )
{
#ifdef _DEBUG

    FILE *fd;
    static int i = 0;

    fd = fopen( "debug.out", "a" );

    if( !fd )
        return false;

    fprintf( fd, " %s\n", pszStr );

    fclose( fd );
#endif
    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxAppSharing::SaveBitmapFile( char *pszFilename, BYTE *pBits, RECTL *pRect )
{
    BITMAPFILEHEADER bmfh;
    CXBITMAPINFO  bminfo;
    unsigned long nSize;
    FILE *fd;

    bminfo = m_MemBitmapInfo;

    if( pRect )
    {
        bminfo.bmih.biHeight    = pRect->bottom - pRect->top;
        bminfo.bmih.biWidth     = pRect->right  - pRect->left;
        bminfo.bmih.biSizeImage = ((((bminfo.bmih.biWidth * bminfo.bmih.biBitCount) + 31) & ~31) >> 3) * bminfo.bmih.biHeight ;
    }

    nSize = (((((bminfo.bmih.biWidth * bminfo.bmih.biBitCount) + 31) & ~31) >> 3) * bminfo.bmih.biHeight );

    bmfh.bfType      = 0x4D42;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits   = sizeof( CXBITMAPINFO ) + sizeof( bmfh );
    bmfh.bfSize      = nSize + bmfh.bfOffBits;

    fd = fopen( pszFilename, "wb" );

    if( !fd )
        return false;

    fwrite( &bmfh, sizeof( bmfh ), 1, fd );
    fwrite( &bminfo, sizeof( bminfo ), 1, fd );
    fwrite( pBits, nSize, 1, fd );
    fclose( fd );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
