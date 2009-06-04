#include "compat.h"
#include "IAudioApi.h"
#include "media_utilities.h"

ULONG IAudioApi::SetAudioTimestamp(ULONG _ulTimestamp)
{
	if (!m_bInit)
		return RET_INIT_ERROR;

	CHECK_ERROR(m_ulAudioTSList.PushBack(_ulTimestamp), RET_OK)
    
    return RET_OK;
}

