#ifndef MEDIA_TRANSCEIVER_H
#define MEDIA_TRANSCEIVER_H

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)	// size_t to int warning
#endif

#include "shared/compat.h"

#include "VBLib/VBLib.h"
#include "shared/socket/sockbuffs.h"
#include "shared/socket/sockmwbuffs.h"
#include "shared/socket/filemw.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "iptv_kernel/MediaTransceiverNotify.h"
#include "iptv_kernel/KernelNotify.h"

#define APP_SHARING_MAX_CONNECTIONS				1
#define APP_SHARING_MAX_PKT						1000  
#define APP_SHARING_MAX_PKT_SIZE				1500       
#define APP_SHARING_PKT_WAIT_FOR_DELAYED        3

/** @brief Manages media stream connections.
 *
 * This class responsibilities are transmitting and receiving IP.TV media streams.
 *
 */
class MediaTransceiver :	public ISockBuffNotify,
							public KernelNotify
{
private:
	MediaTransceiverNotify *m_notify;
	SockBuffs *m_sockBuffs;
	bool m_sockBuffsIsAborted;
	SockMWBuffs *m_sockMWBuffs;

	// SockBuffs notifications.
	void NotifyPkt(DWORD notifyId, ULONG id, double fPercent, BYTE flags);
    void NotifyEOF(void);
    BOOL NewID(ULONG id, BYTE mediaType);

public:
	MediaTransceiver(MediaTransceiverNotify *notify = NULL);
	~MediaTransceiver();
	
	// Reception
    bool InitializeReception(LPCSTR protocol, LPCSTR host, DWORD port, LPCSTR connStr, BOOL listen);
	void StopReception();
	void AbortReception();
	void DestroyReception();

	// File Playback
    bool InitializeFilePlayback(LPCSTR filename);
	void SeekFilePlayback(double percent);
	void PlayFilePlayback();
	void PauseFilePlayback();

	// Transmission
	bool InitializeTransmission(LPCSTR protocol, LPCSTR host, DWORD port, LPCSTR connStr, BOOL listen);
	void WriteToTransmission(unsigned char *data, unsigned dataLength);
	void StopTransmission();
	void DestroyTransmission();

	// Recording
	void StartRecording(ISockBuff *fileMediaWrite);

	ISockBuff *GetISockBuff(long mediaId);
	ISockBuff *CreateAppSharingBuffer(long mediaId);

	// Notification methods
	void SetNotify(MediaTransceiverNotify *notify);
	bool OnKernelNotify(KernelParam &param);
};

#endif
