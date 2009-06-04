// VBDate.h
// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

/////////////////////////////////////////////////////////////////////
//
// class VBDate
//
/////////////////////////////////////////////////////////////////////

class VBLIB_DLL_API VBDateTime; // forward definition

// _VBWeekdays and _VBMonths are not used. They are declared not to have
// compiler error in some compilers
typedef enum VBWeekdays { VBsun, VBmon, VBtue, VBwed, VBthu, VBfri, VBsat, VBinvalid } _VBWeekdays;
// one should not use "oct" as short for october, and
// "dec" as short for december, because "oct" and "dec" identifiers
// are already used by the standard library
typedef enum VBMonths { VBjan, VBfeb, VBmar, VBapr, VBmay, VBjun,
	VBjul, VBaug, VBsep, VBoct, VBnov, VBdec } _VBMonths;


//                               jan fev mar apr may jun jul ago sep oct nov dec   
const int VBDaysInMonths[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

class VBLIB_DLL_API VBDate 
{
	int m_baseYear;
	VBMonths m_baseMonth;
	int m_baseDay;
	VBWeekdays m_baseWeekDay;

	// set base date if given date is valid. Do nothing otherwise
	// ATTENTION: does not change m_baseWeekDay !
	bool p_setDate(int year, int month, int day);

	// return input with 2 digits
	::br::com::sbVB::VBLib::VBString twoDigits(int i) const;

	// place in delta the number of days that separates the given day
	// from base day.
	// if date is not valid, do nothing and return false
	bool deltaDays(int year, int month, int day, long & deltaDays) const;

	// return true given date is valid
	// return false otherwise
	// does not depend on base date
	bool validDate(int year, int month, int day) const;

	// return true if the given day is future compared with base day
	// return false otherwise
	// if given day and base day are the same, return false
	// does not depend on base date
	bool isFuture(int year, int month, int day) const;

	// return the number of days from a given date to end of month
	// does not depend on base date
	// example: given date: November 10, 2001; return value will be 20
	int remainingDaysOfGivenMonth(int year, int month, int day) const;

	// return the number of days from a given date to given date's current year,
	// not counting the current month, that is, from the next month on
	// does not depend on base date
	// Examples: year 2001, month 12, return 0;
	// year 2001, month 11, return 31;
	int remainingDaysOfGivenYearFromNextMonthOn(int year, int month) const;

	// return the number of days from a given date to this date's current year's final day
	// example: given date: November 15, 2001; return value will be 15+31 = 46
	int remainingDaysofGivenYear(int year, int month, int day) const;

	// return the number of days 
	// example: base date: April 15, 2001; given year 2005.
	// this function will return the sum of number of days of year 2002, 2003, 2004,
	// that is, 365+365+366, that is, 1096
	long numberOfDaysOfFullYearsFromNextYearToYearBeforeGivenYear(int year) const;

	// example: base date: April 21, 2001; given date is December 27, 2001
	// return is 361
	int numberOfDaysOfGivenYearUntilGivenDate(int year, int month, int day) const;

public:

	VBDate(); // default constructor

	// get system date and set it to this
	void setSystemDate();

	// return the weekday of a given date. sun=0, .. sat = 6
	// if date is invalid, the return value is -1
	int getWeekDay(const int year,const int month,const int day) const;

	//                                         jan fev mar apr may jun jul aug sep oct nov dec   
	static const int VBDaysInMonths[12]; // = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	// return true if hear is leap 
	// does not depend on base date
	bool leapYear (int year) const;

	// return the number of days of a given year, with attention to 
	// leap year. The return value is 365 or 366.
	// does not depend on base date
	int daysInAYear(int year) const;

	// return the number of days of a given month, 
	// with attention to leap year. 
	// if date is invalid return -1
	// does not depend on base date
	int daysInAMonth(int year, int month) const;

	// set date if given date is valid do nothing otherwise
	bool setDate(int year, int month, int day);
	bool setDate(VBDateTime dateTime);
	bool setDateYYYYMMDD(::br::com::sbVB::VBLib::VBString date);

	// return the delta days between this and cal
	long deltaDays(const VBDate & cal) const;

	// Return a VBDate whose date is this VBDate added (or subtracted) by delta days.
	VBDate dateAfterDeltaDays(const long deltaDays) const;

	// get weekDay of this
	int getWeekDay() const;

	// return a string with base date in given format
	::br::com::sbVB::VBLib::VBString getDate(int format=1) const;

	// Set this date to the day after.
	void tomorrow();

	// Set this date to the day before.
	void yesterday();

}; // end of VBDate

/////////////////////////////////////////////////////////////////////
//
// class VBDateTime
//
/////////////////////////////////////////////////////////////////////

/**********************************************************
struct tm 
{
	int tm_sec;     // seconds after the minute - [0,59] 
	int tm_min;     // minutes after the hour - [0,59] 
	int tm_hour;    // hours since midnight - [0,23] 
	int tm_mday;    // day of the month - [1,31] 
	int tm_mon;     // months since January - [0,11] 
	int tm_year;    // years since 1900 
	int tm_wday;    // days since Sunday - [0,6] 
	int tm_yday;    // days since January 1 - [0,365] 
	int tm_isdst;   // daylight savings time flag 
};
**********************************************************/

class VBLIB_DLL_API VBDateTime
{
	tm m_tm; // standard lib's struct tm to store date and time
	unsigned m_miliseconds;

	// set this struct tm from time_t
	void setThisTM(time_t ltime);

	// return input with 2 digits
	::br::com::sbVB::VBLib::VBString twoDigits(int i) const;

public:
	friend class VBDate; 

	VBDateTime(); // default constructor

	static const char *monthName[12];
	
	// return the struct tm from this
	operator struct tm () const {return m_tm;} ; 

	// return a VBDate object from this 
	operator VBDate () const; 

	// turn the this VBDateTime object to now (reading from the system)
	void setNow();

	// return date and time as string
	::br::com::sbVB::VBLib::VBString getDateStr(int dateFormat=1) const;

	// return time as string
	::br::com::sbVB::VBLib::VBString getTimeStr(unsigned format=0) const;

	// return date-time as string
	::br::com::sbVB::VBLib::VBString getDateTimeStr() const;
	
	// set this based in argument, compatible with getDateTimeStr
	// yyyy|mm(-1)|dd|hh|mm|ss|ts ts = tenths of second
	void setDateTimeStr(const char *setTime);

	// return true if this is less than argument
	bool operator<(const VBDateTime & dt) const;
	
	// return true if this is greater than argument
	bool operator>(const VBDateTime & dt) const;

	// return true if this is equal to argument
	bool operator==(const VBDateTime & dt) const;

	int getYear() const;
	unsigned getMonth() const;
	unsigned getDay() const;
	unsigned getHour() const;
	unsigned getMinute() const;
	unsigned getSecond() const;
	unsigned getMiliSecond() const;

	// add or subtract time to this VBDateTime
	void addYear(int year);
	void addMonth(int month);
	void addDay(int day);
	void addHour(int hour);
	void addMinute(int minute);
	void addSecond(int second);

	void setDate(int year, unsigned month, unsigned day);
	void setTime(unsigned hour, unsigned minute, unsigned second, unsigned miliseconds=0);
	double getDifSeconds(const VBDateTime & dt) const;

	// julian date is 1970/1/1
	unsigned long getSecondsToJulianDate() const;

}; // end of VBDateTime

/////////////////////////////////////////////////////////////////////
//
// class VBDateTimeTimezone
//
/////////////////////////////////////////////////////////////////////

class VBDateTimeTimezone : public VBDateTime
{
	int m_timeRelativeToGreenwitch;
public:
	VBDateTimeTimezone() {};

}; // end of VBDateTimeTimezone


}}}} // close namespace ::br::com::sbVB::VBLib
