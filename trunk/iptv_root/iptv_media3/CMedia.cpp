#include "compat.h"
#include "mediapkt.h"
#include "Data_reg.h"
#include "CThreadSafeDeque.h"
#include "CMedia.h"
#include "CTransmission.h"
#include "Global_functions.h"
#include "avcodec.h"
#include "media_utilities.h"
#include "log.h"
#include "common.h"

CMedia::CMedia() : 
    m_CodecSemaph(),
    m_MediaSemaph(),
    m_AudioObjectApi()
{        
    m_bMuteAll   = FALSE;
    m_bConfEnded = FALSE;

    m_pKernelNotify = NULL;
}

CMedia::~CMedia()
{        
    if (!m_bConfEnded)
        EndAllTransmissions();
}

ULONG CMedia::SetMute(BOOL _bMuteAudio)
{
    m_MediaSemaph.Wait();

    if (m_bMuteAll != _bMuteAudio)
    {
        m_bMuteAll = _bMuteAudio;
        
        if ( m_pTransmissionList.size() )
            SetAudioMute(_bMuteAudio, TRUE);
    }

    m_MediaSemaph.Signal();

    return RET_OK;
}

ULONG CMedia::AddTransmission(ULONG _id, 
                              BOOL _bHeader, 
                              BOOL _bBase64Encoded, 
                              BYTE *_pHeader, 
                              ULONG _uHeaderLen)
{
    if (!m_pKernelNotify)
        return RET_INIT_ERROR;
 
    IMediaApi * pAudioApi;

    unsigned ret;

    m_MediaSemaph.Wait();

    if ((ret = m_AudioObjectApi.GetApiInterface(&pAudioApi)) == RET_OK)
    {
        CTransmission * pTransmission = new CTransmission(_id, m_pKernelNotify, pAudioApi, &m_CodecSemaph);         
	   
        if (pTransmission)
        {
            if (m_bMuteAll)
                pTransmission->SetAudioMute(TRUE);
 
            if (_bHeader && _pHeader && _uHeaderLen)
            {
                if (_bBase64Encoded)
                {
                    BYTE *pDecodedHeader = new BYTE [_uHeaderLen]; // melhorar isto

                    if (pDecodedHeader)
                    {
                        int iDecodedHeaderLen = _uHeaderLen; // melhorar isto

                        if (decode64((const char *)_pHeader, _uHeaderLen, (char *)pDecodedHeader, &iDecodedHeaderLen) == 0)
                        {
                            ret = pTransmission->SetDemuxStreamHeader(pDecodedHeader, iDecodedHeaderLen);
                        }
                        else
                        {
                            ret = RET_ERROR;
                        }

                        delete pDecodedHeader;
                    }
                    else
                    {
                        ret = RET_LOW_MEMORY;
                    }
                }
                else
                {
                    ret = pTransmission->SetDemuxStreamHeader(_pHeader, _uHeaderLen);
                }
            }

            pTransmission->CreateConferenceWnd();
            m_pTransmissionList.push_back(pTransmission);
        }
        else
            ret = RET_LOW_MEMORY;
    }

    m_MediaSemaph.Signal();

    return ret;
}


ULONG CMedia::EndTransmission(ULONG _id)
{

    m_MediaSemaph.Wait();

    ULONG ulCurId,
          ret;

    ret = RET_INVALID_ARG;

    std::deque<CTransmission *>::iterator it = m_pTransmissionList.begin();

    for(ULONG i=0; i < m_pTransmissionList.size(); i++, it++)
    {
        if (m_pTransmissionList[i])
        { 
            m_pTransmissionList[i]->GetId(&ulCurId);
	
            if (ulCurId == _id)
            {
                m_pTransmissionList[i]->DestroyConferenceWnd();		
                delete m_pTransmissionList[i];
				
                m_pTransmissionList.erase(it);
                	
                ret = RET_OK;
                
                break;
            }
        }
        else
        {    
            ret = RET_ERROR;
            break;
        }
    }

    m_MediaSemaph.Signal();

    return ret;
}

ULONG CMedia::GetVideoReg(ULONG _id, VideoFrame_reg ** _ppVideoFrameReg)
{

    ULONG ulCurId,
          ret;

    m_MediaSemaph.Wait();

    ret = RET_INVALID_ARG;

    for(ULONG i=0; i < m_pTransmissionList.size(); i++)
    {
        if (m_pTransmissionList[i])
        {
            m_pTransmissionList[i]->GetId(&ulCurId);
        
            if (_id == ulCurId)
            {
                m_pTransmissionList[i]->GetVideoReg(_ppVideoFrameReg);
                ret = RET_OK;
                break;
            }
        }
        else
        {   
            ret = RET_ERROR;         
            break;
        }
    }

    m_MediaSemaph.Signal();

    return ret;
}
    
ULONG CMedia::GetMediaDbgInfo(ULONG _ulId, MediaDbgInfo & _mediaDbgInfo)
{
    ULONG ulCurId,
          ret;

    ret = RET_INVALID_ARG;

    m_MediaSemaph.Wait();

    for(ULONG i=0; i< m_pTransmissionList.size(); i++)
    {
        if (m_pTransmissionList[i])
        {		
            m_pTransmissionList[i]->GetId(&ulCurId);
			
            if (_ulId == ulCurId)
            {	
                m_pTransmissionList[i]->GetMediaDbgInfo(_mediaDbgInfo);
                ret = RET_OK;
                break;
            }
        }
        else
        {    
            ret = RET_ERROR;
            break;
        }
    }

    m_MediaSemaph.Signal();

    return ret;
}


ULONG CMedia::SetPacket(BYTE *_pData, ULONG _ulDatalen)
{	
    if (!_pData || !_ulDatalen)
        return RET_INVALID_ARG;

    m_MediaSemaph.Wait();

    MediaPkt *pMediaPkt = (MediaPkt *) _pData;
	
    ULONG id,
          ret;

    ret = RET_ERROR;

    for (ULONG i=0; i<m_pTransmissionList.size(); i++ )
    {
        if (m_pTransmissionList[i])
        {
            m_pTransmissionList[i]->GetId(&id);
			
            if (id == pMediaPkt->id)
            {	
                ret = m_pTransmissionList[i]->SetPacket(_pData, _ulDatalen);
                break;
            }
        }
        else
	        break;
    }

    m_MediaSemaph.Signal();

    return ret;
}


ULONG CMedia::EndAllTransmissions()
{
    if (!m_bConfEnded)
    {        
        while(m_pTransmissionList.size())
        {	
            m_MediaSemaph.Wait();
            CTransmission *pFirst = m_pTransmissionList[0];
            m_MediaSemaph.Signal();

            if (pFirst)
            {
                pFirst->DestroyConferenceWnd();
                delete pFirst;
            }

            m_MediaSemaph.Wait();  
            m_pTransmissionList.pop_front();
            m_MediaSemaph.Signal();
        }
    
        m_bConfEnded = TRUE;
    }

    return RET_OK;
}

ULONG CMedia::SetAudioMute(BOOL _bMuteAudio, BOOL _bApplyToAll, ULONG _id)
{
    if (!m_pTransmissionList.size())
        return RET_INVALID_CALL;

    ULONG ret;

    if (_bApplyToAll)
        ret = RET_OK;
    else
        ret = RET_INVALID_ARG;

    for (ULONG index=0; index < m_pTransmissionList.size(); index++ )
    {
        if (m_pTransmissionList[index])
        {
            if (_bApplyToAll)
            {
                m_pTransmissionList[index]->SetAudioMute(_bMuteAudio);
            }
            else
            {
                ULONG currentId;

                m_pTransmissionList[index]->GetId(&currentId);
            
                if (_id == currentId)
                {   
                    ret = m_pTransmissionList[index]->SetAudioMute(_bMuteAudio);
                                        
                    break;
                }
            }
        }
        else
        {
            ret = RET_ERROR;
            break;
        }
    }

    return ret;
}
