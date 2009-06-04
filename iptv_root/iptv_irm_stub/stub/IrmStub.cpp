#include "IrmStub.h"

/////////////////////////////////////////////////////////////////////////////
// CIrmStub


static LPCSTR IRM_STUB_VERSION = "irmstub30.dll v3.3.01";


CIrmStub::CIrmStub(IrmStubDataSink *_pDataSink)
{
    flConnecting    = FALSE;
    flDisconnecting = FALSE;
    ConnectThread   = NULL;
    pSink           = _pDataSink;

    pChat = new cxProxyChat( (IMessageDataSink*) this ) ;
}


CIrmStub::~CIrmStub(void)
{
    Disconnect();

    if (pChat)
        delete pChat;
}


LPCSTR CIrmStub::GetVersion(void)
{
    return IRM_STUB_VERSION;
}


LPCSTR CIrmStub::GetChatVersion(void)
{
    LPCSTR pRet;

    if (pChat)
        pRet = pChat->GetVersion();
    else
        pRet = "";

    return pRet;
}


BOOL CIrmStub::GetClientIP(LPSTR _pIPStr, UINT _IPMaxLen)
{
    BOOL ret;

    ret = FALSE;

    if (pChat)
        ret = pChat->GetClientIP(_pIPStr, _IPMaxLen);

    return ret;
}


int CIrmStub::_DoConnect(IThread *pthread, void *_pCtx)
{
    CIrmStub *p = (CIrmStub *) _pCtx;

    if ( p->pChat->Connect(p->host, p->port, p->fluPnP, &(p->m_proxyparam)) )
    {
        IrmParam nullv;
        IrmParam vCmd  ((LPCSTR) "Connected to Host");

        p->pSink->Fire_OnMessage(CONNECTED_HOST, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, vCmd);

        p->pChat->ConnectIRC(p->nick, p->email, p->realname, p->cliversion, p->password);
    }
    else
    {
        IrmParam nullv;
        IrmParam vCmd  ((LPCSTR) "Unable to Connect");

        p->pSink->Fire_OnMessage( DISCONNECTED_CHAT, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
    }

    p->flConnecting = FALSE;

    return CONNECT_THREAD;
}


void CIrmStub::Terminate(void)
{
    pChat->Terminate();
}


void CIrmStub::AppPong(void)
{
    pChat->AppPong();
}

void CIrmStub::UDPPing(LPCSTR _ip, int _port, LPCSTR _data)
{
    pChat->UDPPing(_ip, _port, _data);
}

void CIrmStub::Disconnect(void)
{
    if (ConnectThread && !flDisconnecting)
    {
        flDisconnecting = TRUE;

        pChat->Disconnect();
        ConnectThread->WaitForMe(INFINITE);

        delete ConnectThread;

        ConnectThread   = NULL;
        flConnecting    = FALSE;
        flDisconnecting = FALSE;
    }
}


long CIrmStub::Connect(IrmParam& _host, IrmParam& _nick, IrmParam& _email, IrmParam& _name, IrmParam& _clientversion, IrmParam& _password, long _port, BOOL _flupnp, ProxyParam *_pproxyparam)
{
    long ret;
 
    LPCSTR lpHost,
           lpNick,
           lpEmail,
           lpName,
           lpCliVersion,
           lpPassword;
 
 
    if ( flDisconnecting )
        return FALSE;

    if ( pChat->IsConnected() || flConnecting )
        Disconnect();

    ret = FALSE;

    m_proxyparam.reset();

    lpHost       = _host.GetString();
    lpNick       = _nick.GetString();
    lpEmail      = _email.GetString();
    lpName       = _name.GetString();
    lpCliVersion = _clientversion.GetString();
    lpPassword   = _password.GetString();

    if (lpHost && lpNick && lpEmail && lpName && lpCliVersion && lpPassword && _port)
    {
        ajusta_str( lpHost       , host       , HOSTLEN  );
        ajusta_str( lpNick       , nick       , NICKLEN  );
        ajusta_str( lpEmail      , email      , EMAILLEN );
        ajusta_str( lpName       , realname   , REALLEN  );
        ajusta_str( lpCliVersion , cliversion , VERSIONLEN );
        ajusta_str( lpPassword   , password   , PASSWDLEN );

        port   = _port;
        fluPnP = _flupnp;

        if ( _pproxyparam && _pproxyparam->IsRdy() )
            m_proxyparam.Copy( *_pproxyparam );
 
        ConnectThread = CreateIThread( _DoConnect, (CIrmStub*) this, TRUE );

        if (ConnectThread)
        {
            flConnecting = TRUE;
            ret = TRUE;
        }
    }
 
    return ret;
}

BOOL CIrmStub::InitViewer(int _rcvPort, IrmParam& _MulticastIP)
{
    BOOL ret;
    LPCSTR lpMcastIP;

    ret = FALSE;

    if (pChat)
    {
        lpMcastIP = _MulticastIP.GetString();

        if (lpMcastIP)
            ret = pChat->InitViewer(_rcvPort, lpMcastIP);
    }

    return ret;
}

void CIrmStub::CloseUDP(void)
{
    if (pChat)
        pChat->CloseUDP();
}

void CIrmStub::CloseViewer(void)
{
    if (pChat)
        pChat->CloseViewer();
}

void CIrmStub::OnMessage(MessageData *lpMd)
{
    long MediaID,
         port;


    IrmParam nullv;

    if (lpMd)
    {
        IrmParam vSource( (LPCSTR) lpMd->lpszSource  );
        IrmParam vTarget( (LPCSTR) lpMd->lpszTarget  );
        IrmParam vCmd   ( (LPCSTR) lpMd->lpszFullCmd );

        switch( lpMd->nOpCode )
        {
            case CONNECTED_CHAT:
            case DISCONNECTED_CHAT:
            case PING_CHAT:
            case APP_PING:
            case OPER:
            case UDP_PONG:
            case MEDIAQUERYEND:
            case ENDOF_CHANNEL_LIST:
            case ENDOF_WHOIS:
            case NOWAWAY:
            case UNAWAY:
                pSink->Fire_OnMessage( lpMd->nOpCode, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;

            // nOpCode - lpszSource - NULL - NULL
            case REGISTER:
            case IDENTIFY:
            case IDENTIFY_REQUEST:
            case CONTACT_LIST_END:
                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;

            // nOpCode - lpszSource - lpszTarget - NULL
            case CHANGE_NICK:
            case JOIN_CHANNEL:
            case PART_CHANNEL:
            case QUIT_CHAT:
            case MEDIAVOICE:
            case UDP_PING:
            case MEDIASTATS:
                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, nullv, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;

            // nOpCode - lpszSource - lpszTarget
            case CONTACT_LIST:
            case CONTACT_ON:
            case CONTACT_OFF:
            case CONTACT_ADD:
            case CONTACT_DELETE:
                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, nullv, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;

            // nOpCode - lpszSource - NULL - LPSTR
            case REGISTER_ERROR:
            case CONTACT_ERROR:
            case IDENTIFY_ERROR:
            case WHOIS:
            {
                IrmParam vData( (LPCSTR) ((cxString *) lpMd->lParam)->GetStr() );
                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, nullv, vData, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;
            }

            // nOpCode - lpszSource - lpszTarget - LPSTR
            case CHANGE_CHANNEL_TOPIC:
            case PRIVMSG:
            case PRIVMSG_CHANNEL:
            case WBOARD:
            case WBOARD_CHANNEL:
            case APPCMD:
            case CMD_ERROR:
            case KICK_USER:
            case MODE:
            case TOPIC_WHO:
            case INVITE:
            case PASSWORD:
            case PROTOCOL:
            {
                IrmParam vData( (LPCSTR) ((cxString *) lpMd->lParam)->GetStr() );
                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, vData, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;
            }

            case GET_CHANNEL_LIST:
            {
                char *c;
                int g = 0, nSize;
                //IrmParam vData( (LPCSTR) ((cxString *) lpMd->lParam)->GetStr() );

                c = (LPSTR) ((cxString *) lpMd->lParam)->GetStr( );
                nSize = strlen( c );

                while( ( c[g] ) && ( c[g] != ' ' ) )
                    g++;

                if( g < nSize )
                {
                    c[g] = 0;
                    IrmParam vnUsers( c );                    
                    IrmParam vData( (LPCSTR) &c[g+1] );
                    pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, vData, vnUsers, nullv, nullv, nullv, nullv, nullv, vCmd );
                    c[g] = ' ';
                }
                else
                {
                    IrmParam vnUsers( "0" );
                    IrmParam vData( (LPCSTR) ((cxString *) lpMd->lParam)->GetStr() );
                    pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, vData, vnUsers, nullv, nullv, nullv, nullv, nullv, vCmd );
                }

                break;
            }

            case GET_USER_LIST:
            {
                Expr userlist( (LPCSTR) ((cxString *) lpMd->lParam)->GetStr() );

                userlist.set_delimiters("");

                //while ( userlist.get_token_expr() )
                //while ( userlist.get_next_token() == EX_TOKEN )
                while ( userlist.get_next_string(",; ") == EX_ALFA )
                {
                    IrmParam vData( (LPCSTR) userlist.token );
                   
                    pSink->Fire_OnMessage( lpMd->nOpCode, vSource, nullv, vData, nullv, nullv, nullv, nullv, nullv, nullv, nullv );
                }

                pSink->Fire_OnMessage( ENDOF_USER_LIST, vSource, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv );

                break;
            }

            case SERVER_REDIR:
            {
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                port = vr->GetMediaPort();
                IrmParam vPort ( port );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, vPort, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;
            }

            case MEDIASNDSTOP:
            case MEDIARESET:
            case MEDIARCVSTOP:
            case MEDIARCVRUN:
            {
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID = vr->GetMediaID();
                IrmParam varMediaID( MediaID );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, varMediaID, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );

                break;
            }

            case MEDIASETPOS:
            {
                ULONG seq;

                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID = vr->GetMediaID();
                IrmParam varMediaID( MediaID );

                seq = vr->GetData1();
                IrmParam varSeq( seq );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, varMediaID, varSeq, nullv, nullv, nullv, nullv, nullv, vCmd );

                break;
            }

            case MEDIAPKTREQUEST:
            {
                ULONG seq;

                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID = vr->GetMediaID();
                IrmParam varMediaID( MediaID );

                seq = vr->GetData1();
                IrmParam varSeqi( seq );

                seq = vr->GetData2();
                IrmParam varSeqf( seq );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, varMediaID, varSeqi, varSeqf, nullv, nullv, nullv, nullv, vCmd );

                break;
            }

            case MEDIARCVALLSTOP:
            case MEDIARCVALLRUN:
            {
                MediaRequest *vr = (MediaRequest *) lpMd->lParam;

                IrmParam vChannel      ( (LPCSTR) vr->GetChannel( )    );
                IrmParam vMediaClassStr( (LPCSTR) vr->GetMediaClass( ) );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, vChannel, vMediaClassStr, nullv, nullv, nullv, nullv, nullv, vCmd );

                break;
            }

            case MEDIARCV:
            {
                long pktlen,
                     headerlen;

                // MEDIARCVACK nick ID PacketLen Ip Port $HeaderLen HEADER
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID   = vr->GetMediaID();
                IrmParam varMediaID ( MediaID );
                pktlen    = vr->GetPacketLen();
                IrmParam vPacketLen ( pktlen );
                IrmParam vIp( (LPCSTR) vr->GetIp() );
                port      = vr->GetMediaPort();
                IrmParam vMediaPort ( port );

                headerlen = vr->GetHeaderLen();


                if (headerlen && vr->GetHeaderBuffer())
                {
                    IrmParam vHeaderLen ( headerlen );
                    IrmParam vHeader    ( (LPCSTR) vr->GetHeaderBuffer() );

                    pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vIp, varMediaID, vPacketLen, vMediaPort, vHeaderLen, vHeader, nullv, nullv, vCmd );
                }
                else
                {
                    headerlen = 0;
                    IrmParam vHeaderLen ( headerlen );

                    pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vIp, varMediaID, vPacketLen, vMediaPort, vHeaderLen, nullv, nullv, nullv, vCmd );
                }

                break;
            }

            case MEDIARCVALL:
            {
                // MEDIARCVALLACK nick #Channel MediaClassStr MulticastIp Port
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                IrmParam vChannel   ( (LPCSTR) vr->GetChannel() );
                IrmParam vMediaClass( (LPCSTR) vr->GetMediaClass() );
                IrmParam vIp        ( (LPCSTR) vr->GetIp() );
                port      = vr->GetMediaPort();
                IrmParam vMediaPort ( port );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, nullv, vChannel, vMediaClass, vMediaPort, vIp, nullv, nullv, nullv, vCmd );

                break;
            }

            case MEDIASND:
            {
                // MEDIASNDACK nick #Channel StrID ID MediaPort :MEDIASND
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID = vr->GetMediaID();
                IrmParam varMediaID ( MediaID );
                IrmParam vChannel   ( (LPCSTR) vr->GetChannel   () );
                IrmParam vMediaClass( (LPCSTR) vr->GetMediaClass() );

                port = vr->GetMediaPort();
                IrmParam vMediaPort ( port );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, nullv, varMediaID, vChannel, vMediaClass, vMediaPort, nullv, nullv, nullv, vCmd );

                break;
            }

            case MEDIA:
            {
                // :srvname MEDIA MediaID nickname #Channel MediaClassStr Bitrate ip port headersize nheadersegs maxsegsize headerchksum

                ULONG bitrate;
                long  headerlen;

                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID   = vr->GetMediaID();
                IrmParam varMediaID ( MediaID );
                IrmParam vOwner     ( (LPCSTR) vr->GetNickname  () );
                IrmParam vChannel   ( (LPCSTR) vr->GetChannel   () );
                IrmParam vMediaClass( (LPCSTR) vr->GetMediaClass() );
                bitrate = vr->GetBitrate();
                IrmParam varBitrate ( bitrate );

                IrmParam vIp( (LPCSTR) vr->GetIp() );
                port      = vr->GetMediaPort();
                IrmParam vMediaPort ( port );

                headerlen = vr->GetHeaderLen();

                if (headerlen && vr->GetHeaderBuffer())
                {
                    IrmParam vHeaderLen ( headerlen );
                    IrmParam vHeader    ( (LPCSTR) vr->GetHeaderBuffer() );

                    pSink->Fire_OnMessage( lpMd->nOpCode, vOwner, vChannel, varMediaID, vMediaClass, vIp, vMediaPort, varBitrate, vHeaderLen, vHeader, vCmd );
                }
                else
                {
                    headerlen = 0;
                    IrmParam vHeaderLen ( headerlen );

                    pSink->Fire_OnMessage( lpMd->nOpCode, vOwner, vChannel, varMediaID, vMediaClass, vIp, vMediaPort, varBitrate, vHeaderLen, nullv, vCmd );
                }

                break;
            }

            case MEDIASET:
            {
                // MEDIASET nickname MediaID IDENT :Data
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID = vr->GetMediaID();
                IrmParam varMediaID( MediaID );
                IrmParam vOption   ( (LPCSTR) vr->GetMediaSetOption() );
                IrmParam vName     ( (LPCSTR) vr->GetName          () );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, nullv, varMediaID, vOption, vName, nullv, nullv, nullv, nullv, vCmd );

                break;
            }

            case MEDIAMUTE:
            {
                // MEDIAMUTE MediaID [ON|OFF]
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID = vr->GetMediaID();
                IrmParam varMediaID ( MediaID );
                IrmParam vNotify    ( (LPCSTR) vr->GetMediaNotify() );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, nullv, varMediaID, vNotify, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;
            }

            case MEDIANOTIFY:
            {
                // MEDIANOTIFY nickowner MediaID MediaClassStr #Channel NOTIFY_CMD
                MediaRequest *vr = (MediaRequest *)lpMd->lParam;

                MediaID = vr->GetMediaID();
                IrmParam varMediaID ( MediaID );
                IrmParam vOwner     ( (LPCSTR) vr->GetNickname()    );
                IrmParam vNotify    ( (LPCSTR) vr->GetMediaNotify() );
                IrmParam vMediaClass( (LPCSTR) vr->GetMediaClass()  );
                IrmParam vChannel   ( (LPCSTR) vr->GetChannel()     );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vOwner, varMediaID, vNotify, vMediaClass, vChannel, nullv, nullv, nullv, vCmd );
                break;
            }

            case MEDIAQUERY:
            {
                MediaQuery *vq = (MediaQuery *)lpMd->lParam;

                MediaID = vq->GetMediaID();
                IrmParam varMediaID ( MediaID );
                IrmParam vOwner     ( (LPCSTR) vq->GetOwner()      );
                IrmParam vChannel   ( (LPCSTR) vq->GetChannel   () );
                IrmParam vMediaClass( (LPCSTR) vq->GetMediaClass() );
                IrmParam vMediaName ( (LPCSTR) vq->GetMediaName () );
                IrmParam vBitrate   ( (long)   vq->GetBitrate   () );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vOwner, varMediaID, vChannel, vMediaClass, vMediaName, vBitrate, nullv, nullv, vCmd );
                break;
            }

            case MEDIAPTPINVITE:
            case MEDIAPTPACK:
            {
                MediaPtP *vptp = (MediaPtP *) lpMd->lParam;

                IrmParam vTarget    ( (LPCSTR) vptp->GetNickTarget() );
                IrmParam vMediaClass( (LPCSTR) vptp->GetMediaClass() );
                IrmParam vIPLocalSrc( (LPCSTR) vptp->GetIPLocalSource() );
                IrmParam vIPSrvSrc  ( (LPCSTR) vptp->GetIPSrvSource() );
                IrmParam portRcvSrc        ( vptp->GetRcvSourcePort() );
                IrmParam portSndSrc        ( vptp->GetSendSourcePort() );
                IrmParam portRcvUDPPingSrc ( vptp->GetUDPPingSourcePort() );
                IrmParam bitrate           ( vptp->GetSourceBitrate() );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, vMediaClass, vIPLocalSrc, vIPSrvSrc, portRcvSrc, portSndSrc, portRcvUDPPingSrc, bitrate, vCmd );

                break;
            }

            case MEDIAPTPREJECT:
            {
                MediaPtP *vptp = (MediaPtP *) lpMd->lParam;

                IrmParam vTarget    ( (LPCSTR) vptp->GetNickTarget() );
                IrmParam vMediaClass( (LPCSTR) vptp->GetMediaClass() );

                pSink->Fire_OnMessage( lpMd->nOpCode, vSource, vTarget, vMediaClass, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );

                break;
            }

            case SERVER_MSG:
                pSink->Fire_OnMessage( lpMd->nOpCode, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, nullv, vCmd );
                break;
        }
    }
}


long CIrmStub::SendMessage(long _cmd, IrmParam& _target, IrmParam& _data, IrmParam& _compl, IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3, IrmParam& _compl4)
{
    long   ret;

    LPCSTR pTarget,
           pData,
           pProtocol,
           pOption,
           pValue,
           pMediaClass,
           pHeader;

    char   buf[TAM_BUFFER];


    if (!_cmd)
        return FALSE;

    ret = FALSE;

    switch (_cmd)
    {
        // CMD nick
        // CMD #channel
        case CHANGE_NICK:
        case PART_CHANNEL:
        case QUIT_CHAT:
        case GET_USER_LIST:
        case USER_COMMAND:
        case CONTACT_ADD:
        case CONTACT_DELETE:
        case IDENTIFY:
        case REGISTER:
        case PROTOCOL:
        {
            pTarget = _target.GetString();

            if (pTarget)
                ret = pChat->SendMessage(_cmd, (long) pTarget);

            break;
        }

        // CMD nick data
        // CMD #channel data
        case MEDIAVOICE:
        case CHANGE_CHANNEL_TOPIC:
        case PRIVMSG:
        case PRIVMSG_CHANNEL:
        case NOTICE:
        case WBOARD:
        case WBOARD_CHANNEL:
        case APPCMD:
        case CMD_ERROR:
        case KICK_USER:
        case MODE:
        case OPER:
        case INVITE:
        case JOIN_CHANNEL:
        {
            pTarget = _target.GetString();
            pData   = _data.GetString();

            if (pTarget && pData)
            {
                sprintf(buf, "%s %s", pTarget, pData);

                ret = pChat->SendMessage(_cmd, (long) buf);
            }

            break;
        }

        case MEDIASTATS:
        {
            if ( _target.GetValue() )
            {
                pData = _data.GetString();

                if ( !comp_branco(pData) )
                    sprintf(buf, "%d %s", _target.GetValue(), pData);
                else
                    sprintf(buf, "%d", _target.GetValue());

                ret = pChat->SendMessage(_cmd, (long) buf);
            }

            break;
        }

        // CMD [param]
        case GET_CHANNEL_LIST:
        {
            pTarget = _target.GetString();

            if ( comp_branco(pTarget) )
                pTarget = NULL;

            ret = pChat->SendMessage(_cmd, (long) pTarget);

            break;
        }

        // lParam = NULL;
        case MEDIAQUERY:
        case CONTACT_LIST:
        {
            ret = pChat->SendMessage(_cmd, NULL);

            break;
        }

        // CMD MediaId
        case MEDIASNDSTOP:
        case MEDIARESET:
        case MEDIARCVSTOP:
        case MEDIARCVRUN:
        {
            if (_target.GetValue())
            {
                MediaRequest mr;

                mr.SetMediaID(_target.GetValue());

                ret = pChat->SendMessage(_cmd, (long) &mr);
            }

            break;
        }

        case MEDIAPKTREQUEST:
        {
            if (_target.GetValue())
            {
                MediaRequest mr;

                mr.SetMediaID(_target.GetValue());
                mr.SetData1  (_data.GetValue());
                mr.SetData2  (_compl.GetValue());

                ret = pChat->SendMessage(_cmd, (long) &mr);
            }

            break;
        }

        case MEDIAMUTE:
        {
            pData = _data.GetString( );

            if (_target.GetValue() && pData)
            {
                MediaRequest mr;

                mr.SetMediaID(_target.GetValue());
                mr.SetMediaNotify( (LPSTR) pData );

                ret = pChat->SendMessage(_cmd, (long) &mr);
            }

            break;
        }

        // CMD #Channel MediaClassStr
        case MEDIARCVALLSTOP:
        case MEDIARCVALLRUN:
        {
            pTarget = _target.GetString( );
            pData   = _data.GetString( );

            if( pTarget && pData )
            {
                MediaRequest mr;

                mr.SetChannel(    (LPSTR) pTarget );
                mr.SetMediaClass( (LPSTR) pData   );

                ret = pChat->SendMessage( _cmd, (long) &mr );
            }

            break;
        }

        //MEDIARCV ID Protocol [Port]
        case MEDIARCV:
        {
            pProtocol = _data.GetString();

            if (_target.GetValue() && pProtocol)
            {
                MediaRequest mr;

                mr.SetMediaID  (_target.GetValue());
                mr.SetProtocol ((LPSTR)pProtocol);

                // port
                if (_compl.GetValue())
                    mr.SetMediaPort(_compl.GetValue());

                ret = pChat->SendMessage(_cmd, (long) &mr);
            }

            break;
        }

        // MEDIARCVALL #Channel MediaClassStr Protocol [Port]
        case MEDIARCVALL:
        {
            pTarget     = _target.GetString();
            pMediaClass = _data.GetString();
            pProtocol   = _compl.GetString();

            if (pTarget && pMediaClass && pProtocol)
            {
                MediaRequest mr;

                mr.SetChannel   ((LPSTR) pTarget);
                mr.SetMediaClass((LPSTR) pMediaClass);
                mr.SetProtocol  ((LPSTR) pProtocol);

                // port
                if (_compl1.GetValue())
                {
                    mr.SetMediaPort(_compl1.GetValue());
                }

                ret = pChat->SendMessage(_cmd, (long) &mr);
            }

            break;
        }

        // MEDIASET MediaID Option :value
        case MEDIASET:
        {
            pOption = _data.GetString();
            pValue  = _compl.GetString();

            if (_target.GetValue() && pOption && pValue)
            {
                MediaRequest mr;

                mr.SetMediaID       (_target.GetValue());
                mr.SetMediaSetOption((LPSTR) pOption);
                mr.SetName          ((LPSTR) pValue);

                ret = pChat->SendMessage(_cmd, (long) &mr);
            }

            break;
        }

        // MEDIASND #Channel StrID Protocol PacketLen [$HeaderLen HEADER]
        case MEDIASND:
        {
            pTarget     = _target.GetString();
            pMediaClass = _data.GetString();
            pProtocol   = _compl.GetString();

            if (pTarget && pMediaClass && pProtocol && _compl1.GetValue())
            {
                MediaRequest mr;

                IrmParam cPacketLen (_compl1);

                mr.SetChannel   ((LPSTR) pTarget);
                mr.SetMediaClass((LPSTR) pMediaClass);
                mr.SetProtocol  ((LPSTR) pProtocol);
                mr.SetPacketLen (_compl1.GetValue());

                ret = TRUE;

                // headerlen && header

                pHeader = _compl3.GetString();

                if (_compl2.GetValue() && pHeader)
                {
                    // headerlen
                    if (_compl2.GetValue())
                    {
                        if (strlen(pHeader) == (UINT) _compl2.GetValue())
                        {
                            mr.SetHeaderLen(_compl2.GetValue());
                            mr.SetHeader   ((LPSTR) pHeader, _compl2.GetValue());
                        }
                        else
                            ret = FALSE;
                    }
                }

                if (ret)
                    ret = pChat->SendMessage(_cmd, (long) &mr);
            }

            break;
        }

        //MEDIAPTPINVITE NickTarget mediaclassStr IPLocalSource portRcvSource
        //portSndSource portRcvUDPPingSource bitrateSource
        //MEDIAPTPACK NickTarget mediaclassStr IPLocalSource portRcvSource
        //portSndSource portRcvUDPPingSource bitrateSource

        case MEDIAPTPINVITE:
        case MEDIAPTPACK:
        {
            LPCSTR pIPLocalSrc;

            pTarget     = _target.GetString();
            pMediaClass = _data.GetString();
            pIPLocalSrc = _compl.GetString();

            if ( pTarget && pMediaClass && pIPLocalSrc &&
                 _compl1.GetValue() && _compl2.GetValue() && _compl3.GetValue() && _compl4.GetValue() )
            {
                MediaPtP mptp;

                mptp.SetNickTarget       ((LPSTR) pTarget);
                mptp.SetMediaClass       ((LPSTR) pMediaClass);
                mptp.SetIPLocalSource    ((LPSTR) pIPLocalSrc);
                mptp.SetRcvSourcePort    (_compl1.GetValue());
                mptp.SetSendSourcePort   (_compl2.GetValue());
                mptp.SetUDPPingSourcePort(_compl3.GetValue());
                mptp.SetSourceBitrate    (_compl4.GetValue());

                ret = pChat->SendMessage(_cmd, (long) &mptp);
            }

            break;
        }

        //MEDIAPTPREJECT NickTarget mediaclassStr
        case MEDIAPTPREJECT:
        {
            pTarget     = _target.GetString();
            pMediaClass = _data.GetString();

            if (pTarget && pMediaClass)
            {
                MediaPtP mptp;

                mptp.SetNickTarget((LPSTR) pTarget);
                mptp.SetMediaClass((LPSTR) pMediaClass);

                ret = pChat->SendMessage(_cmd, (long) &mptp);
            }

            break;
        }
    }

	return ret;
}
