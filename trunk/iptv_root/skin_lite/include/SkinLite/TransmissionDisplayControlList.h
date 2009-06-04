#ifndef TRANSM_DISPLAY_CTRL_LIST_H
#define TRANSM_DISPLAY_CTRL_LIST_H

#include <list>

#include "TransmissionDisplayControl.h"

typedef std::list<TransmissionDisplayControl *> TDCListType;
typedef TDCListType::iterator TDCListIterator;

bool TDCListCompareFunction(TransmissionDisplayControl *left, TransmissionDisplayControl *right);

/** @brief Create a Transmission display control list.
*
*/
class TransmissionDisplayControlList
{
private:
	TDCListType m_list;
public:
	void Add(TransmissionDisplayControl *tdc);
	void Remove(TransmissionDisplayControl *tdc);
	TDCListIterator GetBeginIterator();
	TDCListIterator GetEndIterator();
	bool IsEmpty() const;
	void Sort();
	void Clear();
};

#endif	// TRANSM_DISPLAY_CTRL_LIST_H
