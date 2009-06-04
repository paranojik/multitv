#ifndef FILE_TRANSFER_HEADER_H
#define FILE_TRANSFER_HEADER_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/FileTransferOrigin.h"

/** @brief Parses the IRM File Transfer header.
 *
 */
class FileTransferHeader
{
public:
	FileTransferHeader(br::com::sbVB::VBLib::VBString header);
	br::com::sbVB::VBLib::VBString GetFileName();
	long GetFileSize();
	long GetPacketSize();
	FileTransferOrigin GetOrigin();

	static br::com::sbVB::VBLib::VBString CreateHeader(	br::com::sbVB::VBLib::VBString fileName,
														unsigned long fileSize,
														unsigned long packetSize,
														FileTransferOrigin origin);

private:
	void Parse(br::com::sbVB::VBLib::VBString header);

	br::com::sbVB::VBLib::VBString m_fileName;
	long m_fileSize;
	long m_packetSize;
	FileTransferOrigin m_origin;

	static const char m_separator;
};

#endif
