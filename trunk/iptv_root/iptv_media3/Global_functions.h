#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include "media_utilities.h"

ULONG GetFrameDimensions(BYTE _type, ULONG *_pulWidth, ULONG *_pulHeight);
ULONG GetMediaSpec(BYTE _type, MediaSpec * _pMediaSpec);
ULONG GetTime(ULONG *_pulTime);


#endif
