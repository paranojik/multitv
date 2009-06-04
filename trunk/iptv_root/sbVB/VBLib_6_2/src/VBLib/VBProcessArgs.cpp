// VBProcessArgs.cpp

#include "VBLib/VBLib.h"
using namespace std;
using namespace br::com::sbVB::VBLib;





/////////////////////////////////////////////////////////////////
//
// VBProcessArgsTest
//
/////////////////////////////////////////////////////////////////

br::com::sbVB::VBLib::VBProcessArgsTest::VBProcessArgsTest()
: m_argv(0)
{
}

br::com::sbVB::VBLib::VBProcessArgsTest::VBProcessArgsTest(const char *args,unsigned & argc,char ** & argv)
{
	setTestArgs(args,argc,argv);
}

br::com::sbVB::VBLib::VBProcessArgsTest::~VBProcessArgsTest() // destructor
{
	if (m_argv != 0)
		delete [] m_argv;
}



void br::com::sbVB::VBLib::VBProcessArgsTest::setTestArgs(const char *args,unsigned & argc,char ** & argv)
{
	unsigned i;
	VBString argsStr = args;
	VBString currArg;
	unsigned fromPos = 0;
	bool lookingForSpace = true;
	for (i=0 ; i <= argsStr.getLength() ; i++)
	{
		if (lookingForSpace && isSpaceChar(argsStr[i]))
		{
			lookingForSpace = false;
			currArg = "";
			currArg = argsStr.strInside(fromPos,i-1);
			m_argvContainer.push_back(currArg);
		}
		if (!lookingForSpace && !isSpaceChar(argsStr[i]))
		{
			lookingForSpace = true;
			fromPos = i;
		}
	}

	argc = m_argvContainer.size(); // set value to argument
	m_argv = new char* [argc]; // save alloced pointer, to delete it in the destructor
	argv = m_argv; // copy to argument of this method
	for (i=0 ; i < argc ; i++)
	{
		// it's dangerous to remove constness here. We are on risk ...
		argv[i] = const_cast<char*>(m_argvContainer[i].c_str());
	}

}


/////////////////////////////////////////////////////////////////
//
// VBProcessArgs
//
/////////////////////////////////////////////////////////////////


br::com::sbVB::VBLib::VBProcessArgs::VBProcessArgs()
: m_optionStr("-"), m_option("option"), m_nOfArgs("Number of arguments")
, m_minimumNumberOfArgs(-1), m_invalidOption("invalid option")
, m_invalidNumberOfArguments("invalid number of arguments")
{
}

br::com::sbVB::VBLib::VBProcessArgs::~VBProcessArgs() // destructor
{
}

void br::com::sbVB::VBLib::VBProcessArgs::setMinimumNumberOfArgs(int m)
{
	m_minimumNumberOfArgs = m;
}


void br::com::sbVB::VBLib::VBProcessArgs::setArgs(unsigned argc, char**argv)
{
	unsigned length = m_optionStr.getLength();
	VBString rawArg, value;

	// for every "raw argument". A "raw argument" is an argument or an option
	for (unsigned i=0 ; i < argc ; i++)
	{
		// cout << "DEBUG raw argument [" << i << "]" << endl;
		rawArg = argv[i];
		OpIterator it;
		if (rawArg.leftSideIs(m_optionStr))
		{
			// rawArg is an option 
			VBString optValue;
			optValue = rawArg.separateLeftString(m_optionStr);
			bool optionIsValid = false;
			// for every defined option
			for (it = m_optionStruct.begin();
			it != m_optionStruct.end(); it++)
			{
				// cout << "DEBUG testing option: " << it->option << endl;
				value = "";
				if (optValue.leftSideIs(it->option))
				{
					value = optValue.separateLeftString(it->option);
					optionIsValid = true;
					break;
				}
			}
			if (!optionIsValid)
				throw VBException_ProcessArgs(m_invalidOption);
			setOption(it->option,value);
			// cout << "DEBUG option added: " << it->option << " : " << value  << endl;
		}
		else
		{
			// rawArg is an argument
			m_args.push_back(rawArg);
			// cout << "DEBUG arg added: " << rawArg << endl;
		}
	}

	if (m_args.size() < getArgsNumber())
		throw VBException_ProcessArgs(m_invalidNumberOfArguments);
}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBProcessArgs::getArg(unsigned i)
{
	if (i >= m_args.size()+1) // +1 due to the program name argument
	{
		cout << "DEBUG: m_invalidNumberOfArguments, m_args.size()=" << m_args.size() << endl;
		throw VBException_ProcessArgs(m_invalidNumberOfArguments);
		return "";
	}
	return m_args[i];
}


void br::com::sbVB::VBLib::VBProcessArgs::defineOption(const char *optionArg, const char *explanation)
{
	Option oneOption;
	oneOption.option = optionArg;
	oneOption.explanation = explanation;
	oneOption.active = false;
	m_optionStruct.push_back(oneOption);
}

bool br::com::sbVB::VBLib::VBProcessArgs::optionIsValid(const char *option)
{
	unsigned size = m_optionStruct.size();

	for (unsigned i=0 ; i < size ; i++)
	{
		if (VBString(option)==m_optionStruct[i].option)
			return true;
	}
	return false;

}

// return true if the option exist in argv
bool br::com::sbVB::VBLib::VBProcessArgs::getOption(const char *option)
{
	unsigned size = m_optionStruct.size();

	for (unsigned i=0 ; i < size ; i++)
	{
		// cout << "DEBUG: " << m_optionStruct[i].option << endl;
		if (VBString(option)==m_optionStruct[i].option)
			return m_optionStruct[i].active;
	}

	throw VBException_ProcessArgs(m_invalidOption);
	return false;
}

// return a list of strings with the contents of what comes after the option.
// Example: "-I/path1 -I/path2 -I/path3" will return a VBStringList with 
// "path1" "path2" and "path3" for getOptionValues("I")
br::com::sbVB::VBLib::VBStringList & br::com::sbVB::VBLib::VBProcessArgs::getOptionValueList(const char *option)
{
	unsigned size = m_optionStruct.size();

	for (unsigned i=0 ; i < size ; i++)
	{
		if (VBString(option)==m_optionStruct[i].option)
			return m_optionStruct[i].valueList;
	}

	throw VBException_ProcessArgs(m_invalidOption);

	// empty only to compile without error
	static VBStringList ret;
	return ret;
}


void br::com::sbVB::VBLib::VBProcessArgs::setOption(const char *option, const char *value)
{
	unsigned size = m_optionStruct.size();

	for (unsigned i=0 ; i < size ; i++)
	{
		if (VBString(option)==m_optionStruct[i].option)
		{
			m_optionStruct[i].active = true;
			m_optionStruct[i].valueList.m_data.push_back(value);
			return;
		}
	}

	throw VBException_ProcessArgs(m_invalidOption);
}


br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBProcessArgs::explanation() const
{
	unsigned i;
	VBString ret = m_nOfArgs;
	ret += ": ";
	ret += getArgsNumber();
	ret += "\n";
	for (i=0 ; i < m_argsExplanation.size() ; i++)
	{
		ret += "arg[";
		ret += i+1;
		ret += "]: ";
		ret += m_argsExplanation[i];
		ret += "\n";
	}

	for (i=0 ; i < m_optionStruct.size() ; i++)
	{
		ret += m_option + " '" + m_optionStr + m_optionStruct[i].option + "' : " + m_optionStruct[i].explanation + "\n";
	}
	return ret;
}


void br::com::sbVB::VBLib::VBProcessArgs::setOptionStr(const char *optsrt)
{
	if (optsrt == 0 || strlen(optsrt)==0)
		throw VBException_ProcessArgs(m_invalidOption);

	// strlen >= 1
	m_optionStr = optsrt;
}


void br::com::sbVB::VBLib::VBProcessArgs::clear()
{
	m_args.clear();
	m_optionStruct.clear();
	m_argsExplanation.clear();
}


// iteration on a list of option values
br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBProcessArgs::getFirstOptionValue(const char *option)
{
	m_saveOption = option;
	VBStringList & sl = getOptionValueList(m_saveOption);
	m_listIterator = sl.m_data.begin();
	m_count = 0;
	unsigned size = sl.m_data.size();
	m_eolist = m_count >= size;
	if (m_eolist)
		return "";
	else
		return *m_listIterator++;;
}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBProcessArgs::getNextOptionValue()
{
	m_count++;
	unsigned size = getOptionValueList(m_saveOption).m_data.size();
	m_eolist = m_count >= size;
	if (m_eolist)
		return "";
	else
		return *m_listIterator++;;
}

bool br::com::sbVB::VBLib::VBProcessArgs::eolist()
{
	return m_eolist;
}


void br::com::sbVB::VBLib::VBProcessArgs::defineArgs(const char *argExplanation)
{
	m_argsExplanation.push_back(argExplanation);
}

unsigned br::com::sbVB::VBLib::VBProcessArgs::getArgsNumber() const
{
	if (m_minimumNumberOfArgs < 0) // m_minimumNumberOfArgs uninitialized, so ignore
		return m_argsExplanation.size();
	else 
		return ::br::com::sbVB::VBLib::VBmin(m_minimumNumberOfArgs, m_argsExplanation.size());
}
