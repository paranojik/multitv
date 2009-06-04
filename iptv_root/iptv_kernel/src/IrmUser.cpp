#include "iptv_kernel/IrmUser.h"

using namespace br::com::sbVB::VBLib;
using namespace std;

IrmUser::IrmUser()
{
	m_nick = "";
	m_passwd = "";
	m_realName = "";
	m_email = "";
}

IrmUser::IrmUser(	VBString nick,
					VBString passwd,
					VBString realName,
					VBString email)
{
	SetNick(nick);
	m_passwd = passwd;
	m_realName = realName;
	m_email = email;
}

IrmUser::~IrmUser()
{

}

VBString IrmUser::GetNick()
{
	return m_nick;
}

VBString IrmUser::GetPasswd()
{
	return m_passwd;
}

VBString IrmUser::GetRealName()
{
	return m_realName;
}

VBString IrmUser::GetEmail()
{
	return m_email;
}

void IrmUser::SetNick(VBString nick)
{
	m_nick = nick;

	int i = 0;
	while(	(nick.getChar(i) == IRM_USER_PREFIX_CHANNEL_OPERATOR)		||
			(nick.getChar(i) == IRM_USER_PREFIX_CHANNEL_COLLABORATOR)	||
			(nick.getChar(i) == IRM_USER_PREFIX_CHANNEL_LOG))
	{
		m_nick = nick.strAfterPosition(i+1);
		m_prefixesList.push_back(nick.getChar(i));
		i++;
	}
}

void IrmUser::SetPasswd(VBString passwd)
{
	m_passwd = passwd;
}

void IrmUser::SetRealName(VBString realName)
{
	m_realName = realName;
}

void IrmUser::SetEmail(VBString email)
{
	m_email = email;
}

void IrmUser::Reset()
{
	m_nick = "";
	m_passwd = "";
	m_realName = "";
	m_email = "";
}

bool IrmUser::HasValidPrefix()
{
	if(IsOperator()||IsMediaCollaborator()||IsLog())
		return true;
	return false;
}

bool IrmUser::IsOperator()
{
	list<char>::iterator it;

	for(it = m_prefixesList.begin(); it != m_prefixesList.end(); it++)
	{
		if(*it == IRM_USER_PREFIX_CHANNEL_OPERATOR)
			return true;
	}

	return false;
}

bool IrmUser::IsMediaCollaborator()
{
	list<char>::iterator it;

	for(it = m_prefixesList.begin(); it != m_prefixesList.end(); it++)
	{
		if(*it == IRM_USER_PREFIX_CHANNEL_COLLABORATOR)
			return true;
	}

	return false;
}

bool IrmUser::IsLog()
{
	list<char>::iterator it;

	for(it = m_prefixesList.begin(); it != m_prefixesList.end(); it++)
	{
		if(*it == IRM_USER_PREFIX_CHANNEL_LOG)
			return true;
	}

	return false;
}
