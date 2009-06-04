#include "TransmissionDisplayControl.h"
#include "TransmissionDisplayControlList.h"

void TransmissionDisplayControlList::Add(TransmissionDisplayControl *tdc)
{
	TDCListIterator it;
	bool found = false;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(*it == tdc)
		{
			found = true;
			break;
		}
	}
	if(!found)
		m_list.push_back(tdc);

	Sort();
}

void TransmissionDisplayControlList::Remove(TransmissionDisplayControl *tdc)
{
	TDCListIterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(*it == tdc)
		{
			m_list.erase(it);
			Sort();
			break;
		}
	}
}

bool TransmissionDisplayControlList::IsEmpty() const
{
	return m_list.empty();
}

void TransmissionDisplayControlList::Sort()
{
	m_list.sort(TDCListCompareFunction);
}

void TransmissionDisplayControlList::Clear()
{
	m_list.clear();
}

TDCListIterator TransmissionDisplayControlList::GetBeginIterator()
{
	return m_list.begin();
}

TDCListIterator TransmissionDisplayControlList::GetEndIterator()
{
	return m_list.end();
}

bool TDCListCompareFunction(TransmissionDisplayControl *left, TransmissionDisplayControl *right)
{
	if(!left)
		return false;
	if(!right)
		return true;
	return (*left < *right);
}
