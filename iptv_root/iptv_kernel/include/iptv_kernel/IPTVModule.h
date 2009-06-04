#ifndef IPTV_MODULE_H
#define IPTV_MODULE_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IPTVModuleNotificationSink.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IrmUser.h"
#include "iptv_kernel/IrmConnection.h"

/** @brief Implements tasks that are common to all IP.TV modules.
 *
 */
class IPTVModule
{
public:
	IPTVModule();
	virtual void OnIrmMessage(IrmMessage &msg, IrmUser &user, IrmConnection &conn) = 0;
	void SetNotificationSink(IPTVModuleNotificationSink *sink);
	bool IsEnabled() const;
	void Enable();
	void Disable();

protected:
	void Notify(IPTVModuleNotificationParameter &parameter) const;
	void SendPrivateMessage(	br::com::sbVB::VBLib::VBString recipient,
								br::com::sbVB::VBLib::VBString message);
	void SendChannelPrivateMessage(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString message);
	void RequestChannelWhiteboard(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString data);

private:
	IPTVModuleNotificationSink *m_notificationSink;
	bool m_enabled;
};

#endif
