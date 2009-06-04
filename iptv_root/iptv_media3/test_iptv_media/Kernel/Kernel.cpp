#include "compat.h"
//#include "CThreadSafeDeque.h"
#include "Kernel.h"
#include "Global_error.h"
#include "Media_const.h"
#include "CMedia.h"
#include "IKernelInterfaceNotify.h"
#include "media_utilities.h"
#include "log.h"

Kernel::Kernel()
{
    m_bInit = FALSE;
    m_pKernelInterfaceNotify = NULL;

    m_pSockBuffs = new SockBuffs(MAX_TRANSMISSIONS, MAX_PKT_SIZE * MAX_PKT_COUNT);
    m_pMedia     = new CMedia();

    if (m_pSockBuffs && m_pMedia)
    {  
        m_bInit = TRUE;
        m_pMedia->SetKernelNotify(this);
    }    
}

Kernel::~Kernel()
{
    if (m_pSockBuffs)
        delete m_pSockBuffs;

    if (m_pMedia)
        delete m_pMedia;
}


ULONG Kernel::InitNetwork(LPCSTR _szFilename)
{    
    if (!m_bInit)
        return RET_INIT_ERROR;
    
    if ( !( m_pSockBuffs->SetUrl(_szFilename) && 
		    m_pSockBuffs->SetINotify(this, 0) && 
			m_pSockBuffs->OpenUrl() ) )
    {
        m_bInit = FALSE;
        return RET_SOCKBUFFS_ERROR;
    }

    return RET_OK;
}

ULONG Kernel::InitNetwork(LPCSTR _szProtocol, LPCSTR _szHost, DWORD _dwPort, LPCSTR _szConnStr, BOOL _flListen)
{
    if (!m_bInit)
        return RET_INIT_ERROR;
    
    if ( !(m_pSockBuffs->SetUrl(_szProtocol, _szHost ,_dwPort, _szConnStr, _flListen, NULL) && 
           m_pSockBuffs->SetINotify(this, 0) &&
           m_pSockBuffs->OpenUrl() ) )
    {
        m_bInit = FALSE;
        return RET_SOCKBUFFS_ERROR;
    }

    return RET_OK;
}

BOOL Kernel::NewID(ULONG _id)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	ULONG ret = FALSE;

    if ( m_pSockBuffs->CreateWnd(_id, MAX_PKT_SIZE, 10, TRUE, SB_MEDIAPKTTYPE_EXT, _id))         
    {        
        if ( m_pMedia->AddTransmission(_id) == RET_OK )
            ret = TRUE;
	}

    return ret;
}

BOOL Kernel::OnMediaNotify(MediaParam *_pParameters)
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
    
    if (!_pParameters)
        return RET_INVALID_ARG;

   ULONG ret = RET_OK;

    switch(_pParameters->code)
    {
        case MN_READ_PACKET:
        {		
            ISockBuff *pSockReg = m_pSockBuffs->GetWndObj(_pParameters->id);

            if (pSockReg)
            {
				BYTE  pData[MAX_PKT_SIZE];
				ULONG ulDatalen = MAX_PKT_SIZE;

                if (pSockReg->Read(pData, ulDatalen, &ulDatalen) >= 0)
					ret = m_pMedia->SetPacket(pData, ulDatalen);
				else
				    ret = RET_ERROR;
			}
            else
                ret = RET_ERROR;   
        }
		break;

		case MN_WRITE_PACKET:
		break;

        case MN_VIDEO_ALIVE:            
            ret = m_pKernelInterfaceNotify->SetMediaAlive(_pParameters->id, VIDEO);
		break;

		case MN_AUDIO_ALIVE:            
            ret = m_pKernelInterfaceNotify->SetMediaAlive(_pParameters->id, AUDIO);
		break;
	
		case MN_BUFFERING_START:
		case MN_BUFFERING_STOP:
        {
            MediaParamExtBuf *pMediaParamExtBuf = (MediaParamExtBuf *) _pParameters;
            
        #ifdef _DEBUG
            char szBuf[200];

            if (_pParameters->code == MN_BUFFERING_START)
                sprintf(szBuf, "MN_BUFFERING_START - perc: %lu - ID: %lu", pMediaParamExtBuf->ulPercent, pMediaParamExtBuf->id);
            else
                sprintf(szBuf, "MN_BUFFERING_STOP - perc: %lu - ID: %lu",  pMediaParamExtBuf->ulPercent, pMediaParamExtBuf->id);
            
            g_FileLog.SetLog(szBuf);
        #endif
        }
        break;
    
		case MN_ERROR:
		break;
	}

    return ret;
}

ULONG Kernel::TerminateConference()
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
    
	m_pSockBuffs->Abort();

    CHECK_ERROR(m_pMedia->EndAllTransmissions() , RET_OK)

    return RET_OK;
}

ULONG Kernel::GetMediaDbgInfo(ULONG _id, MediaDbgInfo & _mediaDbgInfo)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    CHECK_ERROR(m_pMedia->GetMediaDbgInfo(_id, _mediaDbgInfo), RET_OK)

    return RET_OK;
}

ULONG Kernel::GetVideoReg(ULONG _id, VideoFrame_reg ** _ppVideoFrameReg)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    CHECK_ERROR(m_pMedia->GetVideoReg(_id, _ppVideoFrameReg), RET_OK)

    return RET_OK;
}
