#ifndef DBG_WND_H
#define DBG_WND_H

/*
	Debug Window:
	
#1	Media id:
#2
#3  Received Packets:
#4	Jitter Packets:
#5	
#6	Video Packets:
#7	Audio Packets:
#8	
#9	Video Frames Buffered:
#10	Audio Frames Buffered:
#11
#12	Video Frames Processed:
#13	Audio Frames Processed:
#14
#15	Current Timestamp:
#16	Video Timestamp:
#17	Audio Timestamp:
#18
#19 Video (not) Started - Running / Paused
#20 Audio (not) Started - Running / Paused
#21
#22 Buffering:
*/

#define MEDIA_ID_LINE			1
#define RCV_PKT_LINE            3
#define JITTER_PKT_LINE			4
#define VIDEO_PKT_LINE			6
#define AUDIO_PKT_LINE			7
#define VIDEO_FRAME_BUF_LINE	9
#define AUDIO_FRAME_BUF_LINE	10
#define VIDEO_FRAME_PROC_LINE	12
#define AUDIO_FRAME_PROC_LINE	13
#define CUR_TIMESTAMP_LINE		15
#define VIDEO_TIMESTAMP_LINE	16
#define	AUDIO_TIMESTAMP_LINE	17
#define VIDEO_THREAD_STATUS     19
#define AUDIO_THREAD_STATUS     20
#define BUFFERING_STATUS        22

#endif