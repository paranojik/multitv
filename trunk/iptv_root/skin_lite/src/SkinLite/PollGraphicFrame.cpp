/////////////////////////////////////////////////////////////////////////////
// Name:        PollGraphicFrame.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     11/06/2008 12:31:09
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "PollGraphicFrame.h"

////@begin XPM images
////@end XPM images


/*!
 * PollGraphicFrame type definition
 */

IMPLEMENT_CLASS( PollGraphicFrame, wxFrame )


/*!
 * PollGraphicFrame event table definition
 */

BEGIN_EVENT_TABLE( PollGraphicFrame, wxFrame )

////@begin PollGraphicFrame event table entries
EVT_PAINT (PollGraphicFrame::OnPaint)
EVT_SIZE(PollGraphicFrame::OnSize) 
////@end PollGraphicFrame event table entries

END_EVENT_TABLE()


/** PollGraphicFrame default constructor.
 * 
 */
PollGraphicFrame::PollGraphicFrame()
{
    Init();
}

/** PollGraphicFrame constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_POLLGRAPHICFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_POLLGRAPHICFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_POLLGRAPHICFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_POLLGRAPHICFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_POLLGRAPHICFRAME_STYLE.
*/
PollGraphicFrame::PollGraphicFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	CHART_DIRECTOR_LICENSE
    Init();
    Create( parent, id, caption, pos, size, style );
	SetMinSize(size);
}

/** PollGraphicFrame constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_POLLGRAPHICFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_POLLGRAPHICFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_POLLGRAPHICFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_POLLGRAPHICFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_POLLGRAPHICFRAME_STYLE.
*/
bool PollGraphicFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PollGraphicFrame creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
////@end PollGraphicFrame creation
    return true;
}


/** PollGraphicFrame destructor.
 * 
 */
PollGraphicFrame::~PollGraphicFrame()
{
////@begin PollGraphicFrame destruction
	/*m_img.Destroy();*/
////@end PollGraphicFrame destruction
}


/*!
 * Member initialisation
 */

void PollGraphicFrame::Init()
{
////@begin PollGraphicFrame member initialisation
	m_fileNameNumber = 1;
////@end PollGraphicFrame member initialisation
}


/*!
 * Control creation for PollGraphicFrame
 */

void PollGraphicFrame::CreateControls()
{    
////@begin PollGraphicFrame content construction
    PollGraphicFrame* itemFrame1 = this;

////@end PollGraphicFrame content construction
}


/*!
 * Should we show tooltips?
 */

bool PollGraphicFrame::ShowToolTips()
{
    return true;
}

/** Draws the graphic with values passed in your parameters.
* Must be claeed after PollGraphic Frame instantiation to draw the graphic.
* @param[in] fileName. File name where the graphic png is saved.
* @param[in] title. Graphic title.
* @param[in] labels. Labels showed on graphic. Is a vector.
* @param[in] data. Data showed on graphic. Is a vector.
*/
void PollGraphicFrame::DrawChart(const wxString &fileName, const wxString &title, std::vector<wxString> labels, std::vector<double> data)
{
	std::vector<double> auxData = data;
	int auxDataSize = auxData.size();

	//The data for the pie chart
	double *doubleData = new double[auxDataSize];
	//double *doubleData;
	for(int i=0;i<auxDataSize;i++)
	{
		//doubleData = new double;
		doubleData[i] = auxData[i];
	}

    // The labels for the pie chart
	std::vector<wxString> auxLabels = labels;
	wxCharBuffer chBufLabels;
	int auxLabelsSize = auxLabels.size();
	char **strLabels = new char *[auxLabelsSize];
	for(int i=0;i<auxLabelsSize;i++)
	{
		chBufLabels = auxLabels[i].ToUTF8();
		strLabels[i] = chBufLabels.release();
		//wxMessageBox(wxString::Format(wxT("%s"), strLabels[i]));
	}

    // Create a PieChart object of size 560 x 280 pixels, with a silver background,
    // black border, 1 pixel 3D border effect and rounded corners
	wxSize size = GetSize();
	PieChart *c = new PieChart(size.x, size.y, Chart::silverColor(), 0x000000, 1);
	c->setRoundedFrame();

    // Add a title box using 15 pts Times Bold Italic font in white color, on a deep
    // blue (0000CC) background with reduced-glare glass effect
	wxCharBuffer chBufTitle = title.ToUTF8();
	//char * chTitle = (char *)strTitle;
	//char * chTitle = "test HardCoded";
	//wxMessageBox(wxString::Format(wxT("%s"), chBufTitle));
	c->addTitle(chBufTitle, "timesbi.ttf", 12, 0xffffff)->setBackground(0x0000cc, 0x000000, Chart::glassEffect(Chart::ReducedGlare));

    // Set donut center at (280, 140), and outer/inner radii as 110/55 pixels
    c->setDonutSize(size.x/2, size.y/2, size.x/4-20, size.x/8-20);

    // Set 3D effect with 3D depth of 20 pixels
    c->set3D(20);

    // Set the label box background color the same as the sector color, with
    // reduced-glare glass effect and rounded corners
	TextBox *t = c->setLabelStyle();
	t->setBackground(Chart::SameAsMainColor, Chart::Transparent, Chart::glassEffect(Chart::ReducedGlare));
	t->setRoundedCorners();

    // Set the sector label format. The label consists of two lines. The first line
    // is the sector name in Times Bold Italic font and is underlined. The second
    // line shows the data value and percentage.
	c->setLabelFormat("<*block,halign=left*><*font=timesbi.ttf,size=12,underline=1*>{label}""<*/font*><*br*>{value} ({percent}%)");

    // Set the pie data and the pie labels
	c->setData(DoubleArray(doubleData, auxDataSize), StringArray(strLabels, auxLabelsSize));

    // Use the side label layout method
	c->setLabelLayout(Chart::SideLayout);


// Chart Director Example end


    // output the chart
	c->makeChart(fileName.ToUTF8());
	MemBlock m = c->makeChart(PNG);
	wxMemoryInputStream in(m.data, m.len);
	m_imgSave = wxImage(in, wxBITMAP_TYPE_PNG);
	m_img = m_imgSave;
	

    //free up resources
	delete c;
	for(int i=0;i<auxLabelsSize;i++)
		delete strLabels[i];
	delete []doubleData;
	delete []strLabels;
}

void PollGraphicFrame::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	dc.DrawBitmap(wxBitmap(m_img),0,0,true);
}

void PollGraphicFrame::OnSize(wxSizeEvent &event)
{
	int width, height;
	GetClientSize(&width, &height);
	m_img = m_imgSave;
	m_img.Rescale(width,height);

	// this produces flicker
	// Refresh();

	// this is the flicker free version
	wxClientDC dc(this);
	dc.DrawBitmap(wxBitmap(m_img),0,0,true);
}
