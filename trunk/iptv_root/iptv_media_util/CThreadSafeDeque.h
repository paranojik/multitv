#ifndef C_THREAD_SAFE_DEQUE_H
#define C_THREAD_SAFE_DEQUE_H

#include <deque>

#include "global_error.h"
#include "semaphore.h"

template <class T>
class CThreadSafeDeque : private std::deque<T>
{
private:
	typedef std::deque<T> dequeT;
	typename std::deque<T>::iterator iteratorT;

	BOOL m_bInit;
	_SEMAPHORE * m_pSemaph;

public:
	CThreadSafeDeque();
	virtual ~CThreadSafeDeque();

	ULONG PushBack(T & obj);
	ULONG PushFront(T & obj);
	ULONG PopBack();
	ULONG PopFront();
	ULONG Erase(ULONG _index);
	ULONG Size();

	T & operator[] (ULONG _index);
	T & First();
	T & Last();
};

template<class T>
CThreadSafeDeque<T>::CThreadSafeDeque()
{
	m_bInit = FALSE;
		
	m_pSemaph = new _SEMAPHORE();
	if (m_pSemaph)
		m_bInit = TRUE;
}

template<class T>
CThreadSafeDeque<T>::~CThreadSafeDeque()
{
	if (m_pSemaph)
		delete m_pSemaph;
}

template<class T>
T & CThreadSafeDeque<T>::operator[] (ULONG _index)
{
	m_pSemaph->Wait();
	T & obj = dequeT::operator[](_index);
	m_pSemaph->Signal();

	return obj;
}

template<class T>
T & CThreadSafeDeque<T>::First()
{
	return operator[](0);
}

template<class T>
T & CThreadSafeDeque<T>::Last()
{
	m_pSemaph->Wait();

	int lastPos = this->size() - 1;		// can't use implicit 'this' in the call to size() due to use of generic programming
	T & ret = operator[](lastPos);

	m_pSemaph->Signal();

	return ret;
}

template<class T>
ULONG CThreadSafeDeque<T>::PushBack(T & obj)
{
	if (!m_bInit)
		return RET_INIT_ERROR;

	m_pSemaph->Wait();
	dequeT::push_back(obj);
	m_pSemaph->Signal();

	return RET_OK;
}
	
template<class T>
ULONG CThreadSafeDeque<T>::PushFront(T & obj)
{
	if (!m_bInit)
		return RET_INIT_ERROR;	
	
	m_pSemaph->Wait();
	dequeT::push_front(obj);
	m_pSemaph->Signal();

	return RET_OK;
}
	
template<class T>
ULONG CThreadSafeDeque<T>::PopBack()
{
	if (!m_bInit)
		return RET_INIT_ERROR;

	m_pSemaph->Wait();
	dequeT::pop_back();
	m_pSemaph->Signal();

	return RET_OK;
}
	
template<class T>
ULONG CThreadSafeDeque<T>::PopFront()
{
	if (!m_bInit)
		return RET_INIT_ERROR;

	m_pSemaph->Wait();
	dequeT::pop_front();
	m_pSemaph->Signal();

	return RET_OK;
}

template<class T>
ULONG CThreadSafeDeque<T>::Erase(ULONG _index)
{
	if (!m_bInit)
		return RET_INIT_ERROR;

	ULONG ret = RET_OK;

	m_pSemaph->Wait();
	
	if (_index < this->size())
	{
		// typename iteratorT it = this->begin();
		typename std::deque<T>::iterator it = this->begin();
		it += _index;
		this->erase(it);
	}
	else
		ret = RET_ERROR;
		
	m_pSemaph->Signal();

	return ret;
}

template<class T>
ULONG CThreadSafeDeque<T>::Size()
{
	if (!m_bInit)
		return 0;
	
	m_pSemaph->Wait();
    ULONG ulSize = (ULONG)dequeT::size();
	m_pSemaph->Signal();

	return ulSize;
}

#endif
