#ifndef DATA_REG_H
#define DATA_REG_H

#include "semaphore.h"
#include "global_error.h"

template <class Type>
class CReadData_reg 
{
protected:

    BOOL m_bNewData;
    Type m_Data;
	
    _SEMAPHORE  m_Semaph;

public:
    
    CReadData_reg();
    virtual ~CReadData_reg () = 0;

    ULONG ReadData(Type & data);
    BOOL  NewData() {return m_bNewData; }
};

template <class Type>
CReadData_reg<Type>::CReadData_reg() : 
    m_Semaph()
{
    m_bNewData = FALSE;
}

template <class Type>
CReadData_reg<Type>::~CReadData_reg()
{
}

template <class Type>
ULONG CReadData_reg<Type>::ReadData(Type & data)
{
    m_Semaph.Wait();
	
    data = m_Data;
    m_bNewData = FALSE;
	
    m_Semaph.Signal();

    return RET_OK;
}

template <class Type>
class CWriteData_reg : public CReadData_reg<Type>
{

public:

    CWriteData_reg()  {}
    virtual ~CWriteData_reg() {}

    ULONG WriteData(Type & data);
};

template <class Type>
ULONG CWriteData_reg<Type>::WriteData(Type & data)
{
    CReadData_reg<Type>::m_Semaph.Wait();
	
    CReadData_reg<Type>::m_Data		= data;
    CReadData_reg<Type>::m_bNewData = TRUE;
	
    CReadData_reg<Type>::m_Semaph.Signal();

    return RET_OK;
}

#endif
