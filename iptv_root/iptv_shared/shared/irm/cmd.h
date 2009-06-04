#ifndef _CMD_STR
#define _CMD_STR


#include "shared/cmd_base.h"
#include "mediapkt.h"


extern Cmd_str srvcmd_vet[];
extern Cmd_str cmd_config[];
extern Cmd_str cmd_ports_vet[];
extern Cmd_str autoupdcmd_vet[];
extern Cmd_str dbms_names[];

extern LPCSTR mediaClassDefault_vet[];
extern LPCSTR mediaClassPing_vet[];


enum ConfigDefs
{
    CONF_NULL,
    CONF_ADMIN,
    CONF_CONNECTTO,
    CONF_HUB,
    CONF_CLIENTAUTH,
    CONF_NAME,
    CONF_LISTENFROM,
    CONF_OPER,
    CONF_SERVICES,
    CONF_CONNCLASS,
    CONF_CHANNELDEF,
    CONF_RESERVEDNICK,
    CONF_MCLASSDEF,
    CONF_DEBUG,
    CONF_PORT,
    CONF_SECURITY,
    CONF_SRVREDIR,
    CONF_DBMSLOG,
    CONF_XSERVICE,
};


enum PortDefs
{
    PORT_NULL,
    PORT_USER,
    PORT_SRVNET,
    PORT_MEDIASRVTCP,
    PORT_MEDIARCVUDP,
    PORT_MEDIARCVTCP,
    PORT_MEDIASNDTCP,
    PORT_DEMOSERVERTCP,
    PORT_SOCKS5SRVTCP,
    PORT_AUTOUPDATETCP,
    PORT_HTTPSERVER,
    PORT_SOCKS5HTTP,
    PORT_HTTPTUNNEL,
};


enum TranscoderMode
{
    TRANSMODE_NULL=0,
    TRANSMODE_POOL,
    TRANSMODE_CALLBACK,
};



#define TMAX_CMD    15


enum srvcmd
{
    C_NULL=0       ,
    C_ADMIN=1000   ,
    C_APPCMD       ,
    C_AUTH         ,
    C_AUTOUPDCONFIG,
    C_AUTOUPDQUERY ,
    C_AWAY         ,
    C_CLOSELINK    ,
    C_CLOSESLINK   ,
    C_CMDERROR     ,
    C_CHCLOSE      ,
    C_CHDISABLE    ,
    C_CHENABLE     ,
    C_CHENLIST     ,
    C_CHOPEN       ,
    C_CONFIG       ,
    C_CONNECT      ,
    C_DEBUGLEVEL   ,
    C_DIE          ,
    C_GLOBOPS      ,
    C_GNOTICE      ,
    C_IDLE         ,
    C_INFO         ,
    C_INVITE       ,
    C_ISON         ,
    C_JOIN         ,
    C_KICK         ,
    C_KILL         ,
    C_LINKS        , 
    C_LIST         ,
    C_LUSERS       ,
    C_MODE         ,
    C_NAMES        ,
    C_NCHANNEL     ,
    C_NICK         ,
    C_NJOIN        ,
    C_NOTICE       ,
    C_OPER         ,
    C_PART         ,
    C_PASSWORD     ,
    C_PING         ,
    C_PONG         ,
    C_PORTQUERY    ,
    C_PRIVMSG      ,
    C_PROTOCOL     ,
    C_QUIT         ,
    C_RAW          ,
    C_REHASH       ,
    C_SAY          ,
    C_SECURITY     ,
    C_SERVER       ,
    C_SERVERREDIR  ,
    C_SERVICE      ,
    C_SHELL        ,
    C_SHUTDOWN     ,
    C_SNIFF        ,
    C_SPING        ,
    C_SPONG        ,
    C_SQUIT        ,
    C_STATS        ,
    C_SVSMODE      ,
    C_TOPIC        ,
    C_TOPICWHO     ,
    C_TRACE        ,
    C_UDPOFF       ,
    C_UDPON        ,
    C_UDPPING      ,
    C_UDPPONG      ,
    C_USER         ,
    C_MEDIA        ,
    C_MEDIAHEADER  ,
    C_MEDIADEBUG      ,
    C_MEDIAKILL       ,
    C_MEDIAMUTE       ,
    C_MEDIANOTIFY     ,
    C_MEDIAPTPACK     ,
    C_MEDIAPTPINVITE  ,
    C_MEDIAPTPREJECT  ,
    C_MEDIAPKTREQUEST ,
    C_MEDIAQUERY      ,
    C_MEDIARCV        ,
    C_MEDIARCVALL     ,
    C_MEDIARCVALLLIST ,
    C_MEDIARCVLIST    ,
    C_MEDIARCVRUN     ,
    C_MEDIARCVALLRUN  ,
    C_MEDIARCVSTOP    ,
    C_MEDIARCVALLSTOP ,
    C_MEDIARESET      ,
    C_MEDIASET        ,
    C_MEDIASETPOS     ,
    C_MEDIASND        ,
    C_MEDIASNDSTOP    ,
    C_MEDIASTATS      ,
    C_MEDIATALK       ,
    C_MEDIAVOICE      ,
    C_VOICEQUEUE      ,
    C_VOICEQUEUEADD   ,
    C_VOICEQUEUEDEL   ,
    C_VOICEQUEUELIST  ,
    C_USERHOST        ,
    C_WALLOPS         ,
    C_WBOARD          ,
    C_WHO             ,
    C_WHOIS           ,
};

enum autoupdcmd
{
    C_AUPDCMD_NULL=0     ,
    C_AUPDCMD_BYE=1000   ,
    C_AUPDCMD_ENDOFLIST  ,
    C_AUPDCMD_EOF        ,
    C_AUPDCMD_ERROR      ,
    C_AUPDCMD_FILE       ,
    C_AUPDCMD_GET        ,
    C_AUPDCMD_HELLO      ,
    C_AUPDCMD_IDLE       ,
    C_AUPDCMD_LIST       ,
    C_AUPDCMD_LISTRPL    ,
    C_AUPDCMD_QUIT       ,
    C_AUPDCMD_REDIR      ,
    C_AUPDCMD_SENDING    ,
};


enum dmbsnames
{
    DBMSNAME_NULL=0      ,
    DMBSNAME_MYSQL=1000  ,
};


enum protocolType
{
    PT_NULL       = 0,
    PT_TCP        = 1,
    PT_UDP        = 2,
    PT_HTTP       = 3,
    PT_SIP        = 4,
    PT_H323       = 5,
};


enum FlagSignals
{
    FS_NONE       =  0,
    FS_POSITIVE   =  1,
    FS_NEGATIVE   = -1,
};


//#define NMAX_MEDIACLASSES   10
#define NMAX_MEDIACLASSES   9

enum mediaClass
{
    MC_NULL       = 0,
    MC_SCREEN     = 1,
    MC_CAM        = 2,
    MC_VOICEIP    = 3,
    MC_MOVIE      = 4,
    MC_CONFERENCE = 5,
    MC_FTP        = 6,
    MC_CONTROL    = 7,
    MC_AVCONF     = 8,
    MC_APP        = 9,
};


enum mediaDebug
{
    MD_NULL       =  0,
    MD_CTRL       =  1,
    MD_PING       =  2,
    MD_SAVEFILE   =  3,
    MD_LOGERRORS  =  4,
    MD_STATS      =  5,
    MD_ALL        = 10,
};


enum dbgLevel
{
    DBGLVL_LOGLEVEL0  =  0,
    DBGLVL_LOGLEVEL1  =  1,
    DBGLVL_LOGLEVEL2  =  2,
    DBGLVL_LOGLEVEL3  =  3,
    DBGLVL_LOGLEVEL4  =  4,
    DBGLVL_LOGLEVEL5  =  5,
    DBGLVL_SEMAPHORES =  7,
    DBGLVL_ALL        = 10,
};


enum logLevel
{
    LOG_NULL      =  0x00,
    LOG_USERS     =  0x01,
    LOG_CHANNELS  =  0x02,
    LOG_SERVERS   =  0x04,
};


enum mediaOnErrorActions
{
    MOEA_DEFAULT  = 0x00,
    MOEA_USESENDQ = 0x01,
};


enum mediaNotify
{
    VN_NULL     = 0,
    VN_SEND     = 1,
    VN_START    = 2,
    VN_STOP     = 3,
    VN_PLAY     = 4,
    VN_BLOCK    = 5,
    VN_VOICE    = 6,
    VN_MUTE     = 7,
    VN_ON       = 8,
    VN_OFF      = 9,
    VN_ERROR    = 10,
};


enum serviceNames
{
    SERVICE_NULL = 0,
    SERVICE_AUTOUPDATE,
    SERVICE_SOCKS5,
    SERVICE_IRM,
    SERVICE_IRM_CONTROL,
    SERVICE_IRM_MEDIARCV,
    SERVICE_IRM_MEDIASND,
    SERVICE_HTTPTUNNEL,
    SERVICE_HTTPPROXY,
};


enum mediaSet
{
    VS_NULL        ,
    VS_NAME        ,
};


int    search_protocol   (LPCSTR key);
int    search_portclass  (LPCSTR key);
int    search_mediaclass (LPCSTR key);
LPCSTR search_mediaclass (UINT _code);
int    search_servicename(LPCSTR key);
int    search_intservice (LPCSTR key);
int    search_xservice   (LPCSTR key);
int    search_dbmsname   (LPCSTR key);
int    search_mediaset   (LPCSTR key);
int    get_nmediaclasses (void);
BOOL   check_bitrate     (UINT _val);
UINT   get_le_bitrate    (UINT _val);


#endif    //_CMD_STR
