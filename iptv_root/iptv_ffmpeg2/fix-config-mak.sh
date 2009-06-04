#!/bin/sh
# Script that replaces a few lines in config.mak with current directory.
TEMP_FILE=config.mak.tmp
ORIG_FILE=config.mak
FFMPEG_DIR=.
BASE_DIR=`pwd`
cd $FFMPEG_DIR
touch $TEMP_FILE
cat $ORIG_FILE | 
while read line; do
    echo $line | 
    if grep -q "^FFMPEG_PATH"; then 
        echo "FFMPEG_PATH    = \"`pwd`\"" >> $TEMP_FILE; 
    else 
        echo $line >> $TEMP_FILE;
    fi;
done
# Overwrite the original file after processing.
mv $TEMP_FILE $ORIG_FILE
cd $BASE_DIR
