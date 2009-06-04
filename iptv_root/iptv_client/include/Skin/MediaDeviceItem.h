#ifndef MEDIADEVICE_H
#define MEDIADEVICE_H

#include <wx/wx.h>

struct VideoDeviceItem
{
	unsigned index;
	wxString name;
};

struct AudioDeviceItem
{
	unsigned index;
	wxString name;
};

#endif
