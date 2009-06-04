#ifndef __FILEMW__
#define __FILEMW__


#include "sockbuffs.h"


#define FMW_RECSTATE_STOP   0x00
#define FMW_RECSTATE_REC    0x01
#define FMW_RECSTATE_PAUSE  0x02


class FileMW : public ISockBuff
{
public:

    FileMW (BOOL _flSaveIndex, BOOL _flSavePktHeader);
   ~FileMW (void);

    void  reset(void);

    BOOL  SetUrl     (LPCSTR _szWriteFile);
    BOOL  SetHeader  (void *_pHeader, ULONG _uHeaderSize);

    BOOL  OpenUrl    (void);
    BOOL  CloseUrl   (void);

    BOOL  Record     (void);
    void  Stop       (void);
    BOOL  Pause      (BOOL _flOn);
    void  Abort      (void);

    BOOL  IsOk       (void) { return  m_flOk; }
    BOOL  IsOpened   (void) { return (m_hWriteFile!=HFILE_ERROR ? TRUE : FALSE); }
    BOOL  IsRecording(void) { return ((m_dwRecState & FMW_RECSTATE_REC)   ? TRUE : FALSE); }
    BOOL  IsPaused   (void) { return ((m_dwRecState & FMW_RECSTATE_PAUSE) ? TRUE : FALSE); }
    BOOL  IsStopped  (void) { return ( m_dwRecState ? FALSE : TRUE); }

    void  SetState   (DWORD _flag)   { m_dwRecState |= _flag;  }
    void  ResetState (DWORD _flag)   { m_dwRecState &= ~_flag; }


    STDMETHODIMP Read ( /* [length_is][size_is][out] */ void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbRead);

    STDMETHODIMP Write( /* [size_is][in] */ const void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbWritten);


private:

    BOOL   m_flOk,               // Classe foi inicializada com sucesso
           m_flUrlSet,           // A funcao SetUrl() foi executada com sucesso ?
           m_flSaveIndex,        // Indica se deve ser gravada area de indices no inicio do arquivo
           m_flSavePktHeader,    // Indica se o header de pacotes MediaPkt deverah ser gravado
           m_flExit;             // Abort thread

    void  *m_pHeader;            // Caso m_flSaveInder==FALSE e m_pHeader!=NULL => grava o header especificado no inicio do arquivo
    ULONG  m_uHeaderSize;        // Tamanho do header

    HFILE  m_hWriteFile;         // Handle do arquivo para gravar medias recebidas
    DWORD  m_dwRecState;         // Indica se estah em estado de REC / STOP / PAUSE
    time_t m_tlastpkt;           // Instante de chegada do ultimo pacote

    IThread *m_pFileWriteThread; // Thread para a leitura de dados do socket

    SockBuf_reg  *m_pBuffer;     // Buffer intermediario para o armazenamento temporario dos pacotes de media

    IndexStr m_Index;            // Estrutura para o armazenamento de indices do arquivo de medias recebidas

    char m_szFilename[TPATH+1];

    _SEMAPHORE *m_pMutex;

    void   SetLastPktTime (time_t _ctime=0);
    BOOL   WriteFile      (LPVOID _buf, UINT _ldata);

    BOOL   SaveData       (LPVOID _pdataBuf, ULONG _szdata);
    BOOL   GetData        (LPVOID _pdataBuf, ULONG *_szdataBuf);

    BOOL   StartFileWriteThread(void);
    static int _DoFileWrite(IThread *pthread, void* ctx);
};



#endif  // __FILEMW__

