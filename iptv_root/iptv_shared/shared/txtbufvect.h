#ifndef __TXT_BUF_VECT__
#define __TXT_BUF_VECT__

#include "BufVectorT.h"


class TxtBuf_reg : public BufVectorReg
{
public:

    TxtBuf_reg(ULONG _maxblen);
   ~TxtBuf_reg(void);

    BOOL  IsOk (void) { return (pBuf ? TRUE:FALSE); }

    void  reset(void);

    BOOL  SaveData(LPVOID _pdataBuf, ULONG _szdata);
    BOOL  GetData (LPVOID _pdataBuf, ULONG *_szdataBuf);

    ULONG GetBytesLeft(void)    { return (maxblen - blen - 1); }
    BOOL  IsEnable    (void)    { return flEnable; }
    BOOL  SetEnable   (BOOL _f) { flEnable = _f; return TRUE;  }

private:

    BOOL  flEnable;

    LPSTR pBuf;
    ULONG blen,
          maxblen;
};



class TxtBufVect : public BufVectorT<TxtBuf_reg>
{
public:

    TxtBufVect(UINT _maxlines, ULONG _maxblen);

    BOOL  SaveTxtData   (UINT _ind, LPSTR _pdataBuf, ULONG _szdata);
    BOOL  GetNextTxtLine(UINT _ind, LPSTR _pdataBuf, ULONG _szdataBuf);

    ULONG GetBytesLeft  (UINT _ind);
    BOOL  IsEnable      (UINT _ind);
    BOOL  SetEnable     (UINT _ind, BOOL _f);
};



#endif		// __TXT_BUF_VECT__

