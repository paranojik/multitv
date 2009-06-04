#ifndef TRANSMISSIONLIST_H
#define TRANSMISSIONLIST_H

#include <list>
#include "ChannelUser.h"
#include "Transmission.h"

typedef std::list<Transmission *> TransmissionList;
typedef TransmissionList::iterator TransmissionIterator;
typedef TransmissionList::const_iterator TransmissionConstIterator;
typedef bool (*TransmissionCompareFunction)(Transmission *, Transmission *);

/** @brief Keeps track of Transmissions.
 *
 * This class is used by the App to keep track of Transmissions. It stores all Transmissions
 * in a list, is used to find them by ID or user name, and keeps count of how many of them
 * exist and how many of those are active (alive) in any given moment.
 */
class TransmissionManager
{
private:
	TransmissionList m_list;
public:
	virtual ~TransmissionManager();

	Transmission *AddNewTransmission(unsigned long mediaID, const wxString &userName = wxEmptyString);
	void RemoveTransmission(unsigned long mediaID);
	Transmission *GetTransmissionByID(unsigned long mediaID);
	Transmission *GetTransmissionByName(const wxString &userName);
	unsigned GetTransmissionCount() const;
	unsigned GetActiveTransmissionCount() const;

	void TerminateAllTransmissions();
};

#endif
