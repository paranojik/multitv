///////////////////////////////////////////////////////////////////////////////
// CxThreadWin.cpp
// ---------------
//
//
// Author : Guilherme Cox <cox@ieee.org>
// Updated: Mon Aug 15 18:00:43 BRT 2005
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "CxThreadCross.h"
///////////////////////////////////////////////////////////////////////////////
CxThreadCross::CxThreadCross( void )
{
    m_pThread = NULL;
	m_pEvent  = new CxEvent( false );
}

CxThreadCross::~CxThreadCross( void )
{
	if( m_pEvent )
	{
        delete m_pEvent;
	}
}

unsigned int CxThreadCross::fnThreadProc( void )
{
    // nothing to do...
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int CxThreadCross::ResumeThread( void )
{
	if (!m_pThread)
		return -1;
	
	return m_pThread->Resume();
}

int CxThreadCross::SuspendThread( void )
{
	if (!m_pThread)
		return -1;
	
	return m_pThread->Suspend();
}
///////////////////////////////////////////////////////////////////////////////
bool CxThreadCross::CreateThread( )
{
    if( m_pThread )
        return false;

    m_pThread = CreateIThread( CxThreadEntry, this, FALSE );

    return( m_pThread != NULL );
}

void CxThreadCross::WaitForMe( void )
{
	if( m_pEvent )
		m_pEvent->Wait();
}

///////////////////////////////////////////////////////////////////////////////
int CxThreadEntry(IThread* pOwner,void* pParams)
{
    CxThreadCross *pThread = (CxThreadCross*) pParams;
	int nRet = 0;

	pThread->m_pEvent->Wait();

    nRet = pThread->fnThreadProc();

	pThread->m_pEvent->SetEvent();
    return nRet;
}
///////////////////////////////////////////////////////////////////////////////

