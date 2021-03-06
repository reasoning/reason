
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


#ifndef REASON_H
#define REASON_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define REASON_CONFIGURE_MULTI_THREADED

#define REASON_CONFIGURE_CALLBACK
#define REASON_CONFIGURE_EXCEPTIONS
#define REASON_CONFIGURE_RTTI

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define REASON_USING_APR
#define REASON_USING_APR_UTIL

#define REASON_USING_STL

#define REASON_USING_MYSQL

#define REASON_USING_POSTGRES
#define REASON_USING_LIBIODBC
#define REASON_USING_ODBC

//#define REASON_USING_OPENSSL
//#define REASON_USING_ZLIB
#define REASON_USING_LIBTAR

#define REASON_USING_LIBJPG
#define REASON_USING_LIBGIF

//#define REASON_USING_BERKLEY_DB
//#define REASON_USING_XML_STYLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32) || defined(_WIN64)

    #ifdef _WIN64
        #define REASON_PLATFORM_X86_64
    #else
        #define REASON_PLATFORM_I386
    #endif

	#define REASON_PLATFORM_WINDOWS

	#include "SdkDdkVer.h"

	#if defined(_WIN32_WINNT)
		#define REASON_PLATFORM_WINVER _WIN32_WINNT
	#elif defined(WINVER)
		#define REASON_PLATFORM_WINVER WINVER
	#endif

	#ifndef _WIN32_WINNT

	#define _WIN32_WINNT 0x0501
		#define REASON_PLATFORM_WINVER _WIN32_WINNT
	#endif

	#define _WINSOCKAPI_	
	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>

	#ifdef _WIN32_WINNT_VISTA
		#define REASON_PLATOFRM_VISTA
	#endif

	#ifdef _WIN32_WINNT_WINXP
		#define REASON_PLATOFRM_WINXP
	#endif

	#ifdef _WIN32_WINNT_WIN7
		#define REASON_PLATOFRM_WIN7
	#endif

	#pragma warning(disable: 4355)
	#pragma warning(disable: 4530)	

	#include <stdarg.h>

	#ifndef va_copy
	#define va_copy(d,s) ((d) = (s))
	#endif

	#define _WINSOCKAPI_	
	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>

	#include <winsock2.h>
#else

    #define REASON_PLATFORM_POSIX

    #ifdef __linux__
        #define REASON_PLATFORM_LINUX

    #endif

    #ifdef __APPLE__

        #define REASON_PLATFORM_APPLE
    #endif

    #ifdef __CYGWIN__
        #define REASON_PLATFORM_CYGWIN
    #endif

    #if __x86_64__ || __LP64__
        #define REASON_PLATFORM_X86_64
    #else
        #define REASON_PLATFORM_I386
    #endif

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

#ifdef REASON_STDERR_OVERIDE
	#include <stdio.h>
	extern FILE* f;	
	#define stderr f
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Graphics
{

	class Pixel
	{

	};

	class Font
	{
	public:

	};

	class Jpeg
	{
	public:

	};

	class Gif
	{
	public:

	};

	namespace Svg {

		class Svg
		{
		public:
		};	

		class Line
		{
		};

		class Curve
		{
		};
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Naming
{
public:
	void Interleaved();

	void Bundle();

	void Author();
	void Model();
	void Hub();
	void Defer();
	void Anchoring();
	void Annotate();
	void Paint();
	void Layer();
	void Format();

	void Initial();
	void Latch();
	void Ontology();
	void Taxonomy();
	void Pseudo();
	void Binding();
	void Synthetic();
	void Artificial();
	void Alias();
	void Compound();

	void Directive();
	void Command();
	void Interpreter();
	void Shell();

	void Channel();
	void Single();
	void Interleave();
	void Pipeline();
	void Transport();

	void Phases();
	void Phase();
	void Morph();

	void Update();
	void Factor();
	void Token();
	void View();		
	void Region();		
	void Hierarchical();
	void Facet();
	void Imposter();
	void Interloper();

	class NamingTransformer
	{
	public:
		void Capture();
		void Release();
	};

	void Increment();
	void Decrement();

	void Create(int amount);

	void Ensure(int amount);	

	void Acquire(int amount);	

	void Resize(int amount);

	void Increase(int amount);
	void Decrease(int amount);

	void Capacity(int amount);	
	void Displace(int amount);	

	void Shrink(int size);
	void Grow(int size);

	void Change(int capacity);
	void Reserve(int capacity);
	void Reallocate(int capacity);
	void Allocate(int capacity);

	void Regression();
	void Inspect();
	void Acquire();
	void Accept();
	void Inherit();
	void Assume();
	void Consume();
	void Procure();

	void Clone();
	void Invoke();
	void Instantiate();
	void Prioritise();
	void Focus();
	void Iterate();
	void Unify();
	void Accumulate();

	void Generic();
	void Prototype();
	void Quantum();
	void Aggregate();
	void Unit();
	void Implementation();
	void Interface();
	void Resolver();
	void Adaptor();
	void Particle();
	void Receptor();
	void Serialise();
	void Atomic();

	void Progress();
	void Topology();
	void Fitness();
	void Facade();
	void Factory();
	void Builder();
	void Bridge();
	void Characteristic();

	void Mutable();
	void Reference();
	void Collector();

	void Container();
	void Item();
	void Locator();
	void Reporter();

	void Release();
	void Destroy();

	void Recycle();
	void Dispose();

	void Emitter();
	void Scanner();
	void Recogniser();
	void Synchronise();

	void Mortal();
	void Imortal();
	void Composite();

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

