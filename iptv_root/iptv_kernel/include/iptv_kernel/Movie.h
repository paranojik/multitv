#ifndef MOVIE_H
#define MOVIE_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IPTVMediaModule.h"
#include "shared/compat.h"
#include "SimposiumSink.h"
#include "IKernelNotify.h"

// disable warnings in MSVC when using shared lib
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)	// size_t to int conversion
#pragma warning(disable: 4005)	// FD_SETSIZE (winsock2.h) macro redefinition
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

enum MovieState	{
					MOVIE_STATE_IDLE,
					MOVIE_STATE_RECEIVING,
					MOVIE_STATE_TRANSMITTING
				};

struct MovieData
{
	MovieData();
	void Reset();

	long m_mediaId;
	MediaConnectionData m_connectionData;
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_user;
	br::com::sbVB::VBLib::VBString m_filePath;
	unsigned m_bitRate;
};

class SockMWBuffs;
class SockBuffs;
class SimposiumNetwork;
class CMedia;
class CVideoFrame;
class MediaProcessorNotify;
class MediaTransceiverNotify;

/** @brief Implements the Movie module.
 *
 * This class responsibilities are handling Symposium Movie IRM messages, managing
 * the movie stream transmission and encode/decode the Movie stream packets.
 *
 */
class Movie :	public IPTVMediaModule,
				public ISimposiumSink,
				public IMediaKernelNotify
{
public:
	Movie(	MediaProcessorNotify *processorNotify = NULL,
			MediaTransceiverNotify *transceiverNotify = NULL);
	~Movie();

	bool IsMovieTransmission(long mediaId);

	bool BeginTransmission(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString filePath,
							unsigned bitRate);

	bool CancelTransmission();

	bool CancelReception();

	bool GetColaborationVideoFrame(	unsigned long id, unsigned char ** videoFrame,
									unsigned long *width, unsigned long *height);

private:
	unsigned GetMediaPort() const;

	void Reset();

	void OnUserPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
								br::com::sbVB::VBLib::VBString message);

	void OnChannelPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString message);

	void OnMediaNotify(	br::com::sbVB::VBLib::VBString notification,
						long mediaId,
						br::com::sbVB::VBLib::VBString mediaClass,
						br::com::sbVB::VBLib::VBString channel,
						br::com::sbVB::VBLib::VBString user);

	void OnMediaReceive(long mediaId,
						long port,
						br::com::sbVB::VBLib::VBString multiCastIp,
						long packetLength,
						long headerLength,
						br::com::sbVB::VBLib::VBString header,
						IrmUser user,
						IrmConnection conn);

	void OnMediaReceiveRun(long mediaId);
	void OnMediaReceiveStop(long mediaId);

	void OnMediaReceiveAll(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString mediaClass,
							long port,
							br::com::sbVB::VBLib::VBString multiCastIp,
							IrmUser user,
							IrmConnection conn);

	void OnMediaReceiveAllRun(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString mediaClass);

	void OnMediaReceiveAllStop(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString mediaClass);

	void OnMediaSend(	long mediaId,
						br::com::sbVB::VBLib::VBString mediaClass,
						br::com::sbVB::VBLib::VBString channel,
						long port,
						IrmUser &user,
						IrmConnection &conn);

	void OnMediaSendStop(long mediaId);

	void OnPacketRequest(	long mediaId,
							unsigned long initialPacketIndex,
							unsigned long finalPacketIndex);

    ULONG OnHeader(BYTE *_pHeader, ULONG _ulHeaderLen);
    ULONG OnPacket(BYTE *_pPacket, ULONG _ulPktLen);
	ULONG OnEOF();

	BOOL OnMediaNotify(MediaParam *_pParameters);

	bool IsIdle() const;
	bool IsReceiving() const;
	bool IsTransmitting() const;
	void SetState(MovieState state);

	MovieState m_state;

	MovieData m_receptionData;
	MovieData m_transmissionData;

	static const unsigned m_movieMediaPort;

	SimposiumNetwork *m_simposiumNetwork;
	SockMWBuffs *m_writeBuffer;
	SockBuffs *m_readBufferContainer;
	CMedia *m_media;
	CVideoFrame *m_mediaVideoFrame;
	MediaProcessorNotify *m_processorNotify;
	MediaTransceiverNotify *m_transceiverNotify;

	void DestroyReadBufferContainer();
};

#endif // MOVIE_H
