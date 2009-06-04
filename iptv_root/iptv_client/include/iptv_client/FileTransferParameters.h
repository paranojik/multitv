#ifndef FILE_TRANSFER_PARAMETERS_H
#define FILE_TRANSFER_PARAMETERS_H

struct FTSendBeginParameter
{
	long m_mediaId;
};

struct FTSendMediaIdParameter
{
	long m_mediaId;
};

struct FTReceptionBeginParameter
{
	long m_mediaId;
	wxString m_fileName;
	wxString m_tempFilePath;
	wxString m_senderNick;
	unsigned long m_fileSize;
	unsigned m_packetSize;
};

struct FTProgressParameter
{
	unsigned long m_currentPacketIndex;
	unsigned long m_lastPacketIndex;
};

struct FTLostPacketParameter
{
	unsigned long m_lostPackets;
	unsigned long m_lostPacketsTotal;
};

#endif
