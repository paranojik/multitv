// VBDate.cpp

#include "VBLib/VBLib.h"
#ifdef WIN32	
	// Windows code
#else
	// Unix code
	#include <sys/time.h>
#endif

/////////////////////////////////////////////////////////////////
//
// class VBDate
//
/////////////////////////////////////////////////////////////////

// static member initialization
//                                          jan fev mar apr may jun jul ago sep oct nov dec   
const int br::com::sbVB::VBLib::VBDate::VBDaysInMonths[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


// set base date if given date is valid. Do nothing otherwise
// ATTENTION: does not change m_baseWeekDay !
bool br::com::sbVB::VBLib::VBDate::p_setDate(int year, int month, int day) 
{
	bool OK = validDate(year,month,day);
	if (!OK) return false;
	m_baseYear = year;
	m_baseMonth = (VBMonths) month;
	m_baseDay = day;
	return true;
}

// default constructor
br::com::sbVB::VBLib::VBDate::VBDate() 
{
	m_baseYear = 2001;
	m_baseMonth = VBapr;
	m_baseDay = 21;
	m_baseWeekDay = VBsat;
}


#if 0 // commented out begin
struct tm {
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
  time_t ltime;
  tm *t;

  time( &ltime ); // get time and date
  t = localtime( &ltime ); // convert to struct tm

  // display
  cout << "Now is:" << endl;
  cout << "Time: " << t->tm_hour << ":" << t->tm_min 
    << ":" << t->tm_sec << endl;
  cout << "Year: " << t->tm_year + 1900 << endl;
  cout << "Month: " << months[t->tm_mon] << endl;
  cout << "Day: " << t->tm_mday << endl;
  cout << "Week day: " << weekDay[t->tm_wday] << endl;

#endif // commented out end

void br::com::sbVB::VBLib::VBDate::setSystemDate()
{

	time_t ltime;
	tm *t;
	time( &ltime ); // get time and date
	t = localtime( &ltime ); // convert to struct tm

	setDate(t->tm_year + 1900, t->tm_mon, t->tm_mday);
}


bool br::com::sbVB::VBLib::VBDate::leapYear (int year) const
{
	int k = year % 4;  // every 4 years, force leap year true
	bool ret = (k == 0);
	k = year % 100; // over previous, every 100 years, force leap year false
	ret = ret && !(k == 0);
	k = year % 400; // over previous, every 400 years, force leap year true
	ret = ret || (k == 0);
	return ret;
}

int br::com::sbVB::VBLib::VBDate::daysInAYear(int year) const
{
	int ret = 365;
	if (leapYear(year)) 
		ret++;
	return ret;
}

int br::com::sbVB::VBLib::VBDate::daysInAMonth(int year, int month) const
{
	// if (!validDate(year,month,1)) return -1;
	// can't test valid date, because validDate calls daysInAMonth
	int ret;
	ret = VBDaysInMonths[month];
	if (leapYear(year) && (month==VBfeb)) 
		ret++;
	return ret;
}

bool br::com::sbVB::VBLib::VBDate::validDate(int year, int month, int day) const
{
	if (month < VBjan || month > VBdec ) return false;
	if (day <= 0 || day > daysInAMonth(year,month)) return false;
	return true;
}

bool br::com::sbVB::VBLib::VBDate::isFuture(int year, int month, int day) const
{
	if (year > m_baseYear) return true;
	if (year < m_baseYear) return false;
	if (month > m_baseMonth) return true;
	if (month < m_baseMonth) return false;
	if (day > m_baseDay) return true;
	return false;
}

int br::com::sbVB::VBLib::VBDate::remainingDaysOfGivenMonth(int year, int month, int day) const
{
	if (!validDate(year,month,day)) return -1;
	return daysInAMonth(year,month) - day;
}

int br::com::sbVB::VBLib::VBDate::remainingDaysOfGivenYearFromNextMonthOn(int year, int month) const
{
	if (!validDate(year,month,1)) return -1;
	int ret=0;
	for (int m = month+1 ; m < 12 ; m++)
		ret += daysInAMonth(year,m);
	return ret;
}

int br::com::sbVB::VBLib::VBDate::remainingDaysofGivenYear(int year, int month, int day) const
{
	int ret;
	ret = remainingDaysOfGivenMonth(year,month,day);
	ret += remainingDaysOfGivenYearFromNextMonthOn(year,month);
	return ret;
}

long br::com::sbVB::VBLib::VBDate::numberOfDaysOfFullYearsFromNextYearToYearBeforeGivenYear(int year) const
{
	long ret=0;
	for (int y = m_baseYear+1 ; y < year ; y++)
		ret += daysInAYear(y);
	return ret;
}

int br::com::sbVB::VBLib::VBDate::numberOfDaysOfGivenYearUntilGivenDate(int year, int month, int day) const
{
	if (!validDate(year,month,day)) return -1;
	int ret=0;
	for (int m = 0 ; m < month ; m++)
		ret += daysInAMonth(year,m);
	// after this loop, all days until previous month are already added
	ret += day;
	return ret;
}

bool br::com::sbVB::VBLib::VBDate::setDate(int year, int month, int day) 
{
	bool OK = validDate(year,month,day);
	if (!OK) return false;
	m_baseWeekDay = (VBWeekdays) getWeekDay(year,month,day);
	p_setDate(year,month,day);
	return true;
}

bool br::com::sbVB::VBLib::VBDate::setDate(VBDateTime dateTime)
{
	return setDate(dateTime.m_tm.tm_year+1900,dateTime.m_tm.tm_mon,dateTime.m_tm.tm_mday);
}

bool br::com::sbVB::VBLib::VBDate::setDateYYYYMMDD(VBString date)
{
	int year, month, day;
	year = atoi(date.strInside(0,3));
	month = atoi(date.strInside(4,5));
	day = atoi(date.strInside(6,7));
	return setDate(year, month-1, day);
}


bool br::com::sbVB::VBLib::VBDate::deltaDays(int year, int month, int day, long & delta) const
{
	bool OK = validDate(year,month,day);
	if (!OK) return false;

	delta = 0;
	if (year == m_baseYear && month == m_baseMonth && day == m_baseDay) 
	{
		// given day is the same as base day
		return true;
	}

	if (isFuture(year,month,day)) 
	{
		// cout << "DEBUG given date is future compared to base date" << endl;
		if (year > m_baseYear) 
		{
			// cout << "DEBUG given year is not the same as base year" << endl;
			delta += remainingDaysofGivenYear(m_baseYear,m_baseMonth,m_baseDay);
			delta += numberOfDaysOfFullYearsFromNextYearToYearBeforeGivenYear(year);
			delta += numberOfDaysOfGivenYearUntilGivenDate(year,month,day);
		}
		else 
		{
			// cout << "DEBUG given year is the same as base year << endl;
			if (month > m_baseMonth) 
			{
				// cout << "DEBUG given month is not the same as base month" << endl;
				delta += remainingDaysOfGivenMonth(m_baseYear,m_baseMonth,m_baseDay);
				// full months from next month, before given month
				for (int m = m_baseMonth+1 ; m < month ; m++)
					delta += daysInAMonth(year,m);
				delta += day; // days in the given month

			}
			else 
			{
				// cout << "DEBUG given month is the same as base month" << endl;
				delta += day - m_baseDay;
			}
		}
	}
	else 
	{
		// to solve the problem for the past, one can only invert the problem
		// so that the problem becomes future (already solved)
		br::com::sbVB::VBLib::VBDate aux;
		aux.p_setDate(year,month,day); // here, setDate can not be used.
		// if used, an infinite re-enntry loop is generated.
		bool ret = aux.deltaDays(m_baseYear,m_baseMonth,m_baseDay,delta);
		delta *= -1;  // invert the delta to fix the inverted problem
		return ret;
	}
	return true;
}

long br::com::sbVB::VBLib::VBDate::deltaDays(const VBDate & cal) const
{
	long ret;
	deltaDays(cal.m_baseYear,cal.m_baseMonth,cal.m_baseDay,ret);
	return ret;
}


int br::com::sbVB::VBLib::VBDate::getWeekDay(const int year,const int month,const int day) const
{
	bool OK = validDate(year,month,day);
	if (!OK) return VBinvalid;

	long delta;
	deltaDays(year,month,day,delta);
	// cout << "DEBUG delta=" << delta << endl;

	// the code below is valid for both positive and negative delta
	int k = delta % 7; // k is positive or negative
	k += m_baseWeekDay + 7;  // plus 7 to guarantee k > 0
	return (k % 7);  // return 0 ~ 6
}

int br::com::sbVB::VBLib::VBDate::getWeekDay() const
{
	return getWeekDay(m_baseYear,m_baseMonth,m_baseDay);
}

br::com::sbVB::VBLib::VBDate br::com::sbVB::VBLib::VBDate::dateAfterDeltaDays(const long d) const
{
	long deltaDays = d;
	br::com::sbVB::VBLib::VBDate ret;
	ret.setDate(m_baseYear,m_baseMonth,m_baseDay);
	if (deltaDays > 0)
	{
		// go to future
		while (deltaDays != 0)
		{
			ret.tomorrow();
			deltaDays--;
		}
	}
	else
	{
		// go to past
		while (deltaDays != 0)
		{
			ret.yesterday();
			deltaDays++;
		}
	}
	return ret;
}


void br::com::sbVB::VBLib::VBDate::tomorrow()
{
	int daysThisMonth = daysInAMonth(m_baseYear,m_baseMonth);
	if (m_baseDay < daysThisMonth)
	{
		setDate(m_baseYear,m_baseMonth,m_baseDay+1);
		return;
	}
	if (m_baseMonth < VBdec)
	{
		// it it the last day of month, so tomorrow is the first day of next month
		setDate(m_baseYear,m_baseMonth+1,1);
		return;
	}
	// it is the last day of year, so tomorrow is the first day of next year
	setDate(m_baseYear+1,VBjan,1);
}


void br::com::sbVB::VBLib::VBDate::yesterday()
{
	if (m_baseDay > 1)
	{
		setDate(m_baseYear,m_baseMonth,m_baseDay-1);
		return;
	}

	if (m_baseMonth != VBjan)
	{
		// it is the first day of a month not January, so yesterday is the last day of previous month
		int daysLastMonth = daysInAMonth(m_baseYear,m_baseMonth-1);
		setDate(m_baseYear,m_baseMonth-1,daysLastMonth);
		return;
	}

	// it is the first day of year, so yesterday is the last day of previous year
	setDate(m_baseYear-1,VBdec,31);
}


br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBDate::getDate(int format) const
{
	const char *monthName[][12] = {
		{ // 0 - english	
			"January", "February", "March", "April", "May", "June", 
			"July", "August", "September", "October", "November", "December" 
		},
		{ // 1 - portuguese
			"Janeiro", "Fevereiro", "Mar�o", "Abril", "Maio", "Junho", 
			"Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro" 
		}
	};

	br::com::sbVB::VBLib::VBString ret;
	switch (format)
	{
	case 1: // Example: "November 24th, 2001"
		ret = monthName[0][this->m_baseMonth];
		ret += " ";
		ret += this->m_baseDay;
		switch (this->m_baseDay)
		{
		case 1:
		case 21:
		case 31:
			ret += "st";
			break;
		case 2:
		case 22:
			ret += "nd";
			break;
		case 3:
		case 23:
			ret += "rd";
			break;
		default:
			ret += "th";
		}
		ret += ", ";
		ret += this->m_baseYear;
		break;
	case 2: // Example: yyyy-mm-dd, that is, "2001-11-24"
		ret = this->m_baseYear;
		ret += "-";
		ret += twoDigits(this->m_baseMonth + 1);
		ret += "-";
		ret += twoDigits(this->m_baseDay);
		break;
	case 3: // Example: "24 de Novembro de 2001"
		ret = this->m_baseDay;
		ret += " de ";
		ret += monthName[1][this->m_baseMonth];
		ret += " de ";
		ret += this->m_baseYear;
		break;
	case 4: // Example: yyyy-mm-dd, that is, "20011124"
		ret = this->m_baseYear;
		ret += twoDigits(this->m_baseMonth + 1);
		ret += twoDigits(this->m_baseDay);
		break;
	default:
		ret = "";
	}

	return ret;

}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBDate::twoDigits(int i) const
{
	VBString ret;
	if (i < 10)
		ret = "0";
	ret += i;
	return ret;
}

/////////////////////////////////////////////////////////////////
//
// class VBDateTime
//
/////////////////////////////////////////////////////////////////

const char *br::com::sbVB::VBLib::VBDateTime::monthName[] = {	
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
};

	
// default constructor
br::com::sbVB::VBLib::VBDateTime::VBDateTime()
{
}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBDateTime::getTimeStr(unsigned format) const
{
	br::com::sbVB::VBLib::VBString ret;
	ret += " ";
	ret += twoDigits(m_tm.tm_hour);
	ret += ":";
	ret += twoDigits(m_tm.tm_min);
	if (format>1)
	{
		ret += ":";
		ret += twoDigits(m_tm.tm_sec);
		if (format>2)
		{
			ret += ",";
			ret += m_miliseconds;
		}
	}
	return ret;
}

// Example: 31-Mar-2002 23:59:59 GMT
br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBDateTime::getDateStr(int dateFormat) const
{
	br::com::sbVB::VBLib::VBString ret;
	br::com::sbVB::VBLib::VBDate date = *this; // get VBDate object from this
	ret = date.getDate(dateFormat);
	ret += " " + getTimeStr();
//	ret += " GMT";
	return ret;
}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBDateTime::twoDigits(int i) const
{
	br::com::sbVB::VBLib::VBString ret;
	if (i < 10)
		ret = "0";
	ret += i;
	return ret;
}

// add or subtract time to this VBDate
void br::com::sbVB::VBLib::VBDateTime::addYear(int year)
{
	m_tm.tm_year += year;

	VBDate cal_from;
	if (!cal_from.setDate(m_tm.tm_year+1900+year,m_tm.tm_mon,m_tm.tm_mday))
	{
		// invalid date. This can only be caused by leap year
		m_tm.tm_mon = 3; // march
		m_tm.tm_mday = 1;
	}
}

void br::com::sbVB::VBLib::VBDateTime::setDate(int year, unsigned month, unsigned day)
{
	m_tm.tm_year = year-1900; // input year is normal year. Stored year is tm style (-1900)
	m_tm.tm_mon = month; // months since January - [0,11] 
	m_tm.tm_mday = day;	 // day of the month - [1,31] 



// 	getWeekDay
//	int tm_wday;    // days since Sunday - [0,6] 
//	int tm_yday;    // days since January 1 - [0,365] 

}

void br::com::sbVB::VBLib::VBDateTime::setTime(unsigned hour, 
	unsigned minute, unsigned second, unsigned miliseconds)
{
	m_tm.tm_hour = hour;  // hours since midnight - [0,23] 
	m_tm.tm_min = minute; // minutes after the hour - [0,59] 
	m_tm.tm_sec = second; // seconds after the minute - [0,59] 
	m_miliseconds = miliseconds;
}

double br::com::sbVB::VBLib::VBDateTime::getDifSeconds(const VBDateTime & dt) const
{
	double ret = 0;
	double aux = (double)(m_miliseconds)/1000.0;
	ret += aux;
	aux = (double)(dt.m_miliseconds)/1000.0;
	ret -= aux;
	ret += m_tm.tm_sec - dt.m_tm.tm_sec;
	ret += m_tm.tm_min*60 - dt.m_tm.tm_min*60;
	ret += m_tm.tm_hour*60*60 - dt.m_tm.tm_hour*60*60;

	// year, month, day not considered yet

	return ret;
}



void br::com::sbVB::VBLib::VBDateTime::addMonth(int month)
{
#if 0
	time_t secondsToAdd = 30 * 24 * 60 * 60; // seconds of 1 month
	secondsToAdd *= month;
	time_t now = mktime(&m_tm);
	time_t newDateTime = now + secondsToAdd;
	setThisTM(newDateTime);
#endif

	m_tm.tm_mon += month;

	br::com::sbVB::VBLib::VBDate cal_from;
	if (!cal_from.setDate(m_tm.tm_year+1900,m_tm.tm_mon+month,m_tm.tm_mday))
	{
		// invalid date. This can only be caused by leap year
		m_tm.tm_mon = 3; // march
		m_tm.tm_mday = 1;
	}
}

void br::com::sbVB::VBLib::VBDateTime::addDay(int day)
{
	time_t secondsToAdd = 24 * 60 * 60; // seconds of 1 day
	secondsToAdd *= day;
	time_t now = mktime(&m_tm);
	time_t newDateTime = now + secondsToAdd;
	setThisTM(newDateTime);
}

void br::com::sbVB::VBLib::VBDateTime::addHour(int hour)
{
	time_t secondsToAdd = 60 * 60; // seconds of 1 hour
	secondsToAdd *= hour;
	time_t now = mktime(&m_tm);
	time_t newDateTime = now + secondsToAdd;
	setThisTM(newDateTime);
}

void br::com::sbVB::VBLib::VBDateTime::addMinute(int minute)
{
	time_t secondsToAdd = 60; // seconds of 1 minute
	secondsToAdd *= minute;
	time_t now = mktime(&m_tm);
	time_t newDateTime = now + secondsToAdd;
	setThisTM(newDateTime);
}

void br::com::sbVB::VBLib::VBDateTime::addSecond(int second)
{
	time_t secondsToAdd = second;
	time_t now = mktime(&m_tm);
	time_t newDateTime = now + secondsToAdd;
	setThisTM(newDateTime);
}

void br::com::sbVB::VBLib::VBDateTime::setThisTM(time_t ltime)
{
	tm *p_tm = localtime( &ltime ); // convert to struct tm
	// copy each attribute
	m_tm.tm_sec  = p_tm->tm_sec;    // seconds after the minute - [0,59] 
	m_tm.tm_min  = p_tm->tm_min;    // minutes after the hour - [0,59] 
	m_tm.tm_hour = p_tm->tm_hour;   // hours since midnight - [0,23] 
	m_tm.tm_mday = p_tm->tm_mday;   // day of the month - [1,31] 
	m_tm.tm_mon  = p_tm->tm_mon;    // months since January - [0,11] 
	m_tm.tm_year = p_tm->tm_year;   // years since 1900 
	m_tm.tm_wday = p_tm->tm_wday;   // days since Sunday - [0,6] 
	m_tm.tm_yday = p_tm->tm_yday;   // days since January 1 - [0,365] 
	m_tm.tm_isdst = p_tm->tm_isdst; // daylight savings time flag 
}


void br::com::sbVB::VBLib::VBDateTime::setNow()
{
	time_t ltime; 
	time( &ltime ); // get number of seconds elapsed since 00:00:00 on January 1, 1970, 
	setThisTM(ltime);

#ifdef WIN32	
	// Windows code
//	time_t tz_sec; // Difference in seconds between Greenwitch time and local time
//	tz_sec = timezone; 

	m_miliseconds = 0;
#else
	// Unix code
	struct timeval tv;
	struct timezone tz;
	gettimeofday( &tv, &tz );
	m_miliseconds = tv.tv_usec/1000;

#endif
	
	//	m_timeRelativeToGreenwitch = tz_sec / 60 / 60; // Difference in hours
	// cout << "DEBUG: " << m_timeRelativeToGreenwitch  << endl; // Brazil is -3
}

br::com::sbVB::VBLib::VBDateTime::operator br::com::sbVB::VBLib::VBDate () const
{
	br::com::sbVB::VBLib::VBDate ret;
	ret.setDate(m_tm.tm_year+1900,m_tm.tm_mon,m_tm.tm_mday);
	return ret;
}

// return date-time as string
::br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBDateTime::getDateTimeStr() const
{
	VBString tok = "|";
	VBString ret;
	ret += getYear();
	ret += tok;
	ret += getMonth();
	ret += tok;
	ret += getDay();
	ret += tok;
	ret += getHour();
	ret += tok;
	ret += getMinute();
	ret += tok;
	ret += getSecond();
	ret += tok;
	ret += getMiliSecond();

	return ret;
}

// set this based in argument, compatible with getDateTimeStr
void br::com::sbVB::VBLib::VBDateTime::setDateTimeStr(const char *setTimeStr)
{
	char tok = '|';
	VBString st = setTimeStr;
	int year = atoi(st.strtok(tok,0));
	unsigned month = atoi(st.strtok(tok,1));
	unsigned day = atoi(st.strtok(tok,2));
	unsigned hour = atoi(st.strtok(tok,3));
	unsigned minute = atoi(st.strtok(tok,4));
	unsigned second = atoi(st.strtok(tok,5));
	unsigned miliseconds = atoi(st.strtok(tok,6));

	setDate(year,month,day);
	setTime(hour,minute,second,miliseconds);
}

// return true if this is less than argument
bool br::com::sbVB::VBLib::VBDateTime::operator<(const VBDateTime & dt) const
{
	if (getYear() > dt.getYear())
		return false;
	if(getYear() < dt.getYear())
		return true;
	if (getMonth() > dt.getMonth())
		return false;
	if (getMonth() < dt.getMonth())
		return true;
	if (getDay() > dt.getDay())
		return false;
	if (getDay() < dt.getDay())
		return true;
	if (getHour() > dt.getHour())
		return false;
	if (getHour() < dt.getHour())
		return true;
	if (getMinute() > dt.getMinute())
		return false;
	if (getMinute() < dt.getMinute())
		return true;
	if (getSecond() > dt.getSecond())
		return false;
	if (getSecond() < dt.getSecond())
		return true;
	if (getMiliSecond() > dt.getMiliSecond())
		return false;
	if (getMiliSecond() < dt.getMiliSecond())
		return true;

	return false;
}

// return true if this is greater than argument
bool br::com::sbVB::VBLib::VBDateTime::operator>(const VBDateTime & dt) const
{
	return dt < *this;
}

bool br::com::sbVB::VBLib::VBDateTime::operator==(const VBDateTime & dt) const
{
	if (getYear() != dt.getYear()) return false;
	if (getMonth() != dt.getMonth()) return false;
	if (getDay() != dt.getDay()) return false;
	if (getHour() != dt.getHour()) return false;
	if (getMinute() != dt.getMinute()) return false;
	if (getSecond() != dt.getSecond()) return false;
	if (getMiliSecond() != dt.getMiliSecond()) return false;

	return true;
}

int br::com::sbVB::VBLib::VBDateTime::getYear() const
{
	return m_tm.tm_year+1900;
}

unsigned br::com::sbVB::VBLib::VBDateTime::getMonth() const
{
	return m_tm.tm_mon;
}

unsigned br::com::sbVB::VBLib::VBDateTime::getDay() const
{
	return m_tm.tm_mday;
}

unsigned br::com::sbVB::VBLib::VBDateTime::getHour() const
{
	return m_tm.tm_hour;
}

unsigned br::com::sbVB::VBLib::VBDateTime::getMinute() const
{
	return m_tm.tm_min;
}

unsigned br::com::sbVB::VBLib::VBDateTime::getSecond() const
{
	return m_tm.tm_sec;
}

unsigned br::com::sbVB::VBLib::VBDateTime::getMiliSecond() const
{
	return m_miliseconds;
}


unsigned long br::com::sbVB::VBLib::VBDateTime::getSecondsToJulianDate() const
{
	unsigned long ret = 0;
	VBDate julianDate, thisDate;
	julianDate.setDate(1970,1,1);
	thisDate.setDate(getYear(),getMonth(),getDay());
	unsigned deltaDays = thisDate.deltaDays(julianDate);

	// add years
	ret += deltaDays * 24 * 60 * 60;
	ret += (getHour()) * 60 * 60;
	ret += (getMinute()) * 60;
	ret += (getSecond());

	return ret;
}
