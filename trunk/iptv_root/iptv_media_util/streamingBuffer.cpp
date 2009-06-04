#include "streamingBuffer.h"
#include "global_error.h"
#include "AutoLock.h"    

StreamingBuffer::StreamingBuffer(ULONG _initialSize) : 
    m_pBuf(NULL),
    m_uBufLen(0),
    m_uWritePos(0),
    m_uReadPos(0),
    m_uReadWaitingBytes(0),
//    m_uWriteWaitingBytes(0),
    m_uTotalWrite(0),
    m_uTotalRead(0),
//    m_bWriteWaiting(false),
    m_bReadWaiting(false),
    m_bReleaseRead(true)
//    m_bReleaseWrite(true)
{
    SetBuffer(_initialSize, false);
}
    
StreamingBuffer::~StreamingBuffer()
{
    AutoLock autolock(m_StreamBufSemaph);

    ResetBuffer();
}


void StreamingBuffer::SetBuffer(ULONG _uBufLen, bool _bSaveData)
{
    if (m_uBufLen != _uBufLen) 
    {
        BYTE *pBufToDelete    = m_pBuf;
        ULONG uBufToDeleteLen = m_uBufLen; 

        if (_uBufLen)
        {
            m_pBuf = new BYTE [_uBufLen];

            if (m_pBuf)
            {
                if (_bSaveData && uBufToDeleteLen && (_uBufLen > uBufToDeleteLen) )
                {
                    ULONG uDataSize = Used();

                    if (uDataSize)
                    {
                        BYTE *pReadPtr = &pBufToDelete[m_uReadPos];

 //                       printf("streamingBuffer.cpp - StreamingBuffer::SetBuffer - uBufToDeleteLen: %lu, _uBufLen: %lu, m_uWritePos: %lu, m_uReadPos: %lu, uDataSize: %lu\n", 
  //                                                             uBufToDeleteLen, _uBufLen, m_uWritePos, m_uReadPos, uDataSize);

                        if (m_uWritePos > m_uReadPos)                        
                        {
                            memcpy(m_pBuf, pReadPtr, uDataSize);
                        }
                        else
                        {
                            ULONG uBytesUntilEnd = uBufToDeleteLen - m_uReadPos;

                            memcpy(m_pBuf, pReadPtr, uBytesUntilEnd);

                            if (uDataSize-uBytesUntilEnd)
                                memcpy(&m_pBuf[uBytesUntilEnd], pBufToDelete, uDataSize-uBytesUntilEnd);
                        }

                        m_uReadPos  = 0;
                        m_uWritePos = uDataSize;
                    }
                }

                m_uBufLen = _uBufLen;
            }
            else
            {
                m_uBufLen = 0;
            }
        }
        else
        {    
            m_pBuf = NULL;
            m_uBufLen = 0;
        }

        if (pBufToDelete)
            delete [] pBufToDelete;
    }

//    printf("streamingBuffer.cpp - StreamingBuffer::SetBuffer - Leaving SetBuffer, m_uBufLen: %lu, m_uWritePos: %lu, m_uReadPos: %lu\n", 
//                                                           m_uBufLen, m_uWritePos, m_uReadPos);
}


void StreamingBuffer::ResetBuffer()
{
    SetBuffer(0, false);

    m_bReleaseRead  = true;
    m_bReadWaiting  = false;

//  m_bReleaseWrite = true;
//  m_bWriteWaiting = false;
//  m_uWriteWaitingBytes = 0;

    m_uReadWaitingBytes  = 0;

    m_uWritePos   = 0;
    m_uReadPos    = 0;
    m_uTotalWrite = 0;
    m_uTotalRead  = 0;
}

ULONG StreamingBuffer::Free()
{
    ULONG uFreeSpace;

    if (m_uReadPos < m_uWritePos)
    {
        uFreeSpace = m_uBufLen - m_uWritePos + m_uReadPos;
    }
    else
    if (m_uReadPos > m_uWritePos)
    {
        uFreeSpace = m_uReadPos - m_uWritePos; 
    }
    else
    {
        if (m_uTotalWrite == m_uTotalRead)
            uFreeSpace = m_uBufLen;
        else
            uFreeSpace = 0;
    }

//    printf("streamingBuffer.cpp - StreamingBuffer::Free - m_uWritePos: %lu, m_uReadPos: %lu, free: %lu\n", m_uWritePos, m_uReadPos, uFreeSpace);

    return uFreeSpace;
}

ULONG StreamingBuffer::Used()
{
    ULONG uBytesUsed;

    if (m_uReadPos < m_uWritePos)
    {
        uBytesUsed = m_uWritePos - m_uReadPos;
    }
    else
    if (m_uReadPos > m_uWritePos)
    {
        uBytesUsed = m_uBufLen - m_uReadPos + m_uWritePos;
    }
    else
    {
        if (m_uTotalWrite == m_uTotalRead)
            uBytesUsed = 0;
        else
            uBytesUsed = m_uBufLen;
    }

//    printf("streamingBuffer.cpp - StreamingBuffer::Used - m_uWritePos: %lu, m_uReadPos: %lu, used: %lu\n", m_uWritePos, m_uReadPos, uBytesUsed);

    return uBytesUsed;
}

ULONG StreamingBuffer::Write(BYTE *_pBuf, ULONG _cbToWrite)
{
    if (!m_pBuf || !m_uBufLen || !_pBuf || !_cbToWrite)
        return 0;

    if (_cbToWrite > m_uBufLen)
        return 0;

    ULONG uBytesUntilEnd,
          uBytesToWrite,
          uPtrPos;

    uBytesUntilEnd = m_uBufLen - m_uWritePos;

//    printf("streamingBuffer.cpp - Write - uBytesUntilEnd: %lu, m_uBufLen: %lu, m_uWritePos: %lu, _cbToWrite: %lu\n", uBytesUntilEnd, m_uBufLen, m_uWritePos, _cbToWrite);

    if (_cbToWrite < uBytesUntilEnd)
    {    
        uBytesToWrite = _cbToWrite;
        uPtrPos = m_uWritePos + uBytesToWrite;
    }
    else
    {    
        uBytesToWrite = uBytesUntilEnd;
        uPtrPos = 0;
    }

    memcpy(&m_pBuf[m_uWritePos], _pBuf, uBytesToWrite); 
    
    m_uWritePos = uPtrPos;

    if (_cbToWrite - uBytesToWrite)
    {
        memcpy(&m_pBuf[m_uWritePos], &_pBuf[uBytesToWrite], _cbToWrite - uBytesToWrite);
        m_uWritePos += _cbToWrite - uBytesToWrite;
    }    

    m_uTotalWrite += _cbToWrite;
    
    return _cbToWrite; 
}

ULONG StreamingBuffer::Read(BYTE *_pBuf, ULONG _cbToRead)
{
    if (!m_pBuf || !m_uBufLen || !_pBuf || !_cbToRead)
        return 0;

    if (_cbToRead > m_uBufLen)
        return 0;

    ULONG uBytesUntilEnd,
          uBytesToRead,
          uPtrPos;

    uBytesUntilEnd = m_uBufLen - m_uReadPos;

    if (_cbToRead <= uBytesUntilEnd)
    {
        uBytesToRead = _cbToRead;
        uPtrPos = m_uReadPos + uBytesToRead;
    }
    else
    {
        uBytesToRead = uBytesUntilEnd;
        uPtrPos = 0;
    }

    memcpy(_pBuf, &m_pBuf[m_uReadPos], uBytesToRead);

    m_uReadPos = uPtrPos;

    if (_cbToRead - uBytesToRead)
    {
        memcpy(&_pBuf[uBytesToRead],  &m_pBuf[m_uReadPos], _cbToRead - uBytesToRead);
        m_uReadPos += _cbToRead - uBytesToRead;
    }

    m_uTotalRead += _cbToRead;

    return _cbToRead;
}

ULONG StreamingBuffer::WriteData(BYTE *_pBuf, ULONG _cbToWrite, ULONG & _cbWrite)
{
    AutoLock autoLock(m_StreamBufSemaph);

    static int counter=0;
    counter++;

//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - Entering StreamingBuffer::WriteData\n", counter);
//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - Requesting %lu bytes to write, Free: %lu, Used: %lu, m_uReadPos: %lu, m_uWritePos: %lu, m_uBufLen: %lu...\n", 
//                                                                       counter, _cbToWrite, Free(), Used(), m_uReadPos, m_uWritePos, m_uBufLen);

    _cbWrite = 0;

    if ( !_pBuf || !_cbToWrite )   
        return RET_ERROR;

    ULONG uBufferSpace = Free();

    if (_cbToWrite > uBufferSpace)
    {
        ULONG uNewBufferSize = m_uBufLen + _cbToWrite - uBufferSpace;
        SetBuffer(uNewBufferSize, true);         
    }

    _cbWrite = Write(_pBuf, _cbToWrite);

    if ( m_bReadWaiting && (Used() >= m_uReadWaitingBytes) )
    {
        m_bReadWaiting = false;
        m_uReadWaitingBytes = 0;

        m_WriteOwned.Signal();
        m_WriteOwned.Wait();
    }

    int ret;

    if (_cbWrite == _cbToWrite)
        ret = RET_OK;
    else 
        ret = RET_ERROR;

//    printf("StreamingBuffer.cpp - %d -StreamingBuffer::WriteData - Leaving StreamingBuffer::WriteData, write: %lu, Free: %lu, Used: %lu, m_uReadPos: %lu, m_uWritePos: %lu, m_uBufLen: %lu, ret: %lu...\n", 
//                                                             counter, _cbWrite, Free(), Used(), m_uReadPos, m_uWritePos, m_uBufLen, ret);

    return ret;
}

ULONG StreamingBuffer::ReadData(BYTE *_pBuf, ULONG _cbToRead, ULONG & _cbRead)
{
    static int counter=0;
    counter++;

    m_StreamBufSemaph.Wait();

//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Entering StreamingBuffer::ReadData\n", counter);
//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Requesting %lu bytes to read, Free: %lu, Used: %lu, m_uReadPos: %lu, m_uWritePos: %lu, m_uBufLen: %lu...\n", 
//                                                                           counter, _cbToRead, Free(), Used(), m_uReadPos, m_uWritePos, m_uBufLen);

    _cbRead = 0;

    if ( !_pBuf || !_cbToRead/* || m_bReleaseRead*/)   
        return RET_ERROR;

    ULONG uBytesUsed = Used();

    if (_cbToRead > uBytesUsed)
    {
        m_bReadWaiting = true;
        m_uReadWaitingBytes = _cbToRead;

        m_StreamBufSemaph.Signal();

//        printf("StreamingBuffer.cpp - StreamingBuffer::ReadData - Waiting %d bytes, Blocking...\n", m_uReadWaitingBytes);

        m_WriteOwned.Wait();
        m_WriteOwned.Signal();

        m_StreamBufSemaph.Wait();

//        printf("StreamingBuffer.cpp - StreamingBuffer::ReadData - Resuming...\n");
    } 

    int ret;

    uBytesUsed = Used();   

    ULONG uBytesToRead;
          
    if (_cbToRead > uBytesUsed) 
        uBytesToRead = uBytesUsed;
    else
        uBytesToRead = _cbToRead;

    _cbRead = Read(_pBuf, uBytesToRead);

    if (_cbRead == _cbToRead)
        ret = RET_OK;
    else
        ret = RET_ERROR;

//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Leaving StreamingBuffer::ReadData, read: %lu, Free: %lu, Used: %lu, m_uReadPos: %lu, m_uWritePos: %lu, m_uBufLen: %lu, first byte: %x, ret: %lu\n", 
//                                                           counter, _cbRead, Free(), Used(), m_uReadPos, m_uWritePos, m_uBufLen, *_pBuf, ret);
 
    m_StreamBufSemaph.Signal();

    return ret;
}

// Public methods definition

#if 0
ULONG StreamingBuffer::WriteData(BYTE *_pBuf, ULONG _cbToWrite, ULONG & _cbWrite)
{
    static int counter=0;
    counter++;

    printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - Entering StreamingBuffer::WriteData\n", counter);
    printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - Requesting %lu bytes to write...\n", counter, _cbToWrite);

    _cbWrite = 0;

    if ( !_pBuf || !_cbToWrite )   
        return RET_ERROR;

//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - before m_StreamBufSemaph.Wait()...\n", counter);
    m_StreamBufSemaph.Wait();
//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - after m_StreamBufSemaph.Wait()...\n", counter);
 
    if ( (_cbToWrite > m_uBufLen) || m_bReleaseWrite)
    {
        printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - Write Released, returning...\n", counter);
        m_StreamBufSemaph.Signal();
        return RET_ERROR;
    }

    ULONG ret = RET_OK;
    ULONG uBytesAvailableToWrite = Free();

//    printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - uBytesAvailableToWrite: %lu\n", counter, uBytesAvailableToWrite);

    if (uBytesAvailableToWrite < _cbToWrite)
    {
//        printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - if (uBytesAvailableToWrite < _cbToWrite), before Write, BytesAvailableToWrite = %lu\n", counter, uBytesAvailableToWrite);

        _cbWrite = Write(_pBuf, uBytesAvailableToWrite );

//        printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - if (uBytesAvailableToWrite < _cbToWrite), after Write, uBytesAvailableToWrite = %lu\n", counter, Free());

        bool bBlock = true;

        if (m_bReadWaiting)    
        {
            m_bReadWaiting = false;

            // at this point the buffer is completely filled so we can unblock reader thread...

            m_WriteOwned.Signal();      // unblock reader
            m_StreamBufSemaph.Signal(); // release mutex, so reader can resume
                    
            m_WriteOwned.Wait();        // wait reader resumes
            m_StreamBufSemaph.Wait();   // wait reader to release the shared mutex

            uBytesAvailableToWrite = Free();

            if (uBytesAvailableToWrite >= _cbToWrite - _cbWrite)
            {
                bBlock = false;
                _cbWrite += Write(&_pBuf[_cbWrite], _cbToWrite - _cbWrite);
            }
            else
            {
                _cbWrite += Write(&_pBuf[_cbWrite], uBytesAvailableToWrite);               
            }
        }

        if (bBlock)
        {
            // need to write bytes, but the buffer is completely full

            m_bWriteWaiting = true;
            m_uWriteWaitingBytes = _cbToWrite - _cbWrite;

            m_StreamBufSemaph.Signal(); // release mutex to avoid deadlock

            m_ReadOwned.Wait();         // waits until reader consumes at least m_uWriteWaitingBytes bytes
            m_StreamBufSemaph.Wait();   // wait reader to release the shared mutex
           
            m_ReadOwned.Signal();       // guive this mutex ownership back to read
           
//            if  (!m_bReleaseWrite)
//            {
                if (Free() >= m_uWriteWaitingBytes)
                {
                    _cbWrite += Write(&_pBuf[_cbWrite], m_uWriteWaitingBytes);
                }
                else
                {       
                    ret = RET_ERROR;
                }
//            }
//            else
//                ret = RET_ERROR;
        }
    }
    else
    {
//        printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - entering ELSE ( uBytesAvailableToWrite >= _cbToWrite ), Writing: %lu bytes\n", counter, _cbToWrite);

        _cbWrite = Write(_pBuf, _cbToWrite);

        if ((m_bReadWaiting) && ( m_uReadWaitingBytes <= Used() ))
        {
//            printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - entering ((m_bReadWaiting) && ( m_uReadWaitingBytes <= Used() )), m_uReadWaitingBytes = %lu bytes, Used: %lu bytes\n", counter, m_uReadWaitingBytes, Used());

            m_WriteOwned.Signal();      // resumes reader
//            printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - After m_WriteOwned.Signal()\n", counter);

            m_WriteOwned.Wait();        // waits reader guive back the ownership of this mutex
//            printf("StreamingBuffer.cpp - %d - StreamingBuffer::WriteData - After m_WriteOwned.Wait()\n", counter);
        }
    }

    m_StreamBufSemaph.Signal();

    printf("StreamingBuffer.cpp - %d -StreamingBuffer::WriteData - Leaving StreamingBuffer::WriteData, write: %lu, ret: %lu\n", counter, _cbWrite, ret);
    return ret;
}

ULONG StreamingBuffer::ReadData(BYTE *_pBuf, ULONG _cbToRead, ULONG & _cbRead)
{
    static int counter=0;
    counter++;

    printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Entering StreamingBuffer::ReadData\n", counter);
    printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Requesting %lu bytes to read...\n", counter, _cbToRead);

    _cbRead = 0;

    if ( !_pBuf || !_cbToRead )   
        return RET_ERROR;

    m_StreamBufSemaph.Wait();
 
    if ( (_cbToRead > m_uBufLen) || m_bReleaseRead)
    {
        printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Read Released, returning...\n", counter);
        m_StreamBufSemaph.Signal();
        return RET_ERROR;
    }

    ULONG ret = RET_OK;
    ULONG uBytesAvailableToRead = Used();

    if (uBytesAvailableToRead < _cbToRead)
    {
//        printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - if (uBytesAvailableToRead < _cbToRead), before Read, uBytesAvailableToRead: %lu...\n", counter, uBytesAvailableToRead);

        _cbRead = Read(_pBuf, uBytesAvailableToRead );

//        printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - if (uBytesAvailableToRead < _cbToRead), after Read, uBytesAvailableToRead: %lu...\n", counter, Used());

        bool bBlock = true;

        if (m_bWriteWaiting)    
        {
            m_bWriteWaiting = false;

            // at this point the buffer is completely empty so we can unblock writer thread...

            m_ReadOwned.Signal();       // unblock writer
            m_StreamBufSemaph.Signal(); // release mutex, so writer can resume
                    
            m_ReadOwned.Wait();         // wait writer resumes
            m_StreamBufSemaph.Wait();   // wait writer release the shared mutex

            uBytesAvailableToRead = Used();

            if (uBytesAvailableToRead >= _cbToRead - _cbRead)
            {
                bBlock = false;
                _cbRead += Read(&_pBuf[_cbRead], _cbToRead - _cbRead);
            }
            else
            {
                _cbRead += Read(&_pBuf[_cbRead], uBytesAvailableToRead);               
            }
        }

        if (bBlock)
        {
            // need to read bytes, but the buffer is completely empty

            m_bReadWaiting = true;
            m_uReadWaitingBytes = _cbToRead - _cbRead;

            m_StreamBufSemaph.Signal(); // release mutex to avoid deadlock

            printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Waiting %lu bytes, Blocking...\n", counter, m_uReadWaitingBytes);
            m_WriteOwned.Wait();        // waits until writer writes at least m_uReadWaitingBytes bytes           
            printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Bytes Available: %lu, Resuming...\n", counter, Used());

            m_WriteOwned.Signal();      // guive this mutex ownership back to reader
            m_StreamBufSemaph.Wait();   // wait writer to release the shared mutex
           
   //         if  (!m_bReleaseRead)
 //           {
                if (Used() >= m_uReadWaitingBytes)
                {
                    _cbRead += Read(&_pBuf[_cbRead], m_uReadWaitingBytes);
                }
                else
                {       
                    ret = RET_ERROR;
                }
   //         }
  //          else
  //              ret = RET_ERROR;
        }
    }
    else
    {
        _cbRead = Read(_pBuf, _cbToRead);

        if ((m_bWriteWaiting) && ( m_uWriteWaitingBytes <= Free() ))
        {
 //           m_StreamBufSemaph.Signal(); // release mutex to avoid deadlock

            m_ReadOwned.Signal();       // resumes writer
            m_ReadOwned.Wait();         // waits writes guive back the ownership of this mutex

 //           m_StreamBufSemaph.Wait();   // wait writer to write the bytes it was waiting for    
        }
    }

    m_StreamBufSemaph.Signal();


    printf("StreamingBuffer.cpp - %d - StreamingBuffer::ReadData - Leaving StreamingBuffer::ReadData, read: %lu, first byte: %x, ret: %lu\n", counter, _cbRead, *_pBuf, ret);
    return ret;

}
#endif

ULONG StreamingBuffer::FreeSpace()
{
    AutoLock autoLock(m_StreamBufSemaph);
    return Free();
}

ULONG StreamingBuffer::UsedSpace()
{
    AutoLock autoLock(m_StreamBufSemaph);
    return Used();
}

ULONG StreamingBuffer::GetBufLen()
{
    AutoLock autoLock(m_StreamBufSemaph);
    return m_uBufLen;
}
