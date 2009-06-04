#include <wx/wx.h>
#include "PrivateChatWindowBase.h"

PrivateChatWindowBase::~PrivateChatWindowBase()
{

}

void PrivateChatWindowBase::SetUserName(const wxString &name)
{
	m_userName = name;
}

const wxString & PrivateChatWindowBase::GetUserName() const
{
	return m_userName;
}

