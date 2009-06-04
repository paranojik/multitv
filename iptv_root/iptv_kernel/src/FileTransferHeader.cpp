#include "iptv_kernel/FileTransferHeader.h"

using namespace br::com::sbVB::VBLib;

const char FileTransferHeader::m_separator = ':';

FileTransferHeader::FileTransferHeader(VBString header) :	m_fileSize(0),
															m_packetSize(0)
{
	Parse(header);
}

VBString FileTransferHeader::GetFileName()
{
	return m_fileName;
}

long FileTransferHeader::GetFileSize()
{
	return m_fileSize;
}

long FileTransferHeader::GetPacketSize()
{
	return m_packetSize;
}

FileTransferOrigin FileTransferHeader::GetOrigin()
{
	return m_origin;
}

void FileTransferHeader::Parse(VBString header)
{
	//Header format:
	//<file name>:<file size>:<packet size>:<origin>
	m_fileName = header.strtok(m_separator, 0);
	m_fileSize = atoi(header.strtok(m_separator, 1));
	m_packetSize = atoi(header.strtok(m_separator, 2));

	int origin = atoi(header.strtok(m_separator, 3));
	m_origin = GetFileTransferOrigin(origin);
}

br::com::sbVB::VBLib::VBString FileTransferHeader::CreateHeader(VBString fileName,
																unsigned long fileSize,
																unsigned long packetSize,
																FileTransferOrigin origin)
{
	//Header format:
	//<file name>:<file size>:<packet size>:<origin>
	VBString header;

	// Getting path separator.
#ifdef WIN32
	char pathSeparator = '\\';
#endif
#ifdef _LINUX
	char pathSeparator = '/';
#endif
	
	// Separating file name from path.
	header = fileName.strtok(pathSeparator, fileName.tokCount(pathSeparator));
	header += m_separator;
	
	// VBString does not overload the += operator for unsigned long nor long.
	header += (unsigned) fileSize;
	header += m_separator;
	header += (unsigned) packetSize;
	header += m_separator;

	switch(origin)
	{
		case FT_ORIGIN_SEND_FILES:
			header += "1";
			break;
		case FT_ORIGIN_WHITE_BOARD:
			header += "2";
			break;
		case FT_ORIGIN_UPDATE:
			header += "3";
			break;
		case FT_ORIGIN_PTP:
			header += "4";
			break;
		default:
			header += "0";
			break;
	}

	return header;
}
