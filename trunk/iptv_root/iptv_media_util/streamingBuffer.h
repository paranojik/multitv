#ifndef STREAMING_BUFFER_H
#define STREAMING_BUFFER_H

#include "shared/compat.h"
#include "shared/semaphore.h"

class StreamingBuffer
{
private:
    BYTE  *m_pBuf;

    ULONG  m_uBufLen,
           m_uWritePos,
           m_uReadPos;

    ULONG  m_uTotalWrite,
           m_uTotalRead;

    bool  m_bReadWaiting,
          m_bReleaseRead;

        // m_bWriteWaiting,
        // m_bReleaseWrite;

    ULONG m_uReadWaitingBytes;
        // m_uWriteWaitingBytes;

    _SEMAPHORE m_StreamBufSemaph,
               m_WriteOwned;
        // m_ReadOwned,


    ULONG Free();
    ULONG Used();
    
    ULONG Write(BYTE *_pBuf, ULONG _cbToWrite);
    ULONG Read(BYTE *_pBuf, ULONG _cbToRead);

    void SetBuffer(ULONG _uBufLen, bool _bSaveData);
    void ResetBuffer();

public:
     
    StreamingBuffer(ULONG _initialSize = 0);
    ~StreamingBuffer();

    ULONG ReadData(BYTE *_pBuf, ULONG _cbToRead, ULONG & _cbRead);
    ULONG WriteData(BYTE *_pBuf, ULONG _cbToWrite, ULONG & _cbWrite);
    ULONG FreeSpace();
    ULONG UsedSpace();
    ULONG GetBufLen();

    // these two methods must be called in the thread that reads from this buffer
 /*   void SetReadMutexOwner()     
    { 
        m_ReadOwned.Wait();
        m_bReleaseWrite = false;
    }

    void ReleaseReadMutexOwner() 
    { 
        m_ReadOwned.Signal(); 
        m_bReleaseWrite = true;
    }
*/    
    // these two methods must be called in the thread that writes to this buffer
    void SetWriteMutexOwner()     
    {
        m_WriteOwned.Wait();    
        m_bReleaseRead = false;
    }

    void ReleaseWriteMutexOwner() 
    { 
        m_WriteOwned.Signal();  
        m_bReleaseRead = true;
    }
};


#endif

