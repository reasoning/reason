
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

#pragma warning(disable:4244)

#include "reason/system/timer.h"

#include <memory.h>

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long long Timer::Frequency = 0;

Timer::Timer()
{

	Reset();
	Start();
}

Timer::~Timer()
{

}

void Timer::Start()
{

	#ifdef REASON_PLATFORM_WINDOWS
	LARGE_INTEGER integer;
	QueryPerformanceCounter(&integer);
	Started = (long long)integer.QuadPart;
	#endif

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE
	if (gettimeofday(&Started,0) != 0)
	#else
    if (clock_gettime(CLOCK_REALTIME,&Started) != 0)
	#endif
    {
		OutputError("Timer::Start - Failed to get time for high resolution timer\n");        
    }

	#endif

	#ifdef REASON_PLATFORM_APPLE

	#endif
}

void Timer::Step()
{

	#ifdef REASON_PLATFORM_WINDOWS
	if (IsStarted())
		Stopped = 0;
	else 
		Start();
	#endif

	#ifdef REASON_PLATFORM_POSIX
	if (IsStarted())
		#ifdef REASON_PLATFORM_APPLE
		memset(&Stopped,0,sizeof(struct timeval));
		#else
		memset(&Stopped,0,sizeof(struct timespec));
		#endif
	else
		Start();
	#endif

}

void Timer::Stop()
{

	#ifdef REASON_PLATFORM_WINDOWS
	LARGE_INTEGER integer;
	QueryPerformanceCounter(&integer);
	Stopped = (long long)integer.QuadPart;
	#endif

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE
	if (gettimeofday(&Stopped,0) != 0)
	#else
    if (clock_gettime(CLOCK_REALTIME,&Stopped) != 0)
	#endif
    {
		OutputError("Timer::Stop - Failed to get time for high resolution timer\n");        
    }
	#endif

}

void Timer::Reset()
{

	#ifdef REASON_PLATFORM_WINDOWS
	Started = 0;
	Stopped = 0;
	if (!Frequency)
	{
		LARGE_INTEGER integer;
		QueryPerformanceFrequency(&integer);
		Frequency = (long long)integer.QuadPart;
	}

	#endif

	#ifdef REASON_PLATFORM_POSIX
	#ifdef REASON_PLATFORM_APPLE
	memset(&Started,0,sizeof(struct timeval));
	memset(&Stopped,0,sizeof(struct timeval));	
	#else
	memset(&Started,0,sizeof(struct timespec));
	memset(&Stopped,0,sizeof(struct timespec));

	#endif
	#endif
}

bool Timer::IsStarted()
{

	#ifdef REASON_PLATFORM_WINDOWS
	return Started != 0;
	#endif

	#ifdef REASON_PLATFORM_POSIX
	#ifdef REASON_PLATFORM_APPLE
	return Started.tv_sec != 0 || Started.tv_usec != 0;
	#else
	return Started.tv_sec != 0 || Started.tv_nsec != 0;
	#endif
	#endif

}
bool Timer::IsStopped()
{

	#ifdef REASON_PLATFORM_WINDOWS
	return Stopped != 0;
	#endif

	#ifdef REASON_PLATFORM_POSIX
	#ifdef REASON_PLATFORM_APPLE
	return Stopped.tv_sec != 0 || Stopped.tv_usec != 0;
	#else
	return Stopped.tv_sec != 0 || Stopped.tv_nsec != 0;
	#endif
	#endif

}

double Timer::Seconds()
{
	Timer timer;
	return (double)timer.Nanoseconds() / Timers::NanosecondsPerSecond;
}

double Timer::Milliseconds()
{
	Timer timer;
	return (double)timer.Nanoseconds() / Timers::NanosecondsPerMillisecond;
}

double Timer::Microseconds()
{
	Timer timer;
	return (double)timer.Nanoseconds() / Timers::NanosecondsPerMicrosecond;
}

long long Timer::Nanoseconds()
{

	Timer timer;

	#ifdef REASON_PLATFORM_WINDOWS

	double nanoseconds = (double)(timer.Started) / ((double)timer.Frequency / Timers::NanosecondsPerSecond);

	return nanoseconds;

	#endif

	#ifdef REASON_PLATFORM_POSIX

	long long secondsToNanoseconds = timer.Started.tv_sec * Timers::NanosecondsPerSecond;

	#ifdef REASON_PLATFORM_APPLE
	return  secondsToNanoseconds + (timer.Started.tv_usec * Timers::NanosecondsPerMicrosecond);
	#else
	return  secondsToNanoseconds + timer.Started.tv_nsec;
	#endif

	#endif
}

double Timer::ElapsedSeconds()
{

	return (double)ElapsedNanoseconds() / Timers::NanosecondsPerSecond;
}

double Timer::ElapsedMilliseconds()
{

	return (double)ElapsedNanoseconds() / Timers::NanosecondsPerMillisecond;
}

double Timer::ElapsedMicroseconds()
{
	return (double)ElapsedNanoseconds() / Timers::NanosecondsPerMicrosecond;
}

long long Timer::ElapsedNanoseconds()
{

	#ifdef REASON_PLATFORM_WINDOWS
	if (IsStopped())
	{
		return  (double)(Stopped-Started) / ((double)Frequency / Timers::NanosecondsPerSecond);
	}
	else
	if (IsStarted())
	{
		LARGE_INTEGER integer;
		QueryPerformanceCounter(&integer);
		long long now = (long long)integer.QuadPart;		

		return  (double)(now-Started) / ((double)Frequency / Timers::NanosecondsPerSecond);
	}
	else
	{
		return 0;
	}
	#endif

	#ifdef REASON_PLATFORM_POSIX

	if (IsStopped())
	{
		long long secondsToNanoseconds = (Stopped.tv_sec - Started.tv_sec) * Timers::NanosecondsPerSecond;
		#ifdef REASON_PLATFORM_APPLE
		return secondsToNanoseconds + ((Stopped.tv_usec - Started.tv_usec) * Timers::NanosecondsPerMicrosecond);
		#else	
		return  secondsToNanoseconds + (Stopped.tv_nsec - Started.tv_nsec);
		#endif
	}
	else
	if (IsStarted())
	{
		#ifdef REASON_PLATFORM_APPLE
		struct timeval now;
		if (gettimeofday(&now,0) != 0)
		#else
		struct timespec now;
		if (clock_gettime(CLOCK_REALTIME,&now) != 0) 
		#endif
		{
			OutputError("Timer::Start - Failed to get time for high resolution timer\n");        
			return 0;
		}

		long long secondsToNanoseconds = (now.tv_sec - Started.tv_sec) * Timers::NanosecondsPerSecond;
		#ifdef REASON_PLATFORM_APPLE
		return  secondsToNanoseconds + ((now.tv_usec - Started.tv_usec) * Timers::NanosecondsPerMicrosecond);
		#else
		return  secondsToNanoseconds + (now.tv_nsec - Started.tv_nsec);
		#endif
	}
	else
	{
		return 0;
	}
	#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Ticker::Ticker()
{

	Reset();
	Start();
}

Ticker::~Ticker()
{

}

void Ticker::Start()
{
	memset(&Stopped,0,sizeof(struct timeb));
	ftime(&Started);
}

void Ticker::Step()
{
	if (IsStarted())
		memset(&Stopped,0,sizeof(struct timeb));
	else
		Start();
}

void Ticker::Stop()
{
	if (IsStarted())
		ftime(&Stopped);
}

void Ticker::Reset()
{
	memset(&Started,0,sizeof(struct timeb));
	memset(&Stopped,0,sizeof(struct timeb));
}

bool Ticker::IsStarted()
{
	return Started.time != 0;
}
bool Ticker::IsStopped()
{
	return Stopped.time != 0;
}

long Ticker::ElapsedMilliseconds()
{
	if (IsStopped())
	{
		long secondsToMilliseconds = (Stopped.time - Started.time) * 1000;
		return  secondsToMilliseconds + (Stopped.millitm - Started.millitm);
	}
	else
	if (IsStarted())
	{
		struct timeb now;
		ftime(&now);

		long secondsToMilliseconds = (now.time - Started.time) * 1000;
		return  secondsToMilliseconds  + (now.millitm - Started.millitm);
	}
	else
	{
		return 0;
	}
}

double Ticker::ElapsedSeconds()
{
	if (IsStopped())
	{
		return (Stopped.time - Started.time) + ((double)(Stopped.millitm - Started.millitm) / 1000);
	}
	else
	if (IsStarted())
	{

		struct timeb now;
		ftime(&now);
		return (now.time - Started.time) + ((double)(now.millitm - Started.millitm) / 1000);
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////