#include "iptv_kernel/UserInfo.h"

using namespace br::com::sbVB::VBLib;

UserInfo::UserInfo(VBString userName, VBString channel)
{
	m_userName = userName;
	m_channel = channel;
}

UserInfo::UserInfo(const UserInfo &info)
{
	m_userName = info.GetUserName();
	m_channel = info.GetChannel();
}

UserInfo::~UserInfo()
{
}

VBString UserInfo::GetUserName() const
{
	return m_userName;
}

VBString UserInfo::GetChannel() const
{
	return m_channel;
}

void UserInfo::SetUserName(VBString userName)
{
	m_userName = userName;
}

void UserInfo::SetChannel(VBString channel)
{
	m_channel = channel;
}
