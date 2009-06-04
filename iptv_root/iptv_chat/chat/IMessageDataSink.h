// IMessageDataSink.h: interface for the IMessageDataSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMESSAGEDATASINK_H__1A027A3E_547C_416E_BFD8_4E3DFB476EC2__INCLUDED_)
#define AFX_IMESSAGEDATASINK_H__1A027A3E_547C_416E_BFD8_4E3DFB476EC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MessageData;

class IMessageDataSink  
{
public:
#ifdef _LINUX
    IMessageDataSink();
    virtual ~IMessageDataSink();
#endif
    virtual void OnMessage(MessageData* lpmd) = 0;
};

#endif // !defined(AFX_IMESSAGEDATASINK_H__1A027A3E_547C_416E_BFD8_4E3DFB476EC2__INCLUDED_)
