#include "SimposiumNetwork.h"
#include "ithread.h"
#include "global_error.h"
#include "media_utilities.h"
#include "log.h"

void SimposiumNetwork::SetSink(ISimposiumSink *_pSink)
{
    m_FileSourceStream.SetSink(_pSink);
}

void SimposiumNetwork::SetId(ULONG _id)
{
    m_FileSourceStream.SetId(_id);
}

void SimposiumNetwork::SetFilename(std::string _strFilename)
{
    std::string & filename = m_FileSourceStream.GetFilenameStr();

    if (filename != _strFilename)
    {
        m_bGeneratedHeader = false;
    }

    m_FileSourceStream.SetFilename(_strFilename);
}

ULONG SimposiumNetwork::GenerateHeader()
{
    ULONG ret;

    if ((ret = m_FileSourceStream.Initialize()) == RET_OK)
    {
        m_FileSourceStream.GenerateHeader();
    }

    return ret;
}

ULONG SimposiumNetwork::GetHeader(BYTE **_ppHeader, ULONG & _uHeaderLen)
{
    ULONG ret = RET_OK;

    if (!m_bGeneratedHeader)
    {
        if ((ret = GenerateHeader()) == RET_OK)
        {
            m_bGeneratedHeader = true;
        }
        else
        {
            return ret;
        }
    }

    if (m_bGeneratedHeader)
    {
        ret = m_FileSourceStream.GetHeader(_ppHeader, _uHeaderLen);
    }
    else
    {
        ret =  RET_INIT_ERROR;
    }

    return ret;
}

ULONG SimposiumNetwork::StartTransmission()
{
    ULONG ret = RET_OK;

    m_pTransmitMoviePktThread = CreateIThread(_TransmitMoviePkt, this, TRUE);

    if (m_pTransmitMoviePktThread)
    {
        m_bThreadRunning = true;
    }
    else
        ret = RET_ERROR;

    return ret;
}

int SimposiumNetwork::_TransmitMoviePkt(IThread *_pThread, void* _pCtx)
{
    g_FileLog.SetLog("Entering _TransmitMoviePkt");

    SimposiumNetwork *pCtx = (SimposiumNetwork *) _pCtx;
    pCtx->m_bTerminate = false;

    ULONG ret;

    do
    {
        ret = pCtx->m_FileSourceStream.DeliverPacket();

    }while ((ret == RET_OK) && (!pCtx->m_bTerminate));

    g_FileLog.SetLog("Leaving _TransmitMoviePkt");

    return 0;
}

void SimposiumNetwork::WaitThread()
{
    if (m_bThreadRunning)
    {
        if (m_pTransmitMoviePktThread)
            m_pTransmitMoviePktThread->WaitForMe(INFINITE_TIME);

        m_bThreadRunning = false;
    }	
}

void SimposiumNetwork::Terminate()
{
    m_bTerminate = true;

    WaitThread();
}

void SimposiumNetwork::WaitUntilFinish()
{
    WaitThread();
}

