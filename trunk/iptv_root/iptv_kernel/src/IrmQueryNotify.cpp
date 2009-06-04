#include "iptv_kernel/IrmQueryNotify.h"

bool IrmQueryParam::IsNotificationCodeValid()
{
	if(m_code != IQNC_INVALID_CODE)
		return true;

	return false;
}
