#include "iptv_shared/shared/compat.h"
#include "ASCompat.h"
#include "ASModuleCross.h"
#include "CxReceiveCross.h"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef ASMODULE_EXPORTS_CROSS
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
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
#endif //ASMODULE_EXPORTS_CROSS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS ASModule::ASModule()
{
	m_nMode      = AS_NULL;
	m_pASReceive = NULL;
//	m_pASCapture = NULL;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS ASModule::~ASModule()
{
	this->End();
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS bool ASModule::Initialize( ISockBuff *pSockBuff, int flMode, long nMediaID, ASListener *pListener )
{
	// Check if already initialized
	if (m_pASReceive)
		return false;

	switch( flMode )
	{
	case AS_RECEIVING:
		m_pASReceive = new CxReceiveCross();
		m_nMode      = AS_RECEIVING;
		return m_pASReceive->Initialize( pSockBuff, nMediaID, pListener);
/*
	case AS_CAPTURING:
		// Capture initialization should go here
		return true;
*/	}

	// No compatible mode specified
	return false;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS bool ASModule::End()
{
	if (m_pASReceive)
	{
		m_pASReceive->End();
		delete m_pASReceive;
		m_pASReceive = NULL;
		return true;
	}

	// Case not initialized
	return false;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS bool ASModule::GetFrame( ASFrame *pFrame, bool pConvertRGB8 )
{
	if (!m_pASReceive)
		return false;
	
	return m_pASReceive->GetFrame( pFrame, pConvertRGB8 );
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS bool ASModule::IsNewFrameAvailable()
{
	if (!m_pASReceive)
		return false;

	return m_pASReceive->IsNewFrameAvailable();
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS bool ASModule::SaveFrameBitmap( ASFrame *pFrame, char* pFileName )
{
	if (!m_pASReceive)
		return false;
	
	return m_pASReceive->SaveFrameBitmap( pFileName );
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
