#include "iptv_kernel/ChannelInfo.h"

using namespace br::com::sbVB::VBLib;
	
ChannelInfo::ChannelInfo(VBString channelName, VBString channelDescription, int userCount)
{
	m_name = channelName;
	m_description = channelDescription;
	m_userCount = userCount;
}

ChannelInfo::ChannelInfo(const ChannelInfo &info)
{
	m_name = info.GetChannelName();
	m_description = info.GetChannelDescription();
	m_userCount = info.GetUserCount();
}

ChannelInfo::~ChannelInfo()
{
}

VBString ChannelInfo::GetChannelName() const
{
	return m_name;
}

VBString ChannelInfo::GetChannelDescription() const
{
	return m_description;
}

int ChannelInfo::GetUserCount() const
{
	return m_userCount;
}

void ChannelInfo::SetChannelName(VBString channelName)
{
	m_name = channelName;
}

void ChannelInfo::SetChannelDescription(VBString channelDescription)
{
	m_description = channelDescription;
}

void ChannelInfo::SetUserCount(int userCount)
{
	m_userCount = userCount;
}

