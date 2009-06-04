// VBProcessArgs.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

/////////////////////////////////////////////////////////////////////
//
// VBProcessArgs
// process main(unsigned argc, char **argv)
//
/////////////////////////////////////////////////////////////////////

// myapp [options]

// myapp -option_1 -option_2

class VBLIB_DLL_API VBException_ProcessArgs : public ::br::com::sbVB::VBLib::VBException
{
public:
	VBException_ProcessArgs (const char *m) : VBException(m) {};
};

class VBLIB_DLL_API VBProcessArgsTest 
{
	std::deque<br::com::sbVB::VBLib::VBString> m_argvContainer;
	char **m_argv;
public:
	VBProcessArgsTest(); // constructor
	VBProcessArgsTest(const char *args,unsigned & argc,char ** & argv); // constructor
	~VBProcessArgsTest(); // destructor

	// set args for test
	void setTestArgs(const char *args,unsigned & argc,char ** & argv);
};

// call as a global function
void VBProcessArgsTestFun(const char *args,unsigned & argc,char ** & argv);

	
class VBLIB_DLL_API VBProcessArgs 
{

	struct Option
	{
		br::com::sbVB::VBLib::VBString option; // the string of option
		br::com::sbVB::VBLib::VBString explanation; // explanation used only for optionsExplanation()
		br::com::sbVB::VBLib::VBStringList valueList; // what comes after the option
		bool active; // true if the option exist in argv
	};

	int m_minimumNumberOfArgs; // negative means uninitilized
	unsigned m_count;
	bool m_eolist;
	br::com::sbVB::VBLib::VBStringList::iterator m_listIterator;
	typedef std::deque<Option>::iterator OpIterator;
	std::deque<Option> m_optionStruct; // option and its explanation
	std::deque<br::com::sbVB::VBLib::VBString> m_args;
	std::deque<br::com::sbVB::VBLib::VBString> m_argsExplanation;
	bool optionIsValid(const char *option);
	// optionStr is set as "-" in the constructos, but that can be changed
	br::com::sbVB::VBLib::VBString m_optionStr;
	br::com::sbVB::VBLib::VBString m_saveOption;
	br::com::sbVB::VBLib::VBStringList & getOptionValueList(const char *option);
public:
	VBProcessArgs(); // constructor
	~VBProcessArgs(); // destructor

	// strings used for explanation. Overwrite them to get languages other than English
	br::com::sbVB::VBLib::VBString m_option, m_nOfArgs, 
		m_invalidOption, m_invalidNumberOfArguments;

	// optionStr is set as "-" in the constructos, but that can be changed
	void setOptionStr(const char *optsrt);

	void clear();
	unsigned getArgsNumber() const;
	void defineArgs(const char *argExplanation);
	void setArgs(unsigned argc, char**argv); 
	br::com::sbVB::VBLib::VBString getArg(unsigned i);
	void defineOption(const char *option, const char *explanation="");
	bool getOption(const char *option);
	void setOption(const char *option, const char *value);
	br::com::sbVB::VBLib::VBString explanation() const;

	// iteration on a list of option values
	br::com::sbVB::VBLib::VBString getFirstOptionValue(const char *option);
	br::com::sbVB::VBLib::VBString getNextOptionValue();
	bool eolist();
	void setMinimumNumberOfArgs(int m);
};

// simulate creation of argv and argv


}}}} // close namespace ::br::com::sbVB::VBLib

