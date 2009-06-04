#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IPTVModule.h"

/** @brief Implements the Whiteboard module.
 *
 * This class responsibilities are handling Whiteboard IRM messages.
 *
 */
class Whiteboard : public IPTVModule
{
public:
	Whiteboard();
	void OnIrmMessage(IrmMessage &msg, IrmUser &user, IrmConnection &conn);

	void SendChannelWhiteboard(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString data);

private:
	// IRM messages handling
	void OnChannelWhiteboard(	br::com::sbVB::VBLib::VBString sender,
								br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString data);
};

#endif
