#ifndef FILE_TRANSFER_CTCP_MESSAGE_H
#define FILE_TRANSFER_CTCP_MESSAGE_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/CtcpMessage.h"
#include "iptv_kernel/FileTransferOrigin.h"

enum FileTransferMessageCode
{
	FT_MESSAGE_CODE_SEND_BEGIN,
	FT_MESSAGE_CODE_SEND_EOF,
	FT_MESSAGE_CODE_RECEIVE_CANCEL,
	FT_MESSAGE_CODE_RECEIVE_END
};

class FileTransferCtcpMessage : public CtcpMessage
{
public:
	FileTransferCtcpMessage();
	FileTransferCtcpMessage(br::com::sbVB::VBLib::VBString message);

	bool IsFileTransferCtcpMessage() const;
	bool IsTransmissionBegin() const;
	bool IsTransmissionEnd() const;
	bool IsReceptionCancel() const;
	bool IsReceptionEnd() const;

	long GetMediaId() const;
	long GetFileSize() const;
	br::com::sbVB::VBLib::VBString GetFileName() const;
	FileTransferOrigin GetOrigin() const;

	void SetMediaId(long mediaId);
	void SetFileSize(long fileSize);
	void SetFileName(br::com::sbVB::VBLib::VBString fileName);
	void SetOrigin(FileTransferOrigin origin);
	void SetFileTransferMessageCode(FileTransferMessageCode code);

	br::com::sbVB::VBLib::VBString GetFileTransferCtcpMessage() const;
	
private:
	void Reset();
	void ParseParameters();

	long m_mediaId;
	long m_fileSize;
	br::com::sbVB::VBLib::VBString m_fileName;
	FileTransferOrigin m_origin;

	bool m_isFileTransferCtcp;
	bool m_isTransmissionBegin;
	bool m_isTransmissionEnd;
	bool m_isReceptionCancel;
	bool m_isReceptionEnd;

	static const char m_fileTransferCtcpSeparator;
	static const br::com::sbVB::VBLib::VBString m_ctcpTransmissionBegin;
	static const br::com::sbVB::VBLib::VBString m_ctcpTransmissionEnd;
	static const br::com::sbVB::VBLib::VBString m_ctcpReceptionCancel;
	static const br::com::sbVB::VBLib::VBString m_ctcpReceptionEnd;
};

#endif

