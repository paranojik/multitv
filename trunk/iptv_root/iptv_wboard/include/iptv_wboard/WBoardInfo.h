#ifndef _WBOARDINFO_H
#define _WBOARDINFO_H


#define A4_WIDTH          765
#define A4_HEIGHT         1088

#define DIF_PEN           2
#define DIF_SCROLL        20
#define SB_SIZE           17
#define PTR_WIDTH         11
#define PTR_HEIGHT        7

#define FONT_SIZE_DEF     10

#define FD_STRIKEOUT      0x01
#define FD_UNDERLINE      0x02
#define FD_BOLD           0x04
#define FD_ITALIC         0x08

#define DC_REPAINT        0x01
#define DC_AUTOSCROLL     0x02

#define TMAX_PATH         128
#define TMAX_TXT          255
#define TFONT_NAME        30
#define NMAX_TRANS        10

#define END_POS           0xFFFF

#define WBN_ADDELEMENT    0x10
#define WBN_ADDPOINTS     0x20
#define WBN_NEWEXEC       0x30
#define WBN_UNDOEXEC      0x40
#define WBN_IMGTRANSFER   0x50
#define WBN_CLOSEWND      0x60
#define WBN_IMGSTOP       0x70

#define WB_OK             0
#define WB_ERROR          1
#define WB_SND_IMG        2
#define WB_RCV_IMG        3

#define WB_CALLTYPE       __stdcall

typedef long              WB_EVENT;
typedef long              WB_RESULT;
typedef void*             WB_HMSG;
typedef void*             WB_PWND;
typedef char*             WB_PSTR;
typedef const char*       WB_PCSTR;
typedef unsigned short    WB_WORD;
typedef unsigned char     WB_BYTE;
typedef void*             WB_HFONT;
typedef void*             WB_HBITMAP;
typedef unsigned long     WB_COLORREF;

typedef struct            __WB_HWB {} *WB_HWB;


enum MouseFlag
{
    WB_KEY_NONE     = 0,
    WB_KEY_CONTROL  = 1,
    WB_KEY_LBUTTON  = 2,
    WB_KEY_MBUTTON  = 3,
    WB_KEY_RBUTTON  = 4,
    WB_KEY_SHIFT    = 5
};


enum CtrlName
{
   WB_NUL	= 0x00,
   WB_DEL	= 0x01,
   WB_TXT	= 0x02,
   WB_MRK	= 0x03,
   WB_SET	= 0x04,
   WB_IMG	= 0x05,
   WB_PEN	= 0x06,
   WB_LIN	= 0x07,
   WB_RCE	= 0x08,
   WB_RCF	= 0x09,
   WB_CRE	= 0x10,
   WB_CRF	= 0x11,
   WB_CUT	= 0x12,
   WB_MMP	= 0x13,
   WB_MMD   = 0x14
};


struct RcPos
{
   WB_WORD Left;
   WB_WORD Top;
   WB_WORD Right;
   WB_WORD Bottom;

   void SetAttr (long l, long t, long r, long b);
};


struct PtPos
{
   WB_WORD X;
   WB_WORD Y;

   void SetAttr (long x, long y);
};


struct FontDef
{
   int      fdSize;
   char     fdName[TFONT_NAME+1];
   WB_BYTE  fdEffect;

   void     Clear   (void);
   void     SetAttr (WB_PSTR lpName, int size, WB_BYTE effect); 
   void     SetFont (WB_HFONT hFon); 
   WB_HFONT GetFont (float fy = 1);
};


struct ColorDef
{
   WB_BYTE  cdRed;
   WB_BYTE  cdGreen;
   WB_BYTE  cdBlue;

   void     Clear    (void);
   void     SetAttr  (WB_BYTE red, WB_BYTE green, WB_BYTE blue); 
   void     SetColor (WB_COLORREF color); 
   WB_COLORREF GetColor (void);
};


struct TextDef
{
   char  tdText[TMAX_TXT+1];
   int   tdLen;

   void  Clear   (void);
   bool  CopyTxt (WB_PSTR lpTxt, int len);
};


struct ImageDef
{
   char       idName[TMAX_PATH+1];
   WB_HBITMAP hBmp;

   void       Clear   (void);
   void       SetName (WB_PCSTR lpImg);
   WB_HBITMAP GetBmp  (void);

   ImageDef ();
};


struct PtVect
{
   PtPos ptPoint[NMAX_TRANS+1];  // Ponteiro para o buffer geral de pontos
   int   ptNum;                  // Quantidade de pontos do vetor

   void  Clear    (void);
   bool  InsPoint (PtPos pt);
   bool  CanTrans (void);
   void  EndVect  (void);
};


class CtrlAttr
{
   public:
      WB_EVENT ctEventCode;
      CtrlName ctName;
      FontDef  ctFont;
      ColorDef ctColor;
      TextDef  ctText;
      ImageDef ctImage;
      PtVect   ctPoints;
      RcPos    ctPos;
      int      ctLine;

      void Clear (void);
      void Init  (CtrlName ctrl, WB_COLORREF color, int line, RcPos pos);

      CtrlAttr (void);
     ~CtrlAttr (void);
};

#endif //_WBOARDINFO_H
