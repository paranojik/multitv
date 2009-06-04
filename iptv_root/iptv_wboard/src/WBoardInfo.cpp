// ----------------------------------------------------------------------------
// WBoardInfo.cpp
// ----------------------------------------------------------------------------

#include "shared/compat.h"
#include "shared/mem_x.h"
#include "shared/file_x.h"

#include "WBoardInfo.h"


//********************************************************************************//

void RcPos::SetAttr (long l, long t, long r, long b)
{
   Left   = (WB_WORD) l;
   Top    = (WB_WORD) t;
   Right  = (WB_WORD) r;
   Bottom = (WB_WORD) b;
}

//********************************************************************************//

void PtPos::SetAttr (long x, long y)
{
   X = (WB_WORD) x;
   Y = (WB_WORD) y;
}

//********************************************************************************//

void FontDef::Clear (void)
{
   fdSize    = 0;
   fdName[0] = '\0';
   fdEffect  = 0;
}

void FontDef::SetAttr (LPSTR lpName, int size, WB_BYTE effect)
{
   fdSize   = size;
   fdEffect = effect;
   if (lpName)
      ajusta_upp_str (lpName, fdName, TFONT_NAME);
}

void FontDef::SetFont (WB_HFONT hFon)
{
}

WB_HFONT FontDef::GetFont (float fy)
{
   WB_BYTE Effect, Style;

   Effect = Style = 0;

   if (fdEffect & FD_STRIKEOUT)
      Effect = 10;
   if (fdEffect & FD_UNDERLINE)
      Effect += 1;

   if (fdEffect & FD_BOLD)
      Style = 10;
   if (fdEffect & FD_ITALIC)
      Style += 1;

   //return ::GetFont(fdName, (int)(fdSize * fy), Effect, Style);
   return NULL;
}

//********************************************************************************//

void ColorDef::Clear (void)
{
   cdRed   = 0;
   cdGreen = 0;
   cdBlue  = 0;
}

void ColorDef::SetAttr (WB_BYTE red, WB_BYTE green, WB_BYTE blue)
{
   cdRed   = red;
   cdGreen = green;
   cdBlue  = blue;
}

void ColorDef::SetColor (WB_COLORREF color)
{
}

WB_COLORREF ColorDef::GetColor (void)
{
    return 0;
}

//********************************************************************************//

void TextDef::Clear (void)
{
   tdText[0] = '\0';
   tdLen     = 0;
}

bool TextDef::CopyTxt (WB_PSTR lpTxt, int len)
{
   bool ret;

   ret = FALSE;

   if (lpTxt && len)
   {
      tdLen = len;
      strcpy (tdText, lpTxt);
      ret = true;
   }

   return (ret);
}

//********************************************************************************//

ImageDef::ImageDef ()
{
    hBmp = NULL;
}

void ImageDef::Clear (void)
{
   idName[0] = '\0';
}

void ImageDef::SetName (LPCSTR lpImg)
{
   if (FileExist(lpImg))
      strcpy (idName, lpImg);
}

WB_HBITMAP ImageDef::GetBmp (void)
{
   WB_PCSTR lpExt;
   int      len;

   if (FileExist(idName))
   {
       len = (int)strlen(idName);
       if (len)
       {
          lpExt = idName;
          lpExt += len - 3;

          if (_stricmp(lpExt, "BMP") == 0)
          {
          }
          else if (_stricmp(lpExt, "JPG") == 0)
          {
          }
       }
   }

   return NULL;
}

//********************************************************************************//

void PtVect::Clear (void)
{
   ptNum = 0;
   fill_mem (ptPoint, sizeof(PtPos)*(NMAX_TRANS+1), 0);
}

bool PtVect::InsPoint (PtPos pt)
{
   bool ret = true;

   ptPoint[ptNum].X = pt.X;
   ptPoint[ptNum].Y = pt.Y;
   ptNum++;

   /* Mensagem na janela do Debug
   char msg[100];
   sprintf(msg, "ptNum = %d    x = %d    y = %d\n", ptNum, pt.X, pt.Y);
   OutputDebugString(msg);
   */

   return (ret);
}

bool PtVect::CanTrans (void)
{
   return (ptNum >= NMAX_TRANS);
}

void PtVect::EndVect (void)
{
   ptPoint[ptNum].X = END_POS;
   ptPoint[ptNum].Y = END_POS;
}

//********************************************************************************//

CtrlAttr::CtrlAttr (void)
{
    Clear();
}

CtrlAttr::~CtrlAttr (void)
{
}

void CtrlAttr::Clear (void)
{
   ctEventCode = 0;
   ctName = WB_NUL;

   ctFont.Clear ();
   ctColor.Clear ();
   ctText.Clear ();
   ctImage.Clear ();
   ctPoints.Clear ();

   ctLine  = 0;

   fill_mem (&ctPos, sizeof(RcPos), 0);
}

void CtrlAttr::Init (CtrlName ctrl, WB_COLORREF color, int line, RcPos pos)
{
    Clear();

    ctName = ctrl;
    ctColor.SetColor (color);
    ctLine = line;
    ctPos.SetAttr (pos.Left, pos.Top, pos.Right, pos.Bottom);
}

//********************************************************************************//
