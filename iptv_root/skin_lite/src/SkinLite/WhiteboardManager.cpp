#include "WhiteboardManager.h"

const long WhiteboardManager::m_maxWhiteboardMessageSize = 4096;

WhiteboardManager::WhiteboardManager(AppInterface *iface, SessionWindow *sessionWindow) :	m_whiteboardModule(NULL),
																							m_appInterface(NULL)
{
	m_whiteboardModule = new WBoardModule();
	m_appInterface = iface;
	m_sessionWindow = sessionWindow;
}

WhiteboardManager::~WhiteboardManager()
{
	if(m_whiteboardModule)
		delete m_whiteboardModule;
}

void WhiteboardManager::CreateWhiteboardFrame(wxWindow* parent, int id)
{
	m_whiteboardModule->Initialize(parent, id, this);
}

void WhiteboardManager::ShowWhiteboardFrame()
{
	m_whiteboardModule->ShowWindow(true);
}

void WhiteboardManager::HideWhiteboardFrame()
{
	m_whiteboardModule->ShowWindow(false);
}

void WhiteboardManager::Enable()
{
	m_whiteboardModule->EnableWindow(true);
}

void WhiteboardManager::Disable()
{
	m_whiteboardModule->EnableWindow(false);
}

void WhiteboardManager::EnableImageControl()
{
	m_whiteboardModule->EnableImageCtrl(true);
}

void WhiteboardManager::DisableImageControl()
{
	m_whiteboardModule->EnableImageCtrl(false);
}

void WhiteboardManager::OnWhiteBoardMessage(long lEvent, WB_HMSG pData)
{
	switch(lEvent)
	{
		case WBN_ADDELEMENT:
		case WBN_NEWEXEC:
		case WBN_UNDOEXEC:
		{
			char *buf = new char[m_maxWhiteboardMessageSize];
			if(m_whiteboardModule->GetWBMessage(lEvent, (WB_HMSG)pData, (WB_PSTR)buf, m_maxWhiteboardMessageSize) == WB_OK)
			{
				m_appInterface->WhiteboardSendMessage(buf);
			}
			break;
		}
		case WBN_IMGTRANSFER:
		{
			m_sessionWindow->StartWhiteboardFileTransmission(wxString((const char *)pData, wxConvISO8859_1));
			break;
		}
		case WBN_CLOSEWND:
		{
			HideWhiteboardFrame();
			break;
		}
	}
}

void WhiteboardManager::OnWhiteboardChannelMessage(const wxString &channel, const wxString &user, const wxString &data)
{
	static bool firstMessage = true;
	if(firstMessage)
	{
		ShowWhiteboardFrame();
		firstMessage = false;
	}

	WB_HMSG decodedMessage;
	if(m_whiteboardModule->FilterWBMessage((const char *)(data.mb_str(wxConvISO8859_1)), &decodedMessage) != WB_OK)
		return;

	// if IMG, replace file with file path.
	CtrlAttr* control;
	control = (CtrlAttr*)decodedMessage;

	switch(control->ctName)
	{
		case WB_IMG:
			char* fileName;
			fileName = ((ImageDef)control->ctImage).idName;
			//const* filePath[255];
			// TODO:
			// Get temp path from kernel.
			// concatenate file name with path.

			//((ImageDef)control->ctImage).SetName(filePath);
		break;
	}

	m_whiteboardModule->AddMetaElement(decodedMessage);
}
