#ifndef _CHANNELINFO_H_
#define _CHANNELINFO_H_

#include "VBLib/VBLib.h"

#include "iptv_kernel/UserInfo.h"

class ChannelInfo
{
private:
	br::com::sbVB::VBLib::VBString m_name;
	br::com::sbVB::VBLib::VBString m_description;
	int m_userCount;
	
public:
	ChannelInfo(	br::com::sbVB::VBLib::VBString channelName = "",
					br::com::sbVB::VBLib::VBString channelDescription = "",
					int userCount = 0);

	ChannelInfo(const ChannelInfo &info);
	virtual ~ChannelInfo();
	
	br::com::sbVB::VBLib::VBString GetChannelName() const;
	br::com::sbVB::VBLib::VBString GetChannelDescription() const;
	int GetUserCount() const;

	void SetChannelName(br::com::sbVB::VBLib::VBString channelName);
	void SetChannelDescription(br::com::sbVB::VBLib::VBString channelDescription);
	void SetUserCount(int userCount);
};

#endif
