#ifndef PARSE_STRING_H
#define PARSE_STRING_H

#include <string>
#include <deque>

struct ParseFieldValue
{
    std::string m_fieldName;

    double      m_value;

    bool        m_valueOK,
                m_booleanValue;
    
    ParseFieldValue(std::string & _fieldName, double _value, bool _booleanValue)
    {
        m_fieldName    = _fieldName;
        m_value        = _value;
        m_booleanValue = _booleanValue;
        m_valueOK      = true;
    }

    ParseFieldValue() 
    {
        m_valueOK = false;
    }
};

class CParseString
{
private:

    std::deque<ParseFieldValue> m_parseFieldValue;

public:
 
    CParseString(std::string & _str, char _token);
    ~CParseString() {}

    ParseFieldValue GetFieldValue(std::string _fieldName, bool _flPopValue=true);
};


#endif
