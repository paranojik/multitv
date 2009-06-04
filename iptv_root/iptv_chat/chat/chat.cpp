// chat.cpp : Defines the entry point for the DLL application.
//
#include "shared/compat.h"

#define _WIN32_DCOM

#define  SOURCE_STRING "Chat"
#include "shared/debug.h"

//#include <stdio.h>
//#include <stdarg.h>
#include <time.h>

#include "shared/expr.h"
#include "shared/hash_cmd.h"
#include "shared/common.h"
#include "shared/socket/proxy.h"
#include "shared/socket/http-tunnel.h"
#include "shared/socket/upnp.h"
#include "shared/irm/cmd.h"

#include "IMessageDataSink.h"

#include "chat.h"

#define NICKSERV    "NICKSERV"

static cxChat *ThiscxChat      = NULL;
static BOOL    flThreadExit    = FALSE;
static BOOL    flThreadCmdExit = FALSE;
static BOOL    flUDPThreadExit = FALSE;

ExprParser   *expr        = NULL;
ExprParser   *exprContact = NULL;
MediaRequest *pViewerCtrl = NULL;


static LPCSTR CHAT_DLL_VERSION = "chat.dll v3.11.10.51";

#define SOCK_CMDCTRL_TXT_BUFSIZE  16384
#define NMAX_MSG_QUEUE            65536
#define UDPPING_PORT               8001
#define UDP_IRM_MEDIA_PORT         8006
#define UDP_IRM_AVCONF_PORT        8009
#define UDPPONG_PORT               8008
#define UDPPING_STR               "abcde"
#define APPPING_MAXWAITTIME          60
#define APPPING_FREQ                 30
#define APPPING_SLEEPTIME          1000
#define MAXIDLETIME                 330
#define MAXPONGTIME                  60
#define MAXCMDSENTTIME               60
#define MAXCONNTIME                  20
#define NMAXPARAM_PARSER             15

#define TMAX_ONOFF      3

#define S_SRVCMD_CONTACT_ERROR      "CONTACT_ERROR"
#define S_SRVCMD_CONTACT_LIST       "CONTACT_LIST"
#define S_SRVCMD_CONTACT_ADD        "CONTACT_ADD"
#define S_SRVCMD_CONTACT_DELETE     "CONTACT_DELETE"
#define S_SRVCMD_CONTACT_LIST_END   "CONTACT_LIST_END"
#define S_SRVCMD_CONTACT            "CONTACT"
#define S_SRVCMD_IDENTIFY_ERROR     "IDENTIFY_ERROR"
#define S_SRVCMD_IDENTIFY_REQUEST   "IDENTIFY_REQUEST"
#define S_SRVCMD_IDENTIFY           "IDENTIFY"
#define S_SRVCMD_REGISTER_ERROR     "REGISTER_ERROR"
#define S_SRVCMD_REGISTER           "REGISTER"
#define S_SRVCMD_CONTACT_ON         "ON"
#define S_SRVCMD_CONTACT_OFF        "OFF"


enum _SERVICES_CMD
{
    S_NULL              =  0,
    S_CONTACT_ERROR     =  1,
    S_CONTACT_LIST      =  2,
    S_CONTACT_ADD       =  3,
    S_CONTACT_DELETE    =  4,
    S_CONTACT_LIST_END  =  5,
    S_CONTACT           =  6,
    S_IDENTIFY_ERROR    =  7,
    S_IDENTIFY_REQUEST  =  8,
    S_IDENTIFY          =  9,
    S_REGISTER_ERROR    = 10,
    S_REGISTER          = 11,
};


Cmd_str servicecmd_vet[] =
{
    { S_SRVCMD_CONTACT_ERROR     , S_CONTACT_ERROR      },
    { S_SRVCMD_CONTACT_LIST      , S_CONTACT_LIST       },
    { S_SRVCMD_CONTACT_ADD       , S_CONTACT_ADD        },
    { S_SRVCMD_CONTACT_DELETE    , S_CONTACT_DELETE     },
    { S_SRVCMD_CONTACT_LIST_END  , S_CONTACT_LIST_END   },
    { S_SRVCMD_CONTACT           , S_CONTACT            },
    { S_SRVCMD_IDENTIFY_ERROR    , S_IDENTIFY_ERROR     },
    { S_SRVCMD_IDENTIFY_REQUEST  , S_IDENTIFY_REQUEST   },
    { S_SRVCMD_IDENTIFY          , S_IDENTIFY           },
    { S_SRVCMD_REGISTER_ERROR    , S_REGISTER_ERROR     },
    { S_SRVCMD_REGISTER          , S_REGISTER           },
    { ""                         , S_NULL               },
};


void Terminate(void)
{
#ifdef _WIN32
    HANDLE hApp;
    

    _TRACEL(LOGLEVEL1, "Terminate Application requested!");

    hApp = GetCurrentProcess();

    TerminateProcess(hApp, 0);

    //_execl("iptv.exe", "iptv.exe", NULL);
#endif
#ifdef _LINUX
    exit(0);
#endif
}


void CloseCallback(SocketCtrl *_psock, SOCKET _ssock, LPCSTR _pCloseMsg)
{
    ThiscxChat->RemoveuPnP();

    ThiscxChat->AddMessage  (DISCONNECTED_CHAT, NULL, NULL, NULL, "Connection to ELCC Server lost");
    ThiscxChat->SetConnected(FALSE);
}


BOOL RecvCallback( SocketCtrl *pSock, SOCKET ssock, char *buf, unsigned int blen )
{
    BOOL   ret,
           flCmdOk;

    UINT   nParam,
           lastCmd,
           nIRCCmd,
           lastServicesCmd,
           nContactParam;

    int    bitrate;

    ULONG  mediaID,
           port,
           packetlen,
           headerlen;

    LPCSTR pParam;
    char   pType;

    char   nick      [NICKLEN+1],
           who       [NICKLEN+1],
           user      [USERLEN+1],
//               email     [HOSTLEN+1],
           host      [HOSTLEN+1],
//               server    [HOSTLEN+1],
           channel   [CHANNELLEN+2],
           topic     [TOPICLEN+1],
           ip        [IPSTR_MAXLEN+1],
           msg       [TMAX_MSG+1],
           msg1      [TMAX_MSG+1],
           mediaclass[MEDIACLASSLEN+1];
            //lpBase64[TAM_LBUFFER+1];
//               protocol  [PROTOCOLLEN+1],

    char ContactBuf[TAM_LBUFFER-1];
    int  bContactLen = TAM_BUFFER;

    MediaRequest *vr;
    MediaQuery   *vq;
    MediaPtP     *vp;

    //ExprParser expr, exprContact;


    ret = TRUE;

    if( blen < TAM_LBUFFER )
        buf[blen] = '\0';
    else
        buf[TAM_LBUFFER-1] = '\0';

    _TRACE2L( LOGLEVEL1, "Mensagem:[%d] %s", blen, buf );

    //tratar o buf, parsing, etc
    //if( ThiscxChat->flConnected )
    {
        if (!buf || !blen)
            return TRUE;

        ret = TRUE;

        if (blen >= TAM_LBUFFER)
        {
            return ret;
        }
        else
            buf[blen++] = '\0';

        if( blen && buf )
        {
            ThiscxChat->UpdateLastCmdTime();

            nParam = expr->parse(buf, blen);

            if (nParam)
            {
                flCmdOk = FALSE;

                if( buf[0] == ':' )
                    pParam  = expr->GetParam( 1 );
                else
                    pParam  = expr->GetParam( 0 );

                lastCmd = ThiscxChat->pHashCmd->search( pParam );

                // Conversao de mensagens padrao IRC para padrao jbj;
                if( !lastCmd && pParam )
                {
                    nIRCCmd = lastCmd = atoi( pParam );
                    switch( lastCmd )
                    {
                        // Informa que a conexao com o host foi estabelecida
                        case RPL_YOURHOST:
                            lastCmd = C_CONNECT;
                            break;

                        case RPL_TOPIC:
                            lastCmd = C_TOPIC;
                            break;

                        case RPL_TOPICWHOTIME:
                            lastCmd = C_TOPICWHO;
                            break;

                        case RPL_NAMREPLY:
                        case RPL_ENDOFNAMES:
                            lastCmd = C_NAMES;
                            break;

                        //case RPL_LISTSTART:
                        case RPL_LIST:
                        case RPL_LISTEND:
                            lastCmd = C_LIST;
                            break;

                        case RPL_MEDIASNDACK:
                            lastCmd = C_MEDIASND;
                            break;

                        case RPL_MEDIARCVACK:
                            lastCmd = C_MEDIARCV;
                            break;

                        case RPL_MEDIASNDSTOP:
                            lastCmd = C_MEDIASNDSTOP;
                            break;

                        case RPL_MEDIARCVRUN:
                            lastCmd = C_MEDIARCVRUN;
                            break;

                        case RPL_MEDIARCVSTOP:
                            lastCmd = C_MEDIARCVSTOP;
                            break;

                        case RPL_MEDIASET:
                            lastCmd = C_MEDIASET;
                            break;

                        case RPL_ENDOFMEDIAQUERY:
                        case RPL_MEDIAQUERY:
                            lastCmd = C_MEDIAQUERY;
                            break;

                        case RPL_STATS:
                            lastCmd = C_MEDIASTATS;
                            break;

                        case RPL_MEDIARCVALLACK:
                            lastCmd = C_MEDIARCVALL;
                            break;

                        case RPL_MEDIARCVALLSTOP:
                            lastCmd = C_MEDIARCVALLSTOP;
                            break;

                        case RPL_MEDIARCVALLRUN:
                            lastCmd = C_MEDIARCVALLRUN;
                            break;

                        case RPL_MEDIAVOICE:
                            lastCmd = C_MEDIAVOICE;
                            break;

                        case RPL_YOUREOPER:
                            lastCmd = C_OPER;
                            break;

                        case RPL_SRVREDIR:
                            lastCmd = C_SERVERREDIR;
                            break;

                        case RPL_CHANNELMODEIS:
                            lastCmd = C_MODE;
                            break;

                        case RPL_PROTOCOL:
                            lastCmd = C_PROTOCOL;
                            break;

                        case RPL_WHOISREGNICK:
                        case RPL_WHOISADMIN:
                        case RPL_WHOISSADMIN:
                        case RPL_WHOISSVCMSG:
                        case RPL_WHOISUSER:
                        case RPL_WHOISSERVER:
                        case RPL_WHOISOPERATOR:
                        case RPL_ENDOFWHOIS:
                            lastCmd = C_WHOIS;
                        break;

                        case ERR_ERRONEUSNICKNAME:
                        case ERR_ERRONEUSUSERNAME:
                        case ERR_SERVERFULL:
                        case ERR_NICKNAMEINUSE:
                        //case ERR_NOMOTD:
                        case ERR_INVALIDCLIVERSION:
                        case ERR_NOSUCHNICK:
                        case ERR_NOSUCHCHANNEL:
                        case ERR_CHANNELISFULL:
                        case ERR_NOSUCHSERVER:
                        case ERR_USERNOTINCHANNEL:
                        case ERR_NOPERMFORHOST:
                        case ERR_YOURENOTOPER:
                        case ERR_YOURENOTCHANOP:
                        case ERR_NOPRIVILEGES:
                        case ERR_CANTCHGUSRMODES:
                        case ERR_BANNEDFROMCHAN:
                        case ERR_INVITEONLYCHAN:
                        case ERR_BADCHANNELKEY:
                        case ERR_TOOMANYCHANNELS:
                        case ERR_CANNOTSENDTOCHAN:
                        case RPL_MEDIAERROR:
                        case ERR_NEEDREGGEDNICK:
                            lastCmd = C_CMDERROR;
                        break;
                    }
                }

                switch( lastCmd )
                {
                    case C_CONNECT:
                        ThiscxChat->AddMessage( CONNECTED_CHAT, NULL, NULL, NULL, buf );
                    break;

                    case RPL_NOWAWAY:
                        ThiscxChat->AddMessage( NOWAWAY, NULL, NULL, NULL, buf );
                    break;

                    case RPL_UNAWAY:
                        ThiscxChat->AddMessage( UNAWAY, NULL, NULL, NULL, buf );
                    break;

                    case C_PING:
                        // Notifica ao Kernel a ocorrencia de PING (server to client)
                        //ThiscxChat->AddMessage( PING_CHAT, NULL, NULL, NULL, buf );

                        pParam = expr->GetParam( 1, &pType );
                        ThiscxChat->SendMessage( PONG_CHAT, (long)pParam );
                    break;

                    case C_UDPPING:
                        if (nParam > 2)
                        {
                            // UDPPING IPTarget :NickSrc
                            pParam = expr->GetParam( 2, &pType );
                            ajusta_str( pParam, nick, NICKLEN );
                            pParam = expr->GetParam( 1, &pType );
                            ThiscxChat->AddMessage( UDP_PING, nick, (char *)pParam, NULL, buf );
                        }
                    break;

                    case C_NICK:
                        if (nParam > 2)
                        {
                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );

                            pParam = expr->GetParam( 2, &pType );

                            if ( comp_branco(ThiscxChat->lpszNickName) || _stricmp(nick, ThiscxChat->lpszNickName)==0 )
                                ThiscxChat->SetNickName((LPSTR)pParam);

                            ThiscxChat->AddMessage( CHANGE_NICK, nick, (char *)pParam, NULL, buf );
                        }
                    break;

                    case C_JOIN:
                        if( nParam > 1 )
                        {
                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );

                            pParam = expr->GetParam( 2, &pType );
                            ThiscxChat->AddMessage( JOIN_CHANNEL, nick, (char *)pParam, NULL, buf );
                        }
                    break;

                    case C_PART:
                        if( nParam > 1 )
                        {
                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );
                            pParam = expr->GetParam( 2, &pType );
                            ThiscxChat->AddMessage( PART_CHANNEL, nick, (char *)pParam, NULL, buf  );
                        }

                    break;

                    case C_INVITE:
                    //:userid INVITE nick #channel
                        if ( nParam > 3 )
                        {
                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );
                            pParam = expr->GetParam( 2, &pType );
                            ajusta_str( pParam, who, NICKLEN );
                            pParam = expr->GetParam( 3, &pType );
                            ajusta_str( pParam, channel, CHANNELLEN );

                            ThiscxChat->AddMessage( INVITE, nick, who, (long *)channel, buf  );
                        }
                    break;

                    case C_PRIVMSG:
                    case C_NOTICE:
                    case C_WBOARD:
                    case C_APPCMD:
                        if ( nParam > 2 )
                        {
                            BOOL flNotice;

                            if (lastCmd == C_NOTICE)
                                flNotice = TRUE;
                            else
                                flNotice = FALSE;

                            pParam = expr->GetParam( 0, &pType );
                            if( pType == EX_HOST )
                            {
                                SplitUserID( pParam, nick, user, host );

                                pParam = expr->GetParam( 3, &pType );
                                ajusta_str( pParam, msg, TMAX_MSG );

                                pParam = expr->GetParam( 2, &pType );

                                if ( pType == EX_CHANNEL )
                                {
                                    // Mensagens de canal para o proprio
                                    // usuario sao descartadas
                                    if ( _stricmp(nick, ThiscxChat->GetNickName() ) != 0 )
                                    {
                                        ajusta_str( pParam, channel, CHANNELLEN );

                                        if( !flNotice && lastCmd == C_PRIVMSG )
                                            ThiscxChat->AddMessage( PRIVMSG_CHANNEL, nick, channel, (long *)msg, buf  );
                                        else if( !flNotice && lastCmd == C_WBOARD )
                                            ThiscxChat->AddMessage( WBOARD_CHANNEL, nick, channel, (long *)msg, buf  );
                                    }
                                }
                                else
                                {
//                                    if( !flNotice )
                                    if( lastCmd == C_PRIVMSG || lastCmd == C_NOTICE )
                                    {
                                        if( _stricmp( nick, NICKSERV) == 0 )
                                        {
                                            strncpy( ContactBuf, msg, bContactLen );
                                            ContactBuf[bContactLen] = '\0';
                                            FilterCtrlChars(ContactBuf, ContactBuf);

                                            nContactParam = exprContact->parse( ContactBuf, bContactLen );
                                            pParam = exprContact->GetParam( 0, &pType );

                                            lastServicesCmd = ThiscxChat->pHashServicesCmd->search( pParam );

                                            switch (lastServicesCmd)
                                            {
                                                case S_CONTACT_ERROR:
                                                    ThiscxChat->AddMessage( CONTACT_ERROR, NICKSERV, NULL, (long *) msg, buf  );
                                                    break;
                                                case S_CONTACT_LIST:
                                                    pParam = exprContact->GetParam( 1, &pType );
                                                    ajusta_str( pParam, who, NICKLEN );
                                                    ThiscxChat->AddMessage( CONTACT_LIST, NICKSERV, who, NULL, buf  );
                                                    break;
                                                case S_CONTACT_ADD:
                                                    pParam = exprContact->GetParam( 1, &pType );
                                                    ajusta_str( pParam, who, NICKLEN );
                                                    ThiscxChat->AddMessage( CONTACT_ADD, NICKSERV, who, NULL, buf  );
                                                    break;
                                                case S_CONTACT_DELETE:
                                                    pParam = exprContact->GetParam( 1, &pType );
                                                    ajusta_str( pParam, who, NICKLEN );
                                                    ThiscxChat->AddMessage( CONTACT_DELETE, NICKSERV, who, NULL, buf  );
                                                    break;
                                                case S_CONTACT_LIST_END:
                                                    ThiscxChat->AddMessage( CONTACT_LIST_END, NICKSERV, NULL, NULL, buf  );
                                                    break;
                                                case S_CONTACT:
                                                {
                                                    pParam = exprContact->GetParam( 1, &pType );
                                                    ajusta_str( pParam, msg1, TMAX_ONOFF );
                                                    if( _stricmp( msg1, S_SRVCMD_CONTACT_ON) == 0 )
                                                    {
                                                        pParam = exprContact->GetParam( 2, &pType );
                                                        ajusta_str( pParam, who, NICKLEN );
                                                        ThiscxChat->AddMessage( CONTACT_ON, NICKSERV, who, NULL, buf  );
                                                    }
                                                    else if( _stricmp( msg1, S_SRVCMD_CONTACT_OFF ) == 0 )
                                                    {
                                                        pParam = exprContact->GetParam( 2, &pType );
                                                        ajusta_str( pParam, who, NICKLEN );
                                                        ThiscxChat->AddMessage( CONTACT_OFF, NICKSERV, who, NULL, buf  );
                                                    }

                                                    break;
                                                }
                                                case S_IDENTIFY_ERROR:
                                                    ThiscxChat->AddMessage( IDENTIFY_ERROR, NICKSERV, NULL, (long *) msg, buf  );
                                                    break;
                                                case S_IDENTIFY_REQUEST:
                                                    ThiscxChat->AddMessage( IDENTIFY_REQUEST, NICKSERV, NULL, NULL, buf  );
                                                    break;
                                                case S_IDENTIFY:
                                                    ThiscxChat->AddMessage( IDENTIFY, NICKSERV, NULL, NULL, buf  );
                                                    break;
                                                case S_REGISTER_ERROR:
                                                    ThiscxChat->AddMessage( REGISTER_ERROR, NICKSERV, NULL, (long *) msg, buf  );
                                                    break;
                                                case S_REGISTER:
                                                    ThiscxChat->AddMessage( REGISTER, NICKSERV, NULL, NULL, buf  );
                                                    break;

                                                //default:
                                                //    ThiscxChat->AddMessage( PRIVMSG, nick, NULL, (long *) msg, buf  );
                                            }
                                        }
                                        else
                                        {
                                            ThiscxChat->AddMessage( PRIVMSG, nick, NULL, (long *) msg, buf  );
                                        }
                                    }
                                    else if ( lastCmd == C_WBOARD )
                                        ThiscxChat->AddMessage( WBOARD, nick, NULL, (long *) msg, buf  );
                                    else if ( lastCmd == C_APPCMD )
                                        ThiscxChat->AddMessage( APPCMD, nick, NULL, (long *) msg, buf  );
                                }
                            }
                        }
                    break;

                    case C_TOPIC:
                        if (nParam == 5 )  // Topic Data quando entramos num canal jah criado
                        {
                            pParam = expr->GetParam( 3, &pType);
                            ajusta_str( pParam, channel, CHANNELLEN );

                             pParam = expr->GetParam( 4, &pType );
                            ajusta_str( pParam, topic, TOPICLEN );

                            ThiscxChat->AddMessage( CHANGE_CHANNEL_TOPIC, NULL, channel, (long *)topic, buf );
                        }
                        else if( nParam == 4 ) // Topic Data quando o TOPIC eh mudado no canal
                        {
                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );
                            *nick = '\0';

                            pParam = expr->GetParam( 2, &pType);
                            ajusta_str( pParam, channel, CHANNELLEN );

                            pParam = expr->GetParam( 3, &pType );
                            ajusta_str( pParam, topic, TOPICLEN );

                            ThiscxChat->AddMessage( CHANGE_CHANNEL_TOPIC, nick, channel, (long *) topic, buf );
                        }

                    break;

                    case C_TOPICWHO:
                        // :sender TOPICWHO #channel nick ctime
                        if (nParam == 5)
                        {
                            pParam = expr->GetParam( 2, &pType);
                            ajusta_str( pParam, channel, CHANNELLEN );
                            pParam = expr->GetParam( 3, &pType);
                            ajusta_str( pParam, nick, NICKLEN );
                            pParam = expr->GetParam( 4, &pType);
                            ThiscxChat->AddMessage( TOPIC_WHO, nick, channel, (long *) pParam, buf );
                        }
                    break;

                    case C_NAMES:
                        // :Server cmd nick = #channel :[@]user1 [@]user2 ... [@]userN
                        // :Server cmd nick #channel :End of /NAMES
                        if ( nParam == 6 ) // Names Reply
                        {
                            pParam = expr->GetParam( 4, &pType);
                            ajusta_str( pParam, channel, CHANNELLEN );

                            pParam = expr->GetParam( 5, &pType );

                            ThiscxChat->CreateChannelUserList( channel, (char *)pParam );
                        }
                        else if( nParam == 5 ) // End of names
                        {
                            pParam = expr->GetParam( 3, &pType);
                            ajusta_str( pParam, channel, CHANNELLEN );

                            pParam = expr->GetParam( 4, &pType );

                            LPSTR UserList;
                            ThiscxChat->GetChannelUserList( channel, &UserList );
                            ThiscxChat->AddMessage( GET_USER_LIST, channel, NULL, (long *)UserList, buf );
                            //MessageBox( NULL, UserList, "UserList", MB_OK );
                            ThiscxChat->DeleteChannelUserList( channel );
                            delete [] UserList;
                        }
                    break;

                    case C_LIST:
                    // :Server cmd nick #chname nusers :topic
                    // :Server cmd nick :End of /LIST
                    if ( nParam == 6 )
                    {
                        pParam = expr->GetParam( 3, &pType);
                        ajusta_str( pParam, channel, CHANNELLEN );

                        pParam = expr->GetParam( 4, &pType);
                        ajusta_str( pParam, msg, TMAX_MSG );

                        // topic
                        pParam = expr->GetParam( 5, &pType );
                        ajusta_str( pParam, topic, TOPICLEN );

                        memset( msg1, 0, TMAX_MSG+1 );
                        _snprintf( msg1, TMAX_MSG, "%s %s", msg, topic );

                        ThiscxChat->AddMessage( GET_CHANNEL_LIST, NULL, channel, (long *) msg1, buf );
                    }
                    else if ( nParam == 4 )
                    {
                        ThiscxChat->AddMessage( ENDOF_CHANNEL_LIST, NULL, NULL, NULL, buf );
                    }

                    break;

                    case C_WHOIS:
                        // Existem varios tipos de retornos para WHOIS, concatena-se em msg1 e repassa.
                        // :Server cmd nick nick :str
                        // :Server cmd nick nick :End of /WHOIS list
                        if ( nParam > 3 )
                        {
                            int l = nParam;

                            pParam = expr->GetParam( 3, &pType);
                            ajusta_str( pParam, nick, NICKLEN );

                            memset( msg1, 0, TMAX_MSG+1 );
                            while( l != 3 )
                            {
                                pParam = expr->GetParam( (nParam-l+4), &pType );
                                ajusta_str( pParam, msg, TMAX_MSG );

                                strcat( msg1, msg );
                                if( l > 4 )
                                    strcat( msg1, " " );
                                l--;
                            }

                            if( nIRCCmd == RPL_ENDOFWHOIS )
                                ThiscxChat->AddMessage( ENDOF_WHOIS, "ENDOF_WHOIS", NULL, NULL, buf );
                            else
                                ThiscxChat->AddMessage( WHOIS, "WHOIS", NULL, (long *) msg1, buf );
                        }
                    break;

                    case C_MODE:
                    {
                        // :sender MODE #channel +/-flags target1 target2 ... targetN
                        // :sender 324 MODE #channel +/-flags target1 target2 ... targetN

                        UINT i,
                             prmoffset;

                        LPCSTR lpTarget,
                               lpSource;

                        prmoffset = 0;

                        //memset( lpBuffer, 0, TAM_LBUFFER+1 );
                        if( nParam > 3 )
                        {
                            if (nParam > 4)
                            {
                                // No caso de RPL_CHANNELMODEIS os parametros ficam deslocados
                                pParam = expr->GetParam( 3, &pType );
                                if ( pType == EX_CHANNEL )
                                    prmoffset++;

                                *msg = '\0';

                                for (i=4+prmoffset; i<nParam; i++)
                                {
                                    pParam = expr->GetParam( i+prmoffset, &pType );
                                    strcat(msg, pParam);
                                    strcat(msg, " ");
                                }

                                lpSource = msg;
                            }
                            else
                                lpSource = NULL;

                            pParam = expr->GetParam( 2+prmoffset, &pType );
                            if ( pType == EX_CHANNEL )
                            {
                                channel[0] = '#';
                                ajusta_str( pParam, &channel[1], CHANNELLEN );
                                lpTarget = channel;
                            }
                            else
                            {
                                ajusta_str( pParam, nick, NICKLEN );
                                lpTarget = nick;
                            }

                            pParam = expr->GetParam( 3+prmoffset, &pType );

                            ThiscxChat->AddMessage( MODE, lpSource, lpTarget, (long *)pParam, buf );


                            /*
                            i = 3;
                            while( i < nParam )
                            {
                                pParam = expr->GetParam( i, &pType );
                                ajusta_str( pParam, msg, TMAX_MSG );
                                strncat( lpBuffer, msg, TAM_LBUFFER );
                                i++;
                            }

                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );

                            pParam = expr->GetParam( 2, &pType );
                            if ( pType == EX_CHANNEL )
                            {
                                ajusta_str( pParam, channel, CHANNELLEN );
                                ThiscxChat->AddMessage( MODE, nick, channel, (long *)msg, buf );
                            }
                            else
                            {
                                ThiscxChat->AddMessage( MODE, nick, nick, (long *)msg, buf );
                            }
                            */
                        }
                    }
                    break;


                    // nick!userid@lapis.com KICK #channel nickname :reason
                    case C_KICK:
                        *msg = '\0';

                        switch( nParam )
                        {
                            case 5:
                                pParam = expr->GetParam( 4, &pType );
                                ajusta_str( pParam, msg, TMAX_MSG );
                            case 4:
                                pParam = expr->GetParam( 2, &pType );
                                ajusta_str( pParam, channel, CHANNELLEN );
                                pParam = expr->GetParam( 3, &pType );
                                ajusta_str( pParam, nick, NICKLEN );
                                ThiscxChat->AddMessage( KICK_USER, nick, channel, (long *)msg, buf );
                            break;
                        }

                    break;

                    // :Server cmd nick tgtnick :Protocol
                    case C_PROTOCOL:
                        pParam = expr->GetParam( 3, &pType );
                        ajusta_str( pParam, nick, NICKLEN );
                        pParam = expr->GetParam( 4, &pType );
                        ajusta_str( pParam, msg, TMAX_MSG );
                        ThiscxChat->AddMessage( PROTOCOL, "PROTOCOL", nick, (long *)msg, buf );
                    break;

                    // :sender ErrorCode nick [data] :msgerror
                    case C_CMDERROR:
                        *msg  = '\0';
                        *msg1 = '\0';
                        switch (nParam)
                        {
                            case 5:
                                pParam = expr->GetParam( 4, &pType );
                                ajusta_str( pParam, msg, TMAX_MSG );
                                pParam = expr->GetParam( 3, &pType );
                                ajusta_str( pParam, msg1, TMAX_MSG );
                            case 4:
                                if (nParam == 4)
                                {
                                    pParam = expr->GetParam( 3, &pType );
                                    ajusta_str( pParam, msg, TMAX_MSG );
                                }

                                // errorCode
                                pParam = expr->GetParam( 1, &pType );

                                ThiscxChat->AddMessage( CMD_ERROR, msg, msg1, (long *)pParam, buf );
                                break;
                        }
                    break;

                    case C_MEDIASND:
                        // MEDIASND #Channel StrID Protocol PacketLen $HeaderLen HEADER
                        // :Server RPL_MEDIASNDACK nick #Channel StrID ID MediaPort :MEDIASND

                        if( nParam > 6 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam(3, &pType);
                            ajusta_str(pParam, channel, CHANNELLEN);
                            vr->SetChannel( channel );

                            pParam = expr->GetParam(4, &pType);
                            ajusta_str(pParam, mediaclass, MEDIACLASSLEN);
                            vr->SetMediaClass( mediaclass );

                            pParam = expr->GetParam(5, &pType);
                            mediaID = atoi( pParam );
                            vr->SetMediaID( mediaID );

                            pParam = expr->GetParam(6, &pType);
                            port = atoi( pParam );
                            vr->SetMediaPort( port );

                            ThiscxChat->AddMessage( MEDIASND, channel, NULL, (long *)vr, buf );
                            delete vr;
                        }
                    break;

                    case C_MEDIASNDSTOP:
                        // :Server RPL_MEDIASNDSTOP nick ID :MEDIASNDSTOP
                        // :Server MEDIASNDSTOP ID
                        if( nParam >= 3 )
                        {
                            vr = new MediaRequest;

                            if( nParam == 3 )
                                pParam = expr->GetParam( 2, &pType );
                            else
                                pParam = expr->GetParam( 3, &pType );

                            mediaID = atol( pParam );

                            vr->SetMediaID( mediaID );
                            ThiscxChat->AddMessage( MEDIASNDSTOP, "MEDIASNDSTOP", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIARESET:
                        // :Server MEDIARESET ID
                        if( nParam >= 3 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam( 2, &pType );

                            mediaID = atol( pParam );

                            vr->SetMediaID( mediaID );
                            ThiscxChat->AddMessage( MEDIARESET, "MEDIARESET", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIAMUTE:
                        // :userid MEDIAMUTE ID [ON|OFF]
                        if( nParam > 2 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );

                            pParam  = expr->GetParam( 2, &pType );
                            mediaID = atol( pParam );
                            vr->SetMediaID( mediaID );

                            if ( nParam > 3 )
                                pParam = expr->GetParam( 3, &pType );
                            else
                                pParam = S_VN_ON;

                            vr->SetMediaNotify( (char *)pParam );

                            ThiscxChat->AddMessage( MEDIAMUTE, nick, NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIARCV:
                        // :Server RPL_MEDIARCVACK nick ID PacketLen Ip Port $HeaderLen HEADER
                        if ( nParam > 6 )
                        {
                            vr = new MediaRequest;
                            UINT   tBuf;
                            LPCSTR pBuf;

                            pParam  = expr->GetParam( 3, &pType );
                            mediaID = atol(pParam);
                            vr->SetMediaID( mediaID );

                            pParam    = expr->GetParam( 4, &pType );
                            packetlen = atol(pParam);
                            vr->SetPacketLen(  packetlen );

                            pParam = expr->GetParam(5, &pType);
                            ajusta_str(pParam, ip, IPSTR_MAXLEN);
                            vr->SetIp( ip );

                            pParam  = expr->GetParam( 6, &pType );
                            port    = atoi(pParam);
                            vr->SetMediaPort( port );

                            pParam    = expr->GetParam( 7, &pType );
                            headerlen = atoi( pParam );

                            pBuf = expr->GetBuffer( &tBuf );
                        
                            if ( pBuf && headerlen && headerlen <= tBuf )
                            {
                                //tBuf = TAM_LBUFFER;
                                //decode64( pBuf, headerlen, lpBase64, (int *)&tBuf );
                                //vr->SetHeaderLen(  tBuf );
                                //vr->SetHeader( (void *)lpBase64, tBuf );
                                vr->SetHeaderLen(  headerlen );
                                vr->SetHeader( (void *)pBuf, headerlen );
                                ThiscxChat->AddMessage( MEDIARCV, "MEDIARCV", NULL, (long *) vr, buf );
                            }

                            delete vr;
                        }
                    break;

                    case C_MEDIARCVALL:
                        //:Server RPL_MEDIARCVALLACK nickname #Channel MediaClassStr MulticastIp Port

                        if ( nParam > 5 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam(3, &pType);
                            ajusta_str(pParam, channel, CHANNELLEN);
                            vr->SetChannel( channel );

                            pParam = expr->GetParam(4, &pType);
                            ajusta_str(pParam, mediaclass, MEDIACLASSLEN);
                            vr->SetMediaClass( mediaclass );

                            pParam = expr->GetParam(5, &pType);
                            ajusta_str(pParam, ip, IPSTR_MAXLEN);
                            vr->SetIp( ip );

                            pParam  = expr->GetParam(6, &pType);
                            port    = atoi(pParam);
                            vr->SetMediaPort( port );

                            ThiscxChat->AddMessage( MEDIARCVALL, "MEDIARCVALL", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIARCVSTOP:
                        // :Server RPL_MEDIARCVSTOP nick ID :MEDIARCVSTOP
                        if( nParam > 3 )
                        {
                            vr = new MediaRequest;

                            pParam  = expr->GetParam( 3, &pType );
                            mediaID = atol( pParam );
                            vr->SetMediaID( mediaID );

                            ThiscxChat->AddMessage( MEDIARCVSTOP, "MEDIARCVSTOP", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIARCVALLSTOP:
                        // :Server RPL_MEDIARCVALLSTOP nickname #Channel MediaClassStr :MEDIARCVALLSTOP
                        if( nParam > 4 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam( 3, &pType );
                            ajusta_str(pParam, channel, CHANNELLEN );
                            vr->SetChannel( channel );

                            pParam = expr->GetParam( 4, &pType );
                            ajusta_str(pParam, mediaclass, MEDIACLASSLEN );
                            vr->SetMediaClass( mediaclass );

                            ThiscxChat->AddMessage( MEDIARCVALLSTOP, "MEDIARCVALLSTOP", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIASET:
                        // :Server RPL_MEDIASET nickname MediaID IDENT :Data
                        if( nParam > 4 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam( 3, &pType );
                            mediaID = atol( pParam );
                            vr->SetMediaID( mediaID );

                            pParam = expr->GetParam( 4, &pType );
                            vr->SetMediaSetOption( (char *)pParam );

                            pParam = expr->GetParam( 5, &pType );
                            vr->SetName( (char *) pParam );

                            ThiscxChat->AddMessage( MEDIASET, "MEDIASET", NULL, (long *) vr, buf );
                            delete vr;
                        }
                    break;

                    case C_MEDIANOTIFY:
                        //:nickuser!user@domain.org MEDIANOTIFY nickowner MediaID MediaClassStr #Channel NOTIFY_CMD
                        if( nParam > 5 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam( 2, &pType );
                            vr->SetNickname( (LPSTR) pParam );

                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );
                            //vr->SetNickname( nick );

                            pParam = expr->GetParam( 3, &pType );
                            mediaID = atol( pParam );
                            vr->SetMediaID( mediaID );

                            pParam = expr->GetParam( 4, &pType);
                            ajusta_str(pParam, mediaclass, MEDIACLASSLEN);
                            vr->SetMediaClass( mediaclass );

                            pParam = expr->GetParam( 5, &pType );
                            ajusta_str(pParam, channel, CHANNELLEN );
                            vr->SetChannel( channel );

                            pParam = expr->GetParam( 6, &pType );
                            vr->SetMediaNotify( (char *)pParam );

                            ThiscxChat->AddMessage( MEDIANOTIFY, nick, NULL, (long *) vr, buf );
                            delete vr;
                        }
                    break;

                    case C_MEDIARCVRUN:
                        //:Server RPL_MEDIARCVRUN nickname MediaID :MEDIARCVRUN
                        if( nParam > 3 )
                        {
                            vr = new MediaRequest;

                            pParam  = expr->GetParam( 3, &pType );
                            mediaID = atol( pParam );

                            vr->SetMediaID( mediaID );
                            ThiscxChat->AddMessage( MEDIARCVRUN, "MEDIARCVRUN", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIASETPOS:
                        //:Server MEDIASETPOS MediaID seq
                        if( nParam > 3 )
                        {
                            vr = new MediaRequest;

                            pParam  = expr->GetParam( 2, &pType );
                            mediaID = atol( pParam );
                            vr->SetMediaID( mediaID );

                            pParam  = expr->GetParam( 3, &pType );
                            vr->SetData1( atol(pParam) );

                            ThiscxChat->AddMessage( MEDIASETPOS, "MEDIASETPOS", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIAPKTREQUEST:
                        //:Server MEDIAPKTREQUEST MediaID seqi seqf
                        if( nParam > 4 )
                        {
                            vr = new MediaRequest;

                            pParam  = expr->GetParam( 2, &pType );
                            mediaID = atol( pParam );
                            vr->SetMediaID( mediaID );

                            pParam  = expr->GetParam( 3, &pType );
                            vr->SetData1( atol(pParam) );

                            pParam  = expr->GetParam( 4, &pType );
                            vr->SetData2( atol(pParam) );

                            ThiscxChat->AddMessage( MEDIAPKTREQUEST, "MEDIAPKTREQUEST", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;

                    case C_MEDIARCVALLRUN:
                        //:Server RPL_MEDIARCVALLRUN #Channel MediaClassStr :MEDIARCVALLRUN
                        if( nParam > 4 )
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam( 3, &pType );
                            ajusta_str( pParam, channel, CHANNELLEN );
                            vr->SetChannel( channel );

                            pParam = expr->GetParam( 4, &pType );
                            ajusta_str(pParam, mediaclass, MEDIACLASSLEN );
                            vr->SetMediaClass( mediaclass );

                            ThiscxChat->AddMessage( MEDIARCVALLRUN, "MEDIARCVALLRUN", NULL, (long *) vr, buf );

                            delete vr;
                        }
                    break;


                    case C_MEDIAQUERY:
//:Servername 801 nickname Owner1 #Channel1 MediaClassStr1 MediaID1 Bitrate1 :MediaName1
//:Servername 801 nickname Owner2 #Channel2 MediaClassStr2 MediaID2 Bitrate2 :MediaName2
//.....................................................
//:Servername 801 nickname OwnerN #ChannelN MediaClassStrN MediaIDN BitrateN :MediaNameN
//:Servername 802 nickname :End of /MEDIAQUERY list.
                        if( nParam > 7 )
                        {
                            UINT i;

                            vq = new MediaQuery;

                            pParam = expr->GetParam( 3, &pType );
                            vq->SetOwner( (char *)pParam );

                            pParam = expr->GetParam( 4, &pType );
                            ajusta_str(pParam, channel, CHANNELLEN);
                            vq->SetChannel( channel );

                            pParam = expr->GetParam( 5, &pType );
                            vq->SetMediaClass( (char *)pParam );

                            pParam = expr->GetParam( 6, &pType );
                            mediaID = atol( pParam );
                            vq->SetMediaID(  mediaID );

                            if ( nParam > 8 )
                            {
                                pParam  = expr->GetParam( 7, &pType );
                                bitrate = atol( pParam );

                                i = 8;
                            }
                            else
                            {
                                bitrate = 0;
                                i = 7;
                            }

                            vq->SetBitrate( bitrate );

                            pParam = expr->GetParam( i, &pType );
                            vq->SetMediaName( (char *)pParam );

                            ThiscxChat->AddMessage( MEDIAQUERY, channel, NULL, (long *)vq, buf );
                            //ThiscxChat->AddMediaQuery( vq );
                            delete vq;
                        }
                        else if (nParam == 4 ) // End of MediaQuery
                        {
                            //vr = new MediaRequest;
                            ThiscxChat->AddMessage( MEDIAQUERYEND, "MEDIAQUERYEND", NULL, NULL, buf );
                            //delete vr;
                        }
                    break;

                    case C_MEDIASTATS:
                        //:Server RPL_STATS user stats
                        if (nParam > 3)
                        {
                            pParam = expr->GetParam( 3, &pType );
                            ThiscxChat->AddMessage( MEDIASTATS, "MEDIASTATS", (char *)pParam, NULL, buf  );
                        }
                    break;

                    case C_MEDIAVOICE:
                        //:Server RPL_MEDIAVOICE user #channel :nick
                        //:owner MEDIAVOICE #channel :nick
                        if (nParam > 3)
                        {
                            if (nParam == 4)
                            {
                                pParam = expr->GetParam(2, &pType);
                                ajusta_str(pParam, channel, CHANNELLEN);
                                pParam = expr->GetParam( 3, &pType );
                            }
                            else // nParam == 5
                            {
                                pParam = expr->GetParam(3, &pType);
                                ajusta_str(pParam, channel, CHANNELLEN);
                                pParam = expr->GetParam( 4, &pType );
                            }

                            ThiscxChat->AddMessage( MEDIAVOICE, channel, (char *)pParam, NULL, buf  );
                        }
                    break;

                    //MEDIAPTPINVITE NickTarget mediaclassStr IPLocalSource portRcvSource
                    //portSndSource portRcvUDPPingSource bitrateSource
                    //:NickSource MEDIAPTPINVITE NickTarget mediaclassStr IPLocalSource
                    //IPSrvSource portRcvSource portSndSource portRcvUDPPingSource
                    //bitrateSource

                    //MEDIAPTPACK NickTarget mediaclassStr IPLocalSource portRcvSource
                    //portSndSource portRcvUDPPingSource bitrateSource
                    //:NickSource MEDIAPTPACK NickTarget mediaclassStr IPLocalSource
                    //IPSrvSource portRcvSource portSndSource portRcvUDPPingSource
                    //bitrateSource
                    case C_MEDIAPTPINVITE:
                    case C_MEDIAPTPACK:
                        if( nParam == 10 )
                        {
                            int q;
                            vp = new MediaPtP;

                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );

                            pParam = expr->GetParam( 2, &pType );
                            vp->SetNickTarget( (char *)pParam );

                            pParam = expr->GetParam( 3, &pType );
                            vp->SetMediaClass( (char *) pParam );

                            pParam = expr->GetParam( 4, &pType );
                            vp->SetIPLocalSource( (char *)pParam );

                            pParam = expr->GetParam( 5, &pType );
                            vp->SetIPSrvSource( (char *) pParam );

                            pParam = expr->GetParam( 6, &pType );
                            q = atol( pParam );
                            vp->SetRcvSourcePort(  q );

                            pParam = expr->GetParam( 7, &pType );
                            q = atol( pParam );
                            vp->SetSendSourcePort(  q );

                            pParam = expr->GetParam( 8, &pType );
                            q = atol( pParam );
                            vp->SetUDPPingSourcePort(  q );

                            pParam = expr->GetParam( 9, &pType );
                            q = atol( pParam );
                            vp->SetSourceBitrate(  q );

                            if( lastCmd == C_MEDIAPTPINVITE )
                                ThiscxChat->AddMessage( MEDIAPTPINVITE, nick, NULL, (long *)vp, buf );
                            else if( lastCmd == C_MEDIAPTPACK )
                                ThiscxChat->AddMessage( MEDIAPTPACK, nick, NULL, (long *)vp, buf );

                            delete vp;
                        }
                    break;

                    //MEDIAPTPREJECT NickTarget mediaclassStr
                    //:NickSource MEDIAPTPREJECT NickTarget mediaclassStr
                    case C_MEDIAPTPREJECT:
                        if( nParam == 4 )
                        {
                            vp = new MediaPtP;

                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );

                            pParam = expr->GetParam( 2, &pType );
                            vp->SetNickTarget( (char *)pParam );

                            pParam = expr->GetParam( 3, &pType );
                            vp->SetMediaClass( (char *) pParam );

                            ThiscxChat->AddMessage( MEDIAPTPREJECT, nick, NULL, (long *)vp, buf );
                            delete vp;
                        }
                    break;

                    // :srvname MEDIA MediaID nickname #Channel MediaClassStr Bitrate ip port headersize nheadersegs maxsegsize headerchksum
                    case C_MEDIA:
                        if( nParam > 12 )
                        {
                            if (!pViewerCtrl)
                                pViewerCtrl = new MediaRequest;
                            else
                                pViewerCtrl->reset();

                            if (pViewerCtrl)
                            {
                                ULONG val;

                                pParam = expr->GetParam( 2, &pType );
                                mediaID = atol( pParam );
                                pViewerCtrl->SetMediaID( mediaID );

                                pParam = expr->GetParam( 3, &pType );
                                pViewerCtrl->SetNickname( pParam );

                                pParam = expr->GetParam( 4, &pType );
                                pViewerCtrl->SetChannel( pParam );

                                pParam = expr->GetParam( 5, &pType);
                                pViewerCtrl->SetMediaClass( pParam );

                                pParam = expr->GetParam( 6, &pType );
                                val = atol( pParam );
                                pViewerCtrl->SetBitrate(val);

                                pParam = expr->GetParam( 7, &pType);
                                pViewerCtrl->SetIp( pParam );

                                pParam = expr->GetParam( 8, &pType );
                                val = atol( pParam );
                                pViewerCtrl->SetMediaPort(val);

                                pParam = expr->GetParam( 9, &pType );
                                val = atol( pParam );
                                pViewerCtrl->SetHeaderLen(val);

                                pParam = expr->GetParam( 10, &pType );
                                val = atol( pParam );
                                pViewerCtrl->SetHeaderNSegs(val);

                                pParam = expr->GetParam( 11, &pType );
                                val = atol( pParam );
                                pViewerCtrl->SetHeaderMaxSegSize(val);

                                pParam = expr->GetParam( 12, &pType );
                                val = strtoul(pParam, NULL, 10);
                                pViewerCtrl->SetHeaderChksum(val);

                                if ( pViewerCtrl->IsViewerCtrlRdy() )
                                    ThiscxChat->AddMessage( MEDIA, "MEDIA", NULL, (long *) pViewerCtrl, buf );
                            }
                        }
                    break;

                    // :srvname PASS #channel :password
                    case C_PASSWORD:
                        if ( nParam > 2 )
                        {
                            pParam  = expr->GetParam( 2, &pType );
                            ajusta_str( pParam, channel, CHANNELLEN );

                            pParam  = expr->GetParam( 3, &pType );

                            if ( !pParam )
                                pParam = "";

                            ThiscxChat->AddMessage( PASSWORD, "PASSWORD", channel, (long*)pParam, buf );
                        }
                    break;

                    // :srvname MEDIAHEADER MediaID seg $size header
                    case C_MEDIAHEADER:
                        if( nParam > 4 )
                        {
                            if (pViewerCtrl)
                            {
                                ULONG seg,
                                      segsize;

                                UINT   tBuf;
                                LPCSTR pBuf;

                                pParam  = expr->GetParam( 2, &pType );
                                mediaID = atol( pParam );

                                pParam  = expr->GetParam( 3, &pType );
                                seg     = atol( pParam );

                                pParam  = expr->GetParam( 4, &pType );
                                segsize = atol( pParam );

                                pBuf = expr->GetBuffer( &tBuf );
                        
                                if ( pBuf && segsize && segsize <= tBuf )
                                    pViewerCtrl->AddHeaderSeg(mediaID, seg, segsize, pBuf);

                                if ( pViewerCtrl->IsViewerCtrlRdy() )
                                    ThiscxChat->AddMessage( MEDIA, "MEDIA", NULL, (long *) pViewerCtrl, buf );
                            }
                        }
                    break;

                    case C_OPER:
                        // :server OPER nick
                        ThiscxChat->AddMessage( OPER, NULL, NULL, NULL, buf );
                    break;

                    case C_CONFIG:
                        // :server CONFIG #channel MulticastIp port
                        if (nParam > 4)
                        {
                            pParam = expr->GetParam(2, &pType);
                            ajusta_str(pParam, channel, CHANNELLEN);
                            pParam = expr->GetParam(3, &pType);
                            ajusta_str(pParam, ip, IPSTR_MAXLEN);
                            pParam = expr->GetParam(4, &pType );
                            port   = atoi(pParam);

                            ThiscxChat->InitMulticastChannel(channel, ip, port);
                        }
                    break;

                    case C_UDPPONG:
                        _TRACEL(LOGLEVEL5, "Comando: UDPPONG");
                        ThiscxChat->AddMessage( UDP_PONG, NULL, NULL, NULL, buf );

                        // Informa ao servidor que eh capaz de receber pacotes UDP
                        ThiscxChat->Send("UDPON");
                    break;

                    case C_DIE:
                        pParam = expr->GetParam( 0, &pType );
                        _TRACE1L(LOGLEVEL5, "Comando: DIE (by %s)", pParam);
                        ThiscxChat->Terminate();
                    break;

                    case C_SERVERREDIR:
                        // :srvid 016 Robson ServiceName port :"srvredir"
                        if (nParam >= 6)
                        {
                            vr = new MediaRequest;

                            pParam = expr->GetParam(4, &pType);
                            port   = atoi( pParam );
                            vr->SetMediaPort(port);

                            pParam = expr->GetParam(5, &pType);
                            ajusta_str(pParam, host, HOSTLEN);

                            pParam = expr->GetParam(3, &pType);

                            _TRACE3L(LOGLEVEL5, "Comando: SRVREDIR %s %lu :%s", pParam, port, host);

                            ThiscxChat->AddMessage( SERVER_REDIR, pParam, host, (long *)vr, buf );

                            delete vr;
                        }
                    break;

                    case C_QUIT:
                        if (nParam > 2)
                        {
                            pParam = expr->GetParam( 0, &pType );
                            SplitUserID( pParam, nick, user, host );

                            pParam = expr->GetParam( 2, &pType );

                            ThiscxChat->AddMessage( QUIT_CHAT, nick, (char *)pParam, NULL, buf );
                        }
                    break;

                    default:
                        ThiscxChat->AddMessage( SERVER_MSG, NULL, NULL, NULL, buf );
                    
// TODO:
// MEDIARCVLIST

                }
            }
       }
    }    

    return ret;
}


BOOL RcvUDPCallback(SocketCtrl *_psock, SOCKET _ssock, struct sockaddr_in *_psin, char *buf, unsigned int blen)
{
    BOOL ret;

    ret = FALSE;

    if (buf && blen)
    {
        if( blen < TMAX_MSG )
            buf[blen++] = '\0';
        else
        {
            blen = TMAX_MSG;
            buf[TMAX_MSG-1] = '\0';
        }

        removeEOL(buf, buf, blen, FALSE);

        ret = RecvCallback(_psock, INVALID_SOCKET, buf, blen);
    }

    return ret;
}


/*
BOOL RcvUDPCallback(SocketCtrl *_psock, struct sockaddr_in *_psin, char *buf, unsigned int blen)
{
    BOOL   ret,
           flCmdOk;

    ULONG  offset,
           tline;

    LPCSTR pBuf;


    MediaPkt *pMediaPkt;


    if (!buf || !blen)
        return TRUE;

    ret = TRUE;

    if (blen > TAM_LBUFFER)
    {
        _TRACE1L(LOGLEVEL4, "RcvUDPmediaCallback: Comando com tamanho invalido (%u)", blen);
        return ret;
    }

    offset = 0;

    while ( blen>offset && (blen-offset)>=sizeof(MediaPkt) )
    {
        pMediaPkt = (MediaPkt *) (&buf[offset]);

        tline = sizeof(MediaPkt) + pMediaPkt->datalen;

        if ((blen-offset)>=tline)
        {
            pBuf    = &buf[offset + sizeof(MediaPkt)];
            flCmdOk = TRUE;
            offset += tline;
        }
        else
        {
            _TRACE1L(LOGLEVEL4, "RcvUDPmediaCallback: Comando Invalido: Buffer %d bytes", blen);

            // Erro => Aponta para o fim do Buffer

            flCmdOk = FALSE;
            offset  = blen;
        }

        if (flCmdOk)
        {
            switch (pMediaPkt->type)
            {
                case MTYPE_PONG:
                {
                    _TRACE1L(LOGLEVEL5, "Comando: UDPPONG  Tamanho: %d bytes", tline);
                    ThiscxChat->AddMessage( UDP_PONG, NULL, NULL, NULL, buf );

                    // Informa ao servidor que eh capaz de receber pacotes UDP
                    ThiscxChat->Send("UDPON");
                    break;
                }
            }
        }
    }

    return ret;
}
*/

static int _DoCmdControl( IThread *pTread, void *_pSock )
{
    SocketCtrl *pSock = (SocketCtrl *)_pSock;

    //while( !flThreadExit && !flThreadCmdExit && pSock->accept( RecvCallback, NULL, CloseCallback, SELECT_BLOCK ) );

    while( !flThreadExit && !flThreadCmdExit && pSock->accept( RecvCallback, NULL, CloseCallback, 2) )
        ;

    return TRUE;
}

int cxChat::_DoAppPing( IThread *pTread, void *lpctx )
{
    time_t ctime;

    cxChat *x = (cxChat *) lpctx;

    while( !flThreadExit )
    {
        x->Wait();
        ctime = time(NULL);

        if ( !x->flAppPing )
        {
            // Caso o PING nao tenha sido enviado
            if ( (ctime - x->LastAppPingTime) >= APPPING_FREQ )
            {
                x->flAppPing = TRUE;
                x->LastAppPingTime = ctime;
                x->Signal();

                x->AddMessage(APP_PING, NULL, NULL, NULL, "App Ping");
            }
            else
                x->Signal();
        }
        else
        {
            if ( (ctime - x->LastAppPingTime) >= APPPING_MAXWAITTIME )
            {
                x->Signal();

                _TRACEL( LOGLEVEL1, "APP PING TIMEOUT! Application Aborted." );

                //MessageBox(NULL, "O IP.TV será encerrado!", "App Ping Timeout", MB_ICONERROR);

                x->Terminate();
            }
            else
                x->Signal();
        }

        x->Wait();
        ctime = time(NULL);

        if ( (ctime - x->LastCmdSentTime) > MAXCMDSENTTIME )
        {
            x->Signal();

            x->SendMessage( IDLE, 0 );
        }
        else
            x->Signal();

        x->Wait();

        if ( !x->flSrvPing )
        {
            if ( x->LastCmdTime && (ctime - x->LastCmdTime)>MAXIDLETIME )
            {
                x->LastSrvPingTime = ctime;

                x->flSrvPing = TRUE;

                x->Signal();

                // Solicita modes do proprio usuario para verificar
                // se conexao estah ativa
                x->SendMessage( MODE_USER, (long)x->lpszNickName);
            }
            else
                x->Signal();
        }
        else
        {
            if ( (ctime - x->LastSrvPingTime) > MAXPONGTIME )
            {
                x->Signal();

                _TRACEL( LOGLEVEL1, "MAX IDLE TIME Reached! Application Aborted." );

                //MessageBox(NULL, "O IP.TV será encerrado!", "MAX IDLE TIME Reached", MB_ICONERROR);

                x->Terminate();
            }
            else
                x->Signal();
        }

        ::Wait(APPPING_SLEEPTIME);
    }

    return TRUE;
}


int cxChat::_DoUDP(IThread *pThread, void* lpctx)
{
    int ret;

    cxChat *x = (cxChat *) lpctx;

    x->pUDPSemaph->Wait();

    ret = 10;

    while ( !flThreadExit && !flUDPThreadExit )
    {
        //if ( !x->pSockUDP->acceptUDP(RcvUDPCallback, NULL, SELECT_BLOCK) )
        //    break;

        if ( !x->pSockUDP->acceptUDP(RcvUDPCallback, NULL, 3) )
            break;
    }

    x->pUDPSemaph->Signal();

    return ret;
}


BOOL cxChat::_DoChatMessage( IThread *pThread, void *lpctx )
{
    BOOL ret  = FALSE;
    cxChat *x = (cxChat *) lpctx;
    MessageData *lpMd;

    //CoInitializeEx(NULL, COINIT_MULTITHREADED);

    if( x )
    {
        while( !flThreadExit )
        {
            //if( x->GetMessage( &lpMd ) )
            if( (lpMd = x->GetMessage())!=NULL )
            {
                // Chamar rotina IrmStub::OnMessage(lpMd)
                x->pSink->OnMessage(lpMd);
                delete lpMd;
            }
        }
    }

#ifdef _WIN32
    ::CoUninitialize();
#endif

    return ret;
}


cxChat::cxChat( IMessageDataSink* p )
{ 
    CmdControlThread = NULL;
    pSock            = new SocketCtrl( TRUE, SOCK_CMDCTRL_TXT_BUFSIZE );
    pSockUDP         = new SocketCtrl;
    pSockUDPSnd      = new SocketCtrl;
    flConnected      = FALSE;
    flUDPListening   = FALSE;
    fluPnP           = FALSE;
    pHashCmd         = new Hash_cmd( srvcmd_vet );
    pHashServicesCmd = new Hash_cmd( servicecmd_vet );
    pMessageList     = new ListT<MessageData>;
    pChannelUserList = new ListT<ChannelUserList>;
    pMediaQueryList  = new ListT<MediaQueryList>;
    pSemaph          = new _SEMAPHORE(SEMAPH_MUTEX);
    pUDPSemaph       = new _SEMAPHORE;
    pMsgQSemaph      = new _SEMAPHORE(SEMAPH_NORMAL, NMAX_MSG_QUEUE, 0, SEMAPH_BLOCK, 0);
    pSink            = p;

    memset( lpszCliVersion, 0, VERSIONLEN+1 );
    memset( lpszNickName  , 0, NICKLEN+1    );
    memset( lpszEmail     , 0, EMAILLEN+1   );
    memset( lpszRealName  , 0, REALLEN+1    );
    memset( lpszHost      , 0, HOSTLEN+1    );
    memset( lpszUser      , 0, USERLEN+1    );
    memset( lpszPassword  , 0, PASSWDLEN+1  );

    memset( lpszMCastChannel, 0, CHANNELLEN+1);

    ThiscxChat = this;

    ChatThread       = NULL;
    CmdControlThread = NULL;
    UDPThread        = NULL;
    AppPingThread    = NULL;

    flAppPing        = FALSE;
    flSrvPing        = FALSE;
    LastAppPingTime  = 0;
    LastSrvPingTime  = 0;
    LastCmdTime      = 0;
    LastCmdSentTime  = time(NULL);

    expr        = new ExprParser(NMAXPARAM_PARSER);
    exprContact = new ExprParser(NMAXPARAM_PARSER);
}


cxChat::~cxChat( void )
{
    KillThreads();

    if( pHashServicesCmd )
        delete pHashServicesCmd;

    if( pHashCmd )
        delete pHashCmd;

    if( pMessageList )
        delete pMessageList;

    if( pChannelUserList )
        delete pChannelUserList;

    if( pMediaQueryList )
        delete pMediaQueryList;

    if ( pSockUDPSnd )
        delete pSockUDPSnd;

    if ( pSockUDP )
        delete pSockUDP;

    if( pSock )
        delete pSock;

    if( pSemaph )
        delete pSemaph;

    if( pUDPSemaph )
        delete pUDPSemaph;

    if( pMsgQSemaph )
        delete pMsgQSemaph;

    ThiscxChat = NULL;

    if ( expr )
    {
        delete expr;
        expr = NULL;
    }

    if ( exprContact )
    {
        delete exprContact;
        exprContact = NULL;
    }

    if ( pViewerCtrl )
    {
        delete pViewerCtrl;
        pViewerCtrl = NULL;
    }
}


LPCSTR cxChat::GetVersion(void)
{
    return CHAT_DLL_VERSION;
}


void cxChat::KillThreads(void)
{
    flThreadExit = TRUE;

    pSock->close();
    CloseUDP();
    pSockUDPSnd->close();
    pMsgQSemaph->Signal();

    if ( AppPingThread )
    {
        AppPingThread->WaitForMe(INFINITE);
        delete AppPingThread;

        AppPingThread = NULL;
    }

    if ( ChatThread )
    {
        ChatThread->WaitForMe(INFINITE);
        delete ChatThread;

        ChatThread = NULL;
    }

    if( CmdControlThread )
    {
        CmdControlThread->WaitForMe(INFINITE);
        delete CmdControlThread;

        CmdControlThread = NULL;
    }
}


void cxChat::SetClientVersion( LPCSTR _lpszStr )
{
    ajusta_str(_lpszStr, lpszCliVersion, VERSIONLEN);
}

void cxChat::SetUserName( LPCSTR _lpszStr )
{
    ajusta_str(_lpszStr, lpszUser, USERLEN);
}

void cxChat::SetNickName( LPCSTR _lpszStr )
{
    ajusta_str(_lpszStr, lpszNickName, NICKLEN);
}

void cxChat::SetRealName( LPCSTR _lpszStr )
{
    ajusta_str(_lpszStr, lpszRealName, REALLEN);
}

void cxChat::SetHost( LPCSTR _lpszStr )
{
    ajusta_str(_lpszStr, lpszHost, HOSTLEN);
}

void cxChat::SetEmail( LPCSTR _lpszStr )
{
    ajusta_str(_lpszStr, lpszEmail, EMAILLEN);
}

void cxChat::SetPassword( LPCSTR _lpszStr )
{
    ajusta_str(_lpszStr, lpszPassword, PASSWDLEN);
}

void cxChat::SetPortNumber( int nNum )
{
    nPort = nNum;
}


// Se nao for informado ip:port envia para o servidor

void cxChat::UDPPing(LPCSTR _ip, int _port, LPCSTR _data)
{
    char   buf[TMAX_MSG+1];
    LPCSTR pdata;

    if (comp_branco(_ip) || !_port)
    {
        MediaPkt *pMediaPkt;

        pMediaPkt = (MediaPkt *) buf;
        memset(pMediaPkt, '\0', sizeof(MediaPkt));

        pMediaPkt->id   = UDPPONG_PORT;
        pMediaPkt->type = MTYPE_PING;
        pMediaPkt->datalen = strlen(UDPPING_STR)+1;

        strcpy(&buf[sizeof(MediaPkt)], UDPPING_STR);

        pSockUDPSnd->_writeto(buf, sizeof(MediaPkt)+pMediaPkt->datalen);
    }
    else
    {
        sockaddr_in sin;

        if ( SetSockAddrIn(&sin, _ip, _port) != INADDR_NONE )
        {
            pdata = _data;

            if ( comp_branco(pdata) )
                pdata = "";

            // UDPPING IPTarget NickSrc
            sprintf(buf, "UDPPING \"%s\" :%s\r\n", _ip, pdata);

            pSockUDPSnd->_writeto(buf, strlen(buf), &sin);
        }
    }
}


void cxChat::Terminate( void )
{
    RemoveuPnP();

    ::Terminate();
}


void cxChat::AppPong(void)
{
    Wait();

    if ( flAppPing )
    {
        flAppPing = FALSE;
    }

    Signal();
}


BOOL cxChat::InituPnP(void)
{
#ifdef _WIN32
    char szClientIP [IPSTR_MAXLEN+1],
         szuPnPLocIP[IPSTR_MAXLEN+1];


    if ( GetClientIP( szClientIP, IPSTR_MAXLEN ) )
    {
        if ( !GetuPnPLocalIP("UDP", UDPPONG_PORT, szuPnPLocIP, IPSTR_MAXLEN) || _stricmp(szuPnPLocIP, szClientIP)==0 )
        {
            fluPnP = TRUE;

            if ( AdduPnPPort("UDP", szClientIP, UDP_IRM_AVCONF_PORT , "irm_avconf") &&
                 AdduPnPPort("TCP", szClientIP, UDP_IRM_AVCONF_PORT , "irm_avconf") &&
                 AdduPnPPort("UDP", szClientIP, UDP_IRM_MEDIA_PORT  , "irm_media") &&
                 AdduPnPPort("UDP", szClientIP, UDPPONG_PORT, "irm_udppong") )
            {
                _TRACEL( LOGLEVEL1, "cxChat::InituPnP: uPnP initialized successfully." );

                // Ok.
            }
            else
            {
                RemoveuPnP();

                _TRACEL( LOGLEVEL1, "cxChat::InituPnP: uPnP initialization error." );
            }
        }
    }

    return fluPnP;
#else
    return FALSE;
#endif
}


void cxChat::RemoveuPnP(void)
{
#ifdef _WIN32
    if (fluPnP)
    {
        fluPnP = FALSE;

        ResetuPnP("UDP", UDP_IRM_AVCONF_PORT);
        ResetuPnP("TCP", UDP_IRM_AVCONF_PORT);
        ResetuPnP("UDP", UDP_IRM_MEDIA_PORT);
        ResetuPnP("UDP", UDPPONG_PORT);

        _TRACEL( LOGLEVEL1, "cxChat::RemoveuPnP: uPnP reset." );
    }
#endif
}


// Returns:
// FALSE - se nao conectar;
// TRUE  - se conectar;
BOOL cxChat::Connect( LPCSTR lpszHost, int nPort, BOOL _fluPnP, ProxyParam *_pproxyparam )
{
    BOOL   ret;
    SOCKET CtrlSock;

    int    iConnPort;

    LPCSTR pConnAddr,
           pProxyUser,
           pProxyPasswd;


    ret = FALSE;

    if( flConnected )
        return TRUE;

    KillThreads();

    pConnAddr    = ( (_pproxyparam && _pproxyparam->IsRdy()) ? _pproxyparam->GetHost() : lpszHost );
    iConnPort    = ( (_pproxyparam && _pproxyparam->IsRdy() && _pproxyparam->GetPort()) ? _pproxyparam->GetPort() : nPort );
    pProxyUser   = NULL;
    pProxyPasswd = NULL;

    if ( pSock && lpszHost && pSock->isSockOk() )
    {
        ret = TRUE;

        if ( !_pproxyparam || !_pproxyparam->IsRdy() || (!_pproxyparam->IsSocks5() && !_pproxyparam->IsHTTPProxy()) )
        {
            if ( !pSock->connect(pConnAddr, S_PT_TCP, iConnPort, NULL, &CtrlSock, NULL, FALSE) ||
                 !pSock->TestWriteSock(CtrlSock, MAXCONNTIME) )
            {
                ret = FALSE;
            }
        }

        if (ret)
        {
            if ( _pproxyparam && _pproxyparam->IsRdy() && _pproxyparam->IsHTTPProxy() )
            {
                Socks5 s5(pSock);

                if ( !s5.connect ("localhost", S5XS_PORT, _pproxyparam->GetUser(), _pproxyparam->GetPassword(), &CtrlSock) ||
                     !s5.xconnecthttp(lpszHost, nPort, lpszHost, HTTP_PORT, pConnAddr, iConnPort) )
                {
                    ret = FALSE;
                    pSock->close(CtrlSock);
                }
                else
                    pSock->set_non_blocking();
            }
            else if ( _pproxyparam && _pproxyparam->IsRdy() && _pproxyparam->IsSocks5() )
            {
                Socks5 s5(pSock);

                if ( !s5.connect (pConnAddr, iConnPort, _pproxyparam->GetUser(), _pproxyparam->GetPassword(), &CtrlSock) ||
                     !s5.xconnect(lpszHost, nPort) )
                {
                    ret = FALSE;
                    pSock->close(CtrlSock);
                }
                else
                    pSock->set_non_blocking();
            }
            else if ( nPort==HTTP_PORT )
            {
                BOOL flProxy;
                HTTPTunnel http(HTTPOP_POST|HTTPOP_GET, pSock);

                char service[SERVICELEN+1];

                ret     = FALSE;
                flProxy = FALSE;

                if ( _pproxyparam && _pproxyparam->IsRdy() && _pproxyparam->IsHTTPProxy() )
                {
                    flProxy = TRUE;

                    if ( _pproxyparam->CanAuth() )
                    {
                        pProxyUser   = _pproxyparam->GetUser();
                        pProxyPasswd = _pproxyparam->GetPassword();
                    }
                }

                if ( http.SendHTTPInitCmd(lpszHost, S_SERVICE_IRM_CONTROL, 0, 0, pProxyUser, pProxyPasswd, flProxy) )
                {
                    if ( http.WaitForServiceLine(service) )
                    {
                        if ( _stricmp(service, S_SERVICE_IRM_CONTROL) == 0 )
                            ret = TRUE;
                    }
                }

                if (!ret)
                    pSock->close(CtrlSock);
            }
        }

        if (ret)
        {
            ret = FALSE;

            if (_fluPnP)
                InituPnP();

            if (pSockUDPSnd)
                pSockUDPSnd->connect( lpszHost, "UDP", UDPPING_PORT );

            flThreadExit    = FALSE;
            flThreadCmdExit = FALSE;

            //SetServer( lpszHost );
            SetPortNumber( nPort );

            ChatThread = CreateIThread( _DoChatMessage, this, TRUE );

            if ( ChatThread )
            {
                flAppPing = FALSE;
                LastAppPingTime = time(NULL);

                AppPingThread = CreateIThread( _DoAppPing, this, TRUE );

                if ( AppPingThread )
                {
                    CmdControlThread = CreateIThread( _DoCmdControl, pSock );

                    if( CmdControlThread && CmdControlThread->IsThreadOk( ) )
                    {
                        flConnected = TRUE;
                        CmdControlThread->Resume( );

                        if ( pSockUDPSnd && ListenUDP(UDPPONG_PORT) )
                        {
                            UDPPing();
                            UDPPing();
                        }

                        ret = TRUE;
                    }
                }
            }
        }
    }

	return ret;
}

BOOL cxChat::ConnectIRC( LPCSTR _lpszNickname, LPCSTR _lpszEmail, LPCSTR _lpszFullName, LPCSTR _lpszClientVersion, LPCSTR _lpszPassword )
{
    BOOL ret;
    UINT lbuf;
    char lpszHostAux[HOSTLEN+1];
    char lpszBuffer [TAM_LBUFFER+1];

    if( !_lpszNickname || !flConnected || !_lpszClientVersion || !_lpszEmail || !_lpszFullName )
        return FALSE;

    SetUserName  ( _lpszNickname );
    SetNickName  ( _lpszNickname );
    SetRealName  ( _lpszFullName );
    SetEmail     ( _lpszEmail    );
    SetPassword  ( _lpszPassword  );
    GetMyHostName( lpszHostAux, HOSTLEN );
    SetHost      ( lpszHostAux );
    SetClientVersion( _lpszClientVersion );

    //memset( lpszBuffer, 0, TAM_LBUFFER+1 );

    if ( !comp_branco(lpszPassword) )
        _snprintf( lpszBuffer, TAM_LBUFFER, "PASS %s\r\n", lpszPassword);
    else
        *lpszBuffer = '\0';

    lbuf = strlen(lpszBuffer);

    _snprintf( &lpszBuffer[lbuf], TAM_LBUFFER-lbuf, "NICK %s\r\nUSER %s \"%s\" %s :%s", lpszNickName,
               lpszUser, lpszHost, lpszCliVersion, lpszRealName );

    ret = SendMessage( CONNECT_CHAT, (LONG)lpszBuffer );

    if (pSockUDPSnd)
    {
        UDPPing();
        UDPPing();
    }

    UpdateLastCmdTime();

    return ret;
}


void cxChat::Disconnect( void )
{
    RemoveuPnP();

    CloseUDP();

    flConnected = FALSE;

    if (pSock)
    {
        pSock->close();
        flThreadCmdExit = TRUE;
    }
}


void cxChat::UpdateLastCmdTime( void )
{
    Wait();

    flSrvPing   = FALSE;
    LastCmdTime = time(NULL);

    Signal();
}


void cxChat::UpdateLastCmdSentTime( void )
{
    Wait();

    LastCmdSentTime = time(NULL);

    Signal();
}


// Returns:
// TRUE  -> OK ( sent to server )
// FALSE -> error
BOOL cxChat::SendMessage( UINT nOpcode, long lParam, int nSize )
{
    char lpBuf[TAM_LBUFFER+1];
    //char lpBufBase64[TAM_LBUFFER+1];
    int i, k;
    MediaRequest *vr;
    MediaPtP     *vp;
    BOOL ret = FALSE;

    if( !flConnected )
        return ret;

    switch( nOpcode )
    {
////////////////////////////////////////////////////////////////////////////////////////////
// Chat/IRC Messages
////////////////////////////////////////////////////////////////////////////////////////////
        // lParam = LPSTR - String de inicializacao NICK <nickname>\r\nUSER <userid> <Host> <Server> :<FullName>\r\n;
        case CONNECT_CHAT:
            Send( (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam = LPSTR - Answer PING string
        case PONG_CHAT:
            Sendf("PONG :%s", (LPSTR) lParam );
            ret = TRUE;
            break;

        case USER_COMMAND:
            Sendf( "%s", (LPSTR) lParam );
            break;

        case PROTOCOL:
            Sendf( "PROTOCOL %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam = LPSTR - Novo Nick;
        case CHANGE_NICK:
            Sendf( "NICK %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam = LPSTR - Nickname do usuario;
        case WHOIS:
            LPCSTR pNick;

            pNick = (LPSTR) lParam;
            Sendf( "WHOIS %s", pNick );

            ret = TRUE;
            break;

        case IDLE:
            Send( "IDLE" );
            ret = TRUE;
            break;

        // lParam = LPSTR - Nome do canal;
        case PART_CHANNEL:
            LPCSTR pChannel;

            pChannel = (LPSTR) lParam;
            Sendf( "PART %s", pChannel );

            if ( *pChannel == '#' )
                pChannel++;

            if ( _stricmp(ThiscxChat->GetMCastChannel(), pChannel)==0 )
                ThiscxChat->CloseUDP();

            ret = TRUE;
            break;

        case MEDIAVOICE:
            i = 0;
            memset( lpBuf, 0, TAM_LBUFFER + 1);

            while( ((LPSTR)lParam)[i] && ((LPSTR)lParam)[i] != ' ' )
                i++;
            if( i < TAM_LBUFFER )
            {
                strncpy( lpBuf, (LPSTR)lParam, i );
                Sendf( "MEDIAVOICE %s :%s", lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                ret = TRUE;
            }
            break;

        case MEDIASTATS:
            i = 0;
            memset( lpBuf, 0, TAM_LBUFFER + 1);

            while( ((LPSTR)lParam)[i] && ((LPSTR)lParam)[i] != ' ' )
                i++;
            if( i < TAM_LBUFFER )
            {
                strncpy( lpBuf, (LPSTR)lParam, i );

                if ( comp_branco( &((LPSTR)lParam)[i] ) )
                    Sendf( "MEDIASTATS %s", lpBuf );
                else
                    Sendf( "MEDIASTATS %s %s", lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );

                ret = TRUE;
            }
            break;

        case OPER:
            i = 0;
            memset( lpBuf, 0, TAM_LBUFFER + 1);

            while( ((LPSTR)lParam)[i] && ((LPSTR)lParam)[i] != ' ' )
                i++;
            if( i < TAM_LBUFFER )
            {
                strncpy( lpBuf, (LPSTR)lParam, i );
                Sendf( "OPER %s %s", lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                ret = TRUE;
            }
            break;

        case CHANGE_CHANNEL_TOPIC:
            i = 0;
            memset( lpBuf, 0, TAM_LBUFFER + 1);

            while( ((LPSTR)lParam)[i] && ((LPSTR)lParam)[i] != ' ' )
                i++;
            if( i < TAM_LBUFFER )
            {
                strncpy( lpBuf, (LPSTR)lParam, i );
                Sendf( "TOPIC %s :%s", lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                ret = TRUE;
            }
            break;

        // lParam = LPSTR - Mensagem de Saida;
        case QUIT_CHAT:
            Sendf( "QUIT :%s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam = LPSTR - Canal;
        case GET_USER_LIST:
            Sendf( "NAMES %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam = LPSTR - Canal;
        case GET_CHANNEL_LIST:
            if ( lParam )
                Sendf( "LIST %s", (LPSTR) lParam );
            else
                Send( "LIST" );

            ret = TRUE;
            break;

        // lParam - LPSTR - Destinatario :Mensagem
        // lParam = LPSTR - Nome do canal :senha
        case PRIVMSG:
        case PRIVMSG_CHANNEL:
        case NOTICE:
        case WBOARD:
        case APPCMD:
        case WBOARD_CHANNEL:
        case INVITE:
        case JOIN_CHANNEL:
            i = 0;
            memset( lpBuf, 0, TAM_LBUFFER + 1);

            while( ((LPSTR)lParam)[i] && ((LPSTR)lParam)[i] != ' ' )
                i++;
            if( i < TAM_LBUFFER )
            {
                strncpy( lpBuf, (LPSTR)lParam, i );

                switch (nOpcode)
                {
                    case INVITE:
                        Sendf( "%s %s #%s", S_SRVCMD_INVITE, lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                        break;
                    case PRIVMSG:
                    case PRIVMSG_CHANNEL:
                        Sendf( "%s %s :%s", S_SRVCMD_PRIVMSG, lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                        break;
                    case NOTICE:
                        Sendf( "%s %s :%s", S_SRVCMD_NOTICE, lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                        break;
                    case WBOARD:
                    case WBOARD_CHANNEL:
                        Sendf( "%s %s :%s", S_SRVCMD_WBOARD, lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                        break;
                    case APPCMD:
                        Sendf( "%s %s :%s", S_SRVCMD_APPCMD, lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                        break;
                    case JOIN_CHANNEL:
                        Sendf( "JOIN %s :%s", lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                        break;
                }

                //if ( nOpcode == INVITE )
                //    Sendf( "%s %s #%s", S_SRVCMD_INVITE, lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );
                //else
                //    Sendf( "%s %s :%s", ( nOpcode == PRIVMSG_CHANNEL ? S_SRVCMD_PRIVMSG : S_SRVCMD_WBOARD ), lpBuf, (LPSTR) &((LPSTR)lParam)[i+1] );

                ret = TRUE;
            }
            break;

        // lParam = LPSTR - Who;
        case CONTACT_ADD:
            Sendf( "PRIVMSG NickServ :CONTACT ADD %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam = LPSTR - Who;
        case CONTACT_DELETE:
            Sendf( "PRIVMSG NickServ :CONTACT DEL %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        case CONTACT_LIST:
            Send( "PRIVMSG NickServ :CONTACT LIST" );
            ret = TRUE;
            break;

        // lParam = LPSTR - password;
        case IDENTIFY:
            Sendf( "PRIVMSG NickServ :IDENTIFY %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam = LPSTR - password;
        case REGISTER:
            Sendf( "PRIVMSG NickServ :REGISTER %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam - LPSTR - #channel kicked-nick :[reason]
        case KICK_USER:
            i = 0;  // kicked-nick id
            k = 0;  // reason id
            memset( lpBuf, 0, TAM_LBUFFER + 1);

            while( ((LPSTR)lParam)[i] && ((LPSTR)lParam)[i] != ' ' )
                i++;
            if( i < TAM_LBUFFER )
            {
                strncpy( lpBuf, (LPSTR)lParam, i+1 );
            }

            if( ((LPSTR)lParam)[i] )
                k = ++i;

            while( ((LPSTR)lParam)[k] && ((LPSTR)lParam)[k] != ' ' )
                k++;
            if( k < TAM_LBUFFER )
            {
                strncat( lpBuf, &((LPSTR)lParam)[k], k-i );
                Sendf( "KICK %s :%s", lpBuf, (LPSTR) &((LPSTR)lParam)[k+1] );
                ret = TRUE;
            }
            break;

        case MODE_USER:
            Sendf( "MODE %s", (LPSTR) lParam );
            ret = TRUE;
            break;

        // lParam - LPSTR - #channel +/-flags nick1 nick2 ... nickN
        case MODE:
            i = 0;
            k = 0;
            memset( lpBuf, 0, TAM_LBUFFER + 1);

            while( ((LPSTR)lParam)[i] && ((LPSTR)lParam)[i] != ' ' )
                i++;
            if( i < TAM_LBUFFER )
            {
                strncpy( lpBuf, (LPSTR)lParam, i+1 );
            }

            if( ((LPSTR)lParam)[i] )
                k = ++i;

            while( ((LPSTR)lParam)[k] && ((LPSTR)lParam)[k] != ' ' )
                k++;
            if( k < TAM_LBUFFER )
            {
                strncat( lpBuf, &((LPSTR)lParam)[k], k-i );
                Sendf( "MODE %s %s", lpBuf, (LPSTR) &((LPSTR)lParam)[k+1] );
                ret = TRUE;
            }
            break;

////////////////////////////////////////////////////////////////////////////////////////////
// Media Messages
////////////////////////////////////////////////////////////////////////////////////////////
        // MEDIASND #Channel StrID Protocol PacketLen $HeaderLen HEADER
        case MEDIASND:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                i = TAM_LBUFFER;
                //encode64( (char *)vr->GetHeaderBuffer( ), vr->GetHeaderLen( ), lpBufBase64, &i );
                if( i )
                {
                    //lpBufBase64[i] = 0;
                    if ( vr->GetHeaderLen() && vr->GetHeaderBuffer() )
                    {
                        _snprintf( lpBuf, TAM_LBUFFER, "MEDIASND %s %s %s %d $%d %s", vr->GetChannel( ),
                                                                                      vr->GetMediaClass( ),
                                                                                      vr->GetProtocol( ),
                                                                                      vr->GetPacketLen( ),
                                                                                      vr->GetHeaderLen( ),
                                                                                      vr->GetHeaderBuffer( ) );
                                                                                      //i,
                                                                                      //lpBufBase64 );
                    }
                    else
                    {
                        _snprintf( lpBuf, TAM_LBUFFER, "MEDIASND %s %s %s %d", vr->GetChannel( ),
                                                                               vr->GetMediaClass( ),
                                                                               vr->GetProtocol( ),
                                                                               vr->GetPacketLen( ) );
                    }

                    Send( lpBuf );
                    ret = TRUE;
                }
            }
            break;

        // lParam = LPSTR - ID;
        case MEDIASNDSTOP:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIASNDSTOP %lu", vr->GetMediaID() );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        // lParam = LPSTR - ID;
        case MEDIARESET:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIARESET %lu", vr->GetMediaID() );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        case MEDIAMUTE:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIAMUTE %d %s", vr->GetMediaID(), vr->GetMediaNotify() );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        // lParam = NULL;
        case MEDIAQUERY:
            Sendf( "MEDIAQUERY" );
            ret = TRUE;
            break;

        // lParam = LPSTR - ID Protocol [Port];
        case MEDIARCV:
            //MEDIARCV ID Protocol [Port]
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIARCV %d %s %d", vr->GetMediaID( ), vr->GetProtocol( ), vr->GetMediaPort( ) );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        case MEDIARCVALL:
            //MEDIARCVALL #Channel MediaClassStr Protocol [Port]

            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIARCVALL %s %s %s %d", vr->GetChannel( ), vr->GetMediaClass( ), vr->GetProtocol( ), vr->GetMediaPort( ) );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        // lParam = LPSTR - ID;
        case MEDIARCVSTOP:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIARCVSTOP %d", vr->GetMediaID( ) );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        case MEDIARCVALLSTOP:
            //MEDIARCVALLSTOP #Channel MediaClassStr

            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIARCVALLSTOP %s %s", vr->GetChannel( ), vr->GetMediaClass( ) );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        // lParam = LPSTR - MEDIARCVRUN ID;  // Apenas para conexoes UDP
        case MEDIARCVRUN:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIARCVRUN %d", vr->GetMediaID( ) );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        // MEDIAPKTREQUEST ID seqi seqf
        case MEDIAPKTREQUEST:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIAPKTREQUEST %d %d %d", vr->GetMediaID(), vr->GetData1(), vr->GetData2() );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        case MEDIARCVALLRUN:
            //MEDIARCVALLRUN #Channel MediaClassStr

            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIARCVALLRUN %s %s", vr->GetChannel( ), vr->GetMediaClass( ) );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        // lParam - LPSTR - MEDIASET MediaID IDENT :Data
        case MEDIASET:
            vr = (MediaRequest*)lParam;

            if( vr )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIASET %d %s :%s", vr->GetMediaID( ),
                                                                     vr->GetMediaSetOption( ),
                                                                     vr->GetName( ) );
                Send( lpBuf );
                ret = TRUE;
            }
            break;

        //MEDIAPTPINVITE NickTarget mediaclassStr IPLocalSource portRcvSource
        //portSndSource portRcvUDPPingSource bitrateSource
        //MEDIAPTPACK NickTarget mediaclassStr IPLocalSource portRcvSource
        //portSndSource portRcvUDPPingSource bitrateSource
        case MEDIAPTPINVITE:
        case MEDIAPTPACK:
            vp = (MediaPtP*)lParam;

            if( vp )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "%s %s %s %s %d %d %d %d", nOpcode == MEDIAPTPINVITE ? "MEDIAPTPINVITE" : "MEDIAPTPACK",
                                                                          vp->GetNickTarget( ),
                                                                          vp->GetMediaClass( ),
                                                                          vp->GetIPLocalSource( ),
                                                                          vp->GetRcvSourcePort( ),
                                                                          vp->GetSendSourcePort( ),
                                                                          vp->GetUDPPingSourcePort( ),
                                                                          vp->GetSourceBitrate( ) );

                Send( lpBuf );
                ret = TRUE;
            }
            break;


        //MEDIAPTPREJECT NickTarget mediaclassStr
        case MEDIAPTPREJECT:
            vp = (MediaPtP*)lParam;

            if( vp )
            {
                _snprintf( lpBuf, TAM_LBUFFER, "MEDIAPTPREJECT %s %s", vp->GetNickTarget( ),
                                                                       vp->GetMediaClass( ) );

                Send( lpBuf );
                ret = TRUE;
            }
            break;

        default:
            ret = FALSE;
            break;
    }

    if (ret)
        UpdateLastCmdSentTime();

    return ret;
}

//BOOL cxChat::GetMessage( MessageData *lpMsgData )
MessageData * cxChat::GetMessage(void)
{
    //BOOL ret = FALSE;
    MessageData *lpMd;


    lpMd = NULL;

    pMsgQSemaph->Wait( );

    if (flThreadExit)
        return FALSE;

    Wait();

    /*
    if( ( lpMd = pMessageList->first( ) ) && ( lpMsgData ) )
    {
        CopyMessage( lpMsgData, lpMd );
        DeleteMessage( lpMd );

        ret = TRUE;
    }
    else
    {
        CopyMessage( lpMsgData, NULL );
    }
    */

    lpMd = pMessageList->pop();

    Signal();

    return lpMd;
}

BOOL cxChat::AddMessage( UINT nCode, LPCSTR lpszSource, LPCSTR lpszTarget, long *lParam, LPCSTR lpszFullCmd )
{
    BOOL ret = FALSE;
    MessageData *lpMd;

    if( nCode )
    {
        lpMd = new MessageData;

        Wait( );

        if( lpMd )
        {
            lpMd->nOpCode = nCode;

            lpMd->lpszSource  = StrAllocAttrib(lpszSource);
            lpMd->lpszTarget  = StrAllocAttrib(lpszTarget);
            lpMd->lpszFullCmd = StrAllocAttrib(lpszFullCmd);

            switch( nCode )
            {
                case PRIVMSG:
                case PRIVMSG_CHANNEL:
                case WBOARD:
                case WBOARD_CHANNEL:
                case APPCMD:
                case GET_USER_LIST:
                case GET_CHANNEL_LIST:
                case CHANGE_CHANNEL_TOPIC:
                case CMD_ERROR:
                case KICK_USER:
                case MODE:
                case TOPIC_WHO:
                case INVITE:
                case IDENTIFY_ERROR:
                case REGISTER_ERROR:
                case CONTACT_ERROR:
                case WHOIS:
                case PASSWORD:
                case PROTOCOL:
                    if( lParam )
                        lpMd->lParam = new cxString( (char *)lParam );
                break;

                case MEDIASND:
                case MEDIASNDSTOP:
                case MEDIARESET:
                case MEDIARCV:
                case MEDIARCVSTOP:
                case MEDIARCVRUN:
                case MEDIASET:
                case MEDIANOTIFY:
                case MEDIAMUTE:
                case MEDIARCVALL:
                case MEDIARCVALLRUN:
                case MEDIARCVALLSTOP:
                case MEDIASETPOS:
                case MEDIAPKTREQUEST:
                case MEDIA:
                case SERVER_REDIR:
                    if( lParam )
                    {
                        if( lpMd->lParam = new MediaRequest )
                        {
                            (*(MediaRequest *)lpMd->lParam) = *(MediaRequest *)lParam;
                        }
                    }

                break;

                case MEDIAQUERY:

                    if( lParam )
                    {
                        if( lpMd->lParam = new MediaQuery )
                        {
                            (*(MediaQuery *)lpMd->lParam) = *(MediaQuery *)lParam;
                        }
                    }

                break;

                case MEDIAPTPINVITE:
                case MEDIAPTPACK:
                case MEDIAPTPREJECT:
                    if( lParam )
                    {
                        if( lpMd->lParam = new MediaPtP )
                        {
                            (*(MediaPtP *)lpMd->lParam) = *(MediaPtP *)lParam;
                        }
                    }
                break;

                case CONNECTED_CHAT:
                case CHANGE_NICK:
                case PING_CHAT:
                case APP_PING:
                case UDP_PING:
                case UDP_PONG:
                case OPER:
                case MEDIAQUERYEND:
                case MEDIAVOICE:
                case ENDOF_CHANNEL_LIST:
                case ENDOF_WHOIS:
                case NOWAWAY:
                case UNAWAY:
                case MEDIASTATS:

                default:
                    lpMd->lParam = NULL;
                break;
            }

            pMessageList->append( lpMd );
            
            pMsgQSemaph->Signal( );

            ret = TRUE;
        }

        Signal( );
    }
    return ret;
}

/*
BOOL cxChat::CopyMessage( MessageData *lpMsgData, MessageData *lpMd )
{
    BOOL ret = FALSE;

    if( lpMsgData )
    {
        if( !lpMd )
        {
            lpMsgData->nOpCode = -1;
        }
        else
        {
            lpMsgData->nOpCode = lpMd->nOpCode;

            lpMsgData->lpszSource  = StrReAllocAttrib(lpMd->lpszSource , lpMsgData->lpszSource);
            lpMsgData->lpszTarget  = StrReAllocAttrib(lpMd->lpszTarget , lpMsgData->lpszTarget);
            lpMsgData->lpszFullCmd = StrReAllocAttrib(lpMd->lpszFullCmd, lpMsgData->lpszFullCmd);

            if( lpMd->lParam )
            {
                switch( lpMd->nOpCode )
                {
                    case PRIVMSG:
                    case PRIVMSG_CHANNEL:
                    case WBOARD:
                    case WBOARD_CHANNEL:
                    case GET_USER_LIST:
                    case GET_CHANNEL_LIST:
                    case CHANGE_CHANNEL_TOPIC:
                    case CMD_ERROR:
                    case KICK_USER:
                    case MODE:
                    case TOPIC_WHO:
                    case INVITE:
                    case IDENTIFY_ERROR:
                    case REGISTER_ERROR:
                    case CONTACT_ERROR:
                    case WHOIS:
                    case PASSWORD:
                        if( lpMsgData->lParam )
                            delete lpMsgData->lParam;

                        lpMsgData->lParam = new cxString( ((cxString*)lpMd->lParam)->GetStr( ) );
//                        lpMsgData->lParam = new cxString;
//                        ((cxString*)lpMsgData->lParam)->SetStr(  ((cxString*)lpMd->lParam)->GetStr( ) );
                    break;

                    case MEDIASND:
                    case MEDIASNDSTOP:
                    case MEDIARESET:
                    case MEDIARCV:
                    case MEDIARCVSTOP:
                    case MEDIARCVRUN:
                    case MEDIASET:
                    case MEDIANOTIFY:
                    case MEDIAMUTE:
                    case MEDIARCVALL:
                    case MEDIARCVALLRUN:
                    case MEDIARCVALLSTOP:
                    case MEDIASETPOS:
                    case MEDIAPKTREQUEST:
                    case MEDIA:
                    case SERVER_REDIR:
                        if( lpMsgData->lParam )
                            delete lpMsgData->lParam;

                        if( lpMsgData->lParam = new MediaRequest )
                        {
                            *(MediaRequest *)(lpMsgData->lParam) = *(MediaRequest *)(lpMd->lParam);
                        }
                    break;

                    case MEDIAQUERY:
                        if( lpMsgData->lParam )
                            delete lpMsgData->lParam;

                        if( lpMsgData->lParam = new MediaQuery )
                        {
                            *(MediaQuery *)(lpMsgData->lParam) = *(MediaQuery *)(lpMd->lParam);
                        }
                    break;

                    case MEDIAPTPINVITE:
                    case MEDIAPTPACK:
                    case MEDIAPTPREJECT:
                        if( lpMsgData->lParam )
                            delete lpMsgData->lParam;

                        if( lpMsgData->lParam = new MediaPtP )
                        {
                            *(MediaPtP *)(lpMsgData->lParam) = *(MediaPtP *)(lpMd->lParam);
                        }
                    break;

                    case CONNECTED_CHAT:
                    case CHANGE_NICK:
                    case PING_CHAT:
                    case APP_PING:
                    case UDP_PING:
                    case UDP_PONG:
                    case OPER:
                    case MEDIAQUERYEND:
                    case MEDIAVOICE:
                    case ENDOF_CHANNEL_LIST:
                    case ENDOF_WHOIS:
                    case NOWAWAY:
                    case UNAWAY:
                    case MEDIASTATS:

                    default:
                        if( lpMsgData->lParam )
                            delete lpMsgData->lParam;

                        lpMsgData->lParam = NULL;
                    break;
                }
            }
        }

        ret = TRUE;
    }

    return ret;
}
*/


BOOL cxChat::DeleteMessage( MessageData *lpMd )
{
    return pMessageList->erase( lpMd );
}

BOOL cxChat::CreateChannelUserList( LPSTR lpszChannel, LPSTR lpszUserList )
{
    BOOL ret = FALSE;
    ChannelUserList *pCUL,
                    *pCurrentCUL = NULL;
    int i, k;
    char *lpszTempStr;

    if( !lpszChannel || !lpszUserList )
        return ret;

    // Verifico se jah existe uma Lista Criada para o canal
    for( pCUL = pChannelUserList->first( ); pCUL; pCUL = pChannelUserList->next( ) )
    {
        if( !_stricmp( pCUL->lpszChannel, lpszChannel ) )
            pCurrentCUL = pCUL;
    }

    if( !pCurrentCUL )
    {
        pCurrentCUL = new ChannelUserList;
        pCurrentCUL->lpszChannel  = StrAllocAttrib(lpszChannel);
        pCurrentCUL->lpszUserList = StrAllocAttrib(lpszUserList);
        pChannelUserList->append( pCurrentCUL );
        ret = TRUE;
    }
    else
    {
        i  = strlen( pCurrentCUL->lpszUserList );
        k  = strlen( lpszUserList ) + 1;

        lpszTempStr = new char[i+k+1];

        strcpy( lpszTempStr, pCurrentCUL->lpszUserList );
        strcat( lpszTempStr, " " );
        strcat( lpszTempStr, lpszUserList );

        delete [] pCurrentCUL->lpszUserList;

        pCurrentCUL->lpszUserList = lpszTempStr;
        ret = TRUE;
    }
    return ret;
}

BOOL cxChat::DeleteChannelUserList( LPSTR lpszChannel )
{
    BOOL ret = FALSE;
    ChannelUserList *pCUL;

    if( !lpszChannel )
        return ret;

    ret = TRUE;

    for( pCUL = pChannelUserList->first( ); pCUL; pCUL = pChannelUserList->next( ) )
    {
        if( !_stricmp( pCUL->lpszChannel, lpszChannel ) )
            pChannelUserList->erase( pCUL );
    }
    return ret;
}

BOOL cxChat::GetChannelUserList( LPSTR lpszChannel, LPSTR *lpszUserList )
{
    BOOL ret = FALSE;
    ChannelUserList *pCUL;

    if( !lpszChannel )
        return ret;

    ret = TRUE;

    for( pCUL = pChannelUserList->first( ); pCUL; pCUL = pChannelUserList->next( ) )
    {
        if( !_stricmp( pCUL->lpszChannel, lpszChannel ) && pCUL->lpszUserList )
        {
            *lpszUserList = StrAllocAttrib(pCUL->lpszUserList);
            break;
        }
    }
    return ret;
}

BOOL cxChat::AddMediaQuery( MediaQuery *vq )
{
    BOOL ret = TRUE;

    // Sempre incluo o MediaQuery na ultima lista;
    MediaQueryList *pLastList, *pList;

    Wait( );

    // Caminho ateh a ultima lista;
    //for( pLastList = pList = pMediaQueryList->first( ); pList; pList = pMediaQueryList->next( ) )
    //    pLastList = pList;

    pLastList = pMediaQueryList->last();

    if( pLastList )
        pLastList->AddMediaQuery( vq );
    else
    {
        pList = new MediaQueryList;
        pList->AddMediaQuery( vq );
        pMediaQueryList->append( pList );
    }

    Signal( );

    return ret;
}

BOOL cxChat::GetNextMediaQuery( MediaQuery *vq )
{
    BOOL ret = FALSE;
    MediaQueryList *pList;

    Wait( );

    pList = pMediaQueryList->first( );

    if( pList )
    {
        pList->GetNextMediaQuery( vq );
        if( vq->GetMediaID( ) == -1 )
        {
            pMediaQueryList->erase( pList );
        }
        else
            ret = TRUE;
    }
    Signal( );

    return ret;
}

BOOL cxChat::Sendf( char *lpszBuffer, ... )
{
    va_list marker;
    char buf[TAM_LBUFFER+1];

    va_start( marker, lpszBuffer );
    vsprintf( buf, lpszBuffer, marker );
    va_end( marker );
    return Send( buf );
}


BOOL cxChat::Send( LPSTR lpszStr )
{
    if( lpszStr )
        return pSock->writeLine( lpszStr );
    return SOCKET_ERROR;
}


/*
void cxChat::CloseUDP(BOOL _flWaitForThreadEnd)
{
    *lpszMCastChannel = '\0';

    if ( pSockUDP && IsUDPListening() )
    {
        pSockUDP->close();
        flUDPListening = FALSE;
    }

    if ( UDPThread && _flWaitForThreadEnd )
    {
        UDPThread->WaitForMe(INFINITE);
        delete UDPThread;

        UDPThread = NULL;
    }
}
*/

void cxChat::CloseUDP(void)
{
    *lpszMCastChannel = '\0';

    if ( UDPThread )
    {
        flUDPThreadExit = TRUE;

        UDPThread->WaitForMe(INFINITE);
        delete UDPThread;

        UDPThread = NULL;
    }

    if ( pSockUDP && IsUDPListening() )
    {
        pSockUDP->close();
        flUDPListening = FALSE;
    }
}


BOOL cxChat::InitUDPThread(void)
{
    BOOL ret;

    ret = FALSE;

    if ( !UDPThread && pSockUDP && pSockUDP->isSockOk() )
    {
        flUDPThreadExit = FALSE;

        UDPThread = CreateIThread( _DoUDP, this, TRUE );

        if ( UDPThread )
            ret = TRUE;
    }

    return ret;
}


BOOL cxChat::ListenUDP(int _rcvPort, LPCSTR _MulticastIP)
{
    BOOL ret;

    ret = FALSE;

    if ( !IsUDPListening() && pSockUDP && pSockUDP->isSockOk() && pSockUDP->listen("UDP", 1024, _rcvPort, NULL) )
    {
        if ( comp_branco(_MulticastIP) || pSockUDP->AddMembership(_MulticastIP) )
        {
            if ( InitUDPThread() )
            {
                ret = TRUE;
                flUDPListening = TRUE;
            }
        }

        if ( !IsUDPListening() )
            pSockUDP->close();
    }

    return ret;
}


BOOL cxChat::InitMulticastChannel(LPCSTR _channel, LPCSTR _MulticastIP, int _port)
{
    BOOL   ret;

    ret = FALSE;

    if (!comp_branco(_channel) && !comp_branco(_MulticastIP) && _port && pSockUDP && pSockUDP->isSockOk())
    {
        CloseUDP();

        if ( ListenUDP(_port, _MulticastIP) )
        {
            ajusta_str(_channel, lpszMCastChannel, CHANNELLEN);
            ret = TRUE;
        }
    }

    return ret;
}


BOOL cxChat::InitViewer (int _rcvPort, LPCSTR _MulticastIP)
{
    BOOL ret;

    ret = FALSE;

    if ( !IsUDPListening() )
    {
        flThreadExit = FALSE;

        if ( ListenUDP(_rcvPort, _MulticastIP) )
        {
            if ( !ChatThread )
                ChatThread = CreateIThread( _DoChatMessage, this, TRUE );

            if ( ChatThread )
                ret = TRUE;
            else
            {
                CloseUDP();
                flThreadExit = TRUE;
            }
        }
    }

    return ret;
}


void cxChat::CloseViewer(void)
{
    KillThreads();
}


BOOL cxChat::GetClientIP(LPSTR _pIPStr, UINT _IPMaxLen)
{
    BOOL ret;

    ret = FALSE;

    if ( pSock->IsConnected() && GetSockName(pSock->GetMasterSocket(), _pIPStr, _IPMaxLen, NULL) )
    {
        ret = TRUE;
    }

    return ret;
}

