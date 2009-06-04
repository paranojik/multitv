/////////////////////////////////////////////////////////////////////////////
// Name:        PollGraphicFrame.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     11/06/2008 12:31:09
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _POLLGRAPHICFRAME_H_
#define _POLLGRAPHICFRAME_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/mstream.h"
#include <vector>
#include "chartdir.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define CHART_DIRECTOR_LICENSE
#define ID_POLLGRAPHICFRAME 1100
#define SYMBOL_POLLGRAPHICFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT
#define SYMBOL_POLLGRAPHICFRAME_TITLE _("PollGraphicFrame")
#define SYMBOL_POLLGRAPHICFRAME_IDNAME ID_POLLGRAPHICFRAME
#define SYMBOL_POLLGRAPHICFRAME_SIZE wxSize(500, 400)
#define SYMBOL_POLLGRAPHICFRAME_POSITION wxDefaultPosition
#define POLLGRAPHICFRAME_GRAPHIC_ARRAY_MAXLENGTH 20
////@end control identifiers


/** @brief Class thats implements dialog with results graphic of poll. 
*
*/
class PollGraphicFrame: public wxFrame
{    
    DECLARE_CLASS( PollGraphicFrame )
    DECLARE_EVENT_TABLE()

private:
	wxImage m_img,m_imgSave;
	int m_fileNameNumber;

public:
    /// Constructors
    PollGraphicFrame();
    PollGraphicFrame( wxWindow* parent, wxWindowID id = SYMBOL_POLLGRAPHICFRAME_IDNAME, const wxString& caption = SYMBOL_POLLGRAPHICFRAME_TITLE, const wxPoint& pos = SYMBOL_POLLGRAPHICFRAME_POSITION, const wxSize& size = SYMBOL_POLLGRAPHICFRAME_SIZE, long style = SYMBOL_POLLGRAPHICFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_POLLGRAPHICFRAME_IDNAME, const wxString& caption = SYMBOL_POLLGRAPHICFRAME_TITLE, const wxPoint& pos = SYMBOL_POLLGRAPHICFRAME_POSITION, const wxSize& size = SYMBOL_POLLGRAPHICFRAME_SIZE, long style = SYMBOL_POLLGRAPHICFRAME_STYLE );

    /// Destructor
    ~PollGraphicFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

	void DrawChart(const wxString &fileName, const wxString &title, std::vector<wxString> labels, std::vector<double> data);

////@begin PollGraphicFrame event handler declarations
	void OnPaint(wxPaintEvent &event);
	void OnSize(wxSizeEvent &event);
////@end PollGraphicFrame event handler declarations

////@begin PollGraphicFrame member function declarations
////@end PollGraphicFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PollGraphicFrame member variables
////@end PollGraphicFrame member variables
};

#endif
    // _POLLGRAPHICFRAME_H_
