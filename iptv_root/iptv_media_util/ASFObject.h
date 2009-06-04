#ifndef ASF_OBJECT_H
#define ASF_OBJECT_H

#include "defs.h"
#include "INSSBuffer.h"

class ASFObject : public INSSBuffer
{
    friend class ASFHeader;
    friend class ASFDataPacket;

public:
    enum ASF_OBJECT_TYPE
    {
        MC_ASF_UNKNOWN,
        MC_ASF_HEADER,
        MC_ASF_DATAOBJECT,
        MC_ASF_DATAPACKET,
        MC_ASF_INDEXOBJECT,
    };

    ASFObject() : m_Type(MC_ASF_UNKNOWN) {}
    virtual ~ASFObject() {}

    ASF_OBJECT_TYPE GetType() { return m_Type; }
	
    virtual QWORD GetObjectSize() 
    { 
        return m_p ? (* (QWORD*) &m_p[16]) : 0; 
    }

    virtual void Refresh() {}

    void CopyFrom(ASFObject& obj);
    void CopyTo(LPBYTE pTarget);

    // {75B22630-668E-11CF-A6D9-00AA0062CE6C}
    static const GUID_W ASF_Header_Object;

    // {75B22636-668E-11CF-A6D9-00AA0062CE6C}
    static const GUID_W ASF_Data_Object; 

    // {33000890-E5B1-11CF-89F4-00A0C90349CB}
    static const GUID_W ASF_Simple_Index_Object;

    // {D6E229D3-35DA-11D1-9034-00A0C90349BE}
    static const GUID_W ASF_Media_Object_Index_Object;
	
    // {FEB103F8-12AD-4C64-840F-2A1D2F7AD48C}
    static const GUID_W ASF_Timecode_Index_Object;

    // {3CB73FD0-0C4A-4803-953D-EDF7B6228F0C}
    static const GUID_W ASF_Index_Object;

    // {8CABDCA1-A947-11CF-8EE4-00C00C205365}
    static const GUID_W ASF_File_Properties_Object;

    // {B7DC0791-A9B7-11CF-8EE6-00C00C205365}
    static const GUID_W ASF_Stream_Properties_Object;

    LPBYTE GetPointer() 
    {
        return m_p;
    }

protected:
    ASF_OBJECT_TYPE m_Type;
};


#endif
