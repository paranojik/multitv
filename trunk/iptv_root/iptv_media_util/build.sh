#!/bin/sh
# Script to automate build.
# By Andre Cotrim
# Date: 2008-07-09

BASE_DIR=`pwd`
LUA_DIR=lua

# First, compile Lua.
cd $BASE_DIR/$LUA_DIR
if !(make clean && make); then
	echo "ERROR: Lua build failed.";
	exit 1;
fi

# Then, compile library.
cd $BASE_DIR
if !(make clean && make); then
	echo "ERROR: iptv_media_util build failed.";
	exit 1;
fi

echo "Done."
exit 0;
