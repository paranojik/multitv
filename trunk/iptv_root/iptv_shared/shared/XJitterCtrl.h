#ifndef _XJITTERCTRL_STR_
#define _XJITTERCTRL_STR_


#include "ithread.h"
#include "semaphore.h"
#include "jitterbuffer.h"


class IJitterBufferEvents
{
public:
    virtual BOOL OnReadHeader (void) = 0;
    virtual BOOL OnReadPacket (void) = 0;
    virtual void OnBufferFull (void) = 0;
    virtual void OnBufferEmpty(void) = 0;
    virtual void OnTerminate  (void) = 0;
    virtual void OnAbort      (void) = 0;
};


class XJitterCtrl
{
public:

    XJitterCtrl          ( void );
   ~XJitterCtrl          ( void );

    BOOL  IsOk           ( void ) { return m_flOk; }
    BOOL  IsStarted      ( void ) { return m_flStarted; }
    BOOL  IsBufEmpty     ( void );
    void  ResetBufFull   ( void ) { m_flBufferFull = FALSE; }
    BOOL  IsBufFull      ( void );
    ULONG GetnPacketsLeft( void );

    BOOL  Initialize     ( ULONG _nSlots, ULONG _uPktSize, ULONG _uPktMaxSize, IJitterBufferEvents *_pIEvents );
    void  Flush          ( BOOL _flNotifyBuffFull=TRUE );
    BOOL  Start          ( void );
    BOOL  Resume         ( void );
    void  Stop           ( void );

    BOOL  Insert         ( ULONG _seq, LPCSTR _pdata, ULONG _szdata );
    BOOL  Remove         ( ULONG _seq );
    ULONG GetNextSeq     ( void );
    ULONG GetPacket      ( LPSTR _pdata , ULONG _szdata, ULONG *_pbytesread, BOOL _flBlock, BOOL _flMustFillBuf=FALSE );
    BOOL  SearchPacket   ( ULONG _seq, LPSTR _pdata=NULL, ULONG _szdata=0, ULONG *_pbytesread=NULL );
    void  GetInfo        ( ULONG *_pnPacketsLeft, ULONG *_ppercFull, ULONG *_pnPacketsLost,
                           ULONG *_pnPacketsInserted, ULONG *_pnPacketsRemoved );

    void  Lock           ( void );
    void  Unlock         ( void );

protected:

    static int _DoReadThread(IThread* _pthread, void* _pctx)
    {
        int ret;
        
        ret = ((XJitterCtrl *) _pctx)->ReadThreadFunc();

        //_pthread->Exit(ret);

        return ret;
    }

    int ReadThreadFunc(void);

private:

    BOOL  m_flOk,
          m_flStarted,
          m_flBufferFull,
          m_flAbort,
          m_flWaitingForData,    // Indica que foram solicitados mais dados atraves de GetPacket() do que os disponiveis => fica em estado de Waiting
          m_flEndOfData,         // Indica que nao ha dados disponiveis para serem lidos
          m_flLock,              // Indica que o buffer esta bloqueado para a leitura de pacotes atraves de GetPacket()
          m_flReadThreadRunning; // Indica que a thread de leitura estah sendo executada (apos ser executado Resume())

    ULONG m_nInitSlots,
          m_uPktSize,
          m_uMaxSlotSize,
          m_nBytesRequested;

    IJitterBufferEvents *pIEvents;
    IThread      *pReadThread;
    JitterBuffer *pJitterBuffer;

    _SEMAPHORE   m_Semaph;
    _SEMAPHORE   m_EventEndOfData;
    _SEMAPHORE   m_WaitingForData;
    _SEMAPHORE   m_ExecutingGetPacket;
};


#endif  // _XJITTERCTRL_STR_




