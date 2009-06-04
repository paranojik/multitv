#define SOURCE_STRING "XFileTransfer"
#include "shared/debug.h"

#include <fcntl.h>

#include "shared/file_ct.h"
#include "shared/common.h"
#include "shared/socket/socketctrl.h"
#include "shared/socket/http-tunnel.h"

#include "shared/irm/srv_defs.h"
#include "shared/irm/irc_defs.h"


#include "XFileTransfer.h"


#define MAXBITRATE      524288
#define NBITSPERSYMBOL      10      // Numero de bits por caractere enviado (com overhead de protocolo embutido)
#define FIRSTPKTDELAY        4      // Tempo de espera para o envio do primeiro pacote (em secs)
#define FIRSTPKTDELAYPTP    10      // Tempo de espera para o envio do primeiro pacote (em secs) (PTP)
#define MAXPKTREQDELAY    1000L     // Tempo maximo de espera (em msecs) entre a chegada da notificacao de fim do arquivo <NotifyEOF()> e a solicitacao de envio dos pacotes perdidos
#define SNDEOFDELAY       8000L     // Tempo de espera (em msecs) entre o envio do ultimo pacote e a notificacao de fim de transmissao
#define SNDENDWAITDELAY   8000L     // Tempo de espera pela requisicao de pacotes perdidos para dar como encerrada a transmissao
#define MAXCONNTIME         20      // Tempo maximo de espera permitido para o estabelecimento de conexao TCP

// Os items abaixo foram desativados
#define GREATESTFIRSTSEQ    20      // Caso o seq do primeiro pacote recebido seja superior a este valor => o recebimento eh abortado
#define MAXPERCPKTLOST      25      // Se o percentual de pacotes perdidos ultrapassar este valor => aborta recebimento



XFileTransfer::XFileTransfer( void ) : m_bSendEnabled     ( FALSE ),
                                       m_bRcvEnabled      ( FALSE ),
                                       m_bSending         ( FALSE ),
                                       m_bReceiving       ( FALSE ),
                                       m_bResetting       ( FALSE ),
                                       m_bSndPaused       ( FALSE ),
                                       m_flPtP            ( FALSE ),
                                       m_uId              ( 0 ),
                                       m_pSockSnd         ( NULL ),
                                       m_pSockRcv         ( NULL ),
                                       m_Semaph           ( SEMAPH_MUTEX ),
                                       m_pTransferThread  ( NULL ),
                                       m_hFile            ( HFILE_ERROR ),
                                       m_vPktLost         ( FALSE ),
                                       m_flNotifyEOF      ( FALSE ),
                                       m_flAbort          ( TRUE )
{
}


XFileTransfer::~XFileTransfer( void )
{
    Reset();
}


BOOL XFileTransfer::Initialize( IXFTPNotify *_p )
{
    BOOL ret;

    ret = FALSE;

    if ( _p )
    {
        ret = TRUE;

        m_pFTPNotify = _p;
    }

    return ret;
}


void XFileTransfer::Reset( void )
{
    if ( !m_bResetting )
    {
        m_bResetting = TRUE;

        ResetSnd();
        ResetRcv();

        _TRACEL(LOGLEVEL3, "File Transfer framework Reseted.");

        m_bResetting = FALSE;
    }
}


void XFileTransfer::ResetSnd( void )
{
    _TRACEL(LOGLEVEL3, "File Transfer - ResetSnd started.");

    if ( IsSendEnabled() )
    {
        StopTX();

        if ( m_hFile != HFILE_ERROR )
        {
            close_ap(m_hFile);
            m_hFile = HFILE_ERROR;
        }

        m_bSendEnabled = FALSE;

        m_uId = 0;

        m_Semaph.Wait( TRUE, "File Transfer - ResetSnd");

        if (m_pSockSnd)
        {
            delete m_pSockSnd;
            m_pSockSnd = NULL;
        }

        m_vPktLost.reset();

        m_Semaph.Signal( "ResetSnd" );
    }

    _TRACEL(LOGLEVEL3, "File Transfer - ResetSnd ended.");
}


void XFileTransfer::ResetRcv( void )
{
    _TRACEL(LOGLEVEL3, "File Transfer - ResetRcv started.");

    if ( m_bRcvEnabled )
    {
        m_flAbort = TRUE;

        if (m_pSockRcv)
            m_pSockRcv->close();

        EndTransferThread();

        m_Semaph.Wait( TRUE, "File Transfer - ResetRcv" );

        m_uId = 0;

        m_bRcvEnabled = FALSE;
        m_bReceiving  = FALSE;

        if (m_pSockRcv)
        {
            delete m_pSockRcv;
            m_pSockRcv = NULL;
        }

        if ( m_hFile != HFILE_ERROR )
        {
            close_ap(m_hFile);
            m_hFile = HFILE_ERROR;

            if ( !m_flRcvComplete )
                DeleteFile( m_strFilename );
        }

        m_vPktLost.reset();

        m_Semaph.Signal( "ResetRcv" );
    }

    _TRACEL(LOGLEVEL3, "File Transfer - ResetRcv ended.");
}


void XFileTransfer::SendEvent(XFTP_CODE _code, DWORD _dwData1, DWORD _dwData2)
{
    char buf[TNOME_SIMB+1];


    *buf = '\0';

    //if (!m_flAbort && !m_bResetting && m_pFTPNotify)
    if (m_pFTPNotify)
    {
        #ifdef _DEBUG

            switch (_code)
            {
                case XFTPC_NULL:
                    strcpy(buf, "XFTPC_NULL");
                    break;
                case XFTPC_EOF:
                    strcpy(buf, "XFTPC_EOF");
                    break;
                case XFTPC_RCVP1STATS:
                    strcpy(buf, "XFTPC_RCVP1STATS");
                    break;
                case XFTPC_RCVP2STATS:
                    strcpy(buf, "XFTPC_RCVP2STATS");
                    break;
                case XFTPC_SNDP1STATS:
                    strcpy(buf, "XFTPC_SNDP1STATS");
                    break;
                case XFTPC_SNDP2STATS:
                    strcpy(buf, "XFTPC_SNDP2STATS");
                    break;
                case XFTPC_SNDERR:
                    strcpy(buf, "XFTPC_SNDERR");
                    break;
                case XFTPC_RCVERR:
                    strcpy(buf, "XFTPC_RCVERR");
                    break;
                case XFTPC_RCVSUCCESS:
                    strcpy(buf, "XFTPC_RCVSUCCESS");
                    break;
                case XFTPC_PKTREQ:
                    strcpy(buf, "XFTPC_PKTREQ");
                    break;
                case XFTPC_SNDEND:
                    strcpy(buf, "XFTPC_SNDEND");
                    break;

                default:
                    strcpy(buf, "UNKNOWN");
                    break;
            }

        #endif

        _TRACE1L(LOGLEVEL3, "File Transfer - SendEvent - Sending <%s>.", buf);
        m_pFTPNotify->OnFileTransferEvent(_code, _dwData1, _dwData2);
        _TRACE1L(LOGLEVEL3, "File Transfer - SendEvent - Event <%s> sent.", buf);
    }
}


/*
    Modifica ponteiro de send para <_seq>
*/

void XFileTransfer::SetSndSeq( ULONG _seq )
{
    m_Semaph.Wait( TRUE, "File Transfer - SetSndSeq." );

    if ( IsSending() && !m_flAbort && _seq && _seq<=m_uTotPkts )
        m_uCurSeq = _seq - 1;

    m_Semaph.Signal( "File Transfer - SetSndSeq." );
}


/*
    Notifica ao processo de receive que a transmissao foi encerrada.
    Caso hajam pacotes perdidos deve-se requisitar o seu re-envio
*/

void XFileTransfer::NotifyEOF( void )
{
    ULONG i,
          seq,
          pkti,
          pktf,
          tWait,
          nTotPktLost;


    if ( m_flNotifyEOF )
        return;

    m_flNotifyEOF = TRUE;

    m_Semaph.Wait( TRUE, "File Transfer - NotifyEOF (1)." );

    //if ( !m_flEOF )
    {
        m_flEOF = TRUE;

        if ( m_uCurSeq < m_uTotPkts )
        {
            for ( i=m_uCurSeq+1; i<=m_uTotPkts; i++ )
            {
                m_vPktLost.insert(i);
            }

            m_uCurSeq = m_uTotPkts;
        }

        if ( m_vPktLost.get_nitens() )
        {
            m_Semaph.Signal( "File Transfer - NotifyEOF (1)." );

            // Espera por um tempo aleatorio entre 0 e MAXPKTREQDELAY msecs
            tWait = GetTickCount() % MAXPKTREQDELAY;
            Wait( tWait );

            m_Semaph.Wait( TRUE, "File Transfer - NotifyEOF (2)." );

            if ( !m_flAbort )
            {
                pkti = m_vPktLost.first();

                // Socilita o re-envio dos pacotes perdidos (se houver)
                while ( pkti )
                {
                    pktf = pkti;

                    while ( (seq=m_vPktLost.next()) != 0 )
                    {
                        if ( seq == pktf+1 )
                            pktf = seq;
                        else
                            break;
                    }

                    m_Semaph.Signal( "File Transfer - NotifyEOF (2/3)." );

                    // O kernel deverah enviar ao server o comando
                    // MEDIAPKTREQUEST MediaId pkti pktf
                    SendEvent( XFTPC_PKTREQ, pkti, pktf );

                    m_Semaph.Wait( TRUE, "File Transfer - NotifyEOF (3)." );

                    pkti = seq;

                    if ( m_flAbort )
                        break;
                }
            }

            if ( !m_flAbort )
            {
                nTotPktLost = m_vPktLost.get_nitens();

                m_Semaph.Signal( "File Transfer - NotifyEOF (2/3)." );

                // Notifica ao client (receive mode) o numero de pacotes a serem recuperados
                SendEvent( XFTPC_RCVP2STATS, 0, nTotPktLost );

                m_Semaph.Wait( TRUE, "File Transfer - NotifyEOF (4)." );
            }
        }
    }

    m_Semaph.Signal( "File Transfer - NotifyEOF (1/2/3/4)." );

    m_flNotifyEOF = FALSE;
}


/*
    Solicita ao processo de send o re-envio dos pacotes no intervalo <_seqi>..<_seqf>
*/

void XFileTransfer::PktRequest( ULONG _seqi, ULONG _seqf )
{
    ULONG i;

    m_Semaph.Wait( TRUE, "File Transfer - PktRequest");

    if ( _seqf > m_uTotPkts )
        _seqf = m_uTotPkts;

    if ( !_seqf || _seqf<_seqi )
        _seqf = _seqi;

    if ( IsSending() && !m_flAbort && _seqi && _seqi<=m_uTotPkts )
    {
        for ( i=_seqi; i<=_seqf; i++ )
        {
            if ( !m_vPktLost.search(i) && m_vPktLost.insert(i) )
                m_nSndTotPktReq++;
        }

        if ( m_tSndEnd && m_vPktLost.get_nitens() )
            m_tSndEnd = 0;

        m_Semaph.Signal( "File Transfer - PktRequest" );
        // Notifica ao client (send mode) o numero de pacotes a serem re-enviados
        SendEvent( XFTPC_SNDP2STATS, m_nSndPktReq, m_nSndTotPktReq );
    }
    else
    {
        m_Semaph.Signal( "File Transfer - PktRequest" );
    }
}


void XFileTransfer::SndPause( void )
{
    if ( !m_bSndPaused && IsSending() )
    {
        m_bSndPaused = TRUE;
        m_SemaphSndPause.Wait( TRUE, "m_SemaphSndPause - SndPause" );
    }
}


void XFileTransfer::SndResume( void )
{
    if ( m_bSndPaused )
    {
        m_SemaphSndPause.Signal( "m_SemaphSndPause - SndResume" );
        m_bSndPaused = FALSE;
    }
}


BOOL XFileTransfer::SendPkt( void )
{
    BOOL  ret,
          flWaiting;

    ULONG seq,
          offset;

    long  pktsize;

    char  buf[TAM_LBUFFER];
    LPSTR pData;

    FTPHeader *pFTPHeader;


    ret = TRUE;

    flWaiting = TRUE;
    m_Semaph.Wait( TRUE, "SendPkt (1)");

    if ( m_flEOF )
        seq = m_vPktLost.popFirst();
    else
        seq = m_uCurSeq + 1;

    if ( seq && seq<=m_uTotPkts )
    {
        ret = FALSE;

        pFTPHeader = (FTPHeader *) buf;
        pData      = buf + sizeof(FTPHeader);

        pFTPHeader->m.id    = m_uId;
        pFTPHeader->m.seq   = seq;
        pFTPHeader->m.type  = MTYPE_FILE;
        pFTPHeader->m.flags = 0;

        if ( (seq == m_uTotPkts) &&
             (m_uFileSize % m_uPktDataLen) )
        {
            pFTPHeader->m.datalen = (USHORT) (m_uFileSize % m_uPktDataLen);
        }
        else
            pFTPHeader->m.datalen = m_uPktDataLen;

        offset = (seq - 1) * m_uPktDataLen;

        if ( seek_ap(m_hFile, offset, 0)!=HFILE_ERROR &&
             read_ap(m_hFile, pData, pFTPHeader->m.datalen)==pFTPHeader->m.datalen )
        {
            pFTPHeader->chksum = adler32(0, (BYTE *)pData, pFTPHeader->m.datalen);

            pktsize = sizeof(FTPHeader) + pFTPHeader->m.datalen;

            pFTPHeader->m.datalen += sizeof(ULONG); // checksum

            if ( m_pSockSnd->write(buf, pktsize, INVALID_SOCKET, FALSE, TRUE, m_bSndUDP, m_flPtP) == pktsize )
            {
                ret = TRUE;

                if ( seq-1 == m_uCurSeq )
                    m_uCurSeq++;

                if ( !m_flEOF )
                {
                    flWaiting = FALSE;
                    m_Semaph.Signal( "SendPkt (1)");

                    SendEvent( XFTPC_SNDP1STATS, m_uCurSeq, m_uTotPkts );

                    if ( m_uCurSeq==m_uTotPkts )
                    {
                        m_flEOF = TRUE;

                        // Aguarda alguns instantes para dar como encerrada a transmissao principal
                        Wait( SNDEOFDELAY );

                        // Ultimo pacote enviado => enviar notificacao de EOF aos usuarios
                        // que estao recebendo o arquivo
                        SendEvent( XFTPC_EOF, m_uTotPkts, 0 );

                        flWaiting = TRUE;
                        m_Semaph.Wait( TRUE, "SendPkt (2)");

                        // Caso nao hajam pacotes perdidos a serem enviados
                        // determina tempo maximo de espera para a finalizacao
                        // do processo de send
                        if ( !m_flAbort && !m_vPktLost.get_nitens() )
                            m_tSndEnd = GetTickCount() + SNDENDWAITDELAY;
                    }
                }
                else
                {
                    m_nSndPktReq++;

                    m_Semaph.Signal( "SendPkt (1)");

                    SendEvent( XFTPC_SNDP2STATS, m_nSndPktReq, m_nSndTotPktReq );

                    m_Semaph.Wait( TRUE, "SendPkt (3)");

                    // Caso nao hajam mais pacotes perdidos a serem enviados
                    // determina tempo maximo de espera para a finalizacao
                    // do processo de send
                    if ( !m_flAbort && !m_vPktLost.get_nitens() )
                    {
                        flWaiting = FALSE;
                        m_Semaph.Signal( "SendPkt (3)");

                        // Ultimo pacote enviado => enviar notificacao de EOF aos usuarios
                        // que estao recebendo o arquivo
                        SendEvent( XFTPC_EOF, m_uTotPkts, 0 );

                        m_tSndEnd = GetTickCount() + SNDENDWAITDELAY;
                    }
                }
            }
        }
    }

    if (flWaiting)
        m_Semaph.Signal( "SendPkt (1/2/3)");

    return ret;
}
 

int XFileTransfer::TransferThreadFunc( void )
{
    BOOL    flExit,
            flWaiting;
    time_t  ctime;
    ULONG64 tWait;


    _TRACEL(LOGLEVEL3, "File Transfer - TransferThreadFunc started.");

    tWait = 0;

    m_Semaph.Wait( TRUE, "TransferThreadFunc (0)");

    if ( IsSending() )
    {
        time(&ctime);

        if ( m_tFirstPktTime > ctime )
            tWait = (m_tFirstPktTime - ctime) * 1000L;
    }

    m_Semaph.Signal( "TransferThreadFunc (0)" );

    if ( tWait )
        Wait((ULONG)tWait);

    flExit = FALSE;

    while ( !flExit )
    {
        // Caso seja forcada pausa na transmissao fica aguardando por SndResume()
        m_SemaphSndPause.Wait( TRUE, "m_SemaphSndPause - TransferThreadFunc" );
        m_SemaphSndPause.Signal( "m_SemaphSndPause - TransferThreadFunc" );

        flWaiting = TRUE;
        m_Semaph.Wait( TRUE, "TransferThreadFunc (1)");

        if ( m_flAbort || (!IsReceiving() && !IsSending()) )
            flExit = TRUE;

        if ( !flExit )
        {
            //if ( IsSending() )
            if ( IsSendEnabled() )
            {
                flWaiting = FALSE;
                m_Semaph.Signal( "TransferThreadFunc (1)" );

                // Se o arquivo nao tiver sido enviado completamente => envia
                // proximo pacote
                // Mesmo apos o envio do ultimo pacote a thread permanece ativa
                // aguardando possiveis requisicoes de pacotes perdidos
                if ( !SendPkt() )
                {
                    // Erro na leitura do arquivo ou na transmissao
                    flExit = TRUE;
                    SendEvent( XFTPC_SNDERR, 0, 0 );
                }

                // Caso nao hajam mais pacotes a serem enviados e expirou o
                // tempo de espera para requisicoes de pacotes => aborta processo de send
                if ( m_tSndEnd && GetTickCount()>m_tSndEnd )
                {
                    flExit = TRUE;
                    SendEvent( XFTPC_SNDEND, 0, 0 );
                }
            }
            else
            {
                flWaiting = FALSE;
                m_Semaph.Signal( "TransferThreadFunc (1)" );

                if ( ReceivePkt() )
                {
                    if ( m_nRcvPkts == m_uTotPkts )
                    {
                        flExit = TRUE;
                        m_flRcvComplete = TRUE;
                        SendEvent( XFTPC_RCVSUCCESS, 0, 0 );
                    }
                }
                else
                {
                    flExit = TRUE;
                    SendEvent( XFTPC_RCVERR, 0, 0 );
                }
            }
        }

        if ( flWaiting )
            m_Semaph.Signal( "TransferThreadFunc (1)" );

        if ( !m_flAbort && !flExit && m_tSndWait )
            Wait( m_tSndWait );
    }

    _TRACEL(LOGLEVEL3, "File Transfer - TransferThreadFunc ended.");

    return 0;
}


BOOL XFileTransfer::StartTransferThread( void )
{
    BOOL ret;

    ret = FALSE;

    if ( !m_pTransferThread )
    {
        m_pTransferThread = CreateIThread( _DoPlayThread, this, TRUE );

        if ( m_pTransferThread )
            ret = TRUE;
    }

    return ret;
}


void XFileTransfer::EndTransferThread( void )
{
    if ( m_pTransferThread )
    {
        m_flAbort = TRUE;

        //m_pTransferThread->WaitForMe(WAITFORENDTHREADSTIME);
        m_pTransferThread->WaitForMe(INFINITE);
        delete m_pTransferThread;

        m_pTransferThread = NULL;
    }
}


BOOL XFileTransfer::SaveRcvPkt(LPCSTR _buf, UINT _blen)
{
    BOOL   ret,
           flSave;

    UINT   i;
    ULONG  offset,
           pktsize,
           TotPktLost;

    time_t ctime;

    LPCSTR pData;

    FTPHeader *pFTPHeader;


    ret    = FALSE;
    flSave = FALSE;

    m_Semaph.Wait( TRUE, "File Transfer - SaveRcvPkt (1)" );

    // Se StopRX() for executado descarta os pacotes recebidos da rede
    if ( !m_flAbort && IsReceiving() && _buf && _blen >= sizeof(FTPHeader) )
    {
        ret = TRUE;

        pFTPHeader = (FTPHeader *) _buf;

        //if ( pFTPHeader->m.id && pFTPHeader->m.seq && _blen>=(sizeof(MediaPkt) + pFTPHeader->m.datalen) && pFTPHeader->m.datalen>sizeof(ULONG) )
        if ( pFTPHeader->m.seq && _blen>=(sizeof(MediaPkt) + pFTPHeader->m.datalen) && pFTPHeader->m.datalen>sizeof(ULONG) )
        {
            pData   = _buf + sizeof(FTPHeader);
            pktsize = pFTPHeader->m.datalen - sizeof(ULONG);

            if ( pFTPHeader->chksum == adler32(0, (BYTE *)pData, pktsize) )
                flSave = TRUE;
            else
            {
                _TRACE1L(LOGLEVEL3, "File Transfer - SaveRcvPkt chksum error: Seq %lu.", pFTPHeader->m.seq);
            }
        }

        if ( m_flPtP && pFTPHeader->m.seq!=m_uCurSeq+1 )
        {
            ret = FALSE;
            _TRACE2L(LOGLEVEL3, "XFileTransfer::SaveRcvPkt: (PTP) - packet lost detected => Abort! LastSeq:%lu CurSeq:%lu.", m_uCurSeq, pFTPHeader->m.seq);
        }

        if ( ret && flSave )
        {
            flSave = FALSE;

            time(&ctime);

            if ( pFTPHeader->m.seq > m_uCurSeq+1 )
            {
                UINT nPktLost;
                char bzero[TAM_LBUFFER];

                nPktLost = pFTPHeader->m.seq - (m_uCurSeq+1);

                // Se o numero de pacotes perdidos ultrapassar <m_uMaxRcvPktLost>
                // ou o sequencial do pacote for inconsistente
                // => aborta recebimento
                if ( pFTPHeader->m.seq <= m_uTotPkts &&
                     //nPktLost + m_vPktLost.get_nitens() <= m_uMaxRcvPktLost &&
                     seek_ap(m_hFile, 0L, SEEK_END)!=HFILE_ERROR )
                {
                    flSave = TRUE;

                    memset(bzero, '\0', m_uPktDataLen);

                    _TRACE2L(LOGLEVEL3, "File Transfer - SaveRcvPkt Lost Pkts: Seq %lu->%lu .", m_uCurSeq+1, pFTPHeader->m.seq-1);

                    // Insere blocos em branco a serem preenchidos posteriormente
                    for ( i=m_uCurSeq+1; i<pFTPHeader->m.seq; i++ )
                    {
                        // Acrescenta sequencial a lista de pacotes perdidos
                        m_vPktLost.insert(i);

                        if ( write_ap(m_hFile, bzero, m_uPktDataLen) != m_uPktDataLen )
                        {
                            ret    = FALSE;
                            flSave = FALSE;

                            break;
                        }
                    }

                    TotPktLost = m_vPktLost.get_nitens();

                    m_Semaph.Signal( "File Transfer - SaveRcvPkt (1)" );

                    // Notifica ao client (receive mode) o numero de pacotes a serem recuperados
                    SendEvent( XFTPC_RCVP2STATS, 0, TotPktLost );

                    m_Semaph.Wait( TRUE, "File Transfer - SaveRcvPkt (2)" );
                }
                else
                    ret = FALSE;
            }
            else if ( pFTPHeader->m.seq < m_uCurSeq+1 )
            {
                UINT ind;

                // Verifica se o pacote ainda nao foi gravado em disco
                // por ter sido perdido anteriormente
                if ( (ind=m_vPktLost.search(pFTPHeader->m.seq)) != 0 )
                {
                    _TRACE1L(LOGLEVEL3, "File Transfer - SaveRcvPkt Rec Pkt: Seq %lu.", pFTPHeader->m.seq);

                    // Posiciona ponteiro na posicao correspondente do pacote recebido
                    offset = (pFTPHeader->m.seq - 1) * m_uPktDataLen;

                    if ( seek_ap(m_hFile, offset, SEEK_SET) != HFILE_ERROR )
                        flSave = TRUE;
                    else
                        ret = FALSE;

                    // Remove sequencial da lista de pacotes perdidos
                    m_vPktLost.remove(ind, FALSE);
                }
            }
            else
            {
                // Append
                if ( seek_ap(m_hFile, 0L, SEEK_END)!=HFILE_ERROR )
                    flSave = TRUE;
                else
                    ret = FALSE;
            }

            if ( flSave && ret && !m_flAbort )
            {
                // Se o inicio do recebimento atrasar a ponto do seq do primeiro
                // pacote recebido ser superior a GREATESTFIRSTSEQ => aborta operacao
                if ( //(m_uCurSeq || m_uCurSeq<=GREATESTFIRSTSEQ) &&
                     (write_ap(m_hFile, (PVOID)pData, pktsize) == pktsize) )
                {
                    m_nRcvPkts++;

                    TotPktLost = m_vPktLost.get_nitens();

                    m_Semaph.Signal( "File Transfer - SaveRcvPkt (1/2)" );

                    if ( pFTPHeader->m.seq > m_uCurSeq )
                        m_uCurSeq = pFTPHeader->m.seq;
                    else
                        SendEvent( XFTPC_RCVP2STATS, 0, TotPktLost );

                    SendEvent( XFTPC_RCVP1STATS, m_uCurSeq, m_uTotPkts );

                    _TRACE1L(LOGLEVEL3, "File Transfer - SaveRcvPkt Seq #%lu saved.", pFTPHeader->m.seq);

                    m_Semaph.Wait( TRUE, "File Transfer - SaveRcvPkt (3)" );
                }
                else
                    ret = FALSE;
            }

            if ( !ret )
            {
                _TRACE1L(LOGLEVEL3, "File Transfer - SaveRcvPkt Seq #%lu save error.", pFTPHeader->m.seq);
            }
        }
    }

    m_Semaph.Signal( "File Transfer - SaveRcvPkt (1/2/3)" );

    return ret;
}


BOOL XFileTransfer::ReceivePkt( void )
{
    BOOL   ret;

    int    blen,
           pktlen;

    char   buf[TAM_LBUFFER];

    MediaPkt *pMediaPkt;


    _TRACEL(LOGLEVEL3, "File Transfer - ReceivePkt started.");

    ret  = FALSE;
    blen = 0;

    if ( !m_flAbort )
    {
        if ( m_bRcvUDP )
        {
            _TRACEL(LOGLEVEL3, "File Transfer - OnReadPacket reading UDP sock.");

            if ( m_pSockRcv )
            {
                blen = m_pSockRcv->read(buf, TAM_LBUFFER);

                _TRACE1L(LOGLEVEL3, "File Transfer - ReceivePkt %ld bytes read from UDP sock.", blen);

                if ( !m_flAbort && blen>0 )
                {
                    if ( SaveRcvPkt(buf, blen) )
                        ret = TRUE;
                }
            }
        }
        else
        {
            if ( m_pSockRcv )
                blen = m_pSockRcv->read(buf, sizeof(MediaPkt), NULL, INVALID_SOCKET, TRUE);

            if ( !m_flAbort && blen==sizeof(MediaPkt) )
            {
                pMediaPkt = (MediaPkt *) buf;
                pktlen    = blen;

                if ( m_pSockRcv )
                    blen = m_pSockRcv->read(&buf[pktlen], pMediaPkt->datalen, NULL, INVALID_SOCKET, TRUE);
                else
                    blen = 0;

                if ( !m_flAbort && blen==pMediaPkt->datalen )
                {
                    pktlen += pMediaPkt->datalen;

                    if ( SaveRcvPkt(buf, pktlen) )
                        ret = TRUE;
                }
            }
            else
            {
                if ( !m_flAbort && m_pSockRcv )
                {
                    // Erro no TCP
                    // => Deve reconectar-se no server e zerar dados do buffer
                    m_pSockRcv->close();

                    if ( !m_flPtP )
                        ret = ConnectTCP(m_pSockRcv, m_uId, FALSE);
                }
            }
        }
    }

    _TRACEL(LOGLEVEL3, "File Transfer - ReceivePkt ended.");

    return ret;
}


BOOL XFileTransfer::ConnectTCP( SocketCtrl *_pSock, ULONG _nId, BOOL _flSend )
{
    BOOL ret,
         flConnectOk;

    char msg[TMAX_MSG+1];
    int  tstr;


    ret = FALSE;
    flConnectOk = FALSE;

    if ( !m_flAbort && _pSock && _pSock->isSockOk() )
    {
        //flConnectOk = _pSock->connect(m_strHost, S_PT_TCP, m_port, NULL, &SockConn, NULL, FALSE);
        //if ( flConnectOk && !_pSock->TestWriteSock(SockConn, MAXCONNTIME) )
        //    flConnectOk = FALSE;

        //flConnectOk = _pSock->connect(m_strHost, S_PT_TCP, m_port);

        if ( !m_flAbort )
        {
            if (m_port == HTTP_PORT)
            {
                HTTPTunnel http;

                char service  [SERVICELEN+1],
                     service_r[SERVICELEN+1];

                if (_flSend)
                    strcpy(service, S_SERVICE_IRM_MEDIASND);
                else
                    strcpy(service, S_SERVICE_IRM_MEDIARCV);

                flConnectOk = FALSE;

				// Daniel Gouvêa: This code was broken by the new version of iptv_shared
                //if ( http.connect(m_strHost, NULL, service) )
                //{
                //    if ( http.WaitForServiceLine(service_r) )
                //    {
                //        if ( _stricmp(service_r, service) == 0 )
                //        {
                //            if ( _pSock->AddActiveSock(http.GetHTTPSock(), TRUE) &&
                //                 http.RemoveSock() )
                //            {
                //                flConnectOk = TRUE;
                //            }
                //        }
                //    }
                //}

                if (!flConnectOk)
                    _pSock->close();
            }
            else
                flConnectOk = _pSock->connect(m_strHost, S_PT_TCP, m_port);

            if (flConnectOk)
            {
                if ( !m_flPtP && !_flSend )
                {
                    // MEDIARCVRUN ID nick

                    sprintf(msg, "%s %lu %s", S_SRVCMD_MEDIARCVRUN, _nId, (LPCSTR)m_strUser);
                    tstr = strlen(msg)+1;

                    if ( _pSock->write(msg, tstr) == tstr )
                        ret = TRUE;
                }
                else
                    ret = TRUE;
            }
        }
    }

    return ret;
}


BOOL XFileTransfer::ListenTCP( SocketCtrl *_pSock )
{
    BOOL ret,
         flConnectOk;

    SOCKET SockConn;


    ret = FALSE;

    if ( !m_flAbort && _pSock && _pSock->isSockOk() )
    {
        flConnectOk = _pSock->listen(S_PT_TCP, SOCK_QLEN, m_port);

        //if ( flConnectOk && _pSock->readall(NULL, 0, FALSE, TRUE, MAXCONNTIME, &SockConn)==0 && SockConn!=INVALID_SOCKET && !m_flAbort )
        if ( flConnectOk && _pSock->readall(NULL, 0, FALSE, TRUE, 300, &SockConn)==0 && SockConn!=INVALID_SOCKET && !m_flAbort )
        {
            ret = TRUE;

            _pSock->SetMasterSocket(SockConn);
        }
    }

    return ret;
}


BOOL XFileTransfer::InitializeRcvNetwork( ULONG _nId, LPCSTR _protocol, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szUser, LPCSTR _szChannel, BOOL _flListen )
{
    BOOL ret;

    ret = FALSE;

    m_pSockRcv = new SocketCtrl;
    m_bRcvUDP  = FALSE;
    m_flListen = _flListen;

    if ( m_pSockRcv && m_pSockRcv->isSockOk() )
    {
        if ( _stricmp(_protocol, S_PT_UDP) == 0 )
        {
            if ( m_pSockRcv->listen(S_PT_UDP, SOCK_QLEN, _nPort) )
            {
                ret = TRUE;
                m_bRcvUDP  = TRUE;
                m_flListen = TRUE;
            }
        }
        else if ( _stricmp(_protocol, S_PT_TCP) == 0 )
        {
            if ( m_flPtP || (!comp_branco(_szUser) && !comp_branco(_szChannel)) )
            {
                if ( !comp_branco(_szHost) )
                    m_strHost = _szHost;
                else
                    m_strHost = "";

                m_strUser    = _szUser;
                m_strChannel = _szChannel;
                m_port       = _nPort;

                if ( !m_flListen )
                {
                    if ( !comp_branco(_szHost) && ConnectTCP(m_pSockRcv, _nId, FALSE) )
                        ret = TRUE;
                }
                else
                {
                    if ( ListenTCP(m_pSockRcv) )
                        ret = TRUE;
                }
            }
        }
        else if ( _stricmp(_protocol, S_PT_MCAST) == 0 )
        {
            if ( m_pSockRcv->listen(S_PT_UDP, SOCK_QLEN, _nPort) &&
                 m_pSockRcv->AddMembership(_szHost) )
            {
                m_pSockRcv->SetDefSin(_szHost, _nPort);

                ret = TRUE;
                m_bRcvUDP  = TRUE;
                m_flListen = TRUE;
            }
        }
    }

    if ( m_pSockRcv && !ret )
    {
        delete m_pSockRcv;
        m_pSockRcv = NULL;
    }

    return ret;
}


BOOL XFileTransfer::InitializeSndNetwork( LPCSTR _protocol, LPCSTR _szHost, UINT16 _nPort, BOOL _flListen )
{
    BOOL ret;

    ret = FALSE;

    m_pSockSnd = new SocketCtrl;
    m_bSndUDP  = FALSE;
    m_flListen = _flListen;

    if ( m_pSockSnd && m_pSockSnd->isSockOk() )
    {
        if ( _stricmp(_protocol, S_PT_UDP) == 0 )
        {
            if ( m_pSockSnd->connect(_szHost, _protocol, _nPort) )
            {
                ret = TRUE;
                m_bSndUDP  = TRUE;
                m_flListen = FALSE;
            }
        }
        else if ( _stricmp(_protocol, S_PT_TCP) == 0 )
        {
            if ( !comp_branco(_szHost) )
                m_strHost = _szHost;
            else
                m_strHost = "";

            m_port = _nPort;

            if ( !m_flListen )
            {
                if ( !comp_branco(_szHost) && ConnectTCP(m_pSockSnd, 0, TRUE) )
                    ret = TRUE;
            }
            else
            {
                if ( ListenTCP(m_pSockSnd) )
                    ret = TRUE;
            }
        }
        else if ( _stricmp(_protocol, S_PT_MCAST) == 0 )
        {
            ret = FALSE;
        }
    }

    if ( m_pSockSnd && !ret )
    {
        delete m_pSockSnd;
        m_pSockSnd = NULL;
    }

    return ret;
}


BOOL XFileTransfer::VerifyProtocol(LPCSTR _protocol)
{
    BOOL ret;

    ret = FALSE;

    if ( !comp_branco(_protocol) )
    {
        if ( _stricmp(_protocol, S_PT_UDP)   == 0 ||
             _stricmp(_protocol, S_PT_TCP)   == 0 ||
             _stricmp(_protocol, S_PT_MCAST) == 0 )
        {
            ret = TRUE;
        }
    }

    return ret;
}


BOOL XFileTransfer::BeginReceive( UINT32 _nId, LPCSTR _protocol, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szUser, LPCSTR _szChannel, LPCSTR _szFilePath, UINT _uFileSize, USHORT _uPktDataLen, BOOL _flPtP, BOOL _flListen )
{
    BOOL ret;

    ret = FALSE;

    _TRACEL(LOGLEVEL3, "File Transfer - BeginReceive() started.");

    //m_Semaph.Wait( TRUE, "BeginReceive" );

    if ( !IsRcvEnabled() && !IsSendEnabled() && (_flPtP || _nId) && _nPort && VerifyProtocol(_protocol) && _uFileSize && !comp_branco(_szFilePath) &&
         (m_hFile=open_ap(_szFilePath, NULL, O_CREAT | O_RDWR | O_BINARY | O_TRUNC))!=HFILE_ERROR )
    {
        m_flAbort = FALSE;
        m_flPtP   = _flPtP;

        if ( InitializeRcvNetwork( _nId, _protocol, _szHost, _nPort, _szUser, _szChannel, _flListen ) )
        {
            ret = TRUE;

            m_strFilename = _szFilePath;

            if ( _uPktDataLen )
                m_uPktDataLen = _uPktDataLen;
            else
                m_uPktDataLen = GetSndDataLen();

            m_uPktSize    = sizeof(FTPHeader) + m_uPktDataLen;
            m_tSndWait    = 0;
            m_nRcvPkts    = 0;
            m_uFileSize   = _uFileSize; 
            m_uTotPkts    = m_uFileSize / m_uPktDataLen;

            if ( m_uFileSize % m_uPktDataLen )
                m_uTotPkts++;

            m_uMaxRcvPktLost = (MAXPERCPKTLOST * m_uTotPkts) / 100;

            m_flEOF         = FALSE;
            m_flRcvComplete = FALSE;
            m_bRcvEnabled   = TRUE;
            m_uId           = _nId;
        }
        else
        {
            m_flAbort = TRUE;
            close_ap(m_hFile);
            m_hFile = HFILE_ERROR;
        }
    }

    //m_Semaph.Signal( "BeginReceive" );

    _TRACE1L(LOGLEVEL3, "File Transfer - BeginReceive() ended ret=%d.", ret);

    return ret;
}


BOOL XFileTransfer::BeginSend( UINT32 _nId, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szFilePath, ULONG _uBitRate, LPCSTR _protocol, BOOL _flPtP, BOOL _flListen )
{
    BOOL ret;

    ret = FALSE;

    _TRACEL(LOGLEVEL3, "File Transfer - BeginSend() started.");

    //m_Semaph.Wait();
    //m_Semaph.Wait( TRUE, "BeginSend" );

    if ( !IsRcvEnabled() && !IsSendEnabled() && (_flPtP || _nId) && !comp_branco(_szHost) && VerifyProtocol(_protocol) &&
         _nPort && _uBitRate>=NBITSPERSYMBOL && _uBitRate<=MAXBITRATE &&
         (m_uFileSize=GetFileLength(_szFilePath)) > 0 &&
         (m_hFile=open_ap(_szFilePath, NULL, O_RDONLY | O_BINARY))!=HFILE_ERROR )
    {
        m_flAbort = FALSE;
        m_flPtP   = _flPtP;

        if ( InitializeSndNetwork( _protocol, _szHost, _nPort, _flListen ) )
        {
            ret = TRUE;

            m_strFilename = _szFilePath;

            m_uPktDataLen = GetSndDataLen();
            m_uPktSize    = GetSndPktSize();
            m_tSndWait    = (1000 * m_uPktSize) / ( (_uBitRate*1024) / NBITSPERSYMBOL );
            m_uTotPkts    = m_uFileSize / m_uPktDataLen;

            if ( m_uFileSize % m_uPktDataLen )
                m_uTotPkts++;

            m_nSndTotPktReq = 0;
            m_nSndPktReq    = 0;
            m_tSndEnd       = 0;
            m_uId           = _nId;
            m_flPtP         = _flPtP;
            m_flEOF         = FALSE;
            m_bSendEnabled  = TRUE;
            m_flAbort       = FALSE;

            m_vPktLost.reset();
        }

        if (!ret)
        {
            m_flAbort = TRUE;

            if (m_pSockSnd)
            {
                delete m_pSockSnd;
                m_pSockSnd = NULL;
            }

            close_ap(m_hFile);
            m_hFile = HFILE_ERROR;
        }
    }

    //m_Semaph.Signal();
    //m_Semaph.Signal( "BeginSend" );

    _TRACE1L(LOGLEVEL3, "File Transfer - BeginSend() ended ret=%d.", ret);

    return ret;
}


BOOL XFileTransfer::RunTX( void )
{
    BOOL ret;

    ret = FALSE;

    _TRACEL(LOGLEVEL3, "File Transfer - RunTX() started.");

    m_Semaph.Wait( TRUE, "RunTX" );

    if ( IsSendEnabled() && !IsSending() )
    {
        m_uCurSeq = 0;

        _TRACEL(LOGLEVEL3, "File Transfer - RunTX() executing.");

        time(&m_tFirstPktTime);

        if (m_flPtP)
            m_tFirstPktTime += FIRSTPKTDELAYPTP;
        else
            m_tFirstPktTime += FIRSTPKTDELAY;

        if ( seek_ap(m_hFile, 0L, SEEK_SET)!=HFILE_ERROR &&
             StartTransferThread() )
        {
            ret = TRUE;
            m_bSending = TRUE;
        }
    }

    m_Semaph.Signal( "RunTX" );

    _TRACE1L(LOGLEVEL3, "File Transfer - RunTX() ended ret=%d.", ret);

    return ret;
}

BOOL XFileTransfer::StopTX( void )
{
    BOOL ret;

    ret = FALSE;

    _TRACEL(LOGLEVEL3, "File Transfer - StopTX() started.");

    //m_Semaph.Wait( TRUE, "StopTX" );

    if ( IsSending() )
    {
        m_bSending = FALSE;

        if (m_pSockSnd)
            m_pSockSnd->close();

        if ( m_bSndPaused )
            SndResume();

        EndTransferThread();

        ret = TRUE;
    }

    //m_Semaph.Signal( "StopTX" );

    _TRACE1L(LOGLEVEL3, "File Transfer - StopTX() ended ret=%d.", ret);

    return ret;
}


BOOL XFileTransfer::RunRX( void )
{
    BOOL ret;

    ret = FALSE;

    _TRACEL(LOGLEVEL3, "File Transfer - RunRX() started.");

    m_Semaph.Wait( TRUE, "RunRX" );

    if ( IsRcvEnabled() && !IsReceiving() )
    {
        m_uCurSeq = 0;

        if ( StartTransferThread() )
        {
            ret = TRUE;
            m_bReceiving = TRUE;
        }
    }

    m_Semaph.Signal( "RunRX" );

    _TRACE1L(LOGLEVEL3, "File Transfer - RunRX() ended ret=%d.", ret);

    return ret;
}

BOOL XFileTransfer::StopRX( void )
{
    BOOL ret;

    ret = FALSE;

    _TRACEL(LOGLEVEL3, "File Transfer - StopRX() started.");

    if ( IsReceiving() )
    {
        ResetRcv();
        ret = TRUE;
    }

    _TRACE1L(LOGLEVEL3, "File Transfer - StopRX() ended ret=%d.", ret);

    return ret;
}


