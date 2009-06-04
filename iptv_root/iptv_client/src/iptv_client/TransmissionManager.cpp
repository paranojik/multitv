#include "Transmission.h"
#include "TransmissionManager.h"

/** Destructor. Terminates all registered Transmissions.
 */
TransmissionManager::~TransmissionManager()
{
	TerminateAllTransmissions();
}

/** Creates a new Transmission object.
 * @param[in] mediaID	Media ID.
 * @param[in] userName	User nickname. Default: wxEmptyString.
 * @return Pointer to the newly-created Transmission object, or NULL if a Transmission object with
 * the same Media ID already exists.
 */
Transmission *TransmissionManager::AddNewTransmission(unsigned long mediaID, const wxString &userName)
{
	Transmission *transm = GetTransmissionByID(mediaID);
	if(!transm)
	{
		transm = new Transmission(mediaID, userName);
		m_list.push_back(transm);
		return transm;
	}
	return NULL;
}

/** Removes a Transmission object. Terminates and deallocates.
 * @param[in] mediaID	Media ID of the transmission to remove.
 * @remarks This method does nothing if there is no Transmissions with the given ID.
 */
void TransmissionManager::RemoveTransmission(unsigned long mediaID)
{
	Transmission *transm = GetTransmissionByID(mediaID);
	if(transm)	// Don't attempt to remove a non-existent transmission.
	{
		m_list.remove(transm);
		delete transm;
	}
}

/** Search for a Transmission object with given media ID in the internal list.
 * @param[in] mediaID	The media ID to look for.
 * @return Pointer to the associated Transmission object, or NULL if not found.
 */
Transmission *TransmissionManager::GetTransmissionByID(unsigned long mediaID)
{
	TransmissionIterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetMediaID() == mediaID)
			return *it;
	}
	return NULL;
}

/** Search for a Transmission object with given user name in the internal list.
 * @param[in] userName	User name to look for.
 * @return Pointer to the associated Transmission object, or NULL if not found.
 */
Transmission *TransmissionManager::GetTransmissionByName(const wxString &userName)
{
	TransmissionIterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetUserName() == userName)
			return *it;
	}
	return NULL;
}

/** Gets the total number of Transmission objects.
 * @return The object count.
 */
unsigned TransmissionManager::GetTransmissionCount() const
{
	return (unsigned)m_list.size();
}

/** Gets the number of active Transmission objects.
 * @return The object count.
 */
unsigned TransmissionManager::GetActiveTransmissionCount() const
{
	unsigned count = 0;
	TransmissionConstIterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->IsActive())
			count++;
	}
	return count;
}

/** Terminates all Transmissions. Clears the internal list.
 */
void TransmissionManager::TerminateAllTransmissions()
{
	TransmissionIterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
		delete *it;
	m_list.clear();
}
