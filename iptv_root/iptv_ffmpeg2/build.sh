#!/bin/sh
#Automated recipe for compiling THIS SPECIFIC ffmpeg.

## Amend config.mak
if [ ! -x fix-config-mak.sh ]; then 
chmod +x fix-config-mak.sh;
fi
./fix-config-mak.sh
# Build avutil.
cd libavutil
make clean && make
ar rcs libavutil.a *.o
# Build avcodec.
cd ../libavcodec
make clean && make
ar rcs libavcodec.a *.o i386/*.o
cd ..
