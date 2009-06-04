#include "ASFObject.h"

const GUID_W ASFObject::ASF_Header_Object = 
{ 0x75B22630, 0x668E, 0x11CF, { 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C } };

// {75B22636-668E-11CF-A6D9-00AA0062CE6C}
const GUID_W ASFObject::ASF_Data_Object = 
{ 0x75B22636, 0x668E, 0x11CF, { 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C } };

// {33000890-E5B1-11CF-89F4-00A0C90349CB}
const GUID_W ASFObject::ASF_Simple_Index_Object =
{ 0x33000890, 0xE5B1, 0x11CF, { 0x89, 0xF4, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xCB } };

// {D6E229D3-35DA-11D1-9034-00A0C90349BE}
const GUID_W ASFObject::ASF_Media_Object_Index_Object =
{ 0xD6E229D3, 0x35DA, 0x11D1, { 0x90, 0x34, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xBE } };

// {FEB103F8-12AD-4C64-840F-2A1D2F7AD48C}
const GUID_W ASFObject::ASF_Timecode_Index_Object =
{ 0xFEB103F8, 0x12AD, 0x4C64, { 0x84, 0x0F, 0x2A, 0x1D, 0x2F, 0x7A, 0xD4, 0x8C } };

// {3CB73FD0-0C4A-4803-953D-EDF7B6228F0C}
const GUID_W ASFObject::ASF_Index_Object =
{ 0x3CB73FD0, 0x0C4A, 0x4803, { 0x95, 0x3D, 0xED, 0xF7, 0xB6, 0x22, 0x8F, 0x0C } };

//////////////////////////////////////////////////////////////////////
// HEADER
// {8CABDCA1-A947-11CF-8EE4-00C00C205365}
const GUID_W ASFObject::ASF_File_Properties_Object =
{ 0x8CABDCA1, 0xA947, 0x11CF, { 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 } };

// {B7DC0791-A9B7-11CF-8EE6-00C00C205365}
const GUID_W ASFObject::ASF_Stream_Properties_Object =
{ 0xB7DC0791, 0xA9B7, 0x11CF, { 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 } };


void ASFObject::CopyFrom(ASFObject& obj)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFObject::Copy()"));

    SetSize((DWORD) obj.GetObjectSize());

    // ASF object defined has 24 or greater
    if (!m_p || Size() < obj.GetObjectSize())
    {
        //_ERROR1C(E_OUTOFMEMORY, MCMSG("Out of memory(%d)"), obj.GetObjectSize());
    }

    memcpy(m_p, obj.m_p, (DWORD) obj.GetObjectSize());

    Refresh();
}

void ASFObject::CopyTo(LPBYTE pTarget)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFObject::Copy()"));

    // ASF object defined has 24 or greater
    if (!m_p || GetObjectSize() < 24)
    {
        //_ERROR0(MCMSG("Invalid object"));
    }

    memcpy(pTarget, m_p, (DWORD) GetObjectSize());
}

