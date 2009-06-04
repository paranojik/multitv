#ifndef IRM_USER_H
#define IRM_USER_H

#include "VBLib/VBLib.h"
#include <list>

#define IRM_USER_PREFIX_INVALID					'\0'
#define IRM_USER_PREFIX_CHANNEL_OPERATOR		'@'
#define IRM_USER_PREFIX_CHANNEL_COLLABORATOR	'+'
#define IRM_USER_PREFIX_CHANNEL_LOG				'*'

/** @brief Represents an IRM user.
 *
 * This class most usefull functionality is that it parses the prefixes in the user name string
 * provided by the IRM messages, such as the operator prefix(@) and the media collaborator
 * prefix (+).
 *
 */
class IrmUser
{
public:
	IrmUser();
	IrmUser(	br::com::sbVB::VBLib::VBString nick,
				br::com::sbVB::VBLib::VBString passwd = "",
				br::com::sbVB::VBLib::VBString realName = "",
				br::com::sbVB::VBLib::VBString email = "");

	~IrmUser();

	br::com::sbVB::VBLib::VBString GetNick();
	br::com::sbVB::VBLib::VBString GetPasswd();
	br::com::sbVB::VBLib::VBString GetRealName();
	br::com::sbVB::VBLib::VBString GetEmail();

	void SetNick(br::com::sbVB::VBLib::VBString nick);
	void SetPasswd(br::com::sbVB::VBLib::VBString passwd);
	void SetRealName(br::com::sbVB::VBLib::VBString realName);
	void SetEmail(br::com::sbVB::VBLib::VBString email);
	void Reset();

	bool HasValidPrefix();
	bool IsOperator();
	bool IsMediaCollaborator();
	bool IsLog();

private:
	br::com::sbVB::VBLib::VBString m_nick;
	br::com::sbVB::VBLib::VBString m_passwd;
	br::com::sbVB::VBLib::VBString m_realName;
	br::com::sbVB::VBLib::VBString m_email;
	std::list<char> m_prefixesList;
};

#endif
