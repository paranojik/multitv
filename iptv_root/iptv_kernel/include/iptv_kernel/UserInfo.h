#ifndef _USERINFO_H_
#define _USERINFO_H_

#include "VBLib/VBLib.h"

class UserInfo
{
private:

	br::com::sbVB::VBLib::VBString m_userName;
	br::com::sbVB::VBLib::VBString m_channel;
	
public:
	UserInfo(	br::com::sbVB::VBLib::VBString userName = "",
				br::com::sbVB::VBLib::VBString channel = "");
	UserInfo(const UserInfo &info);
	virtual ~UserInfo();
	
	br::com::sbVB::VBLib::VBString GetUserName() const;
	br::com::sbVB::VBLib::VBString GetChannel() const;

	void SetUserName(br::com::sbVB::VBLib::VBString userName);
	void SetChannel(br::com::sbVB::VBLib::VBString channel);
};

#endif
