#ifndef __STRING_AP
#define __STRING_AP


#define FORMAT_CHAR      '_'


#ifdef __cplusplus
  extern "C" {
#endif


void   CopyString(LPSTR lpTarget, LPCSTR lpSource, ULONG nLen);

LPSTR  StrAllocAttrib  (LPCSTR _source, ULONG _maxsize=0);
LPSTR  StrReAllocAttrib(LPCSTR _source, LPSTR _oldptr, ULONG _maxsize=0);
void * BufAllocAttrib  (void *_psource, ULONG _sz);

int    latoi (LPCSTR ent);
long   latol (LPCSTR ent);
double latof (LPCSTR ent);

int    hctoi (char hdig);
int    lhtoi (LPCSTR ent);
long   lhtol (LPCSTR ent);

LPSTR  litoa (int val, LPSTR saida);
LPSTR  lltoa (long val, LPSTR saida);
LPSTR  lgcvt (double val, LPSTR saida);

int   strlen_b   (LPCSTR str);
BOOL  iswhite    (char c);
BOOL  comp_branco(LPCSTR ent);
BOOL  is_in      (char ch, LPCSTR s);
int   ch_in_str  (char ch, LPCSTR str);
BOOL  comp_char  (LPCSTR string, UINT tam, char code);
BOOL  ha_wild    (LPCSTR string, LPWORD pos);
BOOL  isdelim    (char c);
BOOL  isctrlchar (char c);

void  SubstChar      (LPCSTR ent, LPSTR saida, int cOrig, int cSubst);
int   SubstCharToStr (LPCSTR ent , LPSTR saida, int c, LPCSTR pStr, int tsaida);

void  FilterCtrlChars(LPCSTR _ent, LPSTR _saida);
void  FilterChar     (LPCSTR _ent, LPSTR _saida, char c);
void  FilterChars    (LPCSTR _ent, LPSTR _saida, LPCSTR _pchars);

int   __toupper (int ch);
int   __tolower (int ch);
void  upper(LPCSTR ent, LPSTR saida);
void  lower(LPCSTR ent, LPSTR saida);
void  passw(LPCSTR ent, LPSTR saida);
LPSTR fill_string(LPSTR string, UINT size, int ch);
LPSTR trim(LPCSTR ent, LPSTR saida);
LPSTR strim(LPCSTR ent, LPSTR saida);
ULONG removeEOL(LPCSTR ent, LPSTR saida, ULONG blen, BOOL flMustEOL);

LPSTR ajusta_str    (LPCSTR ent, LPSTR saida, UINT tam);
LPSTR ajusta_upp_str(LPCSTR ent, LPSTR saida, UINT tam);
LPSTR ajusta_string (LPCSTR var1, UINT tam, char forma, LPSTR saida);

LPSTR form_dec(LPCSTR ent, UINT tam, UINT casas, LPSTR saida);
LPSTR form_num(LPCSTR ent, UINT tam, UINT casas, char forma, LPSTR saida);
LPSTR form_int(LPCSTR ent, UINT tam, char forma, LPSTR saida, char chPwChar);
int   tam_form_din(UINT tam, UINT casas);

BOOL  letra_padrao(char ch);

int   search_seq(LPCSTR key, LPCSTR base, UINT tam, UINT nelem);
int   search_seq_ptr(LPCSTR key, char * (FAR *base), UINT nelem);
void  sep_expr(LPCSTR ent, LPSTR esq, LPSTR dir);
BOOL  separa_elemento_da_seq(LPCSTR ent, LPSTR saida, UINT pos, LPCSTR sinais,
                           UINT tam);

void  atr_buffer  (LPSTR buf, unsigned tam, int casas, char tipo_r, LPCSTR var);
void  atr_szbuffer(LPSTR buf, unsigned tam, int casas, char tipo_r, LPCSTR var);

void  PoeFormato  (LPCSTR lpVar, LPCSTR lpFormat, LPSTR lpVarDest, char chPwChar);
void  TiraFormato (LPCSTR lpVarForm, LPCSTR lpFormat, LPSTR lpVarDest);
LPSTR form_var   (LPCSTR var, int iw, int casas, int tipo, LPCSTR formato1,
                 char just, int tedit, LPSTR saida, char chPwChar);

UINT  calc_tam_var (char tipo, UINT tam, UINT casas, LPCSTR formato);
UINT  calc_tam_var_format (LPCSTR formato);

char  crip_char(int mode, char c);
LPSTR cripto(int mode, LPCSTR ent, LPSTR saida);

LPSTR transf_buf_str(void FAR *ent, UINT tam, LPSTR saida);
void  FAR * transf_str_buf(LPCSTR ent, UINT tam, void FAR *saida);
void  FAR * ajusta_str_buf(LPCSTR ent, UINT tam, char forma, void FAR *saida);

long transf_buf_long(void FAR *ent, UINT tam);
void FAR * transf_long_buf(long ent, UINT tam, int forma, void FAR *saida);
void FAR * transf_long_str(long ent, UINT tam, int forma, LPSTR saida);

void ConvAltColnStr  (LPCSTR ent, LPSTR saida, int n);
void ConvAltColStr   (LPCSTR ent, LPSTR saida);
int  CompAltColStr   (LPCSTR lpStr1, LPCSTR lpStr2, int n);
int  CompStrAltColStr(LPCSTR lpStr, LPCSTR lpAltColStr, int n);

UINT TranslateText   (LPSTR dest, LPSTR org, int tipo, int chEof);


#ifdef __cplusplus
  }
#endif


#endif   // __STR_AP



