#include <wx/wx.h>
#include "Mode.h"

static const Mode g_nullMode = Mode();

/** Constructor.
 * @param[in] id			Mode ID (see Mode.h).
 * @param[in] parameter		Mode parameter.
 */
Mode::Mode(int id, const wxString &parameter)
{
	m_id = id;
	m_parameter = parameter;
}

/** Get mode ID.
 * @return Mode ID.
 */
int Mode::GetID() const
{
	return m_id;
}

/** Get mode parameter.
 * @return Mode parameter string.
 */
const wxString &Mode::GetParameter() const
{
	return m_parameter;
}

/** Set mode ID.
 * @param[in] id	Mode ID.
 */
void Mode::SetID(int id)
{
	m_id = id;
}

/** Set mode parameter.
 * @param[in] parameter	Mode parameter string.
 */
void Mode::SetParameter(const wxString &parameter)
{
	m_parameter = parameter;
}
