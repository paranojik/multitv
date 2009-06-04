#ifndef ASF_DATA_OBJECT_H
#define ASF_DATA_OBJECT_H

#include "ASFObject.h"

class ASFDataObject : public ASFObject
{
public:

	ASFDataObject();
	virtual ~ASFDataObject();

	QWORD GetObjectSize() { return 50; }
};

#endif

