#include "parseString.h"


CParseString::CParseString(std::string & _str, char _token)
{
    m_parseFieldValue.clear();

    size_t pos = _str.find_first_of(_token);
    size_t lastpos = 0;

    while (pos != std::string::npos)
    {
        pos         = _str.find_first_of(_token,lastpos);
        size_t size = pos - lastpos;          
        
        std::string tokenStr;
        
        tokenStr    = _str.substr(lastpos, size);
        lastpos     = pos+1;
       
        size_t equalPos       = tokenStr.find("=");   
        std::string fieldName = tokenStr.substr(0, equalPos);

        if (equalPos != std::string::npos)
        {           
            std::string value = tokenStr.substr(equalPos+1, tokenStr.size() - equalPos+1);
            
            double dblValue = atof ( value.c_str() );
            
            m_parseFieldValue.push_back(ParseFieldValue(fieldName, dblValue, false));
        }
        else
            m_parseFieldValue.push_back(ParseFieldValue(fieldName, 0, true));
    }
}

ParseFieldValue CParseString::GetFieldValue(std::string _fieldName, bool _flPopValue)
{
    ParseFieldValue fieldValue;

    for (unsigned index=0; index < m_parseFieldValue.size(); index++)
    {
        if (_fieldName == m_parseFieldValue[index].m_fieldName)
        {
            fieldValue = m_parseFieldValue[index];
            
            if (_flPopValue)
            {
                std::deque<ParseFieldValue>::iterator it = m_parseFieldValue.begin();
                it += index;

                m_parseFieldValue.erase(it);
            }

            break;
        }
    }

    return fieldValue;
}


#if 0
void parseString(std::string & _str, char _token, std::deque<parseFieldValue> & _fieldValueList)
{
    _fieldValueList.clear();

    size_t pos = _str.find_first_of(_token);
    size_t lastpos = 0;

    while (pos != std::string::npos)
    {
        pos         = _str.find_first_of(_token,lastpos);
        size_t size = pos - lastpos;          
        
        std::string tokenStr;
        
        tokenStr    = _str.substr(lastpos, size);
        lastpos     = pos+1;
       
        size_t equalPos       = tokenStr.find("=");   
        std::string fieldName = tokenStr.substr(0, equalPos);

        if (equalPos != std::string::npos)
        {           
            std::string value = tokenStr.substr(equalPos+1, tokenStr.size() - equalPos+1);
            
            double dblValue = atof ( value.c_str() );
            
            _fieldValueList.push_back(parseFieldValue(fieldName, dblValue));
        }
        else
            _fieldValueList.push_back(parseFieldValue(fieldName, -1));
    }
}
#endif
