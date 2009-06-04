#ifndef ASF_SIMPLE_INDEX_OBJECT
#define ASF_SIMPLE_INDEX_OBJECT

#include "ASFIndexObject.h"

class ASFSimpleIndexObject : public ASFIndexObject
{
public:
	ASFSimpleIndexObject();
	virtual ~ASFSimpleIndexObject();

	void Refresh();

	DWORD GetPacketNumber(QWORD qwTimeOffset);

private:

	#pragma pack(push)
	#pragma pack(2)

	struct INDEX_ENTRY
	{
		DWORD dwPacketNumber; // Packet Number	DWORD	32
		WORD wPacketCount; // Packet Count	WORD	16
	};

	#pragma pack(pop)

	QWORD				m_qwTimeInterval;
	DWORD				m_nIndexEntries;
	INDEX_ENTRY*		m_pIndexEntries;
};

#endif

