#include "ASFSimpleIndexObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ASFSimpleIndexObject::ASFSimpleIndexObject() :
	m_qwTimeInterval(0),
	m_nIndexEntries(0),
	m_pIndexEntries(NULL)
{
	//_TRACE0(LOGLEVEL5, TEXT("CMCASFSimpleIndexObject::CMCASFSimpleIndexObject()"));

	m_Type = MC_ASF_INDEXOBJECT;
}

ASFSimpleIndexObject::~ASFSimpleIndexObject()
{
	//_TRACE0(LOGLEVEL5, TEXT("CMCASFSimpleIndexObject::~CMCASFSimpleIndexObject()"));

	delete [] m_pIndexEntries;
}

void ASFSimpleIndexObject::Refresh()
{
	//_TRACE0(LOGLEVEL5, TEXT("CMCASFSimpleIndexObject::Refresh()"));

	//Object ID	GUID	128
	//Object Size	QWORD	64
	//File ID	GUID	128
	//Index Entry Time Interval	QWORD	64
	//Maximum Packet Count	DWORD	32
	//Index Entries Count	DWORD	32
	//Index Entries	See below	varies

	//The fields are defined as follows:

	//Object ID 
	// Specifies the GUID for the Simple Index Object. 
	// The value of this field shall be set to ASF_Simple_Index_Object.
	//Object Size 
	// Specifies the size, in bytes, of the Simple Index Object.
	// Valid values are at least 56 bytes.
	//File ID 
	// Specifies the unique identifier for this ASF file.
	// The value of this field should be changed every time the file is modified in any way. The value of this field may be set to 0 or set to be identical to the value of the File ID field of the Data Object and the Header Object. ASF parsers may safely ignore this value.
	//Index Entry Time Interval 
	// Specifies the time interval between each index entry in 100-nanosecond units.
	// The most common value is 10000000, to indicate that the index entries are in 1-second intervals, though other values can be used as well.
	//Maximum Packet Count 
	// Specifies the maximum Packet Count value of all Index Entries.
	//Index Entries Count 
	// Specifies the number of Index Entries structures contained in the next field.
	//Index Entries 
	// The structure of each Index Entry is shown in the following table.
	//Field name	Field type	Size (bits)
	//Packet Number	DWORD	32
	//Packet Count	WORD	16

	//The fields are defined as follows:
	//Packet Number 
	// Specifies the number of the Data Packet associated with this index entry. Note that for video streams that contain both key frames and non-key frames, this field will always point to the closest key frame prior to the time interval.
	//Packet Count 
	// Specifies the number of Data Packets to send at this index entry. If a video key frame has been fragmented into two Data Packets, the value of this field will be equal to 2.

	// in 100ns units

	m_qwTimeInterval = * (QWORD*) &m_p[40];
	m_nIndexEntries  = * (DWORD*) &m_p[52];

	if (m_nIndexEntries <= MC_ASF_SIMPLE_INDEX_ASF_ENTRIES_MAX)
	{
		//_TRACE1(DsLOGLEVEL3 | DsLOG_TRACE | DsLOG_IO, TEXT("Allocating (%d) index entries"), m_nIndexEntries);

		m_pIndexEntries = new INDEX_ENTRY[m_nIndexEntries];

		if (!m_pIndexEntries)
		{
			//_ERROR0C(E_OUTOFMEMORY, MCMSG("Cannot allocate index entries"));
		}

		//DsCBuffer::Copy(m_pIndexEntries, &p[56], m_nIndexEntries * sizeof(INDEX_ENTRY));
	    memcpy(m_pIndexEntries, &m_p[56], m_nIndexEntries * sizeof(INDEX_ENTRY));
    }
	else
	{
		//_WARN1(DsCAT_WARNING, TEXT("Invalid number of index entries (%d)"), m_nIndexEntries);
	}
}

DWORD ASFSimpleIndexObject::GetPacketNumber(QWORD qwTimeOffset)
{
	//_TRACE1(LOGLEVEL5, TEXT("CMCASFSimpleIndexObject::GetPacketNumber(%I64d)"), qwTimeOffset);

	if (!m_pIndexEntries || m_qwTimeInterval == 0 || m_nIndexEntries == 0)
	{
		//_ERROR0C(E_INVALIDARG, MCMSG("Index is not valid"));
	}

	DWORD dwIndexEntry = (DWORD) (qwTimeOffset / m_qwTimeInterval);

	if (dwIndexEntry >= m_nIndexEntries)
	{
		dwIndexEntry = m_nIndexEntries - 1;
	}

	return m_pIndexEntries[dwIndexEntry].dwPacketNumber;
}
