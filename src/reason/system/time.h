
/**
 * Reason
 * Copyright (C) 2009  Emerson Clarke
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/platform/platform.h"
#include "reason/system/timer.h"
#include "reason/system/string.h"
#include "reason/system/format.h"
#include "reason/system/parser.h"

#include <sys/timeb.h>
#include <time.h>

#pragma warning(disable:4996)

#ifdef REASON_PLATFORM_WINDOWS
#include "winsock2.h"

#endif

#ifdef REASON_PLATFORM_POSIX
#include <stdio.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Calendar;
class Interval;
class Elapsed;

class Times : public Timers
{
public:

	static bool IsLeapYear(int year)
	{

		if (year == 0) return false;

		if ((year % 4) != 0) return false;
		return ((year % 100) == 0)?((year % 400) == 0):true;
	}

	static int DaysInYear(int year)
	{
		return IsLeapYear(year)?366:365;
	}

	static int DaysInMonth(int month, int year=0)
	{
		switch (month)
		{
		case 1:case 3: case 5: case 7: case 8: case 10: case 12:
			return 31;
		case 2:
			return IsLeapYear(year)?29:28;
		default:
			return 30;
		}
	}

	static time_t Mktime(struct tm * lt)
	{
		return mktime(lt);
	}

	static char * Asctime(const struct tm * lt, char * data)
	{

		char * result = 0;
		data = 0;

		#ifdef REASON_PLATFORM_POSIX
		result = asctime_r(lt,data);
		#endif
		#ifdef REASON_PLATFORM_WINDOWS
		result = asctime(lt);
		if (result)
			strcpy(data,result);
		#endif

		return result;
	}

	static char * Ctime(const time_t * time, char * data)
	{

		char * result = 0;
		data = 0;

		#ifdef REASON_PLATFORM_POSIX
		result = ctime_r(time,data);
		#endif
		#ifdef REASON_PLATFORM_WINDOWS
		result = ctime(time);
		if (result)
			strcpy(data,result);
		#endif

		return result;
	}

	static struct tm * Localtime(const time_t * time, struct tm * local)
	{

		struct tm * result = 0;

		#ifdef REASON_PLATFORM_POSIX
		result = localtime_r(time,local);	
		#endif
		#ifdef REASON_PLATFORM_WINDOWS
		result = localtime(time);
		if (result)
			*local = *result;
		#endif

		return result;
	}

	static struct tm * Gmtime(const time_t * time, struct tm * local)
	{

		struct tm * result = 0;

		#ifdef REASON_PLATFORM_POSIX
		result = gmtime_r(time,local);	
		#endif
		#ifdef REASON_PLATFORM_WINDOWS
		result = gmtime(time);
		if (result)
			*local = *result;
		#endif

		return result;
	}

};

class Interval;
class Elapsed;

class Time: public Times, public Printable
{
public:

	time_t	Seconds;
	unsigned short Milliseconds;

	Time(timeb time) {*this=time;}
	Time(time_t seconds):Seconds(seconds),Milliseconds(0) {}
	Time(time_t seconds, unsigned short milliseconds):Seconds(seconds),Milliseconds(milliseconds) {}
	Time(const char * data) {Construct(data);}
	Time(const Sequence & sequence) {Construct(sequence);}
	Time(char * data, int size) {Construct(data,size);}
	Time(const Calendar & calendar) {Construct(calendar);}
	Time(const Time & time):Seconds(time.Seconds),Milliseconds(time.Milliseconds) {}
	Time() {Construct();}

	bool Construct();
	bool Construct(time_t seconds) {Seconds = seconds;Milliseconds = 0;return true;}
	bool Construct(timeb time) {Seconds = time.time;Milliseconds=time.millitm;return true;}
	bool Construct(const char * data) {return Construct((char*)data,String::Length(data));}
	bool Construct(const Sequence & sequence) {return Construct(sequence.Data,sequence.Size);}
	bool Construct(char * data, int size);
	bool Construct(const Calendar & calendar);
	bool Construct(const Time & time) {Seconds=time.Seconds;Milliseconds=time.Milliseconds;return true;}

	static Time Now() {return Time();}

	int Hour()
	{
		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_hour:0;
	}

	int Minute()
	{
		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_min:0;
	}

	int Second()
	{
		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_sec:0;
	}

	int Millisecond()
	{
		return Milliseconds;
	}

	int Weekday()
	{

		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_wday:0;
	}

	static String Weekday(Time & time);

	int Day()
	{

		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_mday:0;
	}

	static String Day(Time & time)
	{

		struct tm local;
		return (Times::Gmtime((time_t*)&time,&local))?String(local.tm_mday):0;
	}

	int Month()
	{

		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_mon+1:0;
	}

	static String Month(Time & time);

	int Year()
	{

		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_year+1900:0;
	}

	static String Year(Time & time)
	{

		struct tm local;
		return (Times::Gmtime((time_t*)&time,&local))?String(local.tm_year+1900):0;
	}

	using Times::IsLeapYear;
	bool IsLeapYear() 
	{
		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?IsLeapYear(local.tm_year+1900):false;
	}

	using Times::DaysInYear;
	int DaysInYear()
	{
		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?DaysInYear(local.tm_year+1900):0;
	}

	using Times::DaysInMonth;
	int DaysInMonth()
	{
		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?DaysInMonth(local.tm_mon+1):0;
	}

	using Printable::Print;
	void Print(String & string)
	{

		Formatter::Append(string,"%s %s %02d %04d",Weekday(*this).Slice(0,3).Print(), Month(*this).Slice(0,3).Print(),this->Day(),this->Year()); 
		string << " ";
		Formatter::Append(string,"%02d:%02d:%02d %s",Hour()%12,Minute(),Second(),((Hour()%12)==Hour())?"AM":"PM");
	}	

	Time & operator = (time_t seconds) {Construct(seconds);return *this;}
	Time & operator = (timeb time) {Construct(time);return *this;}
	Time & operator = (const char * data) {Construct(data);return *this;}
	Time & operator = (const Sequence & sequence) {Construct(sequence);return *this;}
	Time & operator = (const Calendar & calendar) {Construct(calendar);return *this;}
	Time & operator = (const Time & time) {Construct(time);return *this;}

	bool operator == (const Time &time) const 
	{
		return Seconds == time.Seconds && Milliseconds == time.Milliseconds;
	}

	bool operator != (const Time &time) const
	{
		return Seconds != time.Seconds || Milliseconds != time.Milliseconds;
	}

	bool operator < (const Time & time) const
	{
		return(Seconds==time.Seconds)?Milliseconds<time.Milliseconds:Seconds<time.Seconds;
	}

	bool operator <= (const Time & time) const
	{
		return(Seconds==time.Seconds)?Milliseconds<=time.Milliseconds:Seconds<=time.Seconds;
	}

	bool operator > (const Time & time) const
	{
		return(Seconds==time.Seconds)?Milliseconds>time.Milliseconds:Seconds>time.Seconds;
	}

	bool operator >= (const Time & time) const
	{
		return(Seconds==time.Seconds)?Milliseconds>=time.Milliseconds:Seconds>=time.Seconds;
	}

	Time & operator += (int seconds)
	{
		Seconds += seconds;
		return *this;
	}

	Time & operator += (double seconds)
	{
		Seconds += (int)seconds;
		Milliseconds += ((seconds-(int)seconds)*Time::MillisecondsPerSecond);
		return *this;
	}

	Time & operator += (const Interval & interval);

	Time operator + (Interval & interval);

	Time operator + (int seconds)
	{
		return Time(Seconds+seconds,Milliseconds);
	}

	Time operator + (double seconds)
	{
		return Time(Seconds+(int)seconds, Milliseconds + ((seconds-(int)seconds)*Time::MillisecondsPerSecond));
	}

	Time & operator -= (int seconds)
	{
		Seconds -= seconds;
		return *this;
	}

	Time & operator -= (double seconds)
	{
		Seconds -= (int)seconds;
		Milliseconds -= ((seconds-(int)seconds)*Time::MillisecondsPerSecond);
		return *this;
	}

	Time & operator -= (const Interval & interval);

	Time operator - (Interval & interval);

	Time operator - (int seconds)
	{
		return Time(Seconds-seconds,Milliseconds);
	}

	Time operator - (double seconds)
	{
		return Time(Seconds-(int)seconds,Milliseconds-((seconds-(int)seconds)*Time::MillisecondsPerSecond));
	}

	Interval operator - (const Time & time);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TimeHandler : public Handler
{
public:

	virtual void ProcessHour(int hour)=0;
	virtual void ProcessMinute(int minute)=0;
	virtual void ProcessSecond(int second)=0;

	virtual void ProcessDay(int day)=0;
	virtual void ProcessWeekday(int weekday)=0;
	virtual void ProcessMonth(int month)=0;
	virtual void ProcessYear(int year)=0;
};

class TimeParser : public StringParser
{
public:

	int Constant;
	TimeHandler * Handled;

	TimeParser():Constant(0),Handled(0){}

	bool Parse();
	bool ParseClock();
	bool ParseClockLiteral();
	bool ParseDate();
	bool ParseDateLiteral();

	bool ParseDay();
	bool ParseDayLiteral();

	bool ParseWeekday();
	bool ParseWeekdayLiteral();

	bool ParseMonth();
	bool ParseMonthLiteral();

	bool ParseYear();	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Timezone
{
public:
	int Offset;				
	int DaylightSavings;	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Clock: public Time, public Timezone
{
public:

	Clock & operator = (Time & time) {Time::operator = (time);return *this;}
	Clock & operator = (time_t seconds) {Time::operator = (seconds);return *this;}
	Clock & operator = (timeb time) {Time::operator = (time);return *this;}

	void Print(String & string)
	{	
		Formatter::Append(string,"%02d:%02d:%02d %s",Hour(),Minute(),Second(),((Hour()%12)==Hour())?"AM":"PM");
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Calendar: public Printable
{
public:

	int Day;
	int Month;
	int Year;

	int Hour;
	int Minute;
	int Second;

	Calendar(const Sequence & sequence):Day(0),Month(0),Year(0),Hour(0),Minute(0),Second(0) {Construct(sequence.Data,sequence.Size);}
    Calendar(const char * data):Day(0),Month(0),Year(0),Hour(0),Minute(0),Second(0) {Construct(data);}
	Calendar(char * data, int size):Day(0),Month(0),Year(0),Hour(0),Minute(0),Second(0) {Construct(data,size);}
	Calendar(const struct tm & local):Day(0),Month(0),Year(0),Hour(0),Minute(0),Second(0) {Construct(local);}
	Calendar(const time_t & seconds):Day(0),Month(0),Year(0),Hour(0),Minute(0),Second(0) {Construct(seconds);}
	Calendar(const Time & time):Day(0),Month(0),Year(0),Hour(0),Minute(0),Second(0) {Construct(time);}
	Calendar(const Calendar & calendar):Day(calendar.Day),Month(calendar.Month),Year(calendar.Year),Hour(calendar.Hour),Minute(calendar.Minute),Second(calendar.Second) {}
	Calendar(int day, int month, int year, int hour=0, int minute=0, int second=0):Day(day),Month(month),Year(year),Hour(hour),Minute(minute),Second(second) {}
	Calendar():Day(0),Month(0),Year(0),Hour(0),Minute(0),Second(0) {}

	bool Construct(const Sequence & sequence) {return Construct(sequence.Data,sequence.Size);}
	bool Construct(const char * data) {return Construct((char*)data,String::Length(data));}
	bool Construct(char * data, int size);
	bool Construct(const struct tm & local);
	bool Construct(const time_t & seconds);
	bool Construct(const Time & time);
	bool Construct(const Calendar & calendar);

	String Format(const char * format)
	{
		String string;
		Format(string,format);
		return string;
	}

	void Format(String & string, const char * format);

	using Printable::Print;
	void Print(class String & string);

	bool IsEmpty() {return !Day && !Month && !Year && !Hour && !Minute && !Second;}

	Calendar & operator = (const Sequence & sequence) {Construct(sequence);return *this;}
	Calendar & operator = (const char * data) {Construct(data);return *this;}
	Calendar & operator = (const struct tm & local) {Construct(local);return *this;}
	Calendar & operator = (const time_t & seconds) {Construct(seconds);return *this;}
	Calendar & operator = (const Time & time) {Construct(time);return *this;}
	Calendar & operator = (const Calendar & calendar) {Construct(calendar);return *this;}

	Calendar & operator -= (const Interval & interval) 
	{
		Time time(*this);
		time -= interval;
		Construct(time);
		return *this;
	}

	Calendar & operator -= (int seconds)
	{
		Time time(*this);
		time -= seconds;
		Construct(time);
		return *this;
	}

	Calendar & operator -= (double seconds)
	{
		Time time(*this);
		time -= seconds;
		Construct(time);
		return *this;
	}

	Calendar & operator += (const Interval & interval) 
	{
		Time time(*this);
		time += interval;
		Construct(time);
		return *this;
	}

	Calendar & operator += (int seconds)
	{
		Time time(*this);
		time += seconds;
		Construct(time);
		return *this;
	}

	Calendar & operator += (double seconds)
	{
		Time time(*this);
		time += seconds;
		Construct(time);
		return *this;
	}

	bool operator == (const Calendar & calendar)
	{

		return (long long)(*this) == (long long)calendar;
	}

	bool operator < (const Calendar & calendar)
	{
		return (long long)(*this) < (long long)calendar;
	}

	bool operator <= (const Calendar & calendar)
	{
		return (long long)(*this) <= (long long)calendar;
	}

	bool operator > (const Calendar & calendar)
	{
		return (long long)(*this) > (long long)calendar;
	}

	bool operator >= (const Calendar & calendar)
	{
		return (long long)(*this) >= (long long)calendar;
	}

	double Years(const Calendar & calendar);
	double Months(const Calendar & calendar);
	double Weeks(const Calendar & calendar);
	double Days(const Calendar & calendar);
	double Hours(const Calendar & calendar);
	double Minutes(const Calendar & calendar);

	double Seconds(const Calendar & calendar)
	{
		return (*this)-calendar;
	}

	long long operator - (const Calendar & calendar);

	operator long long ()
	{

		return ((const Calendar&)(*this)).operator long long();
	}

	operator long long () const
	{

		unsigned long long number = Year*10000;
		number = number*100 + Month;
		number = number*100 + Day;
		number = number*100 + Hour;
		number = number*100 + Minute;
		number = number*100 + Second;
		return number;
	}

protected:

	double CalendarMonths()
	{

		double months=Month-1;
		months += (double)(Day-1) / Time::DaysInMonth(Month,Year);
		months += (double)((Hour*Time::SecondsPerHour) + (Minute*Time::SecondsPerMinute) + Second) / (Time::DaysInMonth(Month,Year) * Time::SecondsPerDay);
		return months;
	}

	double CalendarMonthsRemaining()
	{
		return Time::MonthsPerYear - CalendarMonths();
	}

	double CalendarWeeks()
	{
		return CalendarSeconds()/Time::SecondsPerWeek;
	}

	double CalendarWeeksRemaining()
	{
		return CalendarSecondsRemaining()/Time::SecondsPerWeek;
	}

	double CalendarDays()
	{
		return CalendarSeconds()/Time::SecondsPerDay;
	}

	double CalendarDaysRemaining()
	{
		return CalendarSecondsRemaining()/Time::SecondsPerDay;
	}

	double CalendarHours()
	{
		return CalendarSeconds()/Time::SecondsPerHour;
	}

	double CalendarHoursRemaining()
	{
		return CalendarSecondsRemaining()/Time::SecondsPerHour;
	}

	double CalendarMinutes()
	{
		return CalendarSeconds()/Time::SecondsPerMinute;
	}

	double CalendarMinutesRemaining()
	{
		return CalendarSecondsRemaining()/Time::SecondsPerMinute;
	}

	double CalendarSeconds()
	{

		double seconds=0;		
		for (int month=1;month<Month;++month)
			seconds += Time::DaysInMonth(month,Year) * Time::SecondsPerDay;

		seconds += (Day-1) * Time::SecondsPerDay;
		seconds += (Hour*Time::SecondsPerHour) + (Minute*Time::SecondsPerMinute) + Second;
		return seconds;
	}

	double CalendarSecondsRemaining()
	{
		return Time::DaysInYear(Year) * Time::SecondsPerDay - CalendarSeconds();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CalendarReader: public TimeParser, public TimeHandler
{	
public:

	class Calendar * Calendar;

	CalendarReader(class Calendar * calendar);

	bool Parse();

	void ProcessHour(int hour);
	void ProcessMinute(int minute);
	void ProcessSecond(int second);

	void ProcessDay(int day);
	void ProcessWeekday(int weekday);
	void ProcessMonth(int month);
	void ProcessYear(int year);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Epoch : public Calendar
{
public:

	long long Ticks;
	int Scale;

	Epoch(time_t time):
		Calendar(1,1,1970), Ticks(time),Scale(1) 
	{
	}

	Epoch(const Time & time):
		Calendar(1,1,1970),Ticks(time.Seconds),Scale(1)
	{
	}

	Epoch(const Calendar & calendar, long long ticks, int scale):
		Calendar(calendar),Ticks(ticks),Scale((scale)>0?scale:1)
	{
	}

	Epoch(const char * data, long long ticks, int scale):
		Calendar(data),Ticks(ticks),Scale((scale)>0?scale:1)
	{
	}

	Epoch(const Sequence & sequence, long long ticks, int scale):
		Calendar(sequence),Ticks(ticks),Scale((scale)>0?scale:1)
	{
	}

	Epoch(char * data, int size, long long ticks, int scale):
		Calendar(data,size),Ticks(ticks),Scale((scale)>0?scale:1)
	{
	}

	Epoch(const Calendar & calendar):
		Calendar(calendar),Ticks(0),Scale(1)
	{
	}

	Epoch(const char * data):
		Calendar(data),Ticks(0),Scale(1)
	{
	}

	Epoch(const Sequence & sequence):
		Calendar(sequence),Ticks(0),Scale(1)
	{
	}

	Epoch(char * data, int size):
		Calendar(data,size),Ticks(0),Scale(1)
	{
	}

	double Years();
	double Years(const Epoch & epoch);

	double Months();
	double Months(const Epoch & epoch);

	double Weeks() {return (double)(Ticks/Scale)/Time::SecondsPerWeek;}
	double Weeks(const Epoch & epoch) {return Seconds(epoch)/Time::SecondsPerWeek;}

	double Days() {return (double)(Ticks/Scale)/Time::SecondsPerDay;}
	double Days(const Epoch & epoch) {return Seconds(epoch)/Time::SecondsPerDay;}

	double Hours() {return (double)(Ticks/Scale)/Time::SecondsPerHour;}
	double Hours(const Epoch & epoch) {return Seconds(epoch)/Time::SecondsPerHour;}

	double Minutes() {return (double)(Ticks/Scale)/Time::SecondsPerMinute;}
	double Minutes(const Epoch & epoch) {return Seconds(epoch)/Time::SecondsPerMinute;}

	double Milliseconds() {return Ticks/(Scale/Time::MillisecondsPerSecond);}
	double Milliseconds(const Epoch & epoch) {return Seconds(epoch)*Time::MillisecondsPerSecond;}

	double Microseconds() {return Ticks/(Scale/Time::MicrosecondsPerSecond);}
	double Microseconds(const Epoch & epoch) {return Seconds(epoch)*Time::MicrosecondsPerSecond;}

	double Nanoseconds() {return Ticks/(Scale/Time::NanosecondsPerSecond);}
	double Nanoseconds(const Epoch & epoch) {return Seconds(epoch)*Time::NanosecondsPerSecond;}

	double Seconds() {return Ticks/Scale;}
	double Seconds(const Epoch & epoch);

	double operator - (Epoch & epoch) {return Seconds(epoch);}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Interval
{
public:

	int Seconds;
	int Microseconds;

	Interval():Seconds(0),Microseconds(0){}
	Interval(int seconds):Seconds(seconds),Microseconds(0){}
	Interval(double interval):Seconds(interval),Microseconds((interval - (int)interval) * Clock::MicrosecondsPerSecond){}
	Interval(const char * data) {Construct(data);}
	Interval(const Sequence & sequence) {Construct(sequence);}
	Interval(char * data, int size) {Construct(data,size);}

	bool Construct(int seconds) {Seconds=seconds;Microseconds=0;return true;}
	bool Construct(double interval) {Seconds=(int)interval;Microseconds=(interval-(int)interval)*Clock::MicrosecondsPerSecond;return true;}
	bool Construct(const char * data) {return Construct((char*)data,String::Length(data));}
	bool Construct(const Sequence & sequence) {return Construct(sequence.Data,sequence.Size);}
	bool Construct(char * data, int size);

	Interval & operator = (int seconds) {Construct(seconds);return *this;}
	Interval & operator = (double interval) {Construct(interval);return *this;}
	Interval & operator = (const char * data) {Construct(data);return *this;}
	Interval & operator = (const Sequence & sequence) {Construct(sequence);return *this;}

	bool operator < (int seconds) const
	{
		return Seconds < seconds;
	}

	bool operator <= (int seconds) const
	{
		return Seconds <= seconds;
	}

	bool operator > (int seconds) const
	{
		return Seconds > seconds;
	}

	bool operator >= (int seconds) const
	{
		return Seconds >= seconds;
	}

	Interval & operator += (int seconds)
	{
		Seconds += seconds;
		return *this;
	}

	Interval & operator += (double seconds)
	{
		Seconds += (int)seconds;
		Microseconds -= (seconds - (int)seconds) * Clock::MicrosecondsPerSecond;
		return *this;
	}

	Interval & operator -= (int seconds)
	{
		Seconds -= seconds;
		return *this;
	}

	Interval & operator -= (double seconds)
	{
		Seconds -= (int)seconds;
		Microseconds -= (seconds - (int)seconds) * Clock::MicrosecondsPerSecond;
		return *this;
	}

	double operator + (Interval & interval)
	{
		Interval result;
		result.Microseconds = this->Microseconds + interval.Microseconds;
		result.Seconds = this->Seconds + interval.Seconds;
		return (double)result;
	}

	double operator + (int seconds)
	{
		return ((double)*this) + seconds;
	}

	double operator + (double seconds)
	{
		return ((double)*this) + seconds;

	}

	friend double operator + (int left, Interval & right)
	{
		return (double)left + (double)right;
	}

	friend double operator + (double left, Interval & right)
	{
		return left + (double)right;
	}

	double operator - (Interval & interval)
	{
		Interval result;
		result.Microseconds = this->Microseconds - interval.Microseconds;
		result.Seconds = this->Seconds - interval.Seconds;
		return (double)result;
	}

	double operator - (int seconds)
	{
		return ((double)*this) - seconds;
	}

	double operator - (double seconds)
	{
		return ((double)*this) - seconds;
	}

	friend double operator - (int left, Interval & right)
	{
		return (double)left - (double)right;
	}

	friend double operator - (double left, Interval & right)
	{
		return left - (double)right;
	}

	operator int()
	{
		return Seconds;
	}

	operator double()
	{
		return ((double)Seconds) + ((double)Microseconds / Clock::MicrosecondsPerSecond);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IntervalReader: public TimeParser, public TimeHandler
{
public:

	class Interval * Interval;

	IntervalReader(class Interval * interval):Interval(interval){}

	bool Parse();

	void ProcessHour(int hour);
	void ProcessMinute(int minute);
	void ProcessSecond(int second);

	void ProcessDay(int day);
	void ProcessWeekday(int weekday);
	void ProcessMonth(int month);
	void ProcessYear(int year);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Elapsed
{
public:

	Reason::System::Interval Interval;

	Elapsed()
	{
		Construct();
	}

	Elapsed(class Interval & interval):Interval(interval){}

	Elapsed(const Time & left, const Time & right)
	{
		if (left != right) Construct(left,right);
	}

	bool Construct();
	bool Construct(const Time & left, const Time & right)
	{
		if (left == right)
		{
			Interval = 0;
		}
		else
		if (left < right)
		{
			Interval.Seconds = right.Seconds-left.Seconds;
			Interval.Microseconds = (right.Milliseconds-left.Milliseconds)*Clock::MicrosecondsPerMillisecond;
		}
		else
		if (left > right)
		{
			Interval.Seconds = left.Seconds-right.Seconds;
			Interval.Microseconds = (left.Milliseconds-right.Milliseconds)*Clock::MicrosecondsPerMillisecond;
		}
		return true;
	}

	int Seconds() {return Interval;}	
	int Milliseconds()	{return Interval.Microseconds/Time::MicrosecondsPerMillisecond;}
	int Microseconds() {return Interval.Microseconds;}
	int Nanoseconds() {return Interval.Microseconds*Time::NanosecondsPerMillisecond;}

	double Weeks() {return (double)Interval / Clock::SecondsPerWeek;}
	double Days() {return (double)Interval / Clock::SecondsPerDay;}
	double Hours() {return (double)Interval / Clock::SecondsPerHour;}
	double Minutes() {return (double)Interval / Clock::SecondsPerMinute;}

	static double Weeks(const Time & left, const Time & right)
	{
		Elapsed elapsed(left,right);
		return (double)elapsed.Interval / Time::SecondsPerWeek;
	}

	static double Weeks(Reason::System::Interval & interval)
	{
		return (double)interval / Time::SecondsPerWeek;
	}

	static double Days(const Time & left, const Time & right)
	{
		Elapsed elapsed(left,right);
		return (double)elapsed.Interval / Time::SecondsPerDay;
	}

	static double Days(Reason::System::Interval & interval)
	{
		return (double)interval / Time::SecondsPerDay;
	}

	static double Hours(const Time & left, const Time & right)
	{
		Elapsed elapsed(left,right);
		return (double)elapsed.Interval / Time::SecondsPerHour;
	}

	static double Hours(Reason::System::Interval & interval)
	{
		return (double)interval / Time::SecondsPerHour;
	}

	static double Minutes(const Time & left, const Time & right)
	{
		Elapsed elapsed(left,right);
		return (double)elapsed.Interval / Time::SecondsPerMinute;
	}

	static double Minutes(Reason::System::Interval & interval)
	{
		return (double)interval / Time::SecondsPerMinute;
	}

	static double Seconds(const Time & left, const Time & right)
	{
		Elapsed elapsed(left,right);
		return (double)elapsed.Interval;
	}

	static double Seconds(Reason::System::Interval & interval)
	{
		return (double)interval;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Weekday: public Time
{
public:

	static const char * Weekdays[];

	Weekday(){};
	Weekday(const Weekday & weekday):Time(weekday){};
	Weekday(const Time & time):Time(time){};

	operator String ()
	{
		String weekday;
		Print(weekday);
		return weekday;
	}

	operator int ()
	{
		struct tm local;
		return (Times::Gmtime((time_t*)&Seconds,&local))?local.tm_wday:0;
	}

	void Print(String & string)
	{
		struct tm local;
		if (Times::Gmtime((time_t*)&Seconds,&local)) string.Append(Weekday::Weekdays[local.tm_wday]);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Day: public Time
{
public:

	Day(){};
	Day(const Day & day):Time(day){};
	Day(const Time & time):Time(time){};

	operator String ()
	{
		String day;
		Print(day);
		return day;
	}

	operator int ()
	{
		return Calendar(*this).Day;
	}

	void Print(String & string)
	{
		string.Append(Calendar(*this).Day);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Month: public Time
{
public:

	static const char * Months[];

	Month(){};
	Month(const Month & month):Time(month){};
	Month(const Time & time):Time(time){};

	void Assign(int month)
	{

	}

	operator String ()
	{
		String month;
		Print(month);
		return month;
	}

	operator int ()
	{
		return Calendar(*this).Month;
	}

	void Print(String & string)
	{
		string.Append(Calendar(*this).Month);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Year: public Time
{
public:

	Year(){};
	Year(const Year & year):Time(year) {}
	Year(const Time & time):Time(time) {}

	operator String ()
	{
		String year;
		Print(year);
		return year;
	}

	operator const int () const
	{
		return Calendar(*this).Year;
	}

	using Time::operator=;

	int operator - (const Year & year)
	{

		Calendar calendar(*this);
		return calendar.Year - (int)year;
	}

	void Print(class String & string)
	{
		string.Append(Calendar(*this).Year);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Date: public Time, public Timezone
{
public:

	void Print(String & string)
	{

		Formatter::Append(string,"%s %s %02d %04d",Weekday(*this).Slice(0,3).Print(), Month(*this).Slice(0,3).Print(),this->Day(),this->Year()); 
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

