#ifndef _EXPR_STR
#define _EXPR_STR


#include "def_ap.h"


//#define DEF_DELIMITERS  "()[]!*/+-<>ту=#&|,;?"
//#define DEF_DELIMITERS  "()!*/<>=&,;?"
#define DEF_DELIMITERS  "#:$()!*/<>=&,;?"
#define DEF_DELIMPARSER "#:$()/<>=&,;?"
#define DEF_SIMBOLS     "<>=,;?"
#define DEF_DELIMSTRING ",;"


enum ex_tokens
{
    EX_NULL,
    EX_DELIMITER,
    EX_NEWLINE,
    EX_CR,
    EX_VARIABLE,
    EX_NUMBER,
    EX_HEXA,
    EX_ALFA,
    EX_FUNCTION,
    EX_LABEL,
    EX_VECTOR,
    EX_REMARK0,
    EX_REMARK1,
    EX_DIRET,
    EX_ERROR,
    EX_CHANNEL,
    EX_LBUFFER,
    EX_HOST,
    EX_FLAGS,
    EX_SIGNAL,
    EX_SIMBOL,
    EX_TOKEN,
};


enum http_cmd
{
    HTTPCMD_ERROR,
    HTTPCMD_EMPTYLINE,
    HTTPCMD_CONNECTION,
    HTTPCMD_GET,
    HTTPCMD_HOST,
    HTTPCMD_POST,
    HTTPCMD_OTHER,
};


class Expr
{
    LPCSTR  expr0;

    public:

    LPCSTR  expr;

    char   tok_type,
           last_delim,                  // Soh funciona para get_next_string()
           simbols    [TAM_BUFFER+1],   // Caracteres especiais (verificacao antes de qq outro caractere)
           delimiters [TAM_BUFFER+1],   // Caracteres que determinam o final do token
           token      [TAM_LBUFFER+1];  // a principio todos os simbols devem ser delimiteres
                                        // mas nem todos os delimiters devem ser simbolos


    Expr(LPCSTR _pstr=NULL, LPCSTR _pdelims=NULL, LPCSTR _psimbols=NULL);

    void reset          (void);
    void set_expr       (LPCSTR _pstr);
    void set_delimiters (LPCSTR _delims);
    void set_simbols    (LPCSTR _simbols);
    int  get_token_expr (void);   // Retorna tok_type e atribui token
    BOOL get_expr       (LPSTR saida);
    int  get_token      (void);
    int  get_next_token (void);
    int  get_next_string(LPCSTR _delimiters=DEF_DELIMSTRING);
    int  http_parse     (LPSTR _cmddatabuf=NULL, UINT _databufsize=0, LPCSTR *_pKeyVect=NULL, LPSTR *_pResultVect=NULL, UINT *_pResultSize=0);
    long get_offset     (void)  { return (expr - expr0); }
};


class ExprParser
{
    private:

    LPCSTR pData;

    UINT   nParam,
           tBuf,
           nmaxParam,
           tmaxParam,
           tmaxBuf;

    char  *pParamData;      // pParamData[nmaxParam][tmaxParam+1]
    char  *pParamBuf;       // pParamBuf [tmaxBuf]
    char  *pParamType;      // pParamType[nmaxParam]

    Expr   expr;

    void   reset (void);
    char * GetParamAddr (UINT n);

    public:

    ExprParser(UINT _nmaxParam=NMAX_PARAM, UINT _tmaxBuf=TAM_LBUFFER, LPCSTR _delims=NULL, LPCSTR _simbols=NULL, UINT _tmaxParam=TAM_BUFFER);
   ~ExprParser(void);

    UINT   GetnParam  (void)        { return nParam; }
    void   SetDelims  (LPCSTR _delims);
    void   SetSimbols (LPCSTR _simbols);
    LPCSTR GetParam   (UINT n, LPSTR pType=NULL);
    LPCSTR GetBuffer  (unsigned int *pBytes);
    UINT   parse      (LPCSTR _pData, ULONG tdata, unsigned long *texpr=NULL);
    UINT   parseSymbol(LPCSTR _pData, char _symbol);
};



class ExprJoin
{
public:

    ExprJoin (UINT _tmaxexpr, LPCSTR _sep);
   ~ExprJoin (void);

    LPCSTR GetExpr(void) { return m_pExpr; }

    void   reset       (void);
    BOOL   CanAddString(LPCSTR _data, BOOL _flCanUseSep=TRUE);
    BOOL   CanAddString(ULONG _udata, BOOL _flCanUseSep=TRUE);
    BOOL   AddString   (LPCSTR _data, BOOL _flCanUseSep=TRUE);
    BOOL   AddString   (ULONG _udata, BOOL _flCanUseSep=TRUE);

private:

    LPSTR  m_pExpr;

    BOOL   m_flOk;

    UINT   m_utMaxExpr,
           m_utExpr,
           m_utSep;

    char   m_szSep[TEXPR+1];
};



#endif
