#include "iptv_kernel/FileTransferOrigin.h"

FileTransferOrigin GetFileTransferOrigin(int origin)
{
	switch(origin)
	{
		case 1:
			return FT_ORIGIN_SEND_FILES;
		case 2:
			return FT_ORIGIN_WHITE_BOARD;
		case 3:
			return FT_ORIGIN_UPDATE;
		case 4:
			return FT_ORIGIN_PTP;
		default:
			return FT_ORIGIN_UNKNOWN;
	}
}

int GetFileTransferOrigin(FileTransferOrigin origin)
{
	switch(origin)
	{
		case FT_ORIGIN_SEND_FILES:
			return 1;
		case FT_ORIGIN_WHITE_BOARD:
			return 2;
		case FT_ORIGIN_UPDATE:
			return 3;
		case FT_ORIGIN_PTP:
			return 4;
		default:
			return -1;
	}
}
