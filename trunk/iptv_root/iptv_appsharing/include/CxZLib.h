/* CxZLib.h
** --------
** ZLib Encode/Decode
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Tue Mar 11 23:06:50 BRT 2008
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "zlib/include/zlib.h"
///////////////////////////////////////////////////////////////////////////////
#ifndef CXZLIB__
#define CXZLIB__

enum CxZLib_Modes
{
    CXZLIB_NULL,
    CXZLIB_DECODE,
    CXZLIB_ENCODE
};
///////////////////////////////////////////////////////////////////////////////
class CxZLib
{
    private:
        struct z_stream_s  m_compStream;
        long   m_nMode;
        bool   fl_bInit;

    public:
        CxZLib( long nMode );
        virtual ~CxZLib( void );

        bool Initialize( void );
        bool End( void  );

        bool Reset( void  );

        bool Encode( BYTE *pIN, unsigned long nSizeIn, BYTE *pOUT, unsigned long nSizeOut, unsigned long *nSize );
        bool Decode( BYTE *pIN, unsigned long nSizeIn, BYTE *pOUT, unsigned long nSizeOut, unsigned long *nSize );
};
#endif /* CXZLIB__ */
///////////////////////////////////////////////////////////////////////////////
