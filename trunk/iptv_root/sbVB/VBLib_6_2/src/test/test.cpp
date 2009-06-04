// VBLib test

#include <string>
#include <deque>
#include <cmath>
#include <iostream>
#include <sstream> // istringstream
using namespace std;
#include "VBLib/VBLib.h"
using namespace br::com::sbVB::VBLib;

#include "test/VBCrossDll_test.h"

void loadStrContainer(deque<VBString> & strContainer)
{
	strContainer.push_back("123");
	strContainer.push_back("123");
	strContainer.push_back("123");
	strContainer.push_back("123");
	strContainer.push_back("123");
	
}

void VBString_test()
{
	{
		deque<VBString> strContainer;
		loadStrContainer(strContainer);
		
	}
	
	{
		
		VBString s = "hello";
		VBAssert (s+"abc" == "helloabc","VBString ==");
		
		s = "";
		VBAssert (s.getLength() == 0,"getLength");
		
		VBString a;
		a = "12345 ";
		VBString b = a + "33";
		VBString c = b;
		
		VBAssert(c == b);
		VBAssert(c >= b);
		VBAssert(!(c < b));
		VBAssert(c <= b);
		VBAssert(a != b);
		VBAssert(b=="12345 33");
		
		// common error:
		// a = "abc" + "def"; // can not add 2 pointers directly
		// use instead
		a = "abc";  a += "def"; 
		// or a = "abc" + VBString("def"); 	
		VBAssert(a=="abcdef");
	}
	
	
	{
		
		VBString a;
		a = "123";
		VBAssert(a=="123");
		a = "456" + a;
		VBAssert(a=="456123");
		a = a + 78;  // int
		VBAssert(a=="45612378");
		a.setDecs(3);
		a = a + 0.12345;  // double
		VBAssert(a=="456123780.123");
		a = "abc";
		a += 12; // int
		VBAssert(a=="abc12");
		a = "abc";
		a += 12.12345; // double
		VBAssert(a=="abc12.123");
		a = "abc";
		a += "12"; // const char *
		VBAssert(a=="abc12");
		
		// cout << "Enter anything:";
		// cin >> a;
		// cout << "a=" << a << endl;
		
		istringstream iss;
		iss.str("something");
		iss >> a;
		VBAssert(a=="something");
	}
	
	{
		VBString a,b;
		a = "123456789";
		b = a.beforeFind("56"); // b = "1234";
		VBAssert(b=="1234");
		b = a.afterFind("xx"); // b = ""; if string not found
		VBAssert(b=="");
		
		a = "0123456789012345678901234567890123456789";
		VBString find = "34";
		VBString replace = "abc";
		a.replace(find,replace); // (changes a)
		VBAssert(a=="012abc56789012abc56789012abc56789012abc56789");
		
		//   0   1   2   3   4   5    6   (tok)
		a = "abc,def,ghi,123,456,7890,yyy";
		char tok = ',';
		VBAssert(a.strtok(tok,0)=="abc");
		VBAssert(a.strtok(tok,1)=="def");
		VBAssert(a.strtok(tok,5)=="7890");
		bool found;
		b = a.strtok(tok,3,found); // found is a return value
		VBAssert(found);
		VBAssert(b=="123");		
	}
	
	
	{
		long k = 1234;
		VBString s = "abc";
		s += k;
		VBAssert(s=="abc1234","+= long");
	}

}


void VBFileDirectory_test_recursive (VBString path) 
{
#if 0
	static unsigned recuseCount = 0;
	const char *existingDirectories[][2] = 
	{
		{
			// recuseCount = 0
			"c:/xxx/x1"
			,"c:/xxx/x2"
			,"c:/xxx/x3"
			,"c:/xxx/x4"
			,"c:/xxx/x5"
		},
		{
			// recuseCount = 1
			"c:/xxx/x1/x11"
			,"c:/xxx/x1/x12"
			,"c:/xxx/x1/x13"
		}
	};
	unsigned size = sizeof(existingDirectories)/sizeof(const char *);
#endif
	
	VBFileDirectory a;
	VBString str;
	unsigned i=0;
	for (str = a.getFirst(path) ; !a.eolist() ; str=a.getNext()) 
	{
		// mark the ones that are directories
		if (a.isDir()) 
		{
			// VBAssert(str==existingDirectories[recuseCount][i++]);
			cout << str << endl;
		}
		if (a.isDir()) 
		{
			// recuseCount++;
			VBFileDirectory_test_recursive(a.getPath()); // recursive call
		}
	}
}


void VBFileDirectory_test()
{
	// execution directory should be c:\xxx
	
	const char *executeScript = 
		"c:\n"
		"cd \\xxx\n"
		"mkdir x1\n"
		"mkdir x2\n"
		"mkdir x3\n"
		"mkdir x4\n"
		"mkdir x5\n"
		"cd x1\n"
		"mkdir x11\n"
		"mkdir x12\n"
		"mkdir x13\n"
		"cd ../x2\n"
		"mkdir x21\n"
		"mkdir x22\n"
		"mkdir x23\n"
		;
	
	const char *batFileName = "b.bat";
	ofstream batFile(batFileName);
	VBAssert(batFile!=0,"Can not open bat file");
	batFile << executeScript << endl;
	batFile.close();
	
	system(batFileName);
	
	cout << "========================================================" << endl;
	
	VBFileDirectory fd;
	
	const char *path = "c:/xxx";
	VBFileDirectory_test_recursive(path);
	
}

void VBProcessArgs_test()
{
	// instantiate args
	VBProcessArgs args;
	unsigned argc;
	char **argv;
	
	try
	{
		
		// create argc and argv for test
		VBProcessArgsTest argsTest;
		argsTest.setTestArgs("abc	def      hij zzz",argc,argv);
		VBAssert(argc==4);
		VBAssert(VBString(argv[0])=="abc");
		VBAssert(VBString(argv[1])=="def");
		VBAssert(VBString(argv[2])=="hij");
		VBAssert(VBString(argv[3])=="zzz");
		
		////////////////////////////////////////////////////////////////////////////
		
		// use argc and argv as if these came from the command line
		// create argc and argv for test
		VBProcessArgsTest argsTest2;
		argsTest2.setTestArgs("myprog -n -1 -2",argc,argv);
		
		
		// define how args should behave for the user program
		args.clear();
		args.defineOption("n","no increment");
		args.defineOption("1","option 1");
		args.defineOption("2","option 2");
		
		// example of translation to portuguese (optional)
		args.m_option = "opcao";
		args.m_nOfArgs = "Numero de argumentos";
		args.m_invalidOption = "opcao invalida";
		args.m_invalidNumberOfArguments = "Numero de argumentos invalido";
		
		// change the option string (optional)
		// args.m_optionStr = "--";
		
		// process the args
		args.setArgs(argc,argv);
		
		// use the information from args to the program
		VBAssert(args.getOption("n"),"n");
		VBAssert(args.getOption("1"),"1");
		VBAssert(args.getOption("2"),"2");
		
		
		////////////////////////////////////////////////////////////////////////////
		
		VBProcessArgsTest argsTest3("myprog -Ipath1 -Ipath2 -Ipath3 -Ipath4 -Ipath5 -Jxxx -Jzzz",argc,argv);
		// process the args
		args.clear();
		args.defineOption("I","path");
		args.defineOption("J","other");
		args.setArgs(argc,argv);
		
		VBString currValue;
		unsigned i=0;
		const char *refValues[] = { "path1", "path2", "path3", "path4", "path5"};
		for (currValue = args.getFirstOptionValue("I"); !args.eolist(); 
		currValue = args.getNextOptionValue())
		{
			VBAssert(currValue==refValues[i++]);
		}
		
		i=0;
		const char *refValues2[] = { "xxx", "zzz"};
		for (currValue = args.getFirstOptionValue("J"); !args.eolist(); 
		currValue = args.getNextOptionValue())
		{
			// cout << currValue << endl;
			VBAssert(currValue==refValues2[i++]);
		}
		
		////////////////////////////////////////////////////////////////////////////
		
		VBProcessArgsTest argsTest4("-noincrement:abc 1 2 3 4 5 6",argc,argv);
		// define myprog profile
		args.clear();
		args.defineArgs("name of template file"); // 1
		args.defineArgs("name of output file"); // 2
		args.defineArgs("name of file to save Build ID"); // 3
		args.defineArgs("string inside the template file to be changed for BuildID"); // 4
		args.defineArgs("string inside the template file to be changed for date time"); // 5
		args.defineArgs("string inside the template file to be changed for customer version"); // 6
		// args.setMinimumNumberOfArgs(3);
		
		args.defineOption("noincrement:","set this if you don't want to increment BuildID");
		args.setArgs(argc,argv);
		
		VBAssert(args.getFirstOptionValue("noincrement:") == "abc");
		
		
		
	}
	catch (const VBException_ProcessArgs & e)
	{
		// print the help for the program
		cout << "myprog [options] [argument1] [argument2]" << endl;
		cout << args.explanation() << endl;
		cout << e.getExplanation() << endl;
	}
	
	
}

struct DS
{
	deque<VBString> m_data;
};

void VBStringList_test()
{
	deque<VBString> ds;
	ds.push_back("path1");
	ds.push_back("path2");
	
	DS ds2;
	ds2.m_data.push_back("path1");
	ds2.m_data.push_back("path2");
	ds2.m_data.push_back("path3");
}


void VBDateTime_test()
{
	VBDateTime now;
	now.setNow();
	VBString time = now.getDateStr();
	cout << time << endl;
	
	VBString dt = now.getDateTimeStr();
	cout << dt << endl;
	VBDateTime otherTime;
	otherTime.setDateTimeStr(dt);
	VBAssert(now==otherTime);
	
	otherTime.addSecond(1);
	VBAssert(now.getSecondsToJulianDate()==otherTime.getSecondsToJulianDate()-1);

	cout << "now.getSecondsToJulianDate()=" << now.getSecondsToJulianDate() << endl;

	VBDate myDate;
	cout << "days on feb 2006=" << myDate.daysInAMonth(2006,1) << endl;

	myDate.setDateYYYYMMDD("20060201");
	cout << "myDate=" << myDate.getDate() << endl;
	VBDate newDate = myDate.dateAfterDeltaDays(365*3);
	cout << "newDate=" << newDate.getDate() << endl;


	
	// cout << now.getSecondsToJulianDate() << ":" << otherTime.getSecondsToJulianDate() << endl;
	
#if 0
	// that code won t compile in Windows
	struct timeval tv;
	struct timezone tz;
	gettimeofday( &tv, &tz );
	for (unsigned i = 0 ; i < 100 ; i++)
	{
		for (unsigned z1=0 ; z1 < 1000 ; z1++)
			for (unsigned z2=0 ; z2 < 1000 ; z2++)
				double d = 1.1; // empty loop
			
			gettimeofday( &tv, &tz );
			cout << i << ":" << tv.tv_usec/1000 << "  ";
	}
	
	cout << endl;
#endif
}

void test_VBString_input()
{
	VBString input;
	cout << "Enter string:";
	cin >> input;
	cout << "You entered '" << input << "'" << endl;
}


void myFun(double a, double b, double c, double & x, double & y)
{
	x = a + b + c;
	y = (2*a + 3*b + 4*c)/9;
}

int math_test()
{
	double r, s;
	myFun(1.1,2.2,3.3,r,s);
	VBAssert(nearEqual(r,6.6),"r");
	VBAssert(nearEqual(s,2.44444444444444444),"s");
	// cout << "s=" << s << endl;
	return 0;
}

void VBString_test_2()
{
	VBString s = "12345";
	VBString s2 = s;
	bool b;
	b = s.existSubstring(s2);
	VBAssert(b==true);
	
	unsigned u = s.countSubString(s2);
	VBAssert(u==1,"u!=1");
	
	VBString test = "teste, teste,<A href=\"url\">Link</A>, este, TesTe.";
	VBString link, link_item;
	bool found;
	unsigned count;
	
	link = test.afterFind("<a", found);
	VBAssert(found==false); // como � atualmente
	
	link = test.afterFind("<a", found, true);
	VBAssert(found==false); // segue o default
	
	link = test.afterFind("<a", found, false);
	VBAssert(found==true);
	VBAssert(link==" href=\"url\">Link</A>, este, TesTe.");
	
	link_item = link.beforeFind("</a", found);
	VBAssert(found==false);
	VBAssert(link_item==link);
	
	link_item = link.beforeFind("</a", found, false);
	//cout << "DEBUG: Link_item: \"" << link_item << "\"" << endl;
	VBAssert(found==true);
	VBAssert(link_item==" href=\"url\">Link");
	
	VBAssert(!test.existSubstring("link"));
	
	// exist "link" testing in a non case sensitive way
	VBAssert(test.existSubstring("link", false)); 
	
	count = test.countSubString("teste");
	VBAssert(count==2);
	
	count = test.countSubString("teste", false);
	VBAssert(count==3);

	VBString t = "abcdef";
	VBAssert(t.separateLeftString("abc")=="def");
}



int main()
{
///////////////////////////////////////////////////////
// tests that require operation or produce output
///////////////////////////////////////////////////////
	// test_VBString_input();
	// VBFileDirectory_test();
	VBDateTime_test();

///////////////////////////////////////////////////////
// 100% automated tests, that does not produce output
///////////////////////////////////////////////////////
#if 1
	VBString_test();
	VBString_test_2();
	VBStringList_test();
	VBProcessArgs_test();
	VBCrossDLL_test();
	math_test();
#endif
	
	return 0;
}

