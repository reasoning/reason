
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

#include "reason/platform/thread.h"
#include "reason/system/time.h"
#include "reason/platform/platform.h"
#include "reason/platform/atomic.h"
#include "reason/system/output.h"

#include "reason/structure/array.h"
#include "reason/structure/list.h"

#include "reason/structure/intrinsic.h"

#include <stdio.h>
#include <assert.h>

#ifdef REASON_PLATFORM_POSIX
#include <unistd.h>
#endif

#ifdef REASON_PLATFORM_WINDOWS
#include <process.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Platform;
using namespace Reason::Structure;
using namespace Reason::Structure::Intrinsic;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Platform {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Critical::Critical()
{
	memset(&Handle,0,sizeof(Handle));
	Create();
}
Critical::~Critical()
{
	Delete();
}
bool Critical::Enter()
{
	#ifdef REASON_PLATFORM_WINDOWS

	__try
	{
		EnterCriticalSection((LPCRITICAL_SECTION)&Handle);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION) 
	{
		return false;
	}

	return true;
	#endif 

	#ifdef REASON_PLATFORM_POSIX
	pthread_mutex_lock((pthread_mutex_t*)&Handle);
	return true;
	#endif

}

bool Critical::Leave()
{

	#ifdef REASON_PLATFORM_WINDOWS
	__try
	{
		LeaveCriticalSection((LPCRITICAL_SECTION)this);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION) 
	{
		return false;
	}

	return true;
	#endif

	#ifdef REASON_PLATFORM_POSIX
	return pthread_mutex_unlock((pthread_mutex_t*)&Handle) == 0;
	#endif
}

bool Critical::Try()
{

	#ifdef REASON_PLATFORM_WINDOWS

	return TryEnterCriticalSection(&Handle) != 0;
	#endif

	#ifdef REASON_PLATFORM_POSIX

	return pthread_mutex_trylock((pthread_mutex_t*)&Handle) == 0;
	#endif
}

bool Critical::Wait()
{

	if (Try())
	{
		Leave();
		return false;
	}

	return true;
}

bool Critical::Owned()
{

	#ifdef REASON_PLATFORM_WINDOWS

	return (unsigned int)((LPCRITICAL_SECTION)&Handle)->OwningThread == GetCurrentThreadId();
	#endif

	#ifdef REASON_PLATFORM_POSIX

	struct timespec ts = Timer::Timespec(timeout*Timer::NanosecondsPerMillisecond);
	int result = pthread_mutex_timedlock((pthread_mutex_t*)&Handle,&ts);

	if (result == EDEADLK)
	{

		return true;
	}
	else
	if (result == 0)
	{

		Leave();
		return false;
	}
	else
	{

		return false;
	}
	#endif

}

void Critical::Create()
{

	if (*(int*)(&Handle) == 0)
	{
	#ifdef REASON_PLATFORM_WINDOWS
	InitializeCriticalSection(&Handle);
	#endif

	#ifdef REASON_PLATFORM_POSIX

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init((pthread_mutex_t*)&Handle,&attr);
	pthread_mutexattr_destroy(&attr);

	#endif
	}
}

void Critical::Delete()
{
	if (*(int*)(&Handle) != 0)
	{
	#ifdef REASON_PLATFORM_WINDOWS
	DeleteCriticalSection(&Handle);
	#endif

	#ifdef REASON_PLATFORM_POSIX
	pthread_mutex_destroy((pthread_mutex_t*)&Handle);
	#endif
	}

	memset(&Handle,0,sizeof(Handle));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Mutex::Lock(int timeout)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return WaitForSingleObject(Handle,timeout) == 0;
	#endif 

	#ifdef REASON_PLATFORM_POSIX

	if (timeout != TimeoutInfinite)
	{
		struct timespec ts = Timer::Timespec(timeout*Timer::NanosecondsPerMillisecond);
		return pthread_mutex_timedlock((pthread_mutex_t*)&Handle,&ts);
	}

	return pthread_mutex_lock((pthread_mutex_t*)&Handle) == 0;
	#endif

}

bool Mutex::Unlock()
{

	#ifdef REASON_PLATFORM_WINDOWS
	return ReleaseMutex(Handle);
	#endif

	#ifdef REASON_PLATFORM_POSIX
	return pthread_mutex_unlock((pthread_mutex_t*)&Handle) == 0;
	#endif
}

bool Mutex::Try()
{
	#ifdef REASON_PLATFORM_WINDOWS

	return WaitForSingleObject(Handle,TimeoutImmediate) == 0;
	#endif

	#ifdef REASON_PLATFORM_POSIX

	return pthread_mutex_trylock((pthread_mutex_t*)&Handle) == 0;
	#endif
}

bool Mutex::Wait(int timeout)
{

	if (Lock(timeout))
	{
		Unlock();
		return false;
	}

	return true;
}

void Mutex::Create()
{

	#ifdef REASON_PLATFORM_WINDOWS
	Handle = CreateMutex(0,false,0);
	OutputAssert(Handle != INVALID_HANDLE_VALUE);
	#endif

	#ifdef REASON_PLATFORM_POSIX
	memset(&Handle,0,sizeof(Handle));

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init((pthread_mutex_t*)&Handle,&attr);
	pthread_mutexattr_destroy(&attr);

	#endif
}

void Mutex::Delete()
{
	#ifdef REASON_PLATFORM_WINDOWS
	CloseHandle(Handle);
	Handle = INVALID_HANDLE_VALUE;
	#endif

	#ifdef REASON_PLATFORM_POSIX
	pthread_mutex_destroy((pthread_mutex_t*)&Handle);
	memset(&Handle,0,sizeof(Handle));
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Thread::Thread(Reason::Platform::Runnable * runnable, void * pointer):Runnable(runnable),Context(pointer),Handle(0),Id(0)
{

}

Thread::Thread(Reason::Platform::Runnable & runnable, void * pointer):Runnable(&runnable),Context(pointer),Handle(0),Id(0)
{

}

Thread::Thread(const Thread & thread):Runnable(thread.Runnable),Context(thread.Context),Handle(thread.Handle),Id(thread.Id)
{

}

Thread::Thread():Runnable(0),Context(0),Handle(0),Id(0)
{

}

Thread::~Thread()
{

}

Thread & Thread::operator = (Thread & thread)
{
	Runnable = thread.Runnable;
	Context = thread.Context;
	Handle = thread.Handle;
	Id = thread.Id;
	return *this;
}

void Thread::Sleep(unsigned long milliseconds)
{
	#ifdef REASON_PLATFORM_POSIX

		#ifdef REASON_PLATFORM_APPLE

		struct timespec ts;
		ts.tv_sec = milliseconds/Time::MillisecondsPerSecond;
		ts.tv_nsec = (milliseconds-(ts.tv_sec*Time::MillisecondsPerSecond))/Time::NanosecondsPerMillisecond;
		nanosleep(&ts,0);

		#else

		if ((milliseconds*Time::MicrosecondsPerMillisecond) >= Time::MicrosecondsPerSecond)
		{

			sleep((milliseconds+(Time::MillisecondsPerSecond-1)) / 1000);
		}
		else
		{

			usleep(milliseconds*Time::MicrosecondsPerMillisecond);
		}

		#endif

	#endif
	#ifdef REASON_PLATFORM_WINDOWS
 		::Sleep(milliseconds);
	#endif
}

void Thread::Pause()
{
	#ifdef REASON_PLATFORM_POSIX
		sched_yield();
	#endif
	#ifdef REASON_PLATFORM_WINDOWS
		SleepEx(0,0);
	#endif
}

unsigned long Thread::Identify()
{
	#ifdef REASON_PLATFORM_POSIX

		return (unsigned long)pthread_self();

	#endif
	#ifdef REASON_PLATFORM_WINDOWS

        return (unsigned long)GetCurrentThreadId();
	#endif
}

Thread Thread::Start(Reason::Platform::Runnable * runnable, void * context)
{
	Thread thread(runnable,context);
	thread.Start();
	return thread;
}

void Thread::Start()
{

	unsigned long id=0;
	void * handle = Handle;

	if ((long long)handle > 0)
		return;

	Id = 0;
	Handle = 0;	

	Thread * thread = new Thread(*this);

	#ifdef REASON_PLATFORM_WINDOWS

	#ifdef REASON_PLATFORM_WINDOWS
	unsigned int t = (unsigned int)id;
	handle = (void *) _beginthreadex(0,0,Spawn,thread,0,&t);
	id = t;
	#else
	handle = (void *) _beginthreadex(0,0,Spawn,thread,0,&id);
	#endif

	if (handle == 0)
	{
		OutputError("Thread::Start - Could not create thread.\n");
	}

    if ((long long)Atomic::Cas((volatile void **)&Handle,0,handle) != 0)	
	{

		CloseHandle((void *)handle);

	}

	#endif

	#ifdef REASON_PLATFORM_POSIX

	pthread_attr_t attributes;
	pthread_attr_init(&attributes);	

	pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_JOINABLE);

	#ifdef REASON_PLATFORM_LINUX

	pthread_t t;
	handle = pthread_create (&t,0,Spawn,thread);
	id = t;
	OutputConsole("Thread::Start - pthread_t: %08lX, id: %ld\n",t,id);
	#else
	handle = (void *) pthread_create ((pthread_t *)((void *)&id),&attributes,Spawn,thread);

	#endif

	if (handle == 0)
	{

        #ifdef REASON_PLATFORM_I386
		Atomic::Cas((volatile int *)&Handle,0,1);	
        #else
        Atomic::Cas((volatile long long *)&Handle,0,1);	
        #endif

	}
	else
	{
		OutputError("Thread::Start - Could not create thread.\n");
	}

	pthread_attr_destroy(&attributes);

	#endif

	Id = id;
}

void Thread::Stop()
{	

	if (Thread::Identify() != Id)
		return;

	#ifdef REASON_PLATFORM_WINDOWS

	void * handle = Handle;
	if ((long long)Atomic::Cas((volatile void **)&Handle,handle,(void*)-1) > 0)
	{
		_endthreadex(0);
		CloseHandle((void *)handle);
	}
	#endif

	#ifdef REASON_PLATFORM_POSIX

	void * handle = Handle;
	if ((long long)Atomic::Cas((volatile void **)&Handle,handle,(void*)-1) > 0)
	{
		pthread_cancel((pthread_t)Id);
	}

	#endif

}

void Thread::Kill()
{

	#ifdef REASON_CONFIGURE_MULTI_THREADED

	#ifdef REASON_PLATFORM_WINDOWS

	void * handle = Handle;
	if ((long long)Atomic::Cas((volatile void **)&Handle,handle,(void*)-1) > 0)
	{
		unsigned long exit=0;
		GetExitCodeThread((void *)handle,&exit);
		TerminateThread((void *)handle,exit);
		CloseHandle((void *)handle);
	}

	#endif

	#ifdef REASON_PLATFORM_POSIX

	void * handle = Handle;
	if ((long long)Atomic::Cas((volatile void **)&Handle,handle,(void*)-1) > 0)
	{	
		pthread_cancel((pthread_t)Id);
	}
	#endif

	#endif

}

void Thread::Join()
{

	void * handle = Handle;

	if ((long long)handle <= 0)
		return;

	#ifdef REASON_PLATFORM_WINDOWS

	if ((long long)Atomic::Cas((volatile void **)&Handle,handle,(void*)-1) > 0)
	{
		WaitForSingleObject((void *)handle,INFINITE);
		CloseHandle((void *)handle);	
	}

	#endif

	#ifdef REASON_PLATFORM_POSIX

	if ((long long)Atomic::Cas((volatile void **)&Handle,handle,(void*)-1)  > 0)
	{
		OutputConsole("Thread::Join - Joining thread %ld...\n",Id);
		OutputConsole("Thread::Join - Handle of thread %ld is %ld\n",Id,(void *)Handle);	

		int error = pthread_join((pthread_t)Id,0);
		if (error)
		{			
			OutputConsole("Thread::Join - Could not join thread %ld, %s\n",Id,strerror(error));
		}			
		else
		{
			OutputConsole("Thread::Join - Joined thread %ld\n",Id);
		}
	}

	#endif
}

#ifdef REASON_PLATFORM_WINDOWS
unsigned int Thread::Spawn(void * spawn)
{

	Thread * thread = (Thread*)spawn;

	if (!thread->Handle)
	{

		void * process = (void *) GetCurrentProcess();
		void * pseudo = (void *) GetCurrentThread();
		void * handle = 0;
		DuplicateHandle((void*)process,(void*)pseudo,(void*)process,(void**)&handle,0,false,DUPLICATE_SAME_ACCESS);

		if ((long long)Atomic::Cas((volatile void **)&thread->Handle,0,handle) > 0)
		{
			CloseHandle((void*)handle);
		}
	}

	thread->Id = GetCurrentThreadId();

	if (thread->Runnable)
		thread->Runnable->Run(thread);

	void * handle = thread->Handle;
	if ((long long)Atomic::Cas((volatile void **)&thread->Handle,handle,(void*)-1) > 0)
	{
		CloseHandle((void *)handle);
	}

	delete thread;

	return 0;
}
#endif

#ifdef REASON_PLATFORM_POSIX
void * Thread::Spawn(void * spawn)
{

	Thread * thread = (Thread*)spawn;

	Atomic::Cas((volatile void **)&thread->Handle,(void*)0,(void*)1);

	thread->Id = (unsigned long)pthread_self();

	if (thread->Runnable)
		thread->Runnable->Run(thread);

	void * handle = thread->Handle;
	if ((long long)Atomic::Cas((volatile void **)&thread->Handle,handle,(void*)-1)  > 0)
	{

		int error = pthread_detach((pthread_t)thread->Id);

	}

	OutputConsole("Thread::Spawn - Exiting thread %d\n",thread->Id);

	delete thread;

	return 0;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

Threaded::Threaded()
{

}

Threaded::~Threaded()
{
	Critical.Enter();
	if (Threads.Count > 0)
	{

		Thread::Sleep(100);

		List::Entry * entry = Threads.First;
		while(entry)
		{
			if (entry->Thread.Handle == 0)
			{
				List::Entry * idle = entry;
				entry = entry->Next;
				Threads.Delete(idle);
			}
			else
			{
				entry = entry->Next;
			}

		}

		if (Threads.Count > 0)
		{

			OutputFailure("Threaded::~Threaded - There are still %d child threads active.\n",Threads.Count);

		}
	}
	Critical.Leave();
}

bool Threaded::Lock()
{
	return Critical.Enter();
}

bool Threaded::Unlock()
{
	return Critical.Leave();
}

Thread Threaded::Start()
{
	List::Entry * entry = new List::Entry((Reason::Platform::Runnable*)this);

	Reason::Platform::Critical::Auto mutexAuto(Threads.Critical);			
	Threads.Insert(entry);

	entry->Thread.Start();	
	return entry->Thread;
}

void Threaded::Stop(unsigned int id)
{

	Threads.Critical.Enter();
	List::Entry * entry = Threads.Select(id);
	if (entry) 
	{
		Thread thread = entry->Thread;
		Threads.Delete(entry);
		Threads.Critical.Leave();

		thread.Stop();
	}
	else
	{		
		Threads.Critical.Leave();
	}
}

void Threaded::Kill(unsigned int id)
{

	Threads.Critical.Enter();
	List::Entry * entry = Threads.Select(id);
	if (entry) 
	{
		Thread thread = entry->Thread;
		Threads.Delete(entry);
		Threads.Critical.Leave();

		thread.Kill();
	}
	else
	{		
		Threads.Critical.Leave();
	}
}

void Threaded::Join(unsigned int id)
{

	Threads.Critical.Enter();
	List::Entry * entry = Threads.Select(id);
	if (entry) 
	{
		Thread thread = entry->Thread;
		Threads.Critical.Leave();

		thread.Join();
	}
	else
	{		
		Threads.Critical.Leave();
	}
}

Threaded::List::List():
	First(0),Last(0),Count(0)
{
}

Threaded::List::~List()
{
	Reason::Platform::Critical::Auto mutexAuto(Critical);

	Threaded::List::Entry * entry = First;
	while (entry != 0)
	{
		First = First->Next;

		delete entry;
		entry = First;
	}

	First = Last = 0;

}

void Threaded::List::Insert(Threaded::List::Entry * entry)
{
	Reason::Platform::Critical::Auto mutexAuto(Critical);

	if (Last == 0)
	{
		Last = entry;
		First = entry;
	}
	else
	{
		Last->Next = entry;
		entry->Prev = Last;
		Last = entry;
	}

	++ Count;
}

void Threaded::List::Delete(unsigned int id)
{
	Threaded::List::Entry * entry = Select(id);

	if (entry) Delete(entry);

}

void Threaded::List::Delete(Thread & thread)
{
	Delete(Select(thread));
}

void Threaded::List::Delete(Threaded::List::Entry * entry)
{
	Reason::Platform::Critical::Auto mutexAuto(Critical);

	if (entry == 0)
		return;

	if (entry->Prev != 0)
	{
		entry->Prev->Next = entry->Next;
	}
	else
	{
		First = entry->Next;
	}

	if (entry->Next != 0)
	{
		entry->Next->Prev = entry->Prev;
	}
	else
	{
		Last = entry->Prev;
	}

	delete entry;

	-- Count;
}

Threaded::List::Entry * Threaded::List::Select(unsigned int id)
{
	Reason::Platform::Critical::Auto mutexAuto(Critical);

	Threaded::List::Entry * entry = First;
	while (entry != 0)
	{
		if (entry->Thread.Id == id)
			return entry;

		entry = entry->Next;
	}

	return entry;
}

Threaded::List::Entry * Threaded::List::Select(Thread & thread)
{
	Reason::Platform::Critical::Auto mutexAuto(Critical);

	Threaded::List::Entry * entry = First;
	while (entry != 0)
	{
		if (entry->Thread.Id == thread.Id)
			return entry;

		entry = entry->Next;
	}

	return entry;
}

Reason::Platform::Thread Threaded::List::operator [] (int index) 
{

	Reason::Platform::Critical::Auto mutexAuto(Critical);

	static Reason::Platform::Thread null;

	Entry * entry = 0;
	if (index < Count)
	{ 
		entry = First;
		while(--index > 0)
			entry = entry->Next;
	}

	return (entry)?entry->Thread:null;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Fibre & Fibre::Start(Reason::Messaging::Callback<void, void *> callback, void * context)
{
	Fibre fibre(callback,context);
	return fibre.Start();
}

Fibre::Runner Fibre::Run;

Fibre & Fibre::Start()
{

	if (!Thread.Handle)
	{

		Thread.Runnable = &Run;
		Thread.Context = (void*)new Fibre(*this);	
		Thread.Start();
	}

	return *this;
}

void Fibre::Join()
{
	Thread.Join();
}

void Fibre::Join(Fibre & fibre)
{
	fibre.Join();
}

void Fibre::Stop()
{
	Thread.Stop();
}

void Fibre::Stop(Fibre & fibre)
{
	fibre.Stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Condition::Condition()
{
	#ifdef REASON_PLATFORM_POSIX

		pthread_cond_init(&Handle,0);
	#endif

	#ifdef REASON_PLATFORM_WINDOWS
		#if REASON_PLATFORM_WINVER >= 0x0600
		InitializeConditionVariable(&Handle);
		#endif

	#endif

}

Condition::~Condition()
{
	#ifdef REASON_PLATFORM_POSIX
		pthread_cond_destroy(&Handle);
	#endif

	#ifdef REASON_PLATFORM_WINDOWS
	#endif
}

bool Condition::Wait(unsigned int timeout)
{

	#ifdef REASON_PLATFORM_POSIX

	Critical.Enter();
	struct timespec ts = Timer::Timespec(timeout*Timer::NanosecondsPerMillisecond);	
	int result = (timeout==TimeoutInfinite)?pthread_cond_wait(&Handle,&Critical.Handle):pthread_cond_timedwait(&Handle,&Critical.Handle,&ts);
	Critical.Leave();

	if (result != 0)
	{
		if (result==ETIMEDOUT)
		{
			OutputError("Condition::Wait - Timeout\n");
		}

		return true;
	}

	#endif

	#ifdef REASON_PLATFORM_WINDOWS

	#if REASON_PLATFORM_WINVER >= 0x0600
	Critical.Enter();
	int result = SleepConditionVariableCS(&Handle,&Critical.Handle,timeout);
	if (result == 0)
	{
		int error = GetLastError();
		OutputAssert(error == ERROR_TIMEOUT);
		Critical.Leave();
		return true;
	}
	Critical.Leave();
	#else

	Timer timer;

	int tick = 0;
	int time = timeout;
	int result = 0;

	do 
	{
		tick = (time>100)?100:time;			
		result = WaitForMultipleObjects(2,Handle.Events,false,tick);
		if (result < 2)
			return false;

		if (Atomic::Cas((volatile int*)&Handle.Signal,1,0) == 1)	
			return false;

		time -= tick;

		int error = GetLastError();

		OutputAssert(result == WAIT_TIMEOUT && error == ERROR_SUCCESS);

	}
	while(timer.ElapsedMilliseconds() < timeout && time > 0);

	if (result >= 2)
		return true;

	#endif	
	#endif	

	return false;
}

bool Condition::Signal()
{
	#ifdef REASON_PLATFORM_POSIX

	pthread_cond_signal(&Handle);

	#endif

	#ifdef REASON_PLATFORM_WINDOWS
	#if REASON_PLATFORM_WINVER >= 0x0600
	WakeConditionVariable(&Handle);
	#else	

	Sleep(0);

	SetEvent((HANDLE)Handle.Events[ConditionObject::SIGNAL]);
	Atomic::Cas((volatile int*)&Handle.Signal,0,1);
	#endif
	#endif	

	return true;
}

bool Condition::Broadcast()
{
	#ifdef REASON_PLATFORM_POSIX

	pthread_cond_broadcast(&Handle);

	#endif

	#ifdef REASON_PLATFORM_WINDOWS
	#if REASON_PLATFORM_WINVER >= 0x0600
	WakeAllConditionVariable(&Handle);

	#else

	Sleep(0);
	PulseEvent((HANDLE)Handle.Events[ConditionObject::BROADCAST]);
	#endif	
	#endif	

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Event::EventWait
{
	Reason::Platform::Condition Condition;

	int Index;
	int Status;

	int References;
	bool All;
	bool Waiting;

	EventWait(bool all, bool waiting=false):Index(-1),Status(0),References(0),All(all),Waiting(waiting) {}	

	void Decrement()
	{
		--References;
		OutputAssert(References >= 0);	
	}

	void Increment()
	{
		OutputAssert(References >= 0);	
		++References;
	}
};

struct Event::EventObject
{
	Reason::Platform::Condition Condition;
	bool Manual;
	bool Set;

	Reason::Structure::Intrinsic::Vector<EventWait*> Waiters;

	EventObject(bool manual=false, bool set=false):Manual(manual),Set(set) 
	{

	}

	~EventObject()
	{

		for (int w=0;w<Waiters.Length();++w)
		{			

			EventWait * wait = Waiters[w];

			wait->Condition.Enter();

			wait->Decrement();
			if (wait->References == 0)
				delete wait;		

		}

		Waiters.Release();
	}
};

Event::Event(bool manual, bool set)
{

	Handle = (void*) new EventObject(manual,set);

}

Event::~Event()
{

	EventObject * object = (EventObject*)Handle;
	if (object)
		delete object;

}

bool Event::Reset() 
{
	Reason::Platform::Mutex::Auto mutex(Mutex);

	EventObject * object = (EventObject*)Handle;
	object->Set = false;

	return true;
}

bool Event::Set()
{

	Reason::Platform::Mutex::Auto mutex(Mutex);

	EventObject * object = (EventObject*)Handle;
	object->Set = true;

	for (int w=0;w<object->Waiters.Length();)
	{

		EventWait * wait = object->Waiters[w];

		wait->Condition.Enter();

		wait->Decrement();

		if (!wait->Waiting)
		{	
			wait->Condition.Leave();	

			if (wait->References == 0)
				delete wait;

			if (!object->Manual)
			{

				object->Waiters.Remove(wait,w);
			}
			else
			{

				++w;
			}

			continue;
		}		

		if (!object->Manual)
			object->Set = false;

		if (wait->All)
		{
			--wait->Status;
			OutputAssert(wait->Status >= 0);
		}
		else
		{
			wait->Status = wait->Index;
			wait->Waiting = false;
		}

		wait->Condition.Leave();
		wait->Condition.Signal();

		if (!object->Manual)
		{

			object->Waiters.Remove(wait,w);

			return true;
		}
		else
		{

			++w;
		}
	}

	if (!object->Manual)
	{
		if (object->Set)
		{	

			object->Condition.Signal();
		}
	}
	else
	{

		object->Waiters.Release();			

		object->Condition.Broadcast();		
	}

	return true;

}

bool Event::Pulse()
{
	Reason::Platform::Mutex::Auto mutex(Mutex);

	if (Set())
		return Reset();

	return false;
}

bool Event::Wait(int timeout)
{

	if (timeout == Event::TimeoutImmediate)
	{
		if (!Mutex.Try())
			return true;
	}
	else
	{

		if (!Mutex.Lock(timeout))
			return true;
	}

	bool wait = Event::WaitForEvent(this,timeout);

	Mutex.Unlock();

	return wait;
}

int Event::WaitForSingleEvent(Event * event, int timeout)
{

	if (!event || event->Wait(timeout)) 
		return -1;

	return 0;	
}

int Event::WaitForMultipleEvents(int count, Event * events, bool all, int timeout)
{
	EventWait * wfme = new EventWait(all,true);

	wfme->References = 1;
	wfme->Status = (all)?count:-1;

	wfme->Condition.Enter();

	bool found = false;
	int index = -1;

	for (int i=0;i<count;++i)
	{
		wfme->Index = i;

		events[i].Mutex.Lock();

		EventObject * object = (EventObject*)events[i].Handle;

		for (int w=0;w<object->Waiters.Length();)
		{

			EventWait * wait = object->Waiters[w];

			if (wait->Condition.Try())
			{
				if (!wait->Waiting)
				{
					wait->Decrement();
					wait->Condition.Leave();

					if (wait->References == 0)
						delete wait;						

					object->Waiters.Remove(wait,w);

					continue;
				}

				wait->Condition.Leave();
			}

			++w;
		}

		if (!Event::WaitForEvent(&events[i],timeout))
		{

			events[i].Mutex.Unlock();
			if (all)
			{
				--wfme->Status;
				OutputAssert(wfme->Status >= 0);
			}
			else
			{
				wfme->Status = i;
				found = true;
				break;
			}		
		}
		else
		{
			++wfme->References;
			object->Waiters.Append(wfme);				

			events[i].Mutex.Unlock();
		}
	}

	wfme->Condition.Leave();

	while (!found)
	{

		found = (all && wfme->Status == 0) || (!all && wfme->Status != -1);

		if (!found)
		{

			if (timeout == Event::TimeoutInfinite)
			{
				wfme->Condition.Wait(timeout);
			}
			else
			{
				Timer timer;
				if (wfme->Condition.Wait(timeout))
					timeout -= timer.ElapsedMilliseconds();

				if (timeout <= 0)
					break;
			}			
		}	
	}

	wfme->Condition.Enter();

	if (found)
		index = wfme->Status;

	wfme->Waiting = false;

	bool referenced = wfme->References > 1;

	wfme->Decrement();
	if (wfme->References == 0)
	{
		delete wfme;
	}
	else
	{
		wfme->Condition.Leave();

		if (false)		

		{
			for (int i=0;i<count;++i)
			{

				events[i].Mutex.Lock();			

				EventObject * object = (EventObject*)events[i].Handle;

				for (int w=0;w<object->Waiters.Length();)
				{

					EventWait * wait = object->Waiters[w];	

					if (wait->Condition.Try())
					{						
						if (!wait->Waiting)
						{
							wait->Decrement();
							wait->Condition.Leave();

							if (wait->References == 0)
								delete wait;

							object->Waiters.Remove(wait,w);

							continue;
						}

						wait->Condition.Leave();
					}

					++w;
				}	

				events[i].Mutex.Unlock();
			}		

		}	
	}

	return index;
}

bool Event::WaitForEvent(Event * event, int timeout)
{

	EventObject * object = (EventObject*)event->Handle;
	if (!object->Set)
	{
		if (timeout == Event::TimeoutImmediate)
			return true;

		object->Condition.Wait(timeout);

	}

	if (object->Set)
	{	
		if (!object->Manual)
			object->Set = false;

		return false;
	}

	return true;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Semaphore::Semaphore(int count)
{
#ifdef REASON_PLATFORM_WINDOWS

	Handle = CreateSemaphore(0,0,count,0);
	if (Handle == 0)
	{
		OutputError("Semaphore::Semaphore - Error %d, Could not create semaphore\n",GetLastError());
	}

#endif
#ifdef REASON_PLATFORM_POSIX

	if (sem_init(Handle,0,count) == -1)
	{
		OutputError("Semaphore::Semaphore - Error %d, Could not create semaphore\n",errno);
	}

#endif
}

Semaphore::~Semaphore()
{
#ifdef REASON_PLATFORM_WINDOWS
	CloseHandle(Handle);
#endif
#ifdef REASON_PLATFORM_POSIX

#endif

}

bool Semaphore::Enter()
{
#ifdef REASON_PLATFORM_WINDOWS

	WaitForSingleObject(Handle, INFINITE);
#endif
#ifdef REASON_PLATFORM_POSIX
	sem_post(Handle);
#endif

	return false;
}

bool Semaphore::Leave()
{
#ifdef REASON_PLATFORM_WINDOWS	
	ReleaseSemaphore(Handle,1,0);
#endif
#ifdef REASON_PLATFORM_POSIX
	sem_wait(Handle);
#endif

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Locked::Lock()
{
	Enter();
	if (!Id) Id = Thread::Identify();
	Leave();
	return true;
}

bool Locked::Owned()
{
	return Id == Thread::Identify();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Monitor::Lock()
{

	if (!MonitorId)
	{
		if (Try())
		{
			MonitorId = Thread::Identify();
			return true;		
		}
	}

	return false;
}

bool Monitor::Unlock()
{

	if (MonitorMutex.Try())
	{

		if (MonitorId == Thread::Identify())
		{
			MonitorId = 0;
			Leave();
		}

		MonitorMutex.Leave();
		return true;
	}

	return false;
}

bool Monitor::Owned()
{
	return MonitorId == Thread::Identify();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

