#ifndef ASF_INDEX_OBJECT_H
#define ASF_INDEX_OBJECT_H

#include "ASFObject.h"

class ASFIndexObject : public ASFObject
{
public:
    ASFIndexObject(void) {}
    virtual ~ASFIndexObject(void) {}

	virtual DWORD GetPacketNumber(QWORD qwTimeOffset) { return 0; }
};

#endif

