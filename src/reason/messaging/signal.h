
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
#ifndef MESSAGING_SIGNAL_H
#define MESSAGING_SIGNAL_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/output.h"

#include "reason/structure/iterator.h"
#include "reason/structure/set.h"
#include "reason/platform/thread.h"
#include "reason/messaging/callback.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Platform;
using namespace Reason::Structure;

namespace Reason { namespace Messaging {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Return_, typename _Arg1_=None,typename _Arg2_=None,typename _Arg3_=None,typename _Arg4_=None,typename _Arg5_=None>
class Signal;

template <typename _Return_, typename _Arg1_=None,typename _Arg2_=None,typename _Arg3_=None,typename _Arg4_=None,typename _Arg5_=None>
class Slot;

template <typename _Return_, typename _Arg1_=None, typename _Arg2_=None, typename _Arg3_=None, typename _Arg4_=None, typename _Arg5_=None>
class Connector
{
public:

	Messaging::Signal<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * Signal;
	Messaging::Slot<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * Slot;

	Connector(const Connector & connector);
	Connector(Messaging::Slot<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * slot=0);
	~Connector();

	bool Connect();
	bool Disconnect();

	_Return_ operator() ();
	_Return_ operator() (_Arg1_ arg1);
	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2);
	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3);
	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4);
	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4, _Arg5_ arg5);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Return_, typename _Arg1_,typename _Arg2_,typename _Arg3_,typename _Arg4_,typename _Arg5_>
class Signal
{
public:

	Reason::Messaging::Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> Connector;

	Signal() {}

	operator bool ()
	{
		return this->Connector.Signal != 0;
	}

	bool Connect(Reason::Messaging::Slot<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> & slot)
	{

		if (this->Connector.Slot != 0 && this->Connector.Slot != &slot)
			this->Connector.Disconnect();

		this->Connector.Slot = &slot;
		this->Connector.Signal = this;
		this->Connector.Connect();
		return true;
	}

	bool Connect()
	{

		if (this->Connector.Slot)
		{
			this->Connector.Signal = this;
			return true;
		}

		return false;
	}

	bool Disconnect(Reason::Messaging::Slot<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> & slot)
	{

		if (this->Connector.Slot != 0 && this->Connector.Slot == &slot)
		{
			this->Connector.Disconnect();
			return true;
		}

		return false;
	}

	bool Disconnect()
	{

		if (this->Connector.Slot)
		{
			this->Connector.Signal = 0;
			return true;
		}

		return false;
	}

	_Return_ operator() ()
	{
		return this->Connector();
	}

	_Return_ operator() (_Arg1_ arg1)
	{
		return this->Connector(arg1);
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2)
	{
		return this->Connector(arg1,arg2);
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3)
	{
		return this->Connector(arg1,arg2,arg3);
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4)
	{
		return this->Connector(arg1,arg2,arg3,arg4);
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4, _Arg5_ arg5)
	{
		return this->Connector(arg1,arg2,arg3,arg4,arg5);
	}

};

template <typename _Return_, typename _Arg1_,typename _Arg2_,typename _Arg3_,typename _Arg4_,typename _Arg5_>
class Slot
{
public:

	Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> Callback;

	Reason::Structure::Set<Reason::Messaging::Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * > Connections;

	Reason::Platform::Critical Mutex;

	Slot()
	{

	}

	~Slot()
	{

		Mutex.Enter();

		Reason::Structure::Iterator<Reason::Messaging::Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * > iterator;
		iterator.Iterate(Connections.Forward());

		for (;iterator.Has();iterator.Move())
		{
			iterator()->Slot = 0;
		}

		Connections.Release();

		Mutex.Leave();
	}

	template<typename _Class_>
	Slot(_Class_ * that, _Return_ (_Class_::*call)()):Callback(that,call) {}

	template<typename _Class_>
	Slot(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_)):Callback(that,call) {}

	template<typename _Class_>
	Slot(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_, _Arg2_)):Callback(that,call) {}

	template<typename _Class_>
	Slot(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_, _Arg2_, _Arg3_)):Callback(that,call) {}

	Slot(_Return_ (*call)()):Callback(call) {}
	Slot(_Return_ (*call)(_Arg1_)):Callback(call) {}
	Slot(_Return_ (*call)(_Arg1_,_Arg2_)):Callback(call) {}
	Slot(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_)):Callback(call) {}
	Slot(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)):Callback(call) {}
	Slot(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)):Callback(call) {}

	operator bool () {return this->Callback.Type == 0;}

	template<typename _Class_>
	void operator() (_Class_ * that, _Return_ (_Class_::*call)()) {this->Callback = Reason::Messaging::Callback<_Return_>(that,call);}

	template<typename _Class_>
	void operator() (_Class_ * that, _Return_ (_Class_::*call)(_Arg1_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_>(that,call);}

	template<typename _Class_>
	void operator() (_Class_ * that, _Return_ (_Class_::*call)(_Arg1_, _Arg2_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_>(that,call);}

	template<typename _Class_>
	void operator() (_Class_ * that, _Return_ (_Class_::*call)(_Arg1_, _Arg2_, _Arg3_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_>(that,call);}

	template<typename _Class_>
	void operator() (_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_>(that,call);}

	template<typename _Class_>
	void operator() (_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>(that,call);}

	void operator() (_Return_ (*call)()) {this->Callback = Reason::Messaging::Callback<_Return_>(call);}
	void operator() (_Return_ (*call)(_Arg1_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_>(call);}
	void operator() (_Return_ (*call)(_Arg1_,_Arg2_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_>(call);}
	void operator() (_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_>(call);}
	void operator() (_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_>(call);}
	void operator() (_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)) {this->Callback = Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>(call);}

	void Connect(Reason::Messaging::Signal<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> & signal)
	{

		signal.Connect(*this);
	}

	void Connect()
	{

		Reason::Structure::Iterator<Reason::Messaging::Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * > iterator;
		for (iterator = Connections.Forward();iterator.Has();iterator.Move())
		{	
			iterator()->Connect();
		}
	}

	void Disconnect(Reason::Messaging::Signal<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> & signal)
	{

		signal.Disconnect(*this);
	}

	void Disconnect()
	{

		Reason::Structure::Iterator<Reason::Messaging::Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * > iterator;
		for (iterator = Connections.Forward();iterator.Has();iterator.Move())
		{	
			iterator()->Disconnect();
		}
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::Connector(const Connector & connector):
	Slot(connector.Slot),Signal(connector.Signal)
{

}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::Connector(Messaging::Slot<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> * slot=0):
	Slot(slot),Signal(0)
{

}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::~Connector()
{
	Disconnect();
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
bool Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::Disconnect()
{
	if (Slot)
	{
		Slot->Mutex.Enter();
		Slot->Connections.Remove(this);
		Slot->Mutex.Leave();

		Slot = 0;
		Signal = 0;

		return true;
	}

	Signal = 0;
	return false;
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
bool Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::Connect()
{
	if (Slot)
	{
		Slot->Mutex.Enter();
		Slot->Connections.Insert(this);
		Slot->Mutex.Leave();
		return true;
	}

	return false;
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
_Return_ Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::operator() ()
{
	if (this->Signal == 0)
	{

		return (_Return_)0;		
	}

	OutputAssert(this->Slot != 0);
	return this->Slot->Callback();
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
_Return_ Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::operator() (_Arg1_ arg1)
{
	if (this->Signal == 0)
	{

		return Null<_Return_>::Value();

	}

	OutputAssert(this->Slot != 0);
	return this->Slot->Callback(arg1);
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
_Return_ Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::operator() (_Arg1_ arg1, _Arg2_ arg2)
{
	if (this->Signal == 0)
	{

		return (_Return_)0;
	}

	OutputAssert(this->Slot != 0);
	return this->Slot->Callback(arg1,arg2);
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
_Return_ Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3)
{
	if (this->Signal == 0)
	{

		return (_Return_)0;
	}

	OutputAssert(this->Slot != 0);
	return this->Slot->Callback(arg1,arg2,arg3);
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
_Return_ Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4)
{
	if (this->Signal == 0)
	{

		return (_Return_)0;
	}

	OutputAssert(this->Slot != 0);
	return this->Slot->Callback(arg1,arg2,arg3,arg4);
}

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_, typename _Arg5_>
_Return_ Connector<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_>::operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4, _Arg5_ arg5)
{
	if (this->Signal == 0)
	{

		return (_Return_)0;
	}

	OutputAssert(this->Slot != 0);
	return this->Slot->Callback(arg1,arg2,arg3,arg4,arg5);
}

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

