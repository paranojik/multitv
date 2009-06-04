#ifndef MEDIA_CONST_H
#define MEDIA_CONST_H

#define MAX_PKT_SIZE 		            65536
#define MAX_SEGMENT_SIZE                1500
#define MAX_PKT_COUNT		            1000
#define MAX_TRANSMISSIONS               10
#define MAX_OUTPUT_VIDEO_QUEUE          6
#define MAX_OUTPUT_AUDIO_QUEUE          15
#define MIN_OUTPUT_VIDEO_START_CONF     1
#define MIN_OUTPUT_AUDIO_START_CONF     4
#define MAX_INPUT_PKT_QUEUE	            30

#define MAX_ENCODED_FRAME_SIZE	        MAX_PKT_SIZE
#define MAX_DECODED_FRAME_SIZE	        (512*1024)
#define MAX_ENCODED_AUDIO_SIZE          10000
#define MAX_ENCODED_VIDEO_SIZE          20000
#define VIDEO_DECODE_BURST              2
#define AUDIO_DECODE_BURST              2

#define VIDEO_FRAME_PKT			        1

#define MAX_WAIT_BEFORE_DESTROY_WND_MS  10000
#define MAX_WAIT_AUDIO_LOAD_MS			200
#define MAX_FREEZE_VIDEO_MS				200
#define MAX_REPAINT_WAIT_MS				150
#define MAX_MEDIADELAY_MS               3000
#define MIN_BUFFERING_INTERVAL_MS       2000

#define MAX_TS_DIFF_MS                  150
#define MAX_ASYNC_MS                    200

#define MAX_AUDIO_SUBSEQ_DELAY          5
#define MAX_VIDEO_SUBSEQ_DELAY          15

#define MN_VIDEOALIVE_INTERVAL          500
#define MN_AUDIOALIVE_INTERVAL          500

#define BUFFERING_SLICE_TIME            60
#define BUFFERING_SLICE_PERCENT         20
#define BUFFERING_COMPLETE_PERCENT      100

#endif
