#include "compat.h"
#include "CClockSync.h"
#include "global_error.h"
#include "Global_functions.h"

CClockSync::CClockSync() :
    m_ClockSemaph()
{
	m_Ref           = 0;
	m_ulTime        = 0;
	m_bClockStarted = FALSE;	
}


CClockSync::~CClockSync()
{
}


ULONG CClockSync::SetClockRef()
{
    m_ClockSemaph.Wait();

    if (!m_bClockStarted)
	{
		GetTime(&m_Ref);
		m_bClockStarted = TRUE;
	}
    
	m_ClockSemaph.Signal();

    return RET_OK;
}

ULONG CClockSync::UpdateTime(ULONG *_pulCurTime)
{
    if (!m_bClockStarted)
	    return RET_INIT_ERROR;

    if (!_pulCurTime)
	    return RET_INVALID_ARG;

    m_ClockSemaph.Wait();

	ULONG ulAuxTime;
    GetTime(&ulAuxTime);

	m_ulTime = ulAuxTime - m_Ref;  
	
	*_pulCurTime = m_ulTime;

	m_ClockSemaph.Signal();
    
	return RET_OK;
}
