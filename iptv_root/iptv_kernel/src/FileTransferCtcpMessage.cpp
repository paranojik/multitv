#include "iptv_kernel/FileTransferCtcpMessage.h"

using namespace br::com::sbVB::VBLib;

const char FileTransferCtcpMessage::m_fileTransferCtcpSeparator = '|';

const VBString FileTransferCtcpMessage::m_ctcpTransmissionBegin		= "FTPNOTIFYSND";
const VBString FileTransferCtcpMessage::m_ctcpTransmissionEnd		= "FTPEOFSND";
const VBString FileTransferCtcpMessage::m_ctcpReceptionCancel		= "FTPCANCELRCV";
const VBString FileTransferCtcpMessage::m_ctcpReceptionEnd			= "FTPENDRCV";

FileTransferCtcpMessage::FileTransferCtcpMessage() : CtcpMessage("")
{
	Reset();
}

FileTransferCtcpMessage::FileTransferCtcpMessage(VBString message) : CtcpMessage(message)
{
	Reset();
	ParseParameters();

	if(this->operator==(m_ctcpTransmissionBegin))
	{
		m_isTransmissionBegin = true;
		m_isFileTransferCtcp = true;	
	}

	else if(this->operator==(m_ctcpTransmissionEnd))
	{
		m_isTransmissionEnd = true;
		m_isFileTransferCtcp = true;
	}

	else if(this->operator==(m_ctcpReceptionCancel))
	{
		m_isReceptionCancel = true;
		m_isFileTransferCtcp = true;
	}

	else if(this->operator==(m_ctcpReceptionEnd))
	{
		m_isReceptionEnd = true;
		m_isFileTransferCtcp = true;
	}
}

void FileTransferCtcpMessage::Reset()
{
	m_mediaId = 0;
	m_fileSize = 0;
	m_origin = FT_ORIGIN_UNKNOWN;
	m_isFileTransferCtcp = false;
	m_isTransmissionBegin = false;
	m_isTransmissionEnd = false;
	m_isReceptionCancel = false;
	m_isReceptionEnd = false;
}

bool FileTransferCtcpMessage::IsFileTransferCtcpMessage() const
{
	return m_isFileTransferCtcp;
}

bool FileTransferCtcpMessage::IsTransmissionBegin() const
{
	return m_isTransmissionBegin;
}

bool FileTransferCtcpMessage::IsTransmissionEnd() const
{
	return m_isTransmissionEnd;
}

bool FileTransferCtcpMessage::IsReceptionCancel() const
{
	return m_isReceptionCancel;
}

bool FileTransferCtcpMessage::IsReceptionEnd() const
{
	return m_isReceptionEnd;
}

long FileTransferCtcpMessage::GetMediaId() const
{
	return m_mediaId;
}

long FileTransferCtcpMessage::GetFileSize() const
{
	return m_fileSize;
}

VBString FileTransferCtcpMessage::GetFileName() const
{
	return m_fileName;
}

FileTransferOrigin FileTransferCtcpMessage::GetOrigin() const
{
	return m_origin;
}

void FileTransferCtcpMessage::SetMediaId(long mediaId)
{
	m_mediaId = mediaId;
}

void FileTransferCtcpMessage::SetFileSize(long fileSize)
{
	m_fileSize = fileSize;
}

void FileTransferCtcpMessage::SetFileName(VBString fileName)
{
	m_fileName = fileName;
}

void FileTransferCtcpMessage::SetOrigin(FileTransferOrigin origin)
{
	m_origin = origin;
}

void FileTransferCtcpMessage::SetFileTransferMessageCode(FileTransferMessageCode code)
{
	Reset();
	m_isFileTransferCtcp = true;

	switch(code)
	{
		case FT_MESSAGE_CODE_SEND_BEGIN:
			m_isTransmissionBegin = true;
		break;
		case FT_MESSAGE_CODE_SEND_EOF:
			m_isTransmissionEnd = true;
		break;
		case FT_MESSAGE_CODE_RECEIVE_CANCEL:
			m_isReceptionCancel = true;
		break;
		case FT_MESSAGE_CODE_RECEIVE_END:
			m_isReceptionEnd = true;
		break;
		default:
			m_isFileTransferCtcp = false;
		break;
	}
}

VBString FileTransferCtcpMessage::GetFileTransferCtcpMessage() const
{
	VBString ctcpMessage;

	if(m_isTransmissionBegin)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpTransmissionBegin;
		ctcpMessage += m_ctcpPrefix;
		ctcpMessage += m_mediaId;
		ctcpMessage += m_fileTransferCtcpSeparator;
		ctcpMessage += m_fileName;
		ctcpMessage += m_fileTransferCtcpSeparator;
		ctcpMessage += m_fileSize;
		ctcpMessage += m_fileTransferCtcpSeparator;
		ctcpMessage += GetFileTransferOrigin(m_origin);
	}
	else if(m_isTransmissionEnd)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpTransmissionEnd;
		ctcpMessage += m_ctcpPrefix;
		ctcpMessage += m_mediaId;
	}
	else if(m_isReceptionCancel)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpReceptionCancel;
		ctcpMessage += m_ctcpPrefix;
		ctcpMessage += m_mediaId;
	}
	else if(m_isReceptionEnd)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpReceptionEnd;
		ctcpMessage += m_ctcpPrefix;
		ctcpMessage += m_mediaId;
	}

	return ctcpMessage;
}

void FileTransferCtcpMessage::ParseParameters()
{
	VBString parameters = GetParameters();
	
	m_mediaId = atoi(parameters.strtok(m_fileTransferCtcpSeparator, 0));
	m_fileName = parameters.strtok(m_fileTransferCtcpSeparator, 1);
	m_fileSize = atoi(parameters.strtok(m_fileTransferCtcpSeparator, 2));

	int origin = atoi(parameters.strtok(m_fileTransferCtcpSeparator, 3));
	m_origin = GetFileTransferOrigin(origin);
}
