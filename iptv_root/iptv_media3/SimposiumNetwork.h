#ifndef SIMPOSIUM_NETWORK_H
#define SIMPOSIUM_NETWORK_H

#include "ASFFileSourceStream.h"
#include "ithread.h"

class ISimposiumSink;
class SimposiumNetwork
{
private:
    
    IThread * m_pTransmitMoviePktThread;

    ASFFileSourceStream m_FileSourceStream;

    bool m_bTerminate,
         m_bThreadRunning,
         m_bGeneratedHeader;

    void WaitThread();

    ULONG GenerateHeader();

public:

    SimposiumNetwork() : m_pTransmitMoviePktThread(NULL),
                         m_bTerminate(false),
                         m_bThreadRunning(false),
                         m_bGeneratedHeader(false)
    {}

    virtual ~SimposiumNetwork() 
    {
        Terminate();

        if (m_pTransmitMoviePktThread)
            delete m_pTransmitMoviePktThread;
    }


    ULONG StartTransmission();

    void WaitUntilFinish();
    void Terminate();

    void SetSink(ISimposiumSink *_pSink);
    void SetId(ULONG _id);
    void SetFilename(std::string _strFilename);

    ULONG GetHeader(BYTE **_ppHeader, ULONG & _uHeaderLen);

    static int _TransmitMoviePkt(IThread *_pThread, void* _pCtx);	
};

#endif

