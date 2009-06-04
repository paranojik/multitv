#include "ASFStream.h"
#include "ASFDataPacket.h"
#include "ASFIndexObject.h"
#include "ASFDataObject.h"
#include "ASFHeader.h"
#include "ASFSimpleIndexObject.h"
#include "global_error.h"
#include "IFilterData.h"
#include "media_utilities.h"
//#include "Types.h"
#include "macros.h"


ASFTransmissionStream::ASFTransmissionStream(FileStream *_pFileStream, BOOL bIrmStream) :
    //CMCStructuredStream(pStream, NAME("CMCASFStream")),
    m_wPacketSize(0),
    m_lDataPacketSequence(-1),
    m_qwPreroll(0),
    m_bIrmStream(bIrmStream),
    m_pIndex(NULL),
    m_ulHeaderRead(0),
    m_pDataFilter(NULL)
{
    m_pStream = _pFileStream;
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFStream::CMCASFStream"));
}

ASFTransmissionStream::~ASFTransmissionStream()
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFStream::~CMCASFStream"));
    //SAFE_DELETE(m_pIndex);
}

void ASFTransmissionStream::AddDataFilter(IFilterData* pDataFilter)
{
    m_pDataFilter = pDataFilter;
}

void ASFTransmissionStream::SetHeader(ASFHeader& _asfHeader)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFStream::SetHeader()"));

    m_Header.CopyFrom(_asfHeader);

    FilterObject(m_Header);

    m_ulHeaderRead = ~0;
    m_lDataPacketSequence = 0;
}

unsigned ASFTransmissionStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
    //_TRACE3(LOGLEVEL5, TEXT("CMCASFStream::Seek(%.0f, orig: %d, newp: %p)"),
    //	(double) dlibMove.QuadPart, dwOrigin, plibNewPosition);
	
    if (dwOrigin == SEEK_SET)
    {
        m_ulHeaderRead = (ULONG) dlibMove.QuadPart;
    } 
    else if (dwOrigin == SEEK_CUR)
    {
        m_ulHeaderRead += (ULONG) dlibMove.QuadPart;

        if (m_ulHeaderRead > (ULONG) m_Header.GetObjectSize())
	{
            //_WARN1C(STG_E_INVALIDFUNCTION, DsCAT_ERROR, TEXT("Attempting to seek to invalid position(%.0f)"),(double) dlibMove.QuadPart);

            return RET_ERROR; //STG_E_INVALIDFUNCTION;
	}
    }
    else if (dwOrigin == MC_ASF_STREAM_SEEK_MEDIA)
    {
        if (m_pIndex && m_pStream)
        {
            DWORD dwPacketNumber = m_pIndex->GetPacketNumber(dlibMove.QuadPart);

            LARGE_INTEGER seekpos;
            seekpos.QuadPart = m_ulHeaderRead + (m_wPacketSize * dwPacketNumber);

            return m_pStream->Seek(seekpos, SEEK_SET, plibNewPosition);
         }
         else
         {
             //_WARN0C(STG_E_INVALIDFUNCTION, DsCAT_ERROR, TEXT("Attempting to media seek to a non indexed stream"));

             return RET_ERROR; //STG_E_INVALIDFUNCTION;
         }
    }

    if (plibNewPosition)
    {
        plibNewPosition->QuadPart = m_ulHeaderRead; 
    }

    return RET_OK;
}       

unsigned ASFTransmissionStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
    //_TRACE1(LOGLEVEL2, TEXT("CMCASFStream::Read(%d)"), cb);

    if (!pv)
    {
        //_WARN0C(E_POINTER, DsCAT_ERROR, TEXT("Invalid pointer"));
        return RET_INVALID_ARG;//STG_E_INVALIDPOINTER;
    }

    if (pcbRead)
    {
        *pcbRead = 0;
    }

    if (!m_pStream)
    {
        //_WARN0(DsCAT_ERROR, TEXT("ASF Stream has no stream associated"));
        return RET_ERROR; //STG_E_READFAULT;
    }
    ULONG cbRead = 0;

    // if Asf Data Packet sequence has not started, read asf object
    if (m_lDataPacketSequence < 0)
    {
        ASFObject::ASF_OBJECT_TYPE type = ASFObject::MC_ASF_UNKNOWN;

        while (cbRead < cb && type != ASFObject::MC_ASF_DATAOBJECT)
        {
            // read header
            ASFObject* pAsfObj = GetObject();

            if (!pAsfObj)
            {
                return RET_ERROR; //E_PENDING;
            }

            // never leave a Com Object without its wrapper
            //CComPtr<IUnknown> pObj;
            //pObj.Attach((LPUNKNOWN) (INSSBuffer*) pAsfObj);

            type = pAsfObj->GetType();

            if (pAsfObj->GetObjectSize() > cb)
            {
                //_WARN2(DsCAT_ERROR, TEXT("Object (%d) is greater than requested (%d)"),
                //	(DWORD) pAsfObj->GetObjectSize(), cb);
                
                return RET_ERROR; //E_FAIL;
             }

             pAsfObj->CopyTo(((LPBYTE) pv) + cbRead);

             cbRead += (ULONG) pAsfObj->GetObjectSize();

             if (pcbRead)
             {
                 *pcbRead = cbRead;
             }
        } // end while
    } 
    else
    {
        unsigned ret;

        if ((ret = GetPacket(pv, cb, pcbRead)) != RET_OK)
            return ret;

    } // end if (m_lDataPack...)

    return RET_OK;
}


unsigned ASFTransmissionStream::GetPacket(void *_pv, ULONG _cb, ULONG *_pcbRead)
{
    //_TRACE0(LOGLEVEL3, TEXT("CMCASFStream::GetPacket()"));

    unsigned ret = RET_OK;

    if (!_pv)
    {
        //_ERROR0C(E_POINTER, MCMSG("Invalid pointer"));
    }

    if (m_wPacketSize > _cb)
    {
        //_WARN2(DsCAT_ERROR, TEXT("Object (%d) is greater than requested (%d)"),m_wPacketSize, cb);
        return RET_ERROR;//STG_E_READFAULT;
    }

    IRM_PKT& pkt = * (IRM_PKT*) _pv;

    ULONG cbRead = 0;
    ULONG cbToRead = m_wPacketSize;
    LPBYTE pPayload;

    if (m_bIrmStream)
    {
        cbToRead += sizeof(pkt.header);
        pPayload = (LPBYTE) _pv;
    }
    else
    {
        pPayload = pkt.payload;
    } // end if irm stream

//    printf("ASFStream.cpp - GetPacket - before m_pStream->Read, Reading buffer from file, %d bytes requested...\n", cbToRead);

    if ((ret = m_pStream->Read(pPayload, cbToRead, &cbRead)) != RET_OK)
        return ret;

//    printf("ASFStream.cpp - GetPacket - after m_pStream->Read, %d bytes read, pkt.payload first byte: %x...\n", cbRead, *pkt.payload);

    // if it's index
    if ((cbRead >= sizeof(GUID_W))
        && (memcmp(pPayload, &ASFObject::ASF_Simple_Index_Object, sizeof(GUID_W)) == 0
        || memcmp(pPayload, &ASFObject::ASF_Index_Object, sizeof(GUID_W)) == 0
        || memcmp(pPayload, &ASFObject::ASF_Media_Object_Index_Object, sizeof(GUID_W)) == 0
        || memcmp(pPayload, &ASFObject::ASF_Timecode_Index_Object, sizeof(GUID_W)) == 0))
    {
//        printf("ASFStream.cpp - GetPacket - entering block if ((cbRead >= sizeof(GUID))...\n");

        if (m_pDataFilter)
        {
            m_pDataFilter->OnEndOfData();
        }

        ASFSimpleIndexObject AsfSimpleIndex;
        ASFIndexObject AsfIndexObject;
        ASFIndexObject* pAsfIndex;

        if (memcmp(pPayload, &ASFObject::ASF_Simple_Index_Object, sizeof(GUID_W)) == 0)
        {
            pAsfIndex = static_cast<ASFSimpleIndexObject*>(&AsfSimpleIndex);
        }
        else if (memcmp(pPayload, &ASFObject::ASF_Index_Object, sizeof(GUID_W)) == 0)
        {
            // TODO: no specific derived class for this index object yet (assuming it won't be used)
            pAsfIndex = &AsfIndexObject;
        }
        else if (memcmp(pPayload, &ASFObject::ASF_Media_Object_Index_Object, sizeof(GUID_W)) == 0)
        {
            // TODO: no specific derived class for this index object yet (assuming it won't be used)
            pAsfIndex = &AsfIndexObject;
        }
        else if (memcmp(pPayload, &ASFObject::ASF_Timecode_Index_Object, sizeof(GUID_W)) == 0)
        {
            // TODO: no specific derived class for this index object yet (assuming it won't be used)
            pAsfIndex = &AsfIndexObject;
        }

        ASFIndexObject& AsfIndex = * pAsfIndex;
        //AsfIndex.AddRef();
        AsfIndex.Attach(pPayload);

        if (cbRead < AsfIndex.GetObjectSize())
        {
            ULONG cbLeft = (ULONG) AsfIndex.GetObjectSize() - cbRead;

            if (m_bIrmStream)
            {
                cbLeft -= sizeof(pkt.header);
            }

            ret = m_pStream->Read(pPayload + cbRead, cbLeft, &cbLeft);

            if (ret != RET_OK || (cbLeft + cbRead) < AsfIndex.GetObjectSize())
            {
                //_WARN1C(hr, DsCAT_ERROR, TEXT("Cannot read (%d) from stream"), cbLeft);
                return ret;
            }
        }	

        AsfIndex.Refresh();
        FilterObject(AsfIndex);
    }
    else
    {
//        printf("ASFStream.cpp - GetPacket - entering block ELSE...\n");

        if (_pcbRead)
        {
            *_pcbRead = cbRead;
        }

        if (cbRead < cbToRead)
        {
            //_TRACE0(DsLOGLEVEL5 | DsLOG_IO, TEXT("End of ASF stream"));
            if (m_pDataFilter)
            {
                m_pDataFilter->OnEndOfData();
            }
        }
        else
        {
            // read data object
            ASFDataPacket AsfObj(m_wPacketSize);
            //AsfObj.AddRef();

            AsfObj.Attach(pkt.payload);
            AsfObj.Refresh();

            if (m_bIrmStream)
            {
                m_lDataPacketSequence = pkt.header.seq;
            }
            else
            {
                pkt.header.datalen = m_wPacketSize;
                pkt.header.id = 0;
                pkt.header.type = MTYPE_MEDIA;
                pkt.header.flags = AsfObj.HasKeyFrame() ? PKT_KEYFRAME : 0;
                pkt.header.seq = ++m_lDataPacketSequence;

                if (_pcbRead)
                {
                    *_pcbRead += sizeof(pkt.header);
                }
            } // end if irm stream

            FilterObject(AsfObj);
        }
    }

    return ret;
}

ASFObject* ASFTransmissionStream::GetObject()
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFStream::GetObject"));

    BYTE pBuf[24];
    ULONG cbRead = 0, cbToRead = 0;

    // read asf object header (guid + size = 24 bytes)
    cbToRead = sizeof(pBuf);

    unsigned ret = m_pStream->Read(pBuf, cbToRead, &cbRead);
    if (ret != RET_OK)
    {
         //_ERROR1C(hr, MCMSG("Cannot read (%d) from stream"), cbToRead);
    }

    if (cbRead < cbToRead)
    {
        //_TRACE0(LOGLEVEL5, TEXT("End of ASF stream"));
        return NULL;
    }

    ASFObject *pObj = NULL;

    if (memcmp(pBuf, &ASFObject::ASF_Header_Object, sizeof(GUID_W)) == 0)
    {
        pObj = new ASFHeader();	
    }
    else
    if (memcmp(pBuf, &ASFObject::ASF_Data_Object, sizeof(GUID_W)) == 0)
    {
        pObj = new ASFDataObject();

        m_lDataPacketSequence = 0;
    }
    else 
    if (memcmp(pBuf, &ASFObject::ASF_Simple_Index_Object, sizeof(GUID_W)) == 0)
    {
        pObj = new ASFSimpleIndexObject();
    }
    else
    {
        //_WARN0(DsCAT_WARNING, TEXT("Object not supported, reporting end of stream (maybe an index)"));
        return NULL;
    }

    if (!pObj)
    {
        //_ERROR0C(E_OUTOFMEMORY, MCMSG("Out of memory"));
    }

    pObj->Attach(pBuf);
    QWORD cbObject = pObj->GetObjectSize();

    // let's create the asf object according to object type
    // and put it in a CComPtr wrapper
    // its type will be INSSBuffer interface, so we are able to set length

    //CComPtr<INSSBuffer> pAsfObject = pObj;
    INSSBuffer * pAsfObject = pObj;

    if (cbObject < cbRead)
    {
        //_ERROR1(MCMSG("Invalid object size (%.0f)"), (double) (LONGLONG) pObj->GetObjectSize());
    }

    // okay, now set the new asf object buffer to the size of asf object
    ret = pAsfObject->SetLength((ULONG) cbObject);
    if (ret != RET_OK)
    {
        //_ERROR1C(hr, MCMSG("Cannot set object length (%.0f)"), (double) (LONGLONG) pObj->GetObjectSize());
    }

    // copy the object header from tmp buf
    // we have to get the pointer to it
    LPBYTE pAsfObjectBuf = NULL;

    ret = pAsfObject->GetBuffer(&pAsfObjectBuf);
    if (ret != RET_OK)
    {
        //_ERROR0C(hr, MCMSG("Cannot get buffer of asf object"));
    }

    // 24 bytes
    memcpy(pAsfObjectBuf, pBuf, cbRead); 

    // now read the rest of it, discounting the object header already read
    cbToRead = (ULONG) cbObject - cbRead;

    if (cbToRead > 0)
    {
        ret = m_pStream->Read(pAsfObjectBuf + cbRead, cbToRead, &cbRead);
        if (ret != RET_OK)
        {
            //_ERROR1C(hr, MCMSG("Cannot read (%d) from stream"), cbToRead);
        }

        if (cbRead < cbToRead)
        {
            //_TRACE0(LOGLEVEL5, TEXT("End of ASF stream"));
            return NULL;
        }
    }

    // refresh obj properties using new buffer data
    pObj->Refresh();

    FilterObject(*pObj);

    // success, let's Detach so we give caller a reference
    // so COM won't delete it.

    //pAsfObject->Detach();

    return pObj;
}

void ASFTransmissionStream::FilterObject(ASFObject& obj)
{
    if (obj.GetType() == ASFObject::MC_ASF_HEADER)
    {
        ASFHeader* pHeader = (ASFHeader*) &obj;

        m_wPacketSize = (WORD) pHeader->GetPacketSize();
        m_qwPreroll = pHeader->GetPreroll() + MC_ASF_DECODER_BUFFER_PLUS;

        m_ulHeaderRead += (ULONG) obj.GetObjectSize();

        if (pHeader->IsSeekable())
        {
            SetIndex(pHeader);
        }

        if (m_pDataFilter)
        {
            m_pDataFilter->OnHeader(pHeader->GetPointer(), (ULONG) obj.GetObjectSize(), pHeader->GetBitrate(), m_wPacketSize, m_qwPreroll);
        }
    }
    else if (obj.GetType() == ASFObject::MC_ASF_DATAOBJECT)
    {
        m_ulHeaderRead += (ULONG) obj.GetObjectSize();
    }
    else if (obj.GetType() == ASFObject::MC_ASF_DATAPACKET)
    {
        ASFDataPacket* pPacket = (ASFDataPacket*) &obj;

        DWORD dwPresentationTime = pPacket->GetPresentationTime();

        //_TRACE1(LOGLEVEL3, TEXT("Filtering packet with timestamp (%d)"), dwPresentationTime);

        if (m_pDataFilter)
        {
            m_pDataFilter->OnTimestamp(pPacket->GetPointer(), 
                                       (ULONG) pPacket->GetObjectSize(),
                                       m_lDataPacketSequence,
                                       MTYPE_MEDIA, 
                                       pPacket->HasKeyFrame() ? PKT_KEYFRAME : 0,
                                       dwPresentationTime);
        } 
    }
    else if (obj.GetType() == ASFObject::MC_ASF_INDEXOBJECT)
    {
        ASFIndexObject* pObj = (ASFIndexObject*) &obj;

        //_TRACE1(LOGLEVEL3, TEXT("Filtering index object with size (%d)"), (DWORD) pObj->GetObjectSize());

        if (m_pDataFilter)
        {
            m_pDataFilter->OnIndex(pObj->GetPointer(), (ULONG) pObj->GetObjectSize());
        }
    }
}

void ASFTransmissionStream::SetIndex(ASFHeader* pHeader)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFStream::GetIndex"));

    //_TRACE0(DsLOGLEVEL4 | DsLOG_IO, TEXT("ASF Stream is seekable, let's find simple index object"));

    unsigned ret;
    BYTE pBuf[24];
    ULONG cbRead = 0, cbToRead = 0;

    LARGE_INTEGER seekpos;
    ULARGE_INTEGER newpos;

    while (1)
    {        // read asf object header (guid + size = 24 bytes)
        cbToRead = sizeof(pBuf);

        ret = m_pStream->Read(pBuf, cbToRead, &cbRead);
        if (ret != RET_OK)
        {
            //_ERROR1C(hr, MCMSG("Cannot read (%d) from stream"), cbToRead);
        }

        if (cbRead >= cbToRead)
        {
            if (memcmp(pBuf, &ASFObject::ASF_Simple_Index_Object, sizeof(GUID_W)) == 0)
            {
                SAFE_DELETE(m_pIndex);

                m_pIndex = new ASFSimpleIndexObject();

                if (!m_pIndex)
                {
                    //_WARN0C(E_OUTOFMEMORY, DsCAT_ERROR, TEXT("Out of memory"));
                }
                
                break;
            }
            else
            {
                ASFObject obj;
                obj.Attach(pBuf);

                seekpos.QuadPart = obj.GetObjectSize() - cbRead;

                ret = m_pStream->Seek(seekpos, SEEK_CUR, &newpos);
                if (ret != RET_OK)
                {
                    //_WARN0(DsCAT_ERROR, TEXT("End of ASF stream, cannot find index"));
                }

                obj.Detach();
             }
        }
        else
        {
            //_WARN0(DsCAT_ERROR, TEXT("End of ASF stream, cannot find index"));
            break;
        }
    }

    if (m_pIndex)
    {
        m_pIndex->Attach(pBuf);
        QWORD cbObject = m_pIndex->GetObjectSize();

        if (cbObject >= cbRead)
        {
            // let's create the asf object according to object type
            // and put it in a CComPtr wrapper
            // its type will be INSSBuffer interface, so we are able to set length
            //CComPtr<INSSBuffer> pAsfObject = m_pIndex;

            INSSBuffer *pAsfObject = m_pIndex;

            // okay, now set the new asf object buffer to the size of asf object
            ret = pAsfObject->SetLength((ULONG) cbObject);
	
            if (ret == RET_OK)
            {
                // copy the object header from tmp buf
                // we have to get the pointer to it
                LPBYTE pAsfObjectBuf = NULL;

                ret = pAsfObject->GetBuffer(&pAsfObjectBuf);
				
                if (ret == RET_OK)
                {
                    // 24 bytes
                    memcpy(pAsfObjectBuf, pBuf, cbRead); 

                    // now read the rest of it, discounting the object header already read
                    cbToRead = (ULONG) cbObject - cbRead;

                    if (cbToRead > 0)
                    {
                        ret = m_pStream->Read(pAsfObjectBuf + cbRead, cbToRead, &cbRead);
                        if (ret != RET_OK)
                        {
                            //_WARN1C(hr, DsCAT_ERROR, TEXT("Cannot read (%d) from stream"), cbToRead);
                        }

                        if (cbRead < cbToRead)
                        {
                            //_WARN0(DsCAT_ERROR, TEXT("End of ASF stream, cannot find index"));
                        }
                    }

                    // refresh obj properties using new buffer data
                    m_pIndex->Refresh();

                    FilterObject(*m_pIndex);
                }
                else
                {
                    //_WARN0C(hr, DsCAT_ERROR, TEXT("Cannot get buffer of asf object"));
                }
            }
            else
            {
                //_WARN1C(hr, DsCAT_ERROR, TEXT("Cannot set object length (%I64d)"), m_pIndex->GetObjectSize());
            }

            // success, let's Detach so we give caller a reference
            // so COM won't delete it.
            pAsfObject->Detach();
        }
        else
        {
            //_WARN1C(hr, DsCAT_ERROR, TEXT("Invalid object size (%I64d)"), m_pIndex->GetObjectSize());
        }
    }

    seekpos.QuadPart = m_ulHeaderRead;

    ret = m_pStream->Seek(seekpos, SEEK_SET, &newpos);

    if (ret != RET_OK)
    {        //_WARN1C(hr, DsCAT_ERROR, TEXT("Cannot seek back to header position to (%I64d)"), seekpos.QuadPart);
    }
}
