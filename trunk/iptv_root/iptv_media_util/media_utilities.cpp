#include "compat.h"
#include "global_error.h"
#include <stdio.h>
#include <deque>
#include "media_utilities.h"

void PrintDbgMsg(const std::string & _functionName, const std::string & _msg, const std::string & _dbgStr, int errorCode) 
{
    fprintf (stderr, "Error In function: %s - msg: %s - dbgStr: %s - code: %d\n",_functionName.c_str(), _msg.c_str(), _dbgStr.c_str(), errorCode);
}

ULONG TopDownToBottomUp(BYTE *_pData, ULONG _ulDatalen, ULONG _ulHeight)
{
	if (!_pData || !_ulDatalen || !_ulHeight)
		return RET_INVALID_ARG;

	ULONG ulLineSize = _ulDatalen/_ulHeight;
	
	BYTE *pAuxLine = new BYTE[ulLineSize];

	if (!pAuxLine)
		return RET_LOW_MEMORY;

	for (ULONG i=0; i<_ulHeight/2; i++)
	{
		memcpy(pAuxLine, &_pData[i*ulLineSize], ulLineSize);
		memcpy(&_pData[i*ulLineSize], &_pData[_ulDatalen - (i+1)*ulLineSize], ulLineSize );
		memcpy(&_pData[_ulDatalen - (i+1)*ulLineSize], pAuxLine, ulLineSize);
	}

	delete [] pAuxLine;

	return RET_OK;
}

unsigned SepareStrings(std::string & _inStr, std::deque<std::string> & _stringList, char _token)
{
    if (!_inStr.size())
        return RET_INVALID_ARG;

    std::string curString;

    bool lastString = false;

    size_t pos,
           lastPos,
           curStrSize;
    
    pos        = 0;
    lastPos    = 0;
    curStrSize = 0;

    do 
    {
        pos = _inStr.find_first_of(_token, lastPos);
                
        if (pos == std::string::npos)
        {    
            curStrSize = _inStr.size() - lastPos;
            lastString = true;
        }
        else
            curStrSize = pos - lastPos;  

        curString  = _inStr.substr(lastPos, curStrSize);

        _stringList.push_back(curString);

         if (!lastString)
             lastPos = pos+1;

    }while (!lastString);

    return RET_OK;
}

unsigned ReallocIfRequired(unsigned char **_pBufPtr, unsigned & _uBufPtrLen, unsigned _uRequiredBufLen)
{
    unsigned ret = RET_OK;

    if (_pBufPtr)
    {
        unsigned char *pBufPtr = *_pBufPtr;
 
        if (_uBufPtrLen < _uRequiredBufLen)
        {
            if (pBufPtr)
                delete pBufPtr;

            pBufPtr = new BYTE [_uRequiredBufLen];

            if (pBufPtr)
            {   
                _uBufPtrLen = _uRequiredBufLen;
                *_pBufPtr   = pBufPtr;
            }
            else    
                ret = RET_LOW_MEMORY;
        }
    }
    else
    {
        _uBufPtrLen = 0;
        ret = RET_INVALID_ARG;
    }

    return ret;
}
