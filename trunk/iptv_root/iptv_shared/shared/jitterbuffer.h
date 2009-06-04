#ifndef _JITTERBUFFER_STR_
#define _JITTERBUFFER_STR_


#include "clbuffer.h"



/*
JitterBuffer( ULONG _nMaxPackets, ULONG _MaxPacketSize )

_nPackets     : Numero de Pacotes pre-alocados na inicializacao do buffer
_MaxPacketSize: Tamanho do pacote


BOOL insert( ULONG _seq, LPCSTR _pdata, ULONG _szdata )

_seq   : Sequencial do pacote
_pdata : buffer de dados do pacote
_szdata: tamanho do pacote (deve ser <= _MaxPacketSize)
 

Insere no buffer o pacote com o numero de sequencia _seq.
Este metodo ira percorrer a posicao correta no buffer de tal maneira
a encaixa-lo na posicao correta. Pacotes de SEQUENCIA iguais serao
substituidos.


BOOL search( ULONG _seq, LPSTR _pdata, ULONG *_pszdata )

_seq   : Sequencial do pacote
_pdata : buffer de dados do pacote
_szdata: tamanho do pacote (deve ser <= _MaxPacketSize)

Retorna dados associados ao numero de sequencia _seq.
 

BOOL remove( ULONG _seq = 0 )

Remove do buffer pacote de sequencial _seq. Caso _seq = 0, remove
o mais antigo presente no buffer ( o proximo da fila ).


ULONG first   ( LPSTR _pdata, ULONG *_pszdata )

Obtem os dados do primeiro pacote do buffer. Retorna seu identificador.
Se houver erro retorna 0.


ULONG popfirst( LPSTR _pdata, ULONG *_pszdata )

Obtem os dados do primeiro pacote do buffer. Retorna seu identificador.
Apos obter dados do pacote, remove-o do buffer.
Se houver erro retorna 0.


ULONG popbytes( LPSTR _pdata, ULONG *_pszdata )

Obtem os *_pszdata bytes do primeiro pacote do buffer. Retorna seu identificador.
Apos obter os bytes requisitados dados do pacote, remove-os do buffer.
Se houver erro retorna 0.


BOOL GetInfo( ULONG *_pnPacketsLeft, ULONG *_ppercFull, ULONG *_pnPacketsLost,
              ULONG *_pnPacketsInserted, ULONG *_pnPacketsRemoved )
_pnPacketsLeft    : [out] Numero de Pacotes disponiveis
_ppercFull        : [out] Porcentagem de preenchimento
_pnPacketsLost    : [out] Numero de Pacotes perdidos
_pnPacketsInserted: [out] Numero de Pacotes inseridos no buffer
_pnPacketsRemoved : [out] Numero de Pacotes removidos do buffer

Pega as estatisticas do buffer de tal forma a podermos fazer uma
inicializacao com mais feedback ao usuario.

*/


class JitterBuffer : public CLBuffer
{
public:
    JitterBuffer  ( ULONG _nPackets, ULONG _MaxPacketSize );

    void  flush   ( void );
    BOOL  insert  ( ULONG _seq, LPCSTR _pdata, ULONG _szdata );
    BOOL  search  ( ULONG _seq, LPSTR _pdata, ULONG *_pszdata );
    BOOL  remove  ( ULONG _seq = 0 );

    //ULONG first   ( LPSTR _pdata, ULONG *_pszdata );
    ULONG popfirst( LPSTR _pdata, ULONG *_pszdata );
    ULONG popbytes( LPSTR _pdata, ULONG *_pszdata );

    ULONG GetnPacketsLeft    (void);
    ULONG GetPercFull        (void);
    ULONG GetnPacketsLost    (void);
    ULONG GetnPacketsInserted(void);
    ULONG GetnPacketsRemoved (void);
    ULONG GetnBytesLeft      (void);

    void  GetInfo ( ULONG *_pnPacketsLeft, ULONG *_ppercFull, ULONG *_pnPacketsLost,
                    ULONG *_pnPacketsInserted, ULONG *_pnPacketsRemoved );

private:

    ULONG m_nLastId;

    ULONG m_nInitPackets;
    ULONG m_nPacketsInserted;
    ULONG m_nPacketsRemoved;
    ULONG m_nPacketsLost;
    ULONG m_nBytesLeft;
};


#endif  // _JITTERBUFFER_STR_




