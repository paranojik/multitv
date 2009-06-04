#ifndef FILE_TRANSFER_ORIGIN_H
#define FILE_TRANSFER_ORIGIN_H

enum FileTransferOrigin
{
	FT_ORIGIN_UNKNOWN,
	FT_ORIGIN_SEND_FILES,
	FT_ORIGIN_WHITE_BOARD,
	FT_ORIGIN_UPDATE,
	FT_ORIGIN_PTP
};

FileTransferOrigin GetFileTransferOrigin(int origin);
int GetFileTransferOrigin(FileTransferOrigin origin);

#endif
