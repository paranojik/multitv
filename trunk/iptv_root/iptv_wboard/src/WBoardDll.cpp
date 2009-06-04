// ----------------------------------------------------------------------------
// wboarddll.cpp
// ----------------------------------------------------------------------------

#define SOURCE_STRING "WBoardDll"
#include <shared/debug.h>

#include "shared/common.h"
#include "shared/mem_x.h"
#include "shared/file_x.h"

#include "WBoardProc.h"
#include "WBoardWnd.h"
#include "WBoardDll.h"

// ----------------------------------------------------------------------------
// DLLMain function
// ----------------------------------------------------------------------------

#ifdef WIN32
BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
   switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
  			break;
    }

    return TRUE;
}
#endif

// ----------------------------------------------------------------------------
// DLL Interface
// ----------------------------------------------------------------------------

WBoardModule::WBoardModule( )
{
    m_pWBProcess   = NULL;
    m_pWBInterface = NULL;
}

WBoardModule::~WBoardModule( )
{
    Delete();
}

bool WBoardModule::Initialize( WB_PWND pParent, int nId, IWBoardNotify* pNotify, RcPos* pRc )
{
    bool ret = false;

    IWBProcess* pProcess = NULL;

    if ( !m_pWBProcess )
    {
        m_pWBProcess = new CWBProcess(pNotify);

        if ( m_pWBProcess )
        {
            pProcess = m_pWBProcess->GetProcessPtr();
        }
    }

    if (pProcess && !m_pWBInterface)
    {
        m_pWBInterface = new CWBInterface(pParent, pProcess, nId, pRc);

        if ( m_pWBInterface )
        {
            ret = true;
        }
    }

    return ret;
}

void WBoardModule::Delete( )
{
    if ( m_pWBProcess )
    {
        if ( m_pWBInterface && m_pWBProcess->IsWindowCreated() )
        {
            delete m_pWBInterface;
        }

        m_pWBInterface = NULL;

        delete m_pWBProcess;
        m_pWBProcess = NULL;
    }
}

bool WBoardModule::Resize( RcPos* pRc )
{
    bool ret = false;

    if ( m_pWBInterface )
    {
        ret = m_pWBInterface->Resize(pRc);
    }

    return ret;
}

void WBoardModule::ShowWindow( bool bOp )
{
    if ( m_pWBInterface )
    {
        m_pWBInterface->ShowWnd( bOp );
    }
}

void WBoardModule::EnableWindow( bool bOp )
{
    if ( m_pWBInterface )
    {
        m_pWBInterface->EnableWnd( bOp );
    }
}

void WBoardModule::EnableImageCtrl( bool bOp )
{
    if ( m_pWBInterface )
    {
        m_pWBInterface->EnableImgCtrl( bOp );
    }
}

void WBoardModule::EnableImageStop( bool bOp )
{
    if ( m_pWBInterface )
    {
        m_pWBInterface->EnableImgStop( bOp );
    }
}

WB_RESULT WBoardModule::FilterWBMessage( WB_PCSTR pbMsg, WB_HMSG * phMsg )
{
    CtrlAttr *lpAttr;
    char *lpMsg, *lpAux;
    int num, ind, nAux;
    char buffer[TAM_BUFFER];
    long ret;

    ret = WB_ERROR;

    if (!m_pWBProcess)
        return ret;

    lpAttr = m_pWBProcess->GetCtrlAttr();
	*phMsg = lpAttr;

    lpMsg = buffer;
    lpAux = (char *)pbMsg;

    nAux = TAM_BUFFER;

    // Se nao ha buffer para ler as informacoes, retorna erro
    if (!lpAux)
        return ret;

    decode64(lpAux, (int)strlen(lpAux), lpMsg, &nAux);

    if (lpAttr)
    {
      switch (*lpMsg)
      {
         case WBN_ADDELEMENT:
            lpAttr->ctEventCode = WBN_ADDELEMENT;
            lpMsg++;
            switch (*lpMsg)
            {
               case WB_DEL:
                  lpAttr->ctName = WB_DEL;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (lpMsg, &lpAttr->ctPoints.ptPoint[ind], sizeof(PtPos));
                        lpMsg += sizeof(PtPos);
                     }
                     lpAttr->ctPoints.ptNum = num;
                  }
                  break;
               case WB_MMD:
                  lpAttr->ctName = WB_MMD;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (lpMsg, &lpAttr->ctPoints.ptPoint[ind], sizeof(PtPos));
                        lpMsg += sizeof(PtPos);
                     }
                     lpAttr->ctPoints.ptNum = num;
                  }
                  break;
               case WB_MRK:
                  lpAttr->ctName = WB_MRK;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (lpMsg, &lpAttr->ctPoints.ptPoint[ind], sizeof(PtPos));
                        lpMsg += sizeof(PtPos);
                     }
                     lpAttr->ctPoints.ptNum = num;
                  }
                  break;
               case WB_PEN:
                  lpAttr->ctName = WB_PEN;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (lpMsg, &lpAttr->ctPoints.ptPoint[ind], sizeof(PtPos));
                        lpMsg += sizeof(PtPos);
                     }
                     lpAttr->ctPoints.ptNum = num;
                  }
                  break;
               case WB_MMP:
                  lpAttr->ctName = WB_MMP;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (lpMsg, &lpAttr->ctPoints.ptPoint[ind], sizeof(PtPos));
                        lpMsg += sizeof(PtPos);
                     }
                     lpAttr->ctPoints.ptNum = num;
                  }
                  break;
               case WB_SET:
                  lpAttr->ctName = WB_SET;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_TXT:
                  lpAttr->ctName = WB_TXT;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     lpAttr->ctText.tdLen = num;
                     mov_mem (lpMsg, &lpAttr->ctText.tdText, num);
                     lpAttr->ctText.tdText[num] = '\0';
                     lpMsg += num;
                  }
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     mov_mem (lpMsg, &lpAttr->ctFont.fdName, num);
                     lpAttr->ctFont.fdName[num] = '\0';
                     lpMsg += num;
                  }
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                     lpAttr->ctFont.fdSize = *lpMsg;
                  else
                     lpAttr->ctFont.fdSize = *lpMsg * (-1);
                  lpMsg++;
                  lpAttr->ctFont.fdEffect = *lpMsg;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_IMG:
                  lpAttr->ctName = WB_IMG;
                  lpMsg++;
                  num = *lpMsg;
                  lpMsg++;
                  if (num > 0)
                  {
                     char szFile[TPATH+1];
                     char szName[TPATH+1];
                     char *pTmpFile = NULL;
                     //char *pTmpFile = getenv("TEMP");

                     mov_mem (lpMsg, szFile, num);
                     szFile[num] = '\0';
                     sep_path(szFile, NULL, szName);
                     if (pTmpFile)
                     {
                        sprintf(lpAttr->ctImage.idName, "%s%s%s", pTmpFile, "\\", szName);
                     }
                     else
                     {
                        sprintf(lpAttr->ctImage.idName, "%s", szName);
                     }
                     lpMsg += num;
                  }
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_LIN:
                  lpAttr->ctName = WB_LIN;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_RCE:
                  lpAttr->ctName = WB_RCE;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_RCF:
                  lpAttr->ctName = WB_RCF;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_CRE:
                  lpAttr->ctName = WB_CRE;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  lpAttr->ctLine = *lpMsg;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_CRF:
                  lpAttr->ctName = WB_CRF;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctColor, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef);
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
               case WB_CUT:
                  lpAttr->ctName = WB_CUT;
                  lpMsg++;
                  mov_mem (lpMsg, &lpAttr->ctPos, sizeof(RcPos));
                  lpMsg += sizeof(RcPos);
                  break;
            }
            break;

         case WBN_ADDPOINTS:
            // Comando obsoleto
            break;

         case WBN_NEWEXEC:
            lpAttr->ctEventCode = WBN_NEWEXEC;
            break;

         case WBN_UNDOEXEC:
            lpAttr->ctEventCode = WBN_UNDOEXEC;
            break;

         default:
            // erro msg eh corrompida
            delete lpAttr;
            break;
      }

      ret = WB_OK;
    }

    return ret;
}

WB_RESULT WBoardModule::GetWBMessage( long lEvent, WB_HMSG hMsg, WB_PSTR pbMsg, long szbMsg )
{
    CtrlAttr *lpAttr;
    char *lpMsg, *lpAux;
    int num, ind, nAux;
    char buffer[TAM_BUFFER];
    long ret, cont;

    ret  = WB_ERROR;
    cont = 0;

    lpAttr = (CtrlAttr*) hMsg;

    lpAux = (char *)pbMsg;
    lpMsg = buffer;

    // Se nao ha buffer para receber as informacoes, retorna erro
    if (!lpAux)
      return ret;

    if (lpAttr)
    {
      switch (lEvent)
      {
         case WBN_ADDELEMENT:
            *lpMsg = (BYTE)lEvent;
            lpMsg++;
            cont++;
            switch (lpAttr->ctName)
            {
               case WB_DEL:
                  *lpMsg = WB_DEL;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  num = lpAttr->ctPoints.ptNum;
                  *lpMsg = num;
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (&lpAttr->ctPoints.ptPoint[ind], lpMsg, sizeof(PtPos));
                        lpMsg += sizeof(PtPos); cont += sizeof(PtPos);
                     }
                  }
                  break;
               case WB_MMD:
                  *lpMsg = WB_MMD;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  num = lpAttr->ctPoints.ptNum;
                  *lpMsg = num;
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (&lpAttr->ctPoints.ptPoint[ind], lpMsg, sizeof(PtPos));
                        lpMsg += sizeof(PtPos); cont += sizeof(PtPos);
                     }
                  }
                  break;
               case WB_MRK:
                  *lpMsg = WB_MRK;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  num = lpAttr->ctPoints.ptNum;
                  *lpMsg = num;
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (&lpAttr->ctPoints.ptPoint[ind], lpMsg, sizeof(PtPos));
                        lpMsg += sizeof(PtPos); cont += sizeof(PtPos);
                     }
                  }
                  break;
               case WB_PEN:
                  *lpMsg = WB_PEN;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  num = lpAttr->ctPoints.ptNum;
                  *lpMsg = num;
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (&lpAttr->ctPoints.ptPoint[ind], lpMsg, sizeof(PtPos));
                        lpMsg += sizeof(PtPos); cont += sizeof(PtPos);
                     }
                  }
                  break;
               case WB_MMP:
                  *lpMsg = WB_MMP;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  num = lpAttr->ctPoints.ptNum;
                  *lpMsg = num;
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     for (ind = 0; ind < num; ind++)
                     {
                        mov_mem (&lpAttr->ctPoints.ptPoint[ind], lpMsg, sizeof(PtPos));
                        lpMsg += sizeof(PtPos); cont += sizeof(PtPos);
                     }
                  }
                  break;
               case WB_SET:
                  *lpMsg = WB_SET;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_TXT:
                  *lpMsg = WB_TXT;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  num = lpAttr->ctText.tdLen;
                  *lpMsg = num;
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     mov_mem (&lpAttr->ctText.tdText, lpMsg, num);
                     lpMsg += num; cont += num;
                  }
                  num = (int) strlen (lpAttr->ctFont.fdName);
                  *lpMsg = num;
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     mov_mem (&lpAttr->ctFont.fdName, lpMsg, num);
                     lpMsg += num; cont += num;
                  }
                  num = lpAttr->ctFont.fdSize;
                  if (num < 0)
                  {
                     *lpMsg = 0;
                     lpMsg++; cont++;
                     *lpMsg = lpAttr->ctFont.fdSize * (-1);
                     lpMsg++; cont++;
                  }
                  else
                  {
                     *lpMsg = 1;
                     lpMsg++; cont++;
                     *lpMsg = lpAttr->ctFont.fdSize;
                     lpMsg++; cont++;
                  }
                  *lpMsg = lpAttr->ctFont.fdEffect;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_IMG:
                  *lpMsg = WB_IMG;
                  lpMsg++; cont++;
                  num = (int) strlen (lpAttr->ctImage.idName);
                  *lpMsg = num; 
                  lpMsg++; cont++;
                  if (num > 0)
                  {
                     mov_mem (lpAttr->ctImage.idName, lpMsg, num);
                     lpMsg += num; cont += num;
                  }
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_LIN:
                  *lpMsg = WB_LIN;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_RCE:
                  *lpMsg = WB_RCE;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_RCF:
                  *lpMsg = WB_RCF;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_CRE:
                  *lpMsg = WB_CRE;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  *lpMsg = lpAttr->ctLine;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_CRF:
                  *lpMsg = WB_CRF;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctColor, lpMsg, sizeof(ColorDef));
                  lpMsg += sizeof(ColorDef); cont += sizeof(ColorDef);
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               case WB_CUT:
                  *lpMsg = WB_CUT;
                  lpMsg++; cont++;
                  mov_mem (&lpAttr->ctPos, lpMsg, sizeof(RcPos));
                  lpMsg += sizeof(RcPos); cont += sizeof(RcPos);
                  break;
               default:{} // this line avoids switch warnings in gcc.
            }
            break;

         case WBN_ADDPOINTS:
            // Comando obsoleto
            break;
      }
    }
    else
    {
      switch (lEvent)
      {
          case WBN_NEWEXEC:
          case WBN_UNDOEXEC:
              *lpMsg = (BYTE)lEvent;
              lpMsg++; cont++;
              break;
      }
    }

    *lpMsg = '\0';

    nAux = szbMsg;

    if (cont)
       ret = encode64(buffer, cont, lpAux, &nAux) == 0 ? WB_OK : WB_ERROR;

    lpAux[nAux] = '\0';

    return ret;
}

WB_PCSTR WBoardModule::GetVersion( )
{
    return NULL;
}

WB_RESULT WBoardModule::AddMetaElement( WB_HMSG hMsg )
{
    CtrlAttr *lpAttr = (CtrlAttr *) hMsg;
    long ret;

    ret = WB_ERROR;

    if (!m_pWBProcess)
        return ret;

    if (lpAttr)
    {
        switch (lpAttr->ctEventCode)
        {
            case WBN_ADDELEMENT:
                m_pWBProcess->AddElement(lpAttr);
                break;

            case WBN_ADDPOINTS:
                m_pWBProcess->AddPoints(lpAttr);
                break;

            case WBN_NEWEXEC:
                m_pWBProcess->NewExec();
                delete lpAttr;
                break;

            case WBN_UNDOEXEC:
                m_pWBProcess->UndoExec();
                delete lpAttr;
                break;
        }

        ret = WB_OK;
    }

    return (ret);
}
