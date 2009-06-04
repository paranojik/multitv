#include "shared/compat.h"
#include "ASFDataObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ASFDataObject::ASFDataObject()
{
	//_TRACE0(LOGLEVEL5, TEXT("CMCASFDataObject::CMCASFDataObject"));

	m_Type = MC_ASF_DATAOBJECT;

}

ASFDataObject::~ASFDataObject()
{
	//_TRACE0(LOGLEVEL5, TEXT("CMCASFDataObject::~CMCASFDataObject"));

}
