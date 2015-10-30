
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/time.h"
#include "reason/system/number.h"
#include "reason/system/format.h"
#include "reason/system/parser.h"

#include <memory.h>

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Elapsed::Construct()
{

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Time::Construct()
{
	Seconds = 0;
	Milliseconds = 0;

	timeb time;
	ftime(&time);

	struct tm local;
	if (Times::Localtime((time_t*)&time.time,&local))
	{
		local.tm_isdst=0;

		time_t seconds = mktime(&local);
		Construct(seconds);

		return true;
	}

	return false;
}

bool Time::Construct(const Calendar & calendar)
{
	Seconds = 0;
	Milliseconds = 0;
	if (calendar.Year && calendar.Month)
	{
		struct tm local;
		memset(&local,0,sizeof(struct tm));

		local.tm_mday = calendar.Day;
		local.tm_mon = calendar.Month-1; 
		local.tm_year = calendar.Year-1900;
		local.tm_hour = calendar.Hour;
		local.tm_min = calendar.Minute;
		local.tm_sec = calendar.Second;

		time_t seconds = mktime(&local);

		{

			seconds -= Calendar(seconds).Seconds(calendar);
		}

		return Construct(seconds);
	}

	return false;
}

bool Time::Construct(char * data, int size)
{
	Seconds = 0;
	Milliseconds = 0;
	Calendar calendar(data,size);
	if (calendar.Year)
	{

		return Construct(calendar);
	}

	return false;
}

Time & Time::operator += (const Interval & interval)
{
	Seconds += interval.Seconds;
	Milliseconds += interval.Microseconds / Time::MicrosecondsPerMillisecond;
	return *this;
}

Time & Time::operator -= (const Interval & interval)
{
	Seconds -= interval.Seconds;
	Milliseconds -= interval.Microseconds / Time::MicrosecondsPerMillisecond;
	return *this;
}

Interval Time::operator - (const Time & time)
{
	return Elapsed(*this,time).Interval;
}

Time Time::operator - (Interval & interval)
{
	return Time(Seconds-interval.Seconds,Milliseconds-(interval.Microseconds/Time::MicrosecondsPerMillisecond));
}

Time Time::operator + (Interval & interval)
{
	return Time(Seconds-interval.Seconds,Milliseconds-(interval.Microseconds/Time::MicrosecondsPerMillisecond));
}

String Time::Weekday(Time & time)
{

	struct tm local;
	return (Times::Gmtime((time_t*)&time.Seconds,&local))?String(Weekday::Weekdays[local.tm_wday]):String();
}

String Time::Month(Time & time)
{

	struct tm local;
	return (Times::Gmtime((time_t*)&time.Seconds,&local))?String(Month::Months[local.tm_mon]):String();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TimeParser::Parse()
{
	while (!Eof())
	{

		while (ParseClock() || ParseDate() || ParseMonth() || ParseDay() || ParseYear() || ParseWeekday() || ParseSymbol())
		{
			if (Eof())
				break;
		}
		return Eof();

	}

	return false;
}

bool TimeParser::ParseClock()
{
	if (IsNumeric())
	{
		class Token state(Scanner->Token);
		while(ParseNumber())
		{
			if (Token.Size <= 2 && Is(':'))
			{

				Next();
				int hour = Token.Integer();
				if (ParseNumber())
				{
					int minute = Token.Integer();
					int second = 0;
					if (Is(':')) 
					{
						Next();
						if (ParseNumber())
							second = Token.Integer();
					}

					SkipWhitespace();

					ParseWord();
					if (Token.Is("am",true))
					{
						SkipWord();

						if (hour == 12 || hour == 24)
							hour = 0;

						OutputAssert(hour < 12);
					}
					else
					if (Token.Is("pm",true))
					{
						SkipWord();

						if (hour < 12)
							hour +=12;
					}

					Handled->ProcessHour(hour);
					Handled->ProcessMinute(minute);
					Handled->ProcessSecond(second);

					return true;
				}					
			}
		}
		Load(state);
	}

	return false;
}

bool TimeParser::ParseClockLiteral()
{

	return false;
}

bool TimeParser::ParseDate()
{
	if (IsAny(1,"/-.") || IsAny(2,"/-.") || IsAny(3,"/-.") || IsAny(4,"/-."))
	{
		class Token state(Scanner->Token);

		int day=0;
		int month=0;
		int year=0;

		ParseDateLiteral();
		Next();	

		if (Token.Size >= 3)
		{

			year = Token.Integer();
			if (ParseDateLiteral() && Token.Size <= 2 && IsAny("/-."))
			{
				month = Token.Integer();
				Next();
				if (ParseDateLiteral() && Token.Size <= 2 && (IsWhitespace() || Eof()))
				{	
					day = Token.Integer();

					if (day <= 31 && month <= 12)
					{
						Handled->ProcessDay(day);
						Handled->ProcessMonth(month);
						Handled->ProcessYear(year);
						return true;
					}
				}
			}
		}
		else
		if (Token.Size <= 2)
		{

			day = Token.Integer();
			if (ParseDateLiteral() && Token.Size <= 2 && IsAny("/-."))
			{
				month = Token.Integer();
				Next();
				if (ParseDateLiteral() && (IsWhitespace() || IsSymbol() || Eof()))
				{	
					year = Token.Integer();

					if (day > 31 || month > 12)
					{
						int num = day;
						day = month;
						month = num;
					}

					if (day <= 31 && month <= 12)
					{
						Handled->ProcessDay(day);
						Handled->ProcessMonth(month);
						Handled->ProcessYear(year);
						return true;
					}
				}
			}
		}

		Load(state);
	}

	return false;
}

bool TimeParser::ParseDateLiteral()
{
	if (IsNumeric())
	{
		ParseNumber();

		if ( Token.Size <= 4 && (IsAny("/-.") || IsWhitespace() || IsSymbol() || Eof()))
		{

			return true;
		}
	}
	return false;
}

bool TimeParser::ParseDay()
{

	if (IsNumeric())
	{
		class Token state(Scanner->Token);
		ParseNumber();

		if (Token.Size <= 2 && (Is(',') || IsWhitespace() || Is(Number::Suffix(Token.Integer())) ))
		{
			if (Is(Number::Suffix(Token.Integer()))) Next();
			Next();
			Handled->ProcessDay(Token.Integer());
			return true;
		}

		Load(state);
	}

	return false;
}

bool TimeParser::ParseDayLiteral()
{

	return false;
}

bool TimeParser::ParseWeekday()
{

	if (IsAlpha())
	{
		if (ParseWeekdayLiteral())
		{
			if (Is(",")) Next();
			return true;
		}
	}

	return false;
}

bool TimeParser::ParseWeekdayLiteral()
{
	if (IsAlpha())
	{
		class Token state(Scanner->Token);
		if (ParseWord())
		{

			int weekday = 0;
			if ((Token.Is("sun",true)||Token.Is("sunday",true))
				|| (++weekday && (Token.Is("mon",true)||Token.Is("monday",true)))
				|| (++weekday && (Token.Is("tue",true)||Token.Is("tuesday",true)))
				|| (++weekday && (Token.Is("wed",true)||Token.Is("wednesday",true)))
				|| (++weekday && (Token.Is("thu",true)||Token.Is("thursday",true)))
				|| (++weekday && (Token.Is("fri",true)||Token.Is("friday",true)))
				|| (++weekday && (Token.Is("sat",true)||Token.Is("saturday",true)))
				)
			{
				Handled->ProcessWeekday(weekday);
				return true;
			}
		}

		Load(state);
	}
	return false;
}

bool TimeParser::ParseMonth()
{
	if (IsAlpha())
	{
		if (ParseMonthLiteral())
		{
			if (Is(",")) Next();
			return true;
		}
	}

	return false;
}

bool TimeParser::ParseMonthLiteral()
{
	if (IsAlpha())
	{
		class Token state(Scanner->Token);
		if (ParseWord())
		{

			int month = 1;
			if ((Token.Is("jan",true)||Token.Is("january",true))
				|| (++month && (Token.Is("feb",true)||Token.Is("february",true)))
				|| (++month && (Token.Is("mar",true)||Token.Is("march",true)))
				|| (++month && (Token.Is("apr",true)||Token.Is("april",true)))
				|| (++month && (Token.Is("may",true)))
				|| (++month && (Token.Is("jun",true)||Token.Is("june",true)))
				|| (++month && (Token.Is("jul",true)||Token.Is("july",true)))
				|| (++month && (Token.Is("aug",true)||Token.Is("august",true)))
				|| (++month && (Token.Is("sep",true)||Token.Is("september",true)))
				|| (++month && (Token.Is("oct",true)||Token.Is("october",true)))
				|| (++month && (Token.Is("nov",true)||Token.Is("november",true)))
				|| (++month && (Token.Is("dec",true)||Token.Is("december",true)))
				)
			{
				Handled->ProcessMonth(month);
				return true;
			}
		}

		Load(state);
	}
	return false;
}

bool TimeParser::ParseYear()
{
	if (IsNumeric())
	{
		class Token state(Scanner->Token);

		if (ParseNumber() && Token.Size == 4 && !IsAny("/-."))
		{
			Handled->ProcessYear(Token.Integer());
			return true;
		}
		Load(state);
	}
	return false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char * Month::Months[] = {"January","February","March","April","May","June","July","August","September","October","November","December"}; 

const char * Weekday::Weekdays[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Calendar::Construct(char * data, int size)
{
	Day = Month = Year = Hour = Minute = Second = 0;

	CalendarReader reader(this);
	reader.Assign(data,size);
	return reader.Parse();
}

bool Calendar::Construct(const Calendar & calendar)
{
	Day = calendar.Day;
	Month = calendar.Month;
	Year = calendar.Year;
	Hour = calendar.Hour;
	Minute = calendar.Minute;
	Second = calendar.Second;
	return true;
}

bool Calendar::Construct(const struct tm & local)
{
	Day = local.tm_mday;
	Month = local.tm_mon+1; 

	Year = local.tm_year+1900;
	Hour = local.tm_hour;
	Minute = local.tm_min;
	Second = local.tm_sec;
	return true;
}

bool Calendar::Construct(const time_t & seconds)
{

	struct tm local;
	if (Times::Gmtime(&seconds,&local))
	{
		return Construct(local);
	}
	return false;
}

bool Calendar::Construct(const Time & time)
{
	return Construct((time_t)time.Seconds);
}

void Calendar::Format(String & string, const char * format)
{

	string.Reserve(64);

	if (Year && Month)
	{
		struct tm local;
		memset(&local,0,sizeof(struct tm));
		local.tm_mday = Day;
		local.tm_mon = Month-1;
		local.tm_year = Year-1900;
		local.tm_hour = Hour;
		local.tm_min = Minute;
		local.tm_sec = Second;

		time_t seconds = mktime(&local);
		if (Times::Gmtime(&seconds,&local)) string.Size = strftime(string.Data,string.Allocated-1,format,&local);
	}

	string.Terminate();
}

void Calendar::Print(class String & string)
{

	Format(string,"%a %b %d %Y %I:%M:%S %p");

}

double Calendar::Years(const Calendar & calendar)
{

	Calendar * prior = this;
	Calendar * after = (Calendar*)&calendar;
	if (calendar  < (*this))
	{
		prior = (Calendar*)&calendar;
		after = this;
	}

	double years = prior->CalendarMonthsRemaining()/Time::MonthsPerYear;

	years += ((prior->Year+1)<after->Year)?after->Year-(prior->Year+1):0;

	if (prior->Year == after->Year)
		years -= after->CalendarMonthsRemaining()/Time::MonthsPerYear;
	else
		years += after->CalendarMonths()/Time::MonthsPerYear;

	return (calendar < (*this))?years:-years;
}

double Calendar::Months(const Calendar & calendar)
{

	Calendar * prior = this;
	Calendar * after = (Calendar*)&calendar;
	if (calendar  < (*this))
	{
		prior = (Calendar*)&calendar;
		after = this;
	}

	double months = prior->CalendarMonthsRemaining();

	for (int year=prior->Year+1;year<after->Year;++year)
		months += Time::MonthsPerYear;

	if (prior->Year == after->Year)
		months -= after->CalendarMonthsRemaining();
	else
		months += after->CalendarMonths();

	return (calendar < (*this))?months:-months;
}

double Calendar::Weeks(const Calendar & calendar)
{

	Calendar * prior = this;
	Calendar * after = (Calendar*)&calendar;
	if (calendar  < (*this))
	{
		prior = (Calendar*)&calendar;
		after = this;
	}

	double weeks = prior->CalendarWeeksRemaining();

	for (int year=prior->Year+1;year<after->Year;++year)
		weeks += Time::DaysInYear(year) / Time::WeeksPerYear;

	if (prior->Year == after->Year)
		weeks -= after->CalendarWeeksRemaining();
	else
		weeks += after->CalendarWeeks();

	return (calendar < (*this))?weeks:-weeks;
}

double Calendar::Days(const Calendar & calendar)
{

	Calendar * prior = this;
	Calendar * after = (Calendar*)&calendar;
	if (calendar  < (*this))
	{
		prior = (Calendar*)&calendar;
		after = this;
	}

	double days = prior->CalendarDaysRemaining();

	for (int year=prior->Year+1;year<after->Year;++year)
		days += Time::DaysInYear(year);

	if (prior->Year == after->Year)
		days -= after->CalendarDaysRemaining();
	else
		days += after->CalendarDays();

	return (calendar < (*this))?days:-days;
}

double Calendar::Hours(const Calendar & calendar)
{

	Calendar * prior = this;
	Calendar * after = (Calendar*)&calendar;
	if (calendar  < (*this))
	{
		prior = (Calendar*)&calendar;
		after = this;
	}

	double hours = prior->CalendarHoursRemaining();

	for (int year=prior->Year+1;year<after->Year;++year)
		hours += Time::DaysInYear(year) * Time::HoursPerDay;

	if (prior->Year == after->Year)
		hours -= after->CalendarHoursRemaining();
	else
		hours += after->CalendarHours();

	return (calendar < (*this))?hours:-hours;
}

double Calendar::Minutes(const Calendar & calendar)
{

	Calendar * prior = this;
	Calendar * after = (Calendar*)&calendar;
	if (calendar  < (*this))
	{
		prior = (Calendar*)&calendar;
		after = this;
	}

	double minutes = prior->CalendarMinutesRemaining();

	for (int year=prior->Year+1;year<after->Year;++year)
		minutes += Time::DaysInYear(year) * Time::MinutesPerDay;

	if (prior->Year == after->Year)
		minutes -= after->CalendarMinutesRemaining();
	else
		minutes += after->CalendarMinutes();

	return (calendar < (*this))?minutes:-minutes;
}

long long Calendar::operator - (const Calendar & calendar)
{

	Calendar * prior = this;
	Calendar * after = (Calendar*)&calendar;
	if (calendar  < (*this))
	{
		prior = (Calendar*)&calendar;
		after = this;
	}

	long long seconds=0;

	for (int month=1;month<prior->Month;++month)
		seconds += Time::DaysInMonth(month,prior->Year) * Time::SecondsPerDay;

	seconds += (prior->Day-1) * Time::SecondsPerDay;
	seconds += (prior->Hour*Time::SecondsPerHour) + (prior->Minute*Time::SecondsPerMinute) + prior->Second;

	seconds = (prior->Year == after->Year)?-seconds:Time::DaysInYear(prior->Year) * Time::SecondsPerDay - seconds;

	for (int year=prior->Year+1;year<after->Year;++year)
		seconds += Time::DaysInYear(year) * Time::SecondsPerDay;

	for (int month=1;month<after->Month;++month)
		seconds += Time::DaysInMonth(month,after->Year) * Time::SecondsPerDay;

	seconds += (after->Day-1) * Time::SecondsPerDay;
	seconds += (after->Hour*Time::SecondsPerHour) + (after->Minute*Time::SecondsPerMinute) + after->Second;

	return (calendar < (*this))?seconds:-seconds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CalendarReader::CalendarReader(class Calendar * calendar):Calendar(calendar)
{
}

bool CalendarReader::Parse()
{
	Handled = this;
	TimeParser::Parse();
	return ((long long)*Calendar) != 0;
}

void CalendarReader::ProcessDay(int day)
{
	Calendar->Day = day;
}

void CalendarReader::ProcessWeekday(int weekday)
{

}

void CalendarReader::ProcessMonth(int month)
{

	Calendar->Month = month;
}

void CalendarReader::ProcessYear(int year)
{
	Calendar->Year = year;
}

void CalendarReader::ProcessHour(int hour)
{
	Calendar->Hour = hour;
}

void CalendarReader::ProcessMinute(int minute)
{
	Calendar->Minute = minute;
}

void CalendarReader::ProcessSecond(int second)
{
	Calendar->Second = second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Interval::Construct(char * data, int size)
{
	Seconds = 0;
	Microseconds = 0;

	IntervalReader reader(this);
	reader.Assign(data,size);
	return reader.Parse();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IntervalReader::Parse()
{
	Handled = this;
	TimeParser::Parse();
	return Interval->Seconds > 0 || Interval->Microseconds > 0;
}

void IntervalReader::ProcessHour(int hour)
{
	(*Interval) += hour * Time::SecondsPerHour;
}

void IntervalReader::ProcessMinute(int minute)
{
	(*Interval) += minute * Time::SecondsPerMinute;
}

void IntervalReader::ProcessSecond(int second)
{
	(*Interval) += second;
}

void IntervalReader::ProcessDay(int day)
{

}

void IntervalReader::ProcessWeekday(int weekday)
{

}

void IntervalReader::ProcessMonth(int month)
{

}

void IntervalReader::ProcessYear(int year)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double Epoch::Years()
{
	if (Ticks == 0) return 0;

	long long ticks = Ticks/Scale;

	double seconds = CalendarSecondsRemaining();
	double years=0; 
	if (ticks < seconds)
	{

		seconds= Time::DaysInYear(Year)*Time::SecondsPerDay;
		years += (double)ticks/seconds;
	}
	else
	{

		years = CalendarMonthsRemaining()/Time::MonthsPerYear;
		ticks -= seconds;
		for (int year=Year+1;ticks>0;++year)
		{
			seconds= Time::DaysInYear(year)*Time::SecondsPerDay;
			years += (ticks-seconds > 0)?1:(double)ticks/seconds;
			ticks -= seconds;
		}
	}

	return years;
}

double Epoch::Years(const Epoch & epoch)
{

	double years = Calendar::Years((Calendar&)epoch);

	if (years > 0)
	{

		years -= Years();

		return years - ((Epoch&)epoch).Years();
	}
	else
	{

		years += ((Epoch&)epoch).Years();

		return Years() + years;
	}
}

double Epoch::Months()
{

	if (Ticks == 0) return 0;

	long long ticks = Ticks/Scale;

	double seconds = CalendarSecondsRemaining();
	double months = 0;
	if (ticks < seconds)
	{

		for (int month=Month;ticks>0;++month)
		{
			seconds = Time::DaysInMonth(month,Year)*Time::SecondsPerDay;
			months += (ticks-seconds > 0)?1:(double)ticks/seconds;
			ticks -= seconds;	
		}
	}
	else
	{

		ticks -= seconds;
		months = CalendarMonthsRemaining();
		for (int year=Year+1;ticks>0;++year)
		{
			seconds= Time::DaysInYear(year)*Time::SecondsPerDay;
			if (ticks-seconds > 0)
			{
				months += Time::MonthsPerYear;
				ticks -= seconds;
			}
			else
			{

				for (int month=1;ticks>0;++month)
				{
					seconds = Time::DaysInMonth(month,year)*Time::SecondsPerDay;
					months += (ticks-seconds > 0)?1:(double)ticks/seconds;
					ticks -= seconds;	
				}
			}
		}
	}

	return months;
}

double Epoch::Months(const Epoch & epoch)
{

	double months = Calendar::Months((Calendar&)epoch);

	if (months > 0)
	{
		months -= Months();
		return months - ((Epoch&)epoch).Months();
	}
	else
	{
		months += ((Epoch&)epoch).Months();
		return Months() + months;
	}
}

double Epoch::Seconds(const Epoch & epoch) 
{

	int factor = (epoch.Scale > Scale)?epoch.Scale:Scale;

	if (factor == 0) factor = 1;

	long long ticks = 0;
	long long seconds = (Calendar&)(*this)-(Calendar&)epoch;
    if (seconds > 0)
	{

		ticks = Ticks/((double)Scale/factor);
		ticks += seconds*factor;

		return (double)(ticks - epoch.Ticks/((double)epoch.Scale/factor))/factor;
	}
	else
	{

		ticks = epoch.Ticks/((double)epoch.Scale/factor);
		ticks -= seconds*factor;

		return (double)(Ticks/((double)Scale/factor) - ticks)/factor;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

