#ifndef __SRV_DEFS__
#define __SRV_DEFS__


// Protocol Types

#define S_PT_TCP              "TCP"
#define S_PT_UDP              "UDP"
#define S_PT_HTTP             "HTTP"
#define S_PT_SIP              "SIP"
#define S_PT_H323             "H323"
#define S_PT_MCAST            "MCAST"
#define S_PT_NULL             "NULL"


// Media Classes

#define S_MC_SCREEN           "SCREEN"
#define S_MC_CAM              "CAM"
#define S_MC_VOICEIP          "VOICEIP"
#define S_MC_MOVIE            "MOVIE"
#define S_MC_CONFERENCE       "CONFERENCE"
#define S_MC_FTP              "FTP"
#define S_MC_CONTROL          "CONTROL"
#define S_MC_AVCONF           "AVCONF"
#define S_MC_APP              "APP"


// Media Notify

#define S_VN_SEND             "SEND"
#define S_VN_START            "START"
#define S_VN_STOP             "STOP"
#define S_VN_PLAY             "PLAY"
#define S_VN_BLOCK            "BLOCK"
#define S_VN_VOICE            "VOICE"
#define S_VN_MUTE             "MUTE"
#define S_VN_ERROR            "ERROR"
#define S_VN_ALL              "ALL"
#define S_VN_ON               "ON"
#define S_VN_OFF              "OFF"
#define S_VN_NULL             "NULL"


// Media Set

#define S_VS_NAME             "NAME"


// Commands - ELCC Config

#define SCONF_NULL           ""
#define SCONF_ADMIN          "A"
#define SCONF_DEBUG          "B"
#define SCONF_CONNECTTO      "C"
#define SCONF_CHANNELDEF     "D"
#define SCONF_HUB            "H"
#define SCONF_CLIENTAUTH     "I"
#define SCONF_DBMSLOG        "L"
#define SCONF_NAME           "M"
#define SCONF_LISTENFROM     "N"
#define SCONF_OPER           "O"
#define SCONF_PORT           "P"
#define SCONF_RESERVEDNICK   "Q"
#define SCONF_SRVREDIR       "R"
#define SCONF_SECURITY       "S"
#define SCONF_MCLASSDEF      "T"
#define SCONF_SERVICES       "U"
#define SCONF_XSERVICE       "X"
#define SCONF_CONNCLASS      "Y"


// Comandos - Auto Update Config

#define AUPDCONF_BITRATE     'B'
#define AUPDCONF_APP         'A'
#define AUPDCONF_FILE        'F'



// Definicao de classes de portas

#define SPORT_NULL             ""
#define SPORT_USER             "UsertPort"
#define SPORT_SRVNET           "SrvNetPort"
#define SPORT_MEDIASRVTCP      "MediaSrvTCPPort"
#define SPORT_MEDIARCVUDP      "MediaRcvUDPPort"
#define SPORT_MEDIARCVTCP      "MediaRcvTCPPort"
#define SPORT_MEDIASNDTCP      "MediaSndTCPPort"
#define SPORT_DEMOSERVERTCP    "DemoServerTCPPort"
#define SPORT_SOCKS5SRVTCP     "Socks5SrvTCPPort"
#define SPORT_AUTOUPDATETCP    "AutoUpdateTCPPort"
#define SPORT_HTTPSERVER       "HTTPSrvPort"
#define SPORT_SOCKS5HTTP       "Socks5HTTPSrvTCPPort"
#define SPORT_HTTPTUNNEL       "HTTPTunnelSrvTCPPort"

#define S_SERVICE_AUTOUPDATE         "AutoUpdate"
#define S_SERVICE_DEMO               "DEMO"
#define S_SERVICE_HTTP               "HTTP"
#define S_SERVICE_HTTPTUNNEL         "HTTPTunnel"
#define S_SERVICE_HTTPPROXY          "HTTPProxy"
#define S_SERVICE_SOCKS5             "Socks5"
#define S_SERVICE_IRM                "IRM"
#define S_SERVICE_IRM_CHANNEL        "IRMChannel"
#define S_SERVICE_IRM_CONTROL        "IRMControl"
#define S_SERVICE_IRM_SRVNET         "IRMSrvNet"
#define S_SERVICE_IRM_MEDIASRVNETTCP "IRMMediaSrvNetTCP"
#define S_SERVICE_IRM_MEDIARCV       "IRMMediaRcv"
#define S_SERVICE_IRM_MEDIARCVTCP    "IRMMediaRcvTCP"
#define S_SERVICE_IRM_MEDIARCVUDP    "IRMMediaRcvUDP"
#define S_SERVICE_IRM_MEDIASND       "IRMMediaSnd"
#define S_SERVICE_CONTACT_LIST       "ContactList"


// Log file

#define S_LOG_LOGIN     "LOGIN"
#define S_LOG_LOGOUT    "LOGOUT"
#define S_LOG_JOIN      "JOIN"
#define S_LOG_PART      "PART"
#define S_LOG_KICK      "KICK"
#define S_LOG_KILL      "KILL"
#define S_LOG_CHOPEN    "CHOPEN"
#define S_LOG_CHCLOSE   "CHCLOSE"
#define S_LOG_SRVCONN   "CONNECT"
#define S_LOG_SQUIT     "SQUIT"
#define S_LOG_SRVSTART  "START"
#define S_LOG_SHUTDOWN  "SHUTDOWN"



// Commands

#define S_SRVCMD_ADMIN        "ADMIN"
#define S_SRVCMD_APPCMD       "APPCMD"
#define S_SRVCMD_AUTH         "AUTH"
#define S_SRVCMD_AWAY         "AWAY"
#define S_SRVCMD_CHCLOSE      "CHCLOSE"
#define S_SRVCMD_CHDISABLE    "CHDISABLE"
#define S_SRVCMD_CHENABLE     "CHENABLE"
#define S_SRVCMD_CHENLIST     "CHENLIST"
#define S_SRVCMD_CHOPEN       "CHOPEN"
#define S_SRVCMD_CONFIG       "CONFIG"
#define S_SRVCMD_CONNECT      "CONNECT"
#define S_SRVCMD_DEBUGLEVEL   "DEBUGLEVEL"
#define S_SRVCMD_DIE          "DIE"
#define S_SRVCMD_GLOBOPS      "GLOBOPS"
#define S_SRVCMD_GNOTICE      "GNOTICE"
#define S_SRVCMD_IDLE         "IDLE"
#define S_SRVCMD_INFO         "INFO"
#define S_SRVCMD_INVITE       "INVITE"
#define S_SRVCMD_ISON         "ISON"
#define S_SRVCMD_JOIN         "JOIN"
#define S_SRVCMD_KICK         "KICK"
#define S_SRVCMD_KILL         "KILL"
#define S_SRVCMD_LINKS        "LINKS"
#define S_SRVCMD_LIST         "LIST"
#define S_SRVCMD_LISTEN       "LISTEN"
#define S_SRVCMD_LUSERS       "LUSERS"
#define S_SRVCMD_MODE         "MODE"
#define S_SRVCMD_NAMES        "NAMES"
#define S_SRVCMD_NCHANNEL     "NCHANNEL"
#define S_SRVCMD_NICK         "NICK"
#define S_SRVCMD_NJOIN        "NJOIN"
#define S_SRVCMD_NOTICE       "NOTICE"
#define S_SRVCMD_OPER         "OPER"
#define S_SRVCMD_PART         "PART"
#define S_SRVCMD_PASSWORD     "PASS"
#define S_SRVCMD_PING         "PING"
#define S_SRVCMD_PONG         "PONG"
#define S_SRVCMD_PORTQUERY    "PORTQUERY"
#define S_SRVCMD_PRIVMSG      "PRIVMSG"
#define S_SRVCMD_PROTOCOL     "PROTOCOL"
#define S_SRVCMD_QUIT         "QUIT"
#define S_SRVCMD_RAW          "RAW"
#define S_SRVCMD_REHASH       "REHASH"
#define S_SRVCMD_SAY          "SAY"
#define S_SRVCMD_SECURITY     "SECURITY"
#define S_SRVCMD_SERVER       "SERVER"
#define S_SRVCMD_SERVICE      "SERVICE"
#define S_SRVCMD_SET          "SET"
#define S_SRVCMD_SHELL        "SHELL"
#define S_SRVCMD_SHUTDOWN     "SHUTDOWN"
#define S_SRVCMD_SNIFF        "SNIFF"
#define S_SRVCMD_SPING        "SPING"
#define S_SRVCMD_SPONG        "SPONG"
#define S_SRVCMD_SQUIT        "SQUIT"
#define S_SRVCMD_STATS        "STATS"
#define S_SRVCMD_SVSMODE      "SVSMODE"
#define S_SRVCMD_TOPIC        "TOPIC"
#define S_SRVCMD_TRACE        "TRACE"
#define S_SRVCMD_UDPON        "UDPON"
#define S_SRVCMD_UDPOFF       "UDPOFF"
#define S_SRVCMD_UDPPING      "UDPPING"
#define S_SRVCMD_UDPPONG      "UDPPONG"
#define S_SRVCMD_USER         "USER"
#define S_SRVCMD_USERHOST     "USERHOST"
#define S_SRVCMD_WALLOPS      "WALLOPS"
#define S_SRVCMD_WBOARD       "WBOARD"
#define S_SRVCMD_WHO          "WHO"
#define S_SRVCMD_WHOIS        "WHOIS"

#define S_SRVCMD_MEDIA           "MEDIA"
#define S_SRVCMD_MEDIAHEADER     "MEDIAHEADER"
#define S_SRVCMD_MEDIADEBUG      "MEDIADEBUG"
#define S_SRVCMD_MEDIAKILL       "MEDIAKILL"
#define S_SRVCMD_MEDIAMUTE       "MEDIAMUTE"
#define S_SRVCMD_MEDIANOTIFY     "MEDIANOTIFY"
#define S_SRVCMD_MEDIAPTPACK     "MEDIAPTPACK"
#define S_SRVCMD_MEDIAPTPINVITE  "MEDIAPTPINVITE"
#define S_SRVCMD_MEDIAPTPREJECT  "MEDIAPTPREJECT"
#define S_SRVCMD_MEDIAPKTREQUEST "MEDIAPKTREQUEST"
#define S_SRVCMD_MEDIAQUERY      "MEDIAQUERY"
#define S_SRVCMD_MEDIARCV        "MEDIARCV"
#define S_SRVCMD_MEDIARCVALL     "MEDIARCVALL"
#define S_SRVCMD_MEDIARCVALLLIST "MEDIARCVALLLIST"
#define S_SRVCMD_MEDIARCVLIST    "MEDIARCVLIST"
#define S_SRVCMD_MEDIARCVRUN     "MEDIARCVRUN"
#define S_SRVCMD_MEDIARCVALLRUN  "MEDIARCVALLRUN"
#define S_SRVCMD_MEDIARCVSTOP    "MEDIARCVSTOP"
#define S_SRVCMD_MEDIARCVALLSTOP "MEDIARCVALLSTOP"
#define S_SRVCMD_MEDIARESET      "MEDIARESET"
#define S_SRVCMD_MEDIASET        "MEDIASET"
#define S_SRVCMD_MEDIASETPOS     "MEDIASETPOS"
#define S_SRVCMD_MEDIASND        "MEDIASND"
#define S_SRVCMD_MEDIASNDSTOP    "MEDIASNDSTOP"
#define S_SRVCMD_MEDIASTATS      "MEDIASTATS"
#define S_SRVCMD_MEDIATALK       "MEDIATALK"
#define S_SRVCMD_MEDIAVOICE      "MEDIAVOICE"

#define S_SRVCMD_VOICEQUEUE      "VOICEQUEUE"
#define S_SRVCMD_VOICEQUEUEADD   "VOICEQUEUEADD"
#define S_SRVCMD_VOICEQUEUEDEL   "VOICEQUEUEDEL"
#define S_SRVCMD_VOICEQUEUELIST  "VOICEQUEUELIST"

#define S_SRVCMD_AUTOUPDCONFIG   "AUTOUPDCONFIG"
#define S_SRVCMD_AUTOUPDQUERY    "AUTOUPDQUERY"

#define S_AUPDCMD_BYE            "BYE"
#define S_AUPDCMD_ENDOFLIST      "ENDOFLIST"
#define S_AUPDCMD_EOF            "EOF"
#define S_AUPDCMD_ERROR          "ERROR"
#define S_AUPDCMD_FILE           "FILE"
#define S_AUPDCMD_GET            "GET"
#define S_AUPDCMD_HELLO          "HELLO"
#define S_AUPDCMD_IDLE           "IDLE"
#define S_AUPDCMD_LIST           "LIST"
#define S_AUPDCMD_LISTRPL        "LISTRPL"
#define S_AUPDCMD_QUIT           "QUIT"
#define S_AUPDCMD_REDIR          "REDIR"
#define S_AUPDCMD_SENDING        "SENDING"


#define S_DBMSNAME_MYSQL         "MYSQL"
#define S_DBMSNAME_NULL          ""


// Server Messages

//#define S_SRVMSG_WELCOME           "Seja benvindo ao E-learning Virtual Center"
#define S_SRVMSG_WELCOME           "Welcome to the IPTV Internet Relay Media Network (http://www.ip.tv)"
#define S_SRVMSG_SERVERNAME        "Your host is %s[%s/%d], running version %s"
#define S_SRVMSG_CREATED           "This server was created"
#define S_SRVMSG_PROTOCTLSUFIX     "are supported by this server"
#define S_SRVMSG_ABCD              ""
#define S_SRVMSG_DEFINES           ""
#define S_SRVMSG_GLOBALUSERS       ""
#define S_SRVMSG_NIRCOPS           "Operators online"
#define S_SRVMSG_NCHANNELS         "%d :channels formed"
#define S_SRVMSG_LUSERCLIENT       "There are %d users on %ld servers"
#define S_SRVMSG_NCONNECTIONS      "I have %d clients and %d servers"
#define S_SRVMSG_MAXLOCALUSERS     "Current local  users: %ld  Max: %ld"
#define S_SRVMSG_MAXGLOBALUSERS    "Current global users: %d  Max: %ld"
#define S_SRVMSG_HIGHCONNECTIONS   "Highest connection count: %d clients (%lu since server was (re)started)"
#define S_SRVMSG_COMMANDUNKNOWN    "Invalid Command"
#define S_SRVMSG_INVALIDPARAM      "Invalid parameters"
#define S_SRVMSG_NICKINVALID       "Invalid Nickname"
#define S_SRVMSG_NICKINUSE         "Nickname is already in use."
#define S_SRVMSG_INVALIDCLIVERSION "Invalid client version. Get new version from http://update.ip.tv"
#define S_SRVMSG_SERVICENOTAVAIL   "Service not available in server"
#define S_SRVMSG_MOTDNODATA        "MOTD File is missing"
#define S_SRVMSG_NOSUCHNICK        "No such nick/channel"
#define S_SRVMSG_REHASHING         "Rehashing IRM Server..."
#define S_SRVMSG_ENDOFREHASH       "Rehash Done."
#define S_SRVMSG_QUITDEFMSG        "Disconnected!"
#define S_SRVMSG_SQUITDEFMSG       "Server Disconnected from Network!"
#define S_SRVMSG_CLOSELINK         "ERROR :Closing Link: %s (Quit: %s)"
#define S_SRVMSG_CLOSESLINK        "ERROR :Closing Link: %s->%s (%s)"
#define S_SRVMSG_LISTSTART         "Channel :Users  Name"
#define S_SRVMSG_LISTEND           "End of /LIST"
#define S_SRVMSG_SERVERFULL        "Server is Full. Try another server"
#define S_SRVMSG_LINKSEND          "End of /LINKS list"
#define S_SRVMSG_OPERGRANTED       "You are IRM Operator"
#define S_SRVMSG_OPERDENIED        "No O-lines for your host"
#define S_SRVMSG_SNIFFACK          "is being sniffed"
#define S_SRVMSG_SNIFFEND          "is not being sniffed anymore"
#define S_SRVMSG_ISOPER            "is IRM Operator"
#define S_SRVMSG_ISCHANNELOWNER    "is Channel Owner"
#define S_SRVMSG_WHOISREGNICK      "is a registered and identified nick"
#define S_SRVMSG_NOTREGNICK        "User is not registered"
#define S_SRVMSG_YOURENOTOPER      "You are NOT IRM Operator"
#define S_SRVMSG_YOURENOTCHANOP    "You are NOT Channel Operator"
#define S_SRVMSG_YOURENOWAWAY      "You have been marked as being away"
#define S_SRVMSG_YOUREUNAWAY       "You are no longer marked as being away"
#define S_SRVMSG_VOICEQUEUEERROR   "Error in Voice Queue command"

#define S_SRVMSG_USERNOTINCHANNEL  "Not on Channel"
#define S_SRVMSG_USERONCHANNEL     "is already on channel"
#define S_SRVMSG_CANTCHGUSRMODES   "Can't change mode for other users"
#define S_SRVMSG_USERINVALID       "USER <username> <localhost> <server> <email> contem caracteres invalidos."
#define S_SRVMSG_ENDNAMES          "End of /NAMES list."
#define S_SRVMSG_ENDOFWHO          "End of /WHO list"
#define S_SRVMSG_ENDOFWHOIS        "End of /WHOIS list."
#define S_SRVMSG_ENDOFINFO         "End of /INFO list."
#define S_SRVMSG_ENDOFSTATS        "End of /STATS list."
#define S_SRVMSG_ENDOFVOICEQUEUE   "End of /VOICEQUEUELIST."
#define S_SRVMSG_ENDOFAUTOUPDQUERY "End of /AUTOUPDQUERY."
#define S_SRVMSG_ENDOFCHENLIST     "End of /CHENLIST."

#define S_SRVMSG_WHOISIDLE         "seconds idle, signon time"
#define S_SRVMSG_NOTOPIC           "No topic is set."
#define S_SRVMSG_NOMEDIAVOICE      "No Media Voice is set."
#define S_SRVMSG_HAVEMEDIAVOICE    "have Media Voice in Channel."
#define S_SRVMSG_ENDOFBANLIST      "End of Channel Ban List"
#define S_SRVMSG_CANNOTSENDTOCHAN  "Cannot send to channel"
#define S_SRVMSG_CANNOTJOINBANNED  "Cannot join channel (+b)"
#define S_SRVMSG_CANNOTJOINKEY     "Cannot join channel (need correct key)"
#define S_SRVMSG_CANNOTJOININVONLY "Cannot join channel (invite only)"
#define S_SRVMSG_PINGTIMEOUT       "Disconnected (Ping Timeout)"
#define S_SRVMSG_IDLETIMEOUT       "Disconnected (Max idle time reached)"
#define S_SRVMSG_MEDIACONNOFFLINE  "Disconnected (TCP Media Connection not established)"
#define S_SRVMSG_NOCLIENTAUTH      "Disconnected (No Auth Lines available for your class)"
#define S_SRVMSG_USERCONNECTDENIED "Disconnected (You are not authorized to connect to this server. Try later)"
#define S_SRVMSG_INTERNALERROR     "Internal Error"
#define S_SRVMSG_SRVREDIR          "You are not authorized to connect to this server. Please connect to"

#define S_SRVMSG_ENDOFMEDIAQUERY   "End of /MEDIAQUERY list."
#define S_SRVMSG_ENDOFMEDIARCVLIST "End of /MEDIARCVLIST list."
#define S_SRVMSG_ENDOFMEDIARCVALLLIST "End of /MEDIARCVALLLIST list."
#define S_SRVMSG_MEDIARCVSTOP      "MEDIARCVSTOP"
#define S_SRVMSG_MEDIASNDSTOP      "MEDIASNDSTOP"
#define S_SRVMSG_MEDIARCVRUN       "MEDIARCVRUN"
#define S_SRVMSG_MEDIAERROR        "MEDIAERROR"
#define S_SRVMSG_MEDIAKILL         "MEDIAKILL"

#define S_SRVMSG_NEEDMOREPARAMS    "Need More Parameters"
#define S_SRVMSG_ALREADYREGISTRED  "Already Registred"
#define S_SRVMSG_NOPRIVILEGES      "No Privileges"
#define S_SRVMSG_NOSUCHSERVER      "No such Server: Verify C/N lines"
#define S_SRVMSG_INVALIDTCPCONN    "Invalid TCP connection or not established"
#define S_SRVMSG_NOSUCHSERVERTOKEN "No such Server Token"
#define S_SRVMSG_NOSUCHCHANNEL     "No such Channel"
#define S_SRVMSG_BANNEDFROMCHAN    "User is Banned from Channel"
#define S_SRVMSG_CHANNELISFULL     "Channel is Full"
#define S_SRVMSG_TOOMANYCHANNELS   "Too many Channels"
#define S_SRVMSG_UDPONACK          "UDP Protocol turned ON for Peer to Peer / Media communication"
#define S_SRVMSG_UDPOFFACK         "UDP Protocol turned OFF for Peer to Peer / Media communication"
#define S_SRVMSG_HTTPON            "Connected via HTTP Protocol"
#define S_SRVMSG_PROTENABLED       "Protocol enabled"
#define S_SRVMSG_MEDIADEBUG        "Media Debug level set to"
#define S_SRVMSG_DEBUGLEVEL        "Debug level set to"
#define S_SRVMSG_SECURITY          "Security level set to"

#define S_SRVMSG_CONNECTDENIED     "Connect Denied (Server is Leaf or Connect List Full)"
#define S_SRVMSG_WRONGSRVVERSION   "Disconnected! (Wrong Version Number)"
#define S_SRVMSG_INVALIDCONNECTION "Disconnected! (Invalid Server Connection)"
#define S_SRVMSG_SRVALRDYCONNECTED "Server Already Connected"
#define S_SRVMSG_PASSWDMISMATCH    "Disconnected! (Password Mismatch)"
#define S_SRVMSG_PASSWDNOTSET      "Disconnected! (Password Command Missing)"
#define S_SRVMSG_PASSWDALRDYSET    "Password Already Set"
#define S_SRVMSG_REMSRVDENIED      "Disconnected! (Remote Server Denied - Verify Remote Server Token)"
#define S_SRVMSG_NOHUBCONNECTION   "Disconnected! (No Hub Connection - Remote Servers not allowed)"
#define S_SRVMSG_HUBINITFAILED     "Disconnected! (Hub Initialization Failed)"
#define S_SRVMSG_INVALIDSERVERIP   "Disconnected! (Invalid Server IP - Verify C/N lines)"
#define S_SRVMSG_INVALIDSERVERNAME "Disconnected! (Invalid Server Name - Verify C/N lines)"
#define S_SRVMSG_TOOMANYLINKS      "Disconnected! (Max Links reached for Connection Class)"

#define S_SRVMSG_CHENABLE          "Channel enabled successfully"
#define S_SRVMSG_CHDISABLE         "Channel disabled successfully"

#define S_AUTOUPDERR_GENERAL       "General error"
#define S_AUTOUPDERR_INVALIDPARAM  "Invalid parameters"
#define S_AUTOUPDERR_INVALIDCMD    "Invalid command"
#define S_AUTOUPDERR_APPNOTFOUND   "Application not found"
#define S_AUTOUPDERR_FILENOTFOUND  "File not found"
#define S_AUTOUPDERR_INTERNAL      "Internal error"
#define S_AUTOUPDERR_ALRDYSENDING  "Already sending file"
#define S_AUTOUPDERR_IOERROR       "I/O Error reading / writing data"


#define S_CHSYMBOL_OP           '@'
#define S_CHSYMBOL_LOG          '*'
#define S_CHSYMBOL_VOICE        '+'

#define S_CHMODE_BAN            'b'
#define S_CHMODE_LOG            'g'
#define S_CHMODE_NAMESLIMIT     'h'
#define S_CHMODE_INVITEONLY     'i'
#define S_CHMODE_KEY            'k'
#define S_CHMODE_NUSERLIMIT     'l'
#define S_CHMODE_MODERATED      'm'
#define S_CHMODE_EXTMESSAGE     'n'
#define S_CHMODE_OP             'o'
#define S_CHMODE_PRIVATE        'p'
#define S_CHMODE_SECRET         's'
#define S_CHMODE_BANDSHARE      'S'
#define S_CHMODE_OPCHGTOPIC     't'
#define S_CHMODE_VOICELIMIT     'u'
#define S_CHMODE_VOICE          'v'
#define S_CHMODE_MULTIVOICE     'V'
#define S_CHMODE_OWNER          'w'
#define S_CHMODE_BITRATE        'x'
#define S_CHMODE_CHANNELBITRATE 'X'
#define S_CHMODE_ONEAUDIO       'y'
#define S_CHMODE_AUTODEVOICE    'd'
#define S_CHMODE_AUDIOMUTE      'a'
#define S_CHMODE_CHGUBITRATE    'B'
#define S_CHMODE_PERSISTENT     'P'

#define S_UMODE_LOCALOPER       'O'
#define S_UMODE_GLOBALOPER      'o'
#define S_UMODE_CANCHANGECMODES 'M'
#define S_UMODE_CANCHANGEUMODES 'U'
#define S_UMODE_INVISIBLE       'i'
#define S_UMODE_SERVERNOTICES   's'
#define S_UMODE_SERVERKILLMSG   'k'
#define S_UMODE_DEBUG           'd'
#define S_UMODE_SNIFFER         'x'
#define S_UMODE_REGISTEREDNICK  'r'
#define S_UMODE_SERVICESADMIN   'a'
#define S_UMODE_IRCADMIN        'A'
#define S_UMODE_GLOBOPS         'g'
#define S_UMODE_GETWALLOPS      'w'
#define S_UMODE_GETMEDIANOTICES 'm'
#define S_UMODE_GETERRORNOTICES 'e'
#define S_UMODE_GETCLIENTCONN   'c'
#define S_UMODE_CAMON           'C'
#define S_UMODE_MICMUTE         'T'
#define S_UMODE_AWAY            'W'

#define SNDMEDIA_BITRATE_NONE     0
#define SNDMEDIA_BITRATE_ALL      1
#define SNDMEDIA_BITRATE_32      32
#define SNDMEDIA_BITRATE_33      33
#define SNDMEDIA_BITRATE_64      64
#define SNDMEDIA_BITRATE_128    128
#define SNDMEDIA_BITRATE_256    256
#define SNDMEDIA_BITRATE_384    384
#define SNDMEDIA_BITRATE_512    512
#define SNDMEDIA_BITRATE_1024  1024


#define HIGHEST_CONNECTION
#define CLIENT_COUNT

#define	RPL_WELCOME          001
#define	RPL_YOURHOST         002
#define	RPL_CREATED          003
#define	RPL_MYINFO           004
#define RPL_PROTOCTL         005

#define RPL_SRVREDIR         020
#define RPL_PORTQUERY        021

#define RPL_TRACELINK        200
#define RPL_TRACECONNECTING  201
#define RPL_TRACEHANDSHAKE   202
#define RPL_TRACEUNKNOWN     203
#define RPL_TRACEOPERATOR    204
#define RPL_TRACEUSER        205
#define RPL_TRACESERVER      206
#define RPL_TRACENEWTYPE     208
#define RPL_TRACECLASS       209

#define RPL_STATS            210
#define RPL_STATSLINKINFO    211
#define RPL_STATSCOMMANDS    212
#define RPL_STATSCLINE       213
#define RPL_STATSNLINE       214
#define RPL_STATSILINE       215
#define RPL_STATSKLINE       216
#define RPL_STATSQLINE       217
#define RPL_STATSYLINE       218
#define RPL_ENDOFSTATS       219
#define RPL_STATSPLINE       220

#define RPL_UMODEIS          221

#define RPL_STATSBLINE	     222
#define RPL_STATSELINE	     223
#define RPL_STATSFLINE	     224
#define RPL_STATSZLINE	     225
#define RPL_STATSCOUNT	     226
#define RPL_STATSGLINE       227
#define RPL_STATSRLINE       228

#define RPL_SERVLIST         234
#define RPL_SERVLISTEND      235

#define	RPL_STATSLLINE       241
#define	RPL_STATSUPTIME      242
#define	RPL_STATSOLINE       243
#define	RPL_STATSHLINE       244
#define	RPL_STATSSLINE       245
#define RPL_STATSULINE	     246
#define	RPL_STATSDEBUG	     249
#ifdef HIGHEST_CONNECTION
#define RPL_STATSCONN 	     250
#endif
#define	RPL_LUSERCLIENT      251
#define RPL_LUSEROP          252
#define	RPL_LUSERUNKNOWN     253
#define	RPL_LUSERCHANNELS    254
#define	RPL_LUSERME          255
#define	RPL_ADMINME          256
#define	RPL_ADMINLOC1        257
#define	RPL_ADMINLOC2        258
#define	RPL_ADMINEMAIL       259

#define	RPL_TRACELOG         261
#define RPL_ENDOFTRACE       262
#define RPL_LOAD2HI          263

#ifdef CLIENT_COUNT
#define RPL_LOCALUSERS       265
#define RPL_GLOBALUSERS      266
#define RPL_NUMOPERS         267
#endif
#define RPL_SILELIST         271
#define RPL_ENDOFSILELIST    272

#define	RPL_NONE             300
#define RPL_AWAY             301
#define RPL_USERHOST         302
#define RPL_ISON             303
#define RPL_TEXT             304
#define	RPL_UNAWAY           305
#define	RPL_NOWAWAY          306
#define RPL_WHOISREGNICK     307
#define RPL_WHOISADMIN       308
#define RPL_WHOISSADMIN      309
#define RPL_WHOISSVCMSG      310
#define RPL_WHOISUSER        311
#define RPL_WHOISSERVER      312
#define RPL_WHOISOPERATOR    313


#define RPL_WHOWASUSER       314
/* rpl_endofwho below (315) */
#define	RPL_ENDOFWHOWAS      369

#define RPL_WHOISCHANOP      316	/* redundant and not needed but
					 * reserved */
#define RPL_WHOISIDLE        317

#define RPL_ENDOFWHOIS       318
#define RPL_WHOISCHANNELS    319

#define RPL_LISTSTART        321
#define RPL_LIST             322
#define RPL_LISTEND          323
#define RPL_CHANNELMODEIS    324
#define RPL_CHANNELOWNERIS   325

#define RPL_CREATIONTIME     329

#define RPL_NOTOPIC          331
#define RPL_TOPIC            332
#define RPL_TOPICWHOTIME     333
#define RPL_COMMANDSYNTAX    334

#define RPL_WHOISACTUALLY    338

#define RPL_INVITING         341
#define	RPL_SUMMONING        342

#define RPL_VERSION          351

#define RPL_WHOREPLY         352
#define RPL_ENDOFWHO         315
#define RPL_NAMREPLY         353
#define RPL_ENDOFNAMES       366

#define RPL_CONNECT          360
#define RPL_KILLDONE         361
#define	RPL_CLOSING          362
#define RPL_CLOSEEND         363
#define RPL_LINKS            364
#define RPL_ENDOFLINKS       365

/* rpl_endofnames above (366) */
#define RPL_BANLIST          367
#define RPL_ENDOFBANLIST     368
/* rpl_endofwhowas above (369) */

#define	RPL_INFO             371
#define	RPL_MOTD             372
#define	RPL_INFOSTART        373
#define	RPL_ENDOFINFO        374
#define	RPL_MOTDSTART        375
#define	RPL_ENDOFMOTD        376

#define RPL_YOUREOPER        381
#define RPL_REHASHING        382
#define RPL_ENDOFREHASH      383
#define RPL_MYPORTIS         384
#define RPL_NOTOPERANYMORE   385
#define ERR_YOURENOTOPER     386
#define RPL_SNIFFACK         387
#define RPL_SNIFFEND         388

#define RPL_TIME             391
#define	RPL_USERSSTART       392
#define	RPL_USERS            393
#define	RPL_ENDOFUSERS       394
#define	RPL_NOUSERS          395

#define RPL_CHENABLE         396
#define RPL_CHDISABLE        397
#define RPL_CHENLIST         398
#define	RPL_ENDOFCHENLIST    399

#define ERR_NOSUCHNICK       401
#define ERR_NOSUCHSERVER     402
#define ERR_NOSUCHCHANNEL    403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS  405
#define ERR_WASNOSUCHNICK    406
#define ERR_TOOMANYTARGETS   407
#define ERR_NOCOLORSONCHAN   408
#define	ERR_NOORIGIN         409
#define	ERR_TOOMANYLINKS     410

//#define ERR_NORECIPIENT      411
//#define ERR_NOTEXTTOSEND     412
//#define ERR_NOTOPLEVEL       413
//#define ERR_WILDTOPLEVEL     414

#define ERR_UNKNOWNCOMMAND   421
#define	ERR_NOMOTD           422
#define	ERR_NOADMININFO      423
#define	ERR_FILEERROR        424
#define	ERR_INVALIDPARAM     425

#define ERR_TOOMANYAWAY      429

#define ERR_NONICKNAMEGIVEN  431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE    433
#define ERR_ERRONEUSUSERNAME 434
#define ERR_BANONCHAN        435
#define	ERR_NICKCOLLISION    436
#define ERR_BANNICKCHANGE    437

#define ERR_SERVICESDOWN     440
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL     442
#define	ERR_USERONCHANNEL    443
#define ERR_NOLOGIN          444
#define	ERR_SUMMONDISABLED   445
#define ERR_USERSDISABLED    446

#define ERR_NOTREGISTERED    451

#define ERR_INVALIDSERVERNAME 460
#define ERR_NEEDMOREPARAMS   461
#define ERR_ALREADYREGISTRED 462
#define ERR_NOPERMFORHOST    463
#define ERR_PASSWDMISMATCH   464
#define ERR_YOUREBANNEDCREEP 465
#define ERR_YOUWILLBEBANNED  466
#define	ERR_KEYSET           467
#define ERR_ONLYSERVERSCANCHANGE 468
#define ERR_INVALIDSERVERIP  469

#define ERR_CHANNELISFULL    471
#define ERR_UNKNOWNMODE      472
#define ERR_INVITEONLYCHAN   473
#define ERR_BANNEDFROMCHAN   474
#define	ERR_BADCHANNELKEY    475
#define	ERR_BADCHANMASK      476
#define ERR_NEEDREGGEDNICK   477
#define ERR_BANLISTFULL      478
#define ERR_BADCHANNAME      479

#define ERR_NOPRIVILEGES     481
#define ERR_CHANOPRIVSNEEDED 482
#define	ERR_CANTKILLSERVER   483
#define ERR_DESYNC           484
#define ERR_CHANBANREASON    485
#define ERR_NONONREG         486
#define ERR_CANTCHGUSRMODES  487
#define ERR_YOURENOTCHANOP   488
#define ERR_VOICEQUEUE       489

#define ERR_NOOPERHOST       491

#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH   502

#define ERR_GHOSTEDCLIENT    503
#define ERR_LAST_ERR_MSG     504
#define ERR_FILELISTFULL     511
#define ERR_TOOMANYWATCH     512

#define ERR_TOOMANYDCC       514

#define ERR_LISTSYNTAX       521
#define ERR_WHOSYNTAX        522
#define ERR_WHOLIMEXCEED     523

#define RPL_LOGON            600
#define RPL_LOGOFF           601
#define RPL_WATCHOFF         602
#define RPL_WATCHSTAT        603
#define RPL_NOWON            604
#define RPL_NOWOFF           605
#define RPL_WATCHLIST        606
#define RPL_ENDOFWATCHLIST   607

#define RPL_DCCSTATUS        617
#define RPL_DCCLIST          618
#define RPL_ENDOFDCCLIST     619
#define RPL_DCCINFO          620

#define RPL_MEDIAQUERY        801
#define RPL_ENDOFMEDIAQUERY   802
#define RPL_MEDIASNDACK       803
#define RPL_MEDIASNDSTOP      804
#define RPL_MEDIARCVACK       805
#define RPL_MEDIARCVRUN       806
#define RPL_MEDIARCVSTOP      807
#define RPL_MEDIAERROR        808
#define RPL_MEDIARCVLIST      809
#define RPL_ENDOFMEDIARCVLIST 810
#define RPL_MEDIASET          811
#define RPL_MEDIARCVALLACK    812
#define RPL_MEDIARCVALLRUN    813
#define RPL_MEDIARCVALLSTOP   814
#define RPL_MEDIADEBUG        815
#define RPL_MEDIAKILL         817
#define RPL_MEDIARCVALLLIST      818
#define RPL_ENDOFMEDIARCVALLLIST 819

#define RPL_UDPON             823
#define RPL_UDPOFF            824
#define RPL_HTTP              825
#define RPL_PROTOCOL          826

#define RPL_USERSPING         828

#define RPL_SECURITY          830
#define RPL_DEBUGLEVEL        831

#define RPL_VOICEQUEUELIST       835
#define RPL_ENDOFVOICEQUEUELIST  836

#define RPL_NOMEDIAVOICE      901
#define RPL_MEDIAVOICE        902
#define RPL_MEDIAVOICEWHOTIME 903

#define RPL_AUTOUPDQUERY      911
#define RPL_ENDOFAUTOUPDQUERY 912

#define ERR_SERVERFULL        950
#define ERR_CONNECTDENIED     951
#define ERR_WRONGSRVVERSION   952
#define ERR_INVALIDCONNECTION 953
#define ERR_SRVALRDYCONNECTED 954
#define ERR_PASSWDNOTSET      955
#define ERR_PASSWDALRDYSET    956
#define ERR_REMSRVDENIED      957
#define ERR_NOHUBCONNECTION   958
#define ERR_HUBINITFAILED     959
#define ERR_NOSUCHSERVERTOKEN 960
#define ERR_INVALIDCLIVERSION 961
#define ERR_SERVICENOTAVAIL   962
#define ERR_INVALIDTCPCONN    963

#define ERR_NUMERIC_ERR       999


#define MEDIAERR_GENERAL            100
#define MEDIAERR_MEDIASND           101 
#define MEDIAERR_MEDIASNDSTOP       102
#define MEDIAERR_MEDIARCV           103 
#define MEDIAERR_MEDIARCVALL        104 
#define MEDIAERR_MEDIARCVRUN        105 
#define MEDIAERR_MEDIARCVRUNTCP     106 
#define MEDIAERR_MEDIARCVALLRUN     107 
#define MEDIAERR_MEDIARCVALLRUNTCP  108
#define MEDIAERR_MEDIARCVSTOP       109
#define MEDIAERR_MEDIARCVALLSTOP    110
#define MEDIAERR_MEDIAPKTSND        111 
#define MEDIAERR_MEDIAPKTTCP        112 
#define MEDIAERR_MEDIAQUERY         113
#define MEDIAERR_MEDIARCVLIST       114
#define MEDIAERR_MEDIASET           115
#define MEDIAERR_MEDIAPKTREQUEST    116
#define MEDIAERR_MEDIAPTPINVITE     117
#define MEDIAERR_MEDIAPTPACK        118
#define MEDIAERR_MEDIAPTPREJECT     119
#define MEDIAERR_MEDIARCVALLLIST    120
#define MEDIAERR_MEDIASTATS         121
#define MEDIAERR_MEDIASETPOS        122
#define MEDIAERR_MEDIASNDDENIED     123
#define MEDIAERR_MEDIABITRATE       124
#define MEDIAERR_MEDIAKILL          125
#define MEDIAERR_MEDIAPKTBLOCKED    126
#define MEDIAERR_MEDIAMUTE          127
#define MEDIAERR_MEDIATALK          128
#define MEDIAERR_MEDIARESET         129

#define AUTOUPDERR_GENERAL          100
#define AUTOUPDERR_INVALIDPARAM     101
#define AUTOUPDERR_INVALIDCMD       102
#define AUTOUPDERR_APPNOTFOUND      103
#define AUTOUPDERR_FILENOTFOUND     104
#define AUTOUPDERR_INTERNAL         105
#define AUTOUPDERR_ALRDYSENDING     106
#define AUTOUPDERR_IOERROR          107


#endif  // __SRV_DEFS__

