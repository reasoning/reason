
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

#include "reason/platform/platform.h"
#include "reason/system/output.h"

using namespace Reason::Platform;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef REASON_PLATFORM_APPLE
#include <libkern/OSAtomic.h>
#endif

#ifdef REASON_PLATFORM_WINDOWS
#include <Windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PlatformMessage::PlatformMessage()
{
	#ifdef REASON_PLATFORM_WINDOWS
	OutputMessage("Running on \"windows\"\n"); 
	#else
	OutputMessage("Running on \"linux\"\n");
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Atomic::Cas(volatile int * dest, int prev, int with)
{
	#ifdef REASON_PLATFORM_WINDOWS

	return InterlockedCompareExchange((long*)((int*)dest),with,prev);
	#endif

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE

	int value = prev;
	while (!OSAtomicCompareAndSwap32(prev,with,dest))
		if ((value = *dest) != prev) break;
	return value;

	#else

	#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

	return __sync_val_compare_and_swap (dest,prev,with);

	#else

	asm volatile("lock;cmpxchgl %1,%2":"=a"(prev):"q"(with),"m"(*dest),"0"(prev):"memory");
	return prev;	

	#if defined(__i386__)

	#endif

	#if defined(__x86_64__)

	#endif

	#endif

	#endif 

	#endif 

	return 0;
}

int Atomic::Swp(volatile int * dest, int with)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return InterlockedExchange((long*)((int*)dest),with);
	#endif

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE

	int value = *dest;
	while (!OSAtomicCompareAndSwap32(value, with, dest))
		value = *dest;
	return value;

	#else

	#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

	return __sync_lock_test_and_set (dest,with);

	#else

	int value;
	asm volatile("lock;xchgl %1,%2":"=r"(value):"m"(*dest),"0"(with):"memory");
	return value;	

	#endif

	#endif 

	#endif 

	return 0;
}

int Atomic::Inc(volatile int * dest)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return InterlockedIncrement((long*)((int*)dest));
	#endif

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE

	return OSAtomicIncrement32Barrier(dest);

	#else

	#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

	return __sync_add_and_fetch(dest,1);

	#else

	int value;
	asm volatile("lock;incl %1;movl %1,%0":"=r"(value):"m"(*dest):"memory");
	return value;

	#endif

	#endif 

	#endif 

	return 0;	

}

int Atomic::Dec(volatile int * dest)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return InterlockedDecrement((long*)((int*)dest));
	#endif	

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE

	return OSAtomicDecrement32Barrier(dest);

	#else

	#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

	return __sync_sub_and_fetch(dest,1);

	#else

	int value;
	asm volatile("lock;decl %1;movl %1,%0":"=r"(value):"m"(*dest):"memory");
	return value;	

	#endif

	#endif 

	#endif 

	return 0;	
}

int Atomic::Add(volatile int * dest, int by)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return InterlockedExchangeAdd((long*)((int*)dest),by);
	#endif

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE

	return OSAtomicAdd32Barrier(by,dest);

	#else

	#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

	return __sync_add_and_fetch(dest,by);

	#else

	int value;
	asm volatile("lock; xaddl %2,%1":"=a"(value):"m"(*dest),"0"(by):"memory");
	return value;		

	#endif

	#endif 

	#endif 

	return 0;	
}

int Atomic::Sub(volatile int * dest, int by)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return InterlockedExchangeAdd((long*)((int*)dest),-by);
	#endif

	#ifdef REASON_PLATFORM_POSIX

	#ifdef REASON_PLATFORM_APPLE

	return OSAtomicAdd32Barrier(-by,dest);

	#else

	#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

	return __sync_sub_and_fetch(dest,by);

	#else

	int value;
	asm volatile("lock; xaddl %2,%1":"=a"(value):"m"(*dest),"0"(-by):"memory");
	return value;

	#endif

	#endif 

	#endif 

	return 0;	

}

int Atomic::Mul(volatile int * dest, int by)
{
	int initial = 0;
	int result = 0;
	do 
	{

		if (initial != *dest)
		{

			initial = *dest;
			result = initial * by; 
		}
	}
	while(Cas(dest,initial,result) != initial);

	return result;
}

int Atomic::Div(volatile int * dest, int by)
{
	int initial = 0;
	int result = 0;
	do 
	{
		if (initial != *dest)
		{
			initial = *dest;
			result = initial / by; 
		}
	}
	while(Cas(dest,initial,result) != initial);

	return result;
}

int Atomic::Mod(volatile int * dest, int by)
{
	int initial = 0;
	int result = 0;
	do 
	{
		if (initial != *dest)
		{

			initial = *dest;
			result = initial % by; 
		}
	}
	while(Cas(dest,initial,result) != initial);

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void * Atomic::Cas(volatile void ** dest, void * prev, void * with)
{
    #ifdef REASON_PLATFORM_X86_64
    return (void*)Atomic::Cas((long long *)dest,(long long)prev,(long long)with);
    #else
    return (void*)Atomic::Cas((int *)dest,(int)prev,(int)with);
    #endif
}

void * Atomic::Swp(volatile void ** dest, void * with)
{
	#ifdef REASON_PLATFORM_X86_64
    return (void*)Atomic::Swp((long long *)dest,(long long)with);
	#else
    return (void*)Atomic::Swp((int *)dest,(int)with);
	#endif    
}

void * Atomic::Add(volatile void ** dest, void * by)
{
	#ifdef REASON_PLATFORM_X86_64
    return (void*)Atomic::Add((long long *)dest,(long long)by);
	#else
    return (void*)Atomic::Add((int *)dest,(int)by);
	#endif
}

void * Atomic::Sub(volatile void ** dest, void * by)
{
	#ifdef REASON_PLATFORM_X86_64
	return (void*)Atomic::Sub((long long *)dest,(long long)by);
	#else
	return (void*)Atomic::Sub((int *)dest,(int)by);
	#endif    
}

void * Atomic::Inc(volatile void ** dest)
{
	#ifdef REASON_PLATFORM_X86_64
	return (void*)Atomic::Inc((long long *)dest);
	#else
	return (void*)Atomic::Inc((int *)dest);
	#endif   
}

void * Atomic::Dec(volatile void ** dest)
{
	#ifdef REASON_PLATFORM_X86_64
	return (void*)Atomic::Dec((long long *)dest);
	#else
	return (void*)Atomic::Dec((int *)dest);
	#endif    
}

void * Atomic::Mul(volatile void ** dest, void * by)
{
	#ifdef REASON_PLATFORM_X86_64
	return (void*)Atomic::Mul((long long *)dest,(long long)by);
	#else
	return (void*)Atomic::Mul((int *)dest,(int)by);
	#endif       
}

void * Atomic::Div(volatile void ** dest, void * by)
{
	#ifdef REASON_PLATFORM_X86_64
	return (void*)Atomic::Div((long long *)dest,(long long)by);
	#else
	return (void*)Atomic::Div((int *)dest,(int)by);
	#endif    
}

void * Atomic::Mod(volatile void ** dest, void * by)
{
	#ifdef REASON_PLATFORM_X86_64
	return (void*)Atomic::Mod((long long *)dest,(long long)by);
	#else
	return (void*)Atomic::Mod((int *)dest,(int)by);
	#endif     
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef REASON_PLATFORM_X86_64

long long Atomic::Cas(volatile long long * dest, long long prev, long long with)
{
    #ifdef REASON_PLATFORM_WINDOWS

    return InterlockedCompareExchange64((long long*)((long long*)dest),with,prev);
    #endif

    #ifdef REASON_PLATFORM_POSIX

    #ifdef REASON_PLATFORM_APPLE

    int value = prev;
    while (!OSAtomicCompareAndSwap64(prev,with,dest))
        if ((value = *dest) != prev) break;
    return value;

    #else

    #if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

    return __sync_val_compare_and_swap(dest,prev,with);

    #endif 

    #endif 

    #endif
    return 0;
}

long long Atomic::Swp(volatile long long * dest, long long with)
{
    #ifdef REASON_PLATFORM_WINDOWS
    return InterlockedExchange64((long long*)((long long*)dest),with);
    #endif

    #ifdef REASON_PLATFORM_POSIX

    #ifdef REASON_PLATFORM_APPLE

    int value = *dest;
    while (!OSAtomicCompareAndSwap64(value, with, dest))
        value = *dest;
    return value;

    #else

    #if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

    return __sync_lock_test_and_set (dest,with);

    #endif 

    #endif 

    #endif

    return 0;
}

long long Atomic::Inc(volatile long long * dest)
{
    #ifdef REASON_PLATFORM_WINDOWS
    return InterlockedIncrement64((long long*)((long long*)dest));
    #endif

    #ifdef REASON_PLATFORM_POSIX

    #ifdef REASON_PLATFORM_APPLE

    return OSAtomicIncrement64Barrier(dest);

    #else

    #if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

    return __sync_add_and_fetch(dest,1);

    #endif 

    #endif 

    #endif

    return 0;	

}

long long Atomic::Dec(volatile long long * dest)
{
    #ifdef REASON_PLATFORM_WINDOWS
    return InterlockedDecrement64((long long*)((long long*)dest));
    #endif	

    #ifdef REASON_PLATFORM_POSIX

    #ifdef REASON_PLATFORM_APPLE

    return OSAtomicDecrement64Barrier(dest);

    #else

    #if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

    return __sync_sub_and_fetch(dest,1);

    #endif 

    #endif 

    #endif

    return 0;	
}

long long Atomic::Add(volatile long long * dest, long long by)
{
    #ifdef REASON_PLATFORM_WINDOWS
    return InterlockedExchangeAdd64((long long*)((long long*)dest),by);
    #endif

    #ifdef REASON_PLATFORM_POSIX

    #ifdef REASON_PLATFORM_APPLE

    return OSAtomicAdd64Barrier(by,dest);

    #else

    #if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

    return __sync_add_and_fetch(dest,by);

    #endif 

    #endif 

    #endif
    return 0;	
}

long long Atomic::Sub(volatile long long * dest, long long by)
{
    #ifdef REASON_PLATFORM_WINDOWS
    return InterlockedExchangeAdd64((long long*)((long long*)dest),-by);
    #endif

    #ifdef REASON_PLATFORM_POSIX

    #ifdef REASON_PLATFORM_APPLE

    return OSAtomicAdd64Barrier(-by,dest);

    #else

    #if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 0)) 

    return __sync_sub_and_fetch(dest,by);

    #endif 

    #endif

    #endif

    return 0;	

}

long long Atomic::Mul(volatile long long * dest, long long by)
{
    long long initial = 0;
    long long result = 0;
    do 
    {

        if (initial != *dest)
        {

            initial = *dest;
            result = initial * by; 
        }
    }
    while(Cas(dest,initial,result) != initial);

    return result;
}

long long Atomic::Div(volatile long long * dest, long long by)
{
    long long initial = 0;
    long long result = 0;
    do 
    {
        if (initial != *dest)
        {
            initial = *dest;
            result = initial / by; 
        }
    }
    while(Cas(dest,initial,result) != initial);

    return result;
}

long long Atomic::Mod(volatile long long * dest, long long by)
{
    long long initial = 0;
    long long result = 0;
    do 
    {
        if (initial != *dest)
        {

            initial = *dest;
            result = initial % by; 
        }
    }
    while(Cas(dest,initial,result) != initial);

    return result;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
