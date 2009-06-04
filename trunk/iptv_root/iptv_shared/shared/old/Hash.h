#ifndef _HASH_STR

#define _HASH_STR

// O hashing é obtido a partir de Mod(HASH_MODE)

typedef struct Hash_elem  far * FPHSELEM;
typedef struct Hash_reg   far * FPHSREG;
typedef struct Hash_compl far * FPHSCOMPL;


#define TNOME_SIMB    20    // Redefinido em def_ap.h
#define HASH_MODE     97
#define NMAX_SINON     5


/*
   Tabela principal
   Contém HASH_MODE linhas (Hash_reg)
*/

class far Hash
{
   private:

   HGLOBAL hmain_table;

   HGLOBAL hlcompl;
   int     line,
           offset;

   int     nitens,
           nItensNotDel;

   UINT formula(LPCSTR str);
   void atrib_elem( FPHSELEM phash_elem, LPCSTR nome, HGLOBAL helem, LPVOID pelem, BOOL flDel);
   void atrib_buf_elem( FPHSELEM phash_elem, LPSTR nome, LPHANDLE phelem, LPINT lpflDel);
   HGLOBAL get_next_compl(HGLOBAL hcompl);
   BOOL search(LPCSTR nome, LPHANDLE phelem, void far * (far *ppelem), BOOL flKill = FALSE );
   HGLOBAL get_last_compl(FPHSREG preg);
   HGLOBAL get_item(LPSTR nome, LPINT lpflDel);

   void Init(void);
   void Kill(void);


   public:

   Hash(void);
  ~Hash(void);

   void Reset    (void);
   int  GetnItens(void) { return nItensNotDel; }

   void add(LPCSTR nome, HGLOBAL helem, LPVOID pelem);
   //void add(LPCSTR nome, LPVOID  pelem);
   //void add  (LPCSTR nome, HGLOBAL helem);
   LPVOID  psearch(LPCSTR nome);

   BOOL kill (LPCSTR nome);

   HGLOBAL hsearch    (LPCSTR nome);
   HGLOBAL get_hfirst (LPSTR nome = NULL);
   HGLOBAL get_hnext  (LPSTR nome = NULL);
};


union  Handle_voidfp
{
   HGLOBAL handle;

   LPVOID  fp;
};


struct Hash_elem
{
   char name[TNOME_SIMB+1];
   BOOL flDeleted;

   Handle_voidfp def;
};


/*
   Define as linhas da tabela principal
   nsinon indica o número de ocorrências de elementos
   situados naquela linha da tabela (sinônimos)
   Se nsinom = 1 ==> handle aponta diretamente p/ handle do elemento
   Se nsinom > 1 ==> handle aponta p/ lista de tabelas complementares
   (Hash_compl)  
*/


struct Hash_reg
{
   UINT nsinon;

   Hash_elem elem;
};



/*
   Tabela complementar p/ hashing
   Contém até NMAX_SINOM elementos
   Se necessário, aponta p/ outra Tabela complementar 
*/

struct Hash_compl
{
   Hash_elem sinon[NMAX_SINON];

   HGLOBAL hnext;
};
 

#endif