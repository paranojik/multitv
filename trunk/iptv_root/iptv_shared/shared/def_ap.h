#ifndef __DEF_AP_STR__
#define __DEF_AP_STR__


//#define IPV6

#ifdef IPV6
    #define IPSTR_MAXLEN     23
#else
    #define IPSTR_MAXLEN     15
#endif

#define HOSTLEN          63
#define NICKLEN          30
#define USERLEN          10
#define PASSWDLEN        63

#define PROTOCOLLEN       3

#define FALSE             0
#define TRUE              1

#define TNOME_SIMB       30    // Redefinido em hash.h
#define TFORMATO        512
#define TVAR            512
#define TEXPR           512
#define TPATH           260
#define TREDUZIDO         8
#define TEXTENSAO         3

#define TMAX_MSG        510    // 510 + CR + LF = 512

#define TMAX_BLOB     32000
#define TAM_BUFFER      513
//#define TAM_LBUFFER    4096
//#define TAM_LBUFFER    8192
#define TAM_LBUFFER   16384
//#define TAM_LBUFFER   32768
//#define TAM_LBUFFER   65535
#define TINT_BUF          5
#define TLONG_BUF        20
#define TFLOAT_BUF       20

#define TDATA_EXIB       10   /* Tamanho dos status de relatorio/tela p/ impressao */ 
#define TPAG_EXIB         5
#define TTIT_EXIB        40

#define NMAX_PARAM       10

#define NMAX_LIN_SELECT  10  /* Numero maximo de linhas do Select */

#define TNOME_VAR        15       /* Tamanho maximo do nome de Variavel */
#define TNOME_OBJ        HOSTLEN  /* Tamanho maximo do nome de um objeto */


#define UNDEFINED_PARAM  -1


#define CR    13
#define ESC   27
#define ZERO   0

#define FL_EOF 0x1a

#define P '_'


#define EXT_TMP     ".tmp"
#define EXT_BAK     ".bak"
#define EXT_EXE     ".exe"


enum EolFormat
{
    EOL_DOS,
    EOL_UNIX
};


#endif
