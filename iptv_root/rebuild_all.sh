#!/bin/sh
# rebuild_all.sh
# Builds all projects from scratch on Linux.

BASEDIR=`pwd`
 
#SPEEX=../speex
#X264=../x264
OPENAL_SOFT=$BASEDIR/openal-soft
OPENAL_INC=$OPENAL_SOFT/include/AL
VBLIB=$BASEDIR/sbVB/VBLib_6_2
FFMPEG=$BASEDIR/iptv_ffmpeg2
SHARED=$BASEDIR/iptv_shared
APPSHARING=$BASEDIR/iptv_appsharing
MEDIA_UTIL=$BASEDIR/iptv_media_util
VIDEO_CAPTURE=$BASEDIR/iptv_video_capture
AUDIO_CAPTURE=$BASEDIR/iptv_audio_capture
MEDIA=$BASEDIR/iptv_media3
CHAT=$BASEDIR/iptv_chat
IRMSTUB=$BASEDIR/iptv_irm_stub
FILETRANSFER=$BASEDIR/iptv_xfiletransfer
KERNEL=$BASEDIR/iptv_kernel
CLIENT=$BASEDIR/iptv_client/build/g++
WBOARD=$BASEDIR/iptv_wboard
SKIN=$BASEDIR/skin_lite

# Accept "-j N" on the command line to speed up compilation on multi-core/
# hyper-threading machines.

if [ "$1" = "-j" ]; then
	JOBSFLAG=-j${2?"Error: -j requires an argument"}
else
	JOBSFLAG=
fi

# Build Speex
#if [ -d $SPEEX ]; then
#cd $SPEEX
#make clean && ./configure && make
#cd $BASEDIR
#fi

# Build x264
#if [ -d $X264 ]; then
#cd $X264
#make clean && ./configure --disable-pthread && make
#cd $BASEDIR
#fi

# Build VBLib.
if [ -d $VBLIB ]; then
	cd $VBLIB/project/g++
	if !(make clean && make $JOBSFLAG); then
		echo "Error: VBLib build failed."
		cd $BASEDIR
		exit 2;
	fi
	cd $BASEDIR
else
	echo "Error: VBLib not found.";
	cd $BASEDIR
	exit 1
fi

# Build ffmpeg. Watch out for the configure flag monster.
# old code
# cd $FFMPEG
# make clean && 
#./configure --enable-static --disable-shared --enable-x264 --enable-gpl --disable-protocols --disable-ffserver --disable-ffplay --enable-memalign-hack --disable-encoders --disable-decoders --enable-encoder=MPEG4_ENCODER --enable-encoder=X264_ENCODER --enable-decoder=MPEG4_DECODER --enable-decoder=H264_DECODER --enable-encoder=SPEEX_ENCODER --enable-decoder=SPEEX_DECODER --disable-muxers --disable-demuxers --disable-parsers --extra-cflags="-I`pwd`/../x264 -I`pwd`/../speex/include -Wno-unused -fno-strict-aliasing" --extra-ldflags="-L`pwd`/../x264 -L`pwd`/../speex/libspeex" && 
# make

# go into ffmpeg dir
#if [ -d $FFMPEG ]; then
#	cd $FFMPEG
#	## Amend config.mak
#	sh fix-config-mak.sh
#	# Invoke custom build script.
#	sh build.sh
#	# Go back to base dir.
#	cd $BASEDIR
#else
#	echo "Error: FFMPEG not found."
#	cd $BASEDIR
#	exit 1
#fi

# Build OpenAL Soft.
if [ -d $OPENAL_SOFT ]; then
	cd $OPENAL_SOFT
	if !(sh build.sh); then
		echo "Error: OpenAL Soft build failed."
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: OpenAL Soft not found."
	cd $BASEDIR
	exit 1
fi

# Build Application Sharing module.
if [ -d $APPSHARING ]; then
	cd $APPSHARING/project/gcc
	if !(make clean && make $JOBSFLAG); then
		echo "Error: Application Sharing module build failed."
		cd $BASEDIR
		exit 2
	fi
else
	echo "Error: Application Sharing module not found."
	exit 1
fi

# Build 'iptv_media_util'
if [ -d $MEDIA_UTIL ]; then
	cd $MEDIA_UTIL
	if !(sh build.sh); then
		echo "Error: iptv_media_util build failed."
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_media_util not found."
	exit 1
fi

# Build 'iptv_video_capture'.
if [ -d $VIDEO_CAPTURE ]; then
	cd $VIDEO_CAPTURE/project/gcc
	if !(make clean && make $JOBSFLAG); then
		echo "Error: iptv_video_capture build failed."
		cd $BASEDIR
		exit 2;
	fi
	cd $BASEDIR
else
	echo "Error: CrossCapture not found.";
	cd $CROSS_CAPTURE
	exit 1
fi

# Build 'iptv_audio_capture'
if [ -d $AUDIO_CAPTURE ]; then
	cd $AUDIO_CAPTURE
	if !(make clean && make OPENAL=$OPENAL_INC OPENAL_LIB=$OPENAL_SOFT/CMakeConf); then
		echo "Error: iptv_audio_capture build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_audio_capture not found."
	cd $BASEDIR
	exit 1
fi


# Build 'media' library.
if [ -d $MEDIA ]; then
	cd $MEDIA
	if !(make clean && make $JOBSFLAG OPENAL=$OPENAL_INC IPTV_FFMPEG=$FFMPEG); then
		echo "Error: iptv_media build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_media not found."
	cd $BASEDIR
	exit 1
fi

# Build 'chat' library.
if [ -d $CHAT ]; then
	cd $CHAT/chat
	if !(make clean && make $JOBSFLAG); then
		echo "Error: iptv_chat build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_chat not found."
	cd $BASEDIR
	exit 1
fi

# Build 'stub' library.
if [ -d $IRMSTUB ]; then
	cd $IRMSTUB/stub
	if !(make clean && make $JOBSFLAG); then
		echo "Error: iptv_irm_stub build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_irm_stub not found."
	cd $BASEDIR
	exit 1
fi

# Build XFileTransfer library.
if [ -d $FILETRANSFER ]; then
	cd $FILETRANSFER/xfiletransfer
	if !(make clean && make $JOBSFLAG IPTV_SHARED=$SHARED); then
		echo "Error: iptv_xfiletransfer build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_xfiletransfer not found."
	cd $BASEDIR
	exit 1
fi

# Build 'kernel'.
if [ -d $KERNEL ]; then
	cd $KERNEL/build/g++
	if !(make clean && make $JOBSFLAG OPENAL=$OPENAL_INC FFMPEG=$FFMPEG); then
		echo "Error: iptv_kernel build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_kernel not found."
	cd $BASEDIR
	exit 1
fi

# Build 'iptv_client' application.
if [ -d $CLIENT ]; then
	cd $CLIENT
	if !(make clean && make $JOBSFLAG OPENAL=$OPENAL_INC FFMPEG=$FFMPEG); then
		echo "Error: iptv_client build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_client not found."
	cd $BASEDIR
	exit 1
fi
# Done.

# Build iptv_wboard.
if [ -d $WBOARD ]; then
	cd $WBOARD/build/gcc
	if !(make clean && make $JOBSFLAG); then
		echo "Error: iptv_wboard build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: iptv_wboard not found."
	cd $BASEDIR
	exit 1
fi

# Build the default skin.
if [ -d $SKIN ]; then
	cd $SKIN/build/g++
	if !(make clean && make $JOBSFLAG); then
		echo "Error: default skin build failed."
		cd $BASEDIR
		exit 2
	fi
	cd $BASEDIR
else
	echo "Error: default skin not found."
	cd $BASEDIR
	exit 1
fi

# All set.
echo "Done."
exit 0
