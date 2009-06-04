#
# Usage: buildxscale_be.sh  {gcc}
#

#! /bin/sh


ARCH=xscale_be
IPPENV=${IPPROOT}/tools/env/ippvarsixpbe.sh

if [ ! -x "${IPPENV}" ]; then
IPPENV=/opt/intel/ipp/5.3/ixp/tools/env/ippvarsixpbe.sh
fi

if [ ! -x "${IPPENV}" ]; then
  echo -e "*************************************************************************"
  echo -e "Intel(R) IPP is not found!"
  echo -e "Please install Intel(R) IPP or set IPPROOT environment variable correctly."
  echo -e "*************************************************************************"
  exit
fi


HOMECC=/opt/montavista/pro/devkit/arm/xscale_be/bin

CC=${HOMECC}/xscale_be-gcc
LIBx=${HOMECC}/xscale_be-ar

CPU=$1
if [ "${CPU}" == "" ]; then 
  CPU=s2
fi
. ${IPPENV}

#######################
make clean ARCH=${ARCH}
make ARCH=${ARCH} CPU=${CPU} CC=${CC} LIBx=${LIBx}