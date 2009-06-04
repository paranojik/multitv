#!/bin/bash
# build.sh
# Script to (re-)build OpenAL Soft - with static library creation.
# By Andre Cotrim
# Date: 2008-04-25

CMAKE=/usr/bin/cmake
STATICLIB=libopenal.a
CMAKECONFDIR=CMakeConf
OBJBASEDIR=CMakeFiles/openal.dir

if [ ! -x $CMAKE ]; then
	echo "ERROR: CMake is required to build OpenAL Soft."
	echo "Make sure CMake is installed and you have permission to run it."
	exit -1;
fi

# Accept "-j N" for parallelizing build
if [ "$1" = "-j" ]; then
	JOBSFLAG=-j${2?"ERROR: -j requires an argument"}
else
	JOBSFLAG=
fi

echo "Removing CMakeCache.txt"
rm -rf $CMAKECONFDIR/CMakeCache.txt

cd $CMAKECONFDIR
echo "Configuring build..."
$CMAKE ..
echo "Building..."
make clean && make $JOBSFLAG
echo "Creating static library..."
ar rcs $STATICLIB $OBJBASEDIR/OpenAL32/*.o $OBJBASEDIR/Alc/*.o
echo Done.
exit 0
