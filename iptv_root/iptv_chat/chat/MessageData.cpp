#include "shared/compat.h"

#include "shared/VectorT.h"
#include "shared/common.h"

#include "shared/irm/cmd.h"

#include "MessageData.h"
#include "chat.h"


typedef VectorT<ULONG> SegCtrlVector;


cxObj::~cxObj( void )
{
}

MessageData::MessageData( void )
{
    reset();
}

void MessageData::reset( void )
{
    lpszSource  = NULL;
    lpszTarget  = NULL;
    lpszFullCmd = NULL;
    lParam      = NULL;
    nOpCode     = -1;
}

MessageData::~MessageData( void )
{
    if( lpszSource )
        delete [] lpszSource;

    if( lpszTarget )
        delete [] lpszTarget;

    if( lpszFullCmd )
        delete [] lpszFullCmd;

    if( lParam )
        delete lParam;
}

/////////////////////////////////////////////////////////////////////////////
// cxString //
/////////////////////////////////////////////////////////////////////////////

cxString::cxString( char *_str ) : cxObj()
{
    str = StrAllocAttrib(_str);
}

cxString::cxString( void )  : cxObj()
{
    str = NULL;
}

cxString::cxString( cxString & cxStr ) : cxObj( )
{
    /*
    int i;

    i = cxStr.GetLen( );
    if(  i > 0 )
    {
        str = new char[i+1];
        strcpy( str, cxStr.GetStr( ) );
    }
    else
        str = NULL;
    */

    str = StrAllocAttrib(cxStr.GetStr());
}

cxString::~cxString()
{
    if( str )
        delete [] str;
}

int cxString::GetLen( void )
{
    return (str ? strlen( str ) : 0);
}

char * cxString::GetStr( void )
{
    return str;
}

void cxString::SetStr( char *_str )
{
    if( _str )
    {
        /*
        if( !str )
            str = new char[strlen( _str ) + 1];
        else if( strlen( _str ) > strlen( str ) )
        {
            delete [] str;
            str = new char[strlen( _str ) + 1];
        }

        strcpy( str, _str );
        */

        if (str)
            delete [] str;

        str = StrAllocAttrib(_str);
    }
}

/////////////////////////////////////////////////////////////////////////////
// MediaRequest //
/////////////////////////////////////////////////////////////////////////////
void MediaRequest::init(void)
{
    nMediaID      = 0;
    mc_MediaClass = 0;
    nMediaPort    = 0;
    wData1        = 0;
    wData2        = 0;
    pt_Protocol   = 0;
    PacketLen     = 0;
    HeaderLen     = 0;
    lpszName      = NULL;
    lpszIp        = NULL;
    lpHeader      = NULL;
    lpszChannel   = NULL;

    uBitrate          = 0;
    nHeaderSegs       = 0;
    uHeaderMaxSegSize = 0;
    wHeaderChksum     = 0;

    pHeaderSegsCtrlVector = NULL;
}


void MediaRequest::reset(void)
{
    if( lpszChannel )
        delete [] lpszChannel;

    if( lpszName )
        delete [] lpszName;

    if( lpszIp )
        delete [] lpszIp;

    if( lpHeader )
        delete [] lpHeader;

    if ( pHeaderSegsCtrlVector )
        delete ((SegCtrlVector *) pHeaderSegsCtrlVector);

    init();
}


MediaRequest::MediaRequest( char *_lpszChannel, int MediaClass, int Protocol, unsigned long nPacketLen, unsigned long nHeaderLen, void *_lpHeader ) : cxObj( )
{
// @MEDIASND #Channel StrID Protocol PacketLen $HeaderLen HEADER

    init();

    mc_MediaClass = MediaClass;
    pt_Protocol   = Protocol;
    PacketLen     = nPacketLen;
    HeaderLen     = nHeaderLen;

    lpszChannel   = StrAllocAttrib(_lpszChannel, CHANNELLEN);
    //lpHeader      = (char *)BufAllocAttrib(_lpHeader, HeaderLen);

    if (HeaderLen)
        lpHeader = StrAllocAttrib((LPSTR)_lpHeader);
}

MediaRequest::MediaRequest( char *_lpszChannel, char *MediaClass, char *Protocol, unsigned long nPacketLen, unsigned long nHeaderLen, void *_lpHeader )  : cxObj( )
{
// @MEDIASND #Channel StrID Protocol PacketLen $HeaderLen HEADER
    init();

    SetMediaClass( MediaClass );
    SetProtocol( Protocol );
    PacketLen     = nPacketLen;
    HeaderLen     = nHeaderLen;

    lpszChannel   = StrAllocAttrib(_lpszChannel, CHANNELLEN);
    //lpHeader      = (char *)BufAllocAttrib(_lpHeader, HeaderLen);

    if (HeaderLen)
        lpHeader = StrAllocAttrib((LPSTR)_lpHeader);
}

MediaRequest::MediaRequest( char *_lpszChannel, int MediaClass, unsigned long nID, unsigned long nPort )  : cxObj( )
{
// :Server RPL_MEDIASNDACK #Channel StrID ID MediaPort :MEDIASND
    init();

    mc_MediaClass = MediaClass;
    nMediaID      = nID;
    nMediaPort    = nPort;

    lpszChannel   = StrAllocAttrib(_lpszChannel, CHANNELLEN);
}

MediaRequest::MediaRequest( char *_lpszChannel, char * MediaClass, unsigned long nID, unsigned long nPort ) : cxObj( )
{
// :Server RPL_MEDIASNDACK #Channel StrID ID MediaPort :MEDIASND
    init();

    SetMediaClass( MediaClass );
    nMediaID      = nID;
    nMediaPort    = nPort;

    lpszChannel   = StrAllocAttrib(_lpszChannel, CHANNELLEN);
}

MediaRequest::MediaRequest( unsigned long nID, int Protocol, unsigned long nPort ) : cxObj( )
{
// MEDIARCV ID Protocol [Port]
    init();

    pt_Protocol   = Protocol;
    nMediaID      = nID;
    nMediaPort    = nPort;
}

MediaRequest::MediaRequest( unsigned long nID, char *Protocol, unsigned long nPort ) : cxObj( )
{
// MEDIARCV ID Protocol [Port]

    init();

    SetProtocol( Protocol );
    nMediaID      = nID;
    nMediaPort    = nPort;
}

MediaRequest::MediaRequest( unsigned long nID, unsigned long nPacketLen, unsigned long nPort, unsigned long nHeaderLen, void *_lpHeader ) : cxObj( )
{
// :Server RPL_MEDIARCVACK ID PacketLen Port $HeaderLen HEADER

    init();

    PacketLen  = nPacketLen;
    HeaderLen  = nHeaderLen;
    nMediaID   = nID;
    nMediaPort = nPort;

    //lpHeader   = (char *)BufAllocAttrib(_lpHeader, HeaderLen);

    if (HeaderLen)
        lpHeader = StrAllocAttrib((LPSTR)_lpHeader);
}

MediaRequest::MediaRequest( void ) : cxObj( )
{
    init();
}

MediaRequest::~MediaRequest( void )
{
    reset();
}

char *MediaRequest::GetMediaClass( void )
{
    switch( mc_MediaClass )
    {
        case MC_CAM:
            return S_MC_CAM;
        break;

        case MC_SCREEN:
            return S_MC_SCREEN;
        break;

        case MC_VOICEIP:
            return S_MC_VOICEIP;
        break;

        case MC_CONFERENCE:
            return S_MC_CONFERENCE;

        case MC_MOVIE:
            return S_MC_MOVIE;

        case MC_FTP:
            return S_MC_FTP;

        case MC_AVCONF:
            return S_MC_AVCONF;

        case MC_APP:
            return S_MC_APP;

        case MC_NULL:
        default:
            return "NULL";
        break;
    }
}

char * MediaRequest::GetMediaSetOption  ( void )
{
    switch( vs_MediaSetOption )
    {
        case VS_NAME:
            return "NAME";
        break;

        case VS_NULL:
        default:
            return "NULL";
        break;
    }
}

char * MediaRequest::GetMediaNotify( void )
{
    switch( vn_MediaNotify )
    {
        case VN_START:
            return S_VN_START;
            break;
        case VN_SEND:
            return S_VN_SEND;
            break;
        case VN_STOP:
            return S_VN_STOP;
            break;
        case VN_PLAY:
            return S_VN_PLAY;
            break;
        case VN_BLOCK:
            return S_VN_BLOCK;
            break;
        case VN_VOICE:
            return S_VN_VOICE;
            break;
        case VN_MUTE:
            return S_VN_MUTE;
            break;
        case VN_ON:
            return S_VN_ON;
            break;
        case VN_OFF:
            return S_VN_OFF;
            break;
        case VN_ERROR:
            return S_VN_ERROR;
            break;

        case VN_NULL:
        default:
            return S_VN_NULL;
        break;
    }
}

char *  MediaRequest::GetProtocol( void )
{
    switch( pt_Protocol )
    {
        case PT_TCP:
            return S_PT_TCP;
        break;

        case PT_UDP:
            return S_PT_UDP;
        break;

        case PT_NULL:
        default:
            return S_PT_NULL;
        break;
    }
}


void MediaRequest::SetChannel( LPCSTR str )
{
    if( str )
    {
        if( lpszChannel )
            delete [] lpszChannel;

        lpszChannel = StrAllocAttrib(str, CHANNELLEN);
    }
}

void MediaRequest::SetName( LPCSTR str )
{
    if( str )
    {
        if( lpszName )
            delete [] lpszName;

        // Esta funcao serve pra armazenar tanto nick do owner da media
        // quanto o nome da media (!!)
        lpszName = StrAllocAttrib(str, NICKLEN);
    }
}

void MediaRequest::SetIp( LPCSTR str )
{
    if( str )
    {
        if( lpszIp )
            delete [] lpszIp;

        lpszIp = StrAllocAttrib(str, IPSTR_MAXLEN);
    }
}

void MediaRequest::SetNickname( LPCSTR str )
{
    SetName( str );
}

void MediaRequest::SetHeader( void *buf, int Len )
{
    if( Len )
    {
        if( lpHeader )
            delete [] lpHeader;

        lpHeader = StrAllocAttrib((LPSTR)buf);

        //lpHeader = new char[Len+1];
        //memcpy( lpHeader, buf, Len );
        //lpHeader[Len] = '\0';

        HeaderLen = Len;
    }
}

void MediaRequest::SetMediaClass( LPCSTR mc )
{
    if( !_stricmp( S_MC_SCREEN, mc ) )
        mc_MediaClass = MC_SCREEN;
    else if( !_stricmp( S_MC_CAM, mc ) )
        mc_MediaClass = MC_CAM;
    else if( !_stricmp( S_MC_VOICEIP, mc ) )
        mc_MediaClass = MC_VOICEIP;
    else if( !_stricmp( S_MC_CONFERENCE, mc ) )
        mc_MediaClass = MC_CONFERENCE;
    else if( !_stricmp( S_MC_MOVIE, mc ) )
        mc_MediaClass = MC_MOVIE;
    else if( !_stricmp( S_MC_FTP, mc ) )
        mc_MediaClass = MC_FTP;
    else if( !_stricmp( S_MC_AVCONF, mc ) )
        mc_MediaClass = MC_AVCONF;
    else if( !_stricmp( S_MC_APP, mc ) )
        mc_MediaClass = MC_APP;
    else
        mc_MediaClass = MC_NULL;
}

void MediaRequest::SetProtocol  ( LPCSTR pt )
{
    if( !_stricmp( "TCP", pt ) )
        pt_Protocol = PT_TCP;
    else if( !_stricmp( "UDP", pt ) )
        pt_Protocol = PT_UDP;
    else
        pt_Protocol = PT_NULL;
}

void MediaRequest::SetMediaSetOption  ( LPCSTR vs )
{
    if( !_stricmp( "NAME", vs ) )
        vs_MediaSetOption = VS_NAME;
    else
        vs_MediaSetOption = VS_NULL;
}

void MediaRequest::SetMediaNotify( LPCSTR vn )
{
    if( !_stricmp( S_VN_START, vn ) )
        vn_MediaNotify = VN_START;
    else if( !_stricmp( S_VN_SEND, vn ) )
        vn_MediaNotify = VN_SEND;
    else if( !_stricmp( S_VN_STOP, vn ) )
        vn_MediaNotify = VN_STOP;
    else if( !_stricmp( S_VN_PLAY, vn ) )
        vn_MediaNotify = VN_PLAY;
    else if( !_stricmp( S_VN_BLOCK, vn ) )
        vn_MediaNotify = VN_BLOCK;
    else if( !_stricmp( S_VN_MUTE, vn ) )
        vn_MediaNotify = VN_MUTE;
    else if( !_stricmp( S_VN_VOICE, vn ) )
        vn_MediaNotify = VN_VOICE;
    else if( !_stricmp( S_VN_ERROR, vn ) )
        vn_MediaNotify = VN_ERROR;
    else if( !_stricmp( S_VN_ON, vn ) )
        vn_MediaNotify = VN_ON;
    else if( !_stricmp( S_VN_OFF, vn ) )
        vn_MediaNotify = VN_OFF;
    else
        vn_MediaNotify = VN_NULL;
}


MediaRequest & MediaRequest::operator=( MediaRequest & vr )
{
    mc_MediaClass     = vr.mc_MediaClass;
    pt_Protocol       = vr.pt_Protocol;
    vs_MediaSetOption = vr.vs_MediaSetOption;
    vn_MediaNotify    = vr.vn_MediaNotify;
    PacketLen         = vr.PacketLen;
    HeaderLen         = vr.HeaderLen;
    nMediaID          = vr.nMediaID;
    nMediaPort        = vr.nMediaPort;
    wData1            = vr.wData1;
    wData2            = vr.wData2;

    uBitrate          = vr.uBitrate;
    nHeaderSegs       = vr.nHeaderSegs;
    uHeaderMaxSegSize = vr.uHeaderMaxSegSize;
    wHeaderChksum     = vr.wHeaderChksum;

    SetHeader ( vr.lpHeader, HeaderLen );
    SetChannel( vr.lpszChannel );
    SetName   ( vr.lpszName );
    SetIp     ( vr.lpszIp );

    return *this;
}

BOOL MediaRequest::IsViewerCtrlRdy(void)
{
    BOOL  ret;
    ULONG i,
          verheadersize,
          chksum;

    SegCtrlVector *pSegCtrlVector;


    ret = FALSE;

    if ( nMediaID && !comp_branco(lpszChannel) && !comp_branco(lpszName) && mc_MediaClass )
    {
        if ( HeaderLen )
        {
            if ( HeaderLen && nHeaderSegs && uHeaderMaxSegSize && lpHeader && pHeaderSegsCtrlVector)
            {
                verheadersize = ( (nHeaderSegs-1) * uHeaderMaxSegSize );

                if ( HeaderLen > verheadersize )
                {
                    pSegCtrlVector = (SegCtrlVector *) pHeaderSegsCtrlVector;

                    ret = TRUE;

                    for (i=1; i<=nHeaderSegs; i++)
                    {
                        if ( pSegCtrlVector->get_elem(i) != i )
                        {
                            ret = FALSE;
                            break;
                        }
                    }

                    if (ret)
                    {
                        chksum = adler32(0, (const BYTE *)lpHeader, HeaderLen);

                        if ( wHeaderChksum != chksum)
                            ret = FALSE;
                    }
                }
            }
        }
        else
            ret = TRUE;
    }

    return ret;
}


BOOL MediaRequest::AddHeaderSeg(ULONG _mediaID, ULONG _seg, ULONG _segsize, LPCSTR _pHeaderSeg)
{
    BOOL  ret;
    ULONG lastsegsize,
          verheadersize,
          offset;

    SegCtrlVector *pSegCtrlVector;


    ret = FALSE;

    if ( _mediaID && _mediaID==nMediaID && _pHeaderSeg && HeaderLen && nHeaderSegs && uHeaderMaxSegSize &&
         _seg && _seg<=nHeaderSegs && _segsize && _segsize<=uHeaderMaxSegSize && _segsize<=HeaderLen )
    {
        verheadersize = ( (nHeaderSegs-1) * uHeaderMaxSegSize );

        if ( HeaderLen > verheadersize )
        {
            lastsegsize = HeaderLen % uHeaderMaxSegSize;

            if (!lastsegsize)
                lastsegsize = uHeaderMaxSegSize;

            if ( (_seg<nHeaderSegs && _segsize==uHeaderMaxSegSize) ||
                (_seg==nHeaderSegs && _segsize==lastsegsize) )
            {
                if ( pHeaderSegsCtrlVector )
                    pSegCtrlVector = (SegCtrlVector *) pHeaderSegsCtrlVector;
                else
                {
                    pSegCtrlVector = new SegCtrlVector(FALSE);
                    pHeaderSegsCtrlVector = pSegCtrlVector;
                }

                if (!lpHeader)
                {
                    lpHeader = new char[HeaderLen+1];

                    if (lpHeader)
                        memset(lpHeader, '\0', (HeaderLen+1)*sizeof(char));
                }

                if (pSegCtrlVector && lpHeader)
                {
                    offset = (_seg-1) * uHeaderMaxSegSize;
                    memmove((lpHeader+offset), _pHeaderSeg, _segsize);

                    if ( pSegCtrlVector->insert(_seg, _seg) )
                        ret = TRUE;
                }
            }
        }
    }

    return ret;
}


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// MediaQuery //
/////////////////////////////////////////////////////////////////////////////
MediaQuery::MediaQuery( void )
{
    lpszOwner       = NULL;
    lpszMediaName   = NULL;
    lpszChannel     = NULL;
    vcMediaClass    = 0;
    nMediaID        = 0;
    nBitrate        = 0;
}

MediaQuery::~MediaQuery( void )
{
    if( lpszChannel )
        delete [] lpszChannel;

    if( lpszOwner )
        delete [] lpszOwner;

    if( lpszMediaName )
        delete lpszMediaName;

    vcMediaClass  = 0;
    nMediaID      = 0;
    nBitrate      = 0;
}

void MediaQuery::SetOwner( char *str )
{
    int i;

    if( str )
    {
        if( lpszOwner )
            delete [] lpszOwner;

        i = strlen( str ) + 1;

        lpszOwner = new char[i];
        strcpy( lpszOwner, str );
    }
}

void MediaQuery::SetChannel( char *str )
{
    int i;

    if( str )
    {
        if( lpszChannel )
            delete [] lpszChannel;

        i = strlen( str ) + 1;

        lpszChannel = new char[i];
        strcpy( lpszChannel, str );
    }
}

void MediaQuery::SetMediaName( char *str )
{
    int i;

    if( str )
    {
        if( lpszMediaName )
            delete [] lpszMediaName;

        i = strlen( str ) + 1;

        lpszMediaName = new char[i];
        strcpy( lpszMediaName, str );
    }
}

void MediaQuery::SetMediaClass( char *vc  )
{
    if( !_stricmp( S_MC_SCREEN, vc ) )
        vcMediaClass = MC_SCREEN;
    else if( !_stricmp( S_MC_CAM, vc ) )
        vcMediaClass = MC_CAM;
    else if( !_stricmp( S_MC_VOICEIP, vc ) )
        vcMediaClass = MC_VOICEIP;
    else if( !_stricmp( S_MC_CONFERENCE, vc ) )
        vcMediaClass = MC_CONFERENCE;
    else if( !_stricmp( S_MC_MOVIE, vc ) )
        vcMediaClass = MC_MOVIE;
    else if( !_stricmp( S_MC_FTP, vc ) )
        vcMediaClass = MC_FTP;
    else if( !_stricmp( S_MC_AVCONF, vc ) )
        vcMediaClass = MC_AVCONF;
    else if( !_stricmp( S_MC_APP, vc ) )
        vcMediaClass = MC_APP;
    else
        vcMediaClass = MC_NULL;
}

void MediaQuery::SetMediaClass( int vc )
{
    vcMediaClass = vc;
}

void MediaQuery::SetMediaID( int nID )
{
    nMediaID = nID;
}

void MediaQuery::SetBitrate( int bitrate )
{
    nBitrate = bitrate;
}

char *MediaQuery::GetOwner( void )
{
    return lpszOwner;
}

char *MediaQuery::GetChannel( void )
{
    return lpszChannel;
}

char *MediaQuery::GetMediaName( void )
{
    return lpszMediaName;
}

char *MediaQuery::GetMediaClass( void )
{
    switch( vcMediaClass )
    {
        case MC_CAM:
            return S_MC_CAM;
        break;

        case MC_SCREEN:
            return S_MC_SCREEN;
        break;

        case MC_VOICEIP:
            return S_MC_VOICEIP;
        break;

        case MC_CONFERENCE:
            return S_MC_CONFERENCE;
        break;

        case MC_MOVIE:
            return S_MC_MOVIE;
        break;

        case MC_FTP:
            return S_MC_FTP;
        break;

        case MC_AVCONF:
            return S_MC_AVCONF;
        break;

        case MC_APP:
            return S_MC_APP;
        break;

        case MC_NULL:
        default:
            return "NULL";
        break;
    }
}

int MediaQuery::GetMediaID( void )
{
    return nMediaID;
}

int MediaQuery::GetBitrate( void )
{
    return nBitrate;
}

MediaQuery & MediaQuery::operator=( MediaQuery & vq )
{
    nMediaID      = vq.nMediaID;
    vcMediaClass  = vq.vcMediaClass;
    nBitrate      = vq.nBitrate;

    SetOwner    ( vq.lpszOwner     );
    SetChannel  ( vq.lpszChannel   );
    SetMediaName( vq.lpszMediaName );

    return *this;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// MediaRcvUser //
/////////////////////////////////////////////////////////////////////////////
MediaRcvUser::MediaRcvUser( void )
{
    lpszNickname = NULL;
    nStatus      = 0;
}

MediaRcvUser::~MediaRcvUser( void )
{
   if( lpszNickname )
        delete [] lpszNickname;

    nStatus  = 0;
}

void MediaRcvUser::SetNickname( char *str )
{
    int i;

    if( str )
    {
        if( lpszNickname )
            delete [] lpszNickname;

        i = strlen( str ) + 1;

        lpszNickname = new char[i];
        strcpy( lpszNickname, str );
    }
}

void MediaRcvUser::SetStatus  ( char *status )
{
    if( !_stricmp( S_VN_START, status ) )
        nStatus = VN_START;
    else if( !_stricmp( S_VN_STOP, status ) )
        nStatus = VN_STOP;
    else if( !_stricmp( S_VN_PLAY, status ) )
        nStatus = VN_PLAY;
    else if( !_stricmp( S_VN_ERROR, status ) )
        nStatus = VN_ERROR;
    else
        nStatus = VN_NULL;
}

void MediaRcvUser::SetStatus( int status )
{
    nStatus = status;
}

char *MediaRcvUser::GetNickname( void )
{
    return lpszNickname;
}

char *MediaRcvUser::GetStatus( void )
{
    switch( nStatus )
    {
        case VN_START:
            return S_VN_START;
            break;
        case VN_STOP:
            return S_VN_STOP;
            break;
        case VN_PLAY:
            return S_VN_PLAY;
            break;
        case VN_ERROR:
            return S_VN_ERROR;
            break;
        case VN_NULL:
        default:
            return S_VN_NULL;
        break;
    }
}

MediaRcvUser & MediaRcvUser::operator=( MediaRcvUser & vru )
{
    nStatus    = vru.nStatus;
    SetNickname( vru.lpszNickname );

    return *this;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// MediaQueryList //
/////////////////////////////////////////////////////////////////////////////
MediaQueryList::MediaQueryList( void )
{
    pList = new ListT<MediaQuery>;
}

MediaQueryList::~MediaQueryList( void )
{
    if( pList )
        delete pList;
}

void MediaQueryList::AddMediaQuery( MediaQuery *vq )
{
    MediaQuery *vqa  = new MediaQuery;
    *vqa = *vq;
    pList->append( vqa );
}

void MediaQueryList::DelMediaQuery( MediaQuery *vq )
{
    pList->erase( vq );
}

BOOL MediaQueryList::GetNextMediaQuery( MediaQuery *vq )
{
    MediaQuery *temp;
    BOOL ret = TRUE;

    if( !pList->GetCurNode( ) )
        temp = pList->first( );
    else
        temp = pList->next( );

    if( temp )
        *vq = *temp;
    else
    {
        vq->SetMediaID( -1 );
        ret = FALSE;
    }
    return ret;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// MediaPtP //
/////////////////////////////////////////////////////////////////////////////

MediaPtP::MediaPtP( void )
{
    pszIPLocalSource = NULL;
    pszIPSrvSource   = NULL;
    pszNickTarget    = NULL;
}

MediaPtP::~MediaPtP( void )
{
    if( pszIPLocalSource )
        delete [] pszIPLocalSource;

    if( pszNickTarget )
        delete [] pszNickTarget;

    if( pszIPSrvSource )
        delete [] pszIPSrvSource;
}

void MediaPtP::SetNickTarget( char *str )
{
    int i;

    if( str )
    {
        if( pszNickTarget )
            delete [] pszNickTarget;

        i = strlen( str ) + 1;

        pszNickTarget = new char[i];
        strcpy( pszNickTarget, str );
    }
}

void MediaPtP::SetIPLocalSource( char *ip )
{
    int i;

    if( ip )
    {
        if( pszIPLocalSource )
            delete [] pszIPLocalSource;

        i = strlen( ip ) + 1;

        pszIPLocalSource = new char[i];
        strcpy( pszIPLocalSource, ip );
    }
}

void MediaPtP::SetIPSrvSource( char *ip )
{
    int i;

    if( ip )
    {
        if( pszIPSrvSource )
            delete [] pszIPSrvSource;

        i = strlen( ip ) + 1;

        pszIPSrvSource = new char[i];
        strcpy( pszIPSrvSource, ip );
    }
}


void MediaPtP::SetMediaClass( char *mc )
{
    if( !_stricmp( S_MC_SCREEN, mc ) )
        mc_MediaClass = MC_SCREEN;
    else if( !_stricmp( S_MC_CAM, mc ) )
        mc_MediaClass = MC_CAM;
    else if( !_stricmp( S_MC_VOICEIP, mc ) )
        mc_MediaClass = MC_VOICEIP;
    else if( !_stricmp( S_MC_CONFERENCE, mc ) )
        mc_MediaClass = MC_CONFERENCE;
    else if( !_stricmp( S_MC_MOVIE, mc ) )
        mc_MediaClass = MC_MOVIE;
    else if( !_stricmp( S_MC_FTP, mc ) )
        mc_MediaClass = MC_FTP;
    else if( !_stricmp( S_MC_AVCONF, mc ) )
        mc_MediaClass = MC_AVCONF;
    else if( !_stricmp( S_MC_APP, mc ) )
        mc_MediaClass = MC_APP;
    else
        mc_MediaClass = MC_NULL;
}

void MediaPtP::SetMediaClass( int mc )
{
    mc_MediaClass = mc;
}

void MediaPtP::SetRcvSourcePort( unsigned long port )
{
    nRcvSourcePort = port;
}

void MediaPtP::SetSendSourcePort( unsigned long port )
{
    nSendSourcePort = port;
}

void MediaPtP::SetUDPPingSourcePort( unsigned long port )
{
    nRcvUDPPingSourcePort = port;
}

void MediaPtP::SetSourceBitrate( unsigned long bitrate )
{
    nSourceBitrate = bitrate;
}
char * MediaPtP::GetNickTarget( void )
{
    return pszNickTarget;
}

char * MediaPtP::GetIPLocalSource( void )
{
    return pszIPLocalSource;
}

char * MediaPtP::GetIPSrvSource( void )
{
    return pszIPSrvSource;
}

char * MediaPtP::GetMediaClass( void )
{
    switch( mc_MediaClass )
    {
        case MC_CAM:
            return S_MC_CAM;
        break;

        case MC_SCREEN:
            return S_MC_SCREEN;
        break;

        case MC_VOICEIP:
            return S_MC_VOICEIP;
        break;

        case MC_CONFERENCE:
            return S_MC_CONFERENCE;

        case MC_MOVIE:
            return S_MC_MOVIE;

        case MC_FTP:
            return S_MC_FTP;

        case MC_AVCONF:
            return S_MC_AVCONF;

        case MC_APP:
            return S_MC_APP;

        case MC_NULL:
        default:
            return "NULL";
        break;
    }
}

unsigned long MediaPtP::GetRcvSourcePort( void )
{
    return nRcvSourcePort;
}

unsigned long MediaPtP::GetSendSourcePort( void )
{
    return nSendSourcePort;
}

unsigned long MediaPtP::GetUDPPingSourcePort( void )
{
    return nRcvUDPPingSourcePort;
}

unsigned long MediaPtP::GetSourceBitrate( void )
{
    return nSourceBitrate;
}

MediaPtP & MediaPtP::operator=( MediaPtP &mptp )
{
    nRcvSourcePort        = mptp.nRcvSourcePort;
    nSendSourcePort       = mptp.nSendSourcePort;
    nRcvUDPPingSourcePort = mptp.nRcvUDPPingSourcePort;
    nSourceBitrate        = mptp.nSourceBitrate;
    mc_MediaClass         = mptp.mc_MediaClass;

    SetNickTarget( mptp.pszNickTarget );
    SetIPLocalSource( mptp.pszIPLocalSource );
    SetIPSrvSource  ( mptp.pszIPSrvSource );

    return *this;
}
///////////////////////////////////////////////////////////////////////////////