#
# Usage: buildiwmmxt_le.sh  {gcc}
#

#! /bin/sh


ARCH=iwmmxt_le

IPPENV=${IPPROOT}/tools/env/ippvarsixple.sh

if [ ! -x "${IPPENV}" ]; then
IPPENV=/opt/intel/ipp/5.3/ixp/tools/env/ippvarsixple.sh
fi

if [ ! -x "${IPPENV}" ]; then
  echo -e "*************************************************************************"
  echo -e "Intel(R) IPP is not found!"
  echo -e "Please install Intel(R) IPP or set IPPROOT environment variable correctly."
  echo -e "*************************************************************************"
  exit
fi


HOMECC=/opt/mvlcee/devkit/arm/iwmmxt_le/bin

CC=${HOMECC}/iwmmxt_le-gcc
LIBx=${HOMECC}/iwmmxt_le-ar

CPU=$1
if [ "${CPU}" == "" ]; then 
  CPU=s2
fi

. ${IPPENV}

#######################
make clean ARCH=${ARCH}
make ARCH=${ARCH} CPU=${CPU} CC=${CC} LIBx=${LIBx}