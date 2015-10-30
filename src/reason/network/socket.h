
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
#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <errno.h>

#include "reason/reason.h"
#include "reason/structure/list.h"
#include "reason/structure/array.h"
#include "reason/system/exception.h"
#include "reason/platform/platform.h"
#include "reason/system/time.h"

#ifdef REASON_PLATFORM_WINDOWS

#endif

#ifdef REASON_PLATFORM_POSIX

	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/types.h>

	#include <fcntl.h>
	#include <unistd.h>

	#include <string.h> 

	#include <netdb.h>

	#ifndef REASON_PLATFORM_CYGWIN
	#include <resolv.h>
	#endif

#endif

using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace Network {
	class Url;
}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Network {

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

class Address;

class Sockets
{
public:

	enum SocketStates
	{
		SOCKET_STATE_ALL			=(~0),
		SOCKET_STATE_NONE			=(0),
		SOCKET_STATE_CONNECTED		=(1),
		SOCKET_STATE_NOT_CONNECTED	=(1)<<1,

		SOCKET_STATE_DISCONNECTED	=(1)<<3,
		SOCKET_STATE_LISTENING		=(1)<<4,
		SOCKET_STATE_ACCEPTING		=(1)<<5,
		SOCKET_STATE_READABLE		=(1)<<6,
		SOCKET_STATE_WRITEABLE		=(1)<<7,

		SOCKET_STATE_BOUND			=(1)<<8,
		SOCKET_STATE_ERROR			=(1)<<9,
		SOCKET_STATE_NOT_READABLE	=(1)<<10,
		SOCKET_STATE_NOT_WRITABLE	=(1)<<11,
		SOCKET_STATE_TIMEOUT		=(1)<<12,

	};

	enum SocketFamily
	{
		#ifdef REASON_PLATFORM_WINDOWS

		SOCKET_FAMILY_UNSPEC		= AF_UNSPEC,	
		SOCKET_FAMILY_UNIX			= AF_UNIX,		
		SOCKET_FAMILY_INET			= AF_INET,		
		SOCKET_FAMILY_IMPLINK		= AF_IMPLINK,	
		SOCKET_FAMILY_PUP			= AF_PUP,		
		SOCKET_FAMILY_CHAOS			= AF_CHAOS,		
		SOCKET_FAMILY_IPX			= AF_IPX,		
		SOCKET_FAMILY_NS			= AF_NS,		
		SOCKET_FAMILY_ISO			= AF_ISO,		
		SOCKET_FAMILY_OSI			= AF_OSI,		
		SOCKET_FAMILY_ECMA			= AF_ECMA,		
		SOCKET_FAMILY_DATAKIT		= AF_DATAKIT,	
		SOCKET_FAMILY_CCITT			= AF_CCITT,		
		SOCKET_FAMILY_SNA			= AF_SNA,		
		SOCKET_FAMILY_DECNET		= AF_DECnet,	
		SOCKET_FAMILY_DLI			= AF_DLI,		
		SOCKET_FAMILY_LAT			= AF_LAT,		
		SOCKET_FAMILY_HYLINK		= AF_HYLINK,	
		SOCKET_FAMILY_APPLETALK		= AF_APPLETALK,	
		SOCKET_FAMILY_NETBIOS		= AF_NETBIOS,	
		SOCKET_FAMILY_VOICEVIEW		= AF_VOICEVIEW,	
		SOCKET_FAMILY_FIREFOX		= AF_FIREFOX,	
		SOCKET_FAMILY_UNKNOWN1		= AF_UNKNOWN1,	
		SOCKET_FAMILY_BAN			= AF_BAN,		
		SOCKET_FAMILY_ATM			= AF_ATM,		
		SOCKET_FAMILY_INET6			= AF_INET6,		
		SOCKET_FAMILY_CLUSTER		= AF_CLUSTER,	
		SOCKET_FAMILY_12844			= AF_12844,		
		SOCKET_FAMILY_IRDA			= AF_IRDA,		
		SOCKET_FAMILY_NETDES		= AF_NETDES,	
		SOCKET_FAMILY_TCNPROCESS	= AF_TCNPROCESS,
		SOCKET_FAMILY_TCNMESSAGE	= AF_TCNMESSAGE,
		SOCKET_FAMILY_ICLFXBM		= AF_ICLFXBM,

		#else

		#ifdef REASON_PLATFORM_POSIX

    #ifdef REASON_PLATFORM_APPLE
		SOCKET_FAMILY_UNSPEC		= AF_UNSPEC,
		SOCKET_FAMILY_LOCAL			= AF_LOCAL,
		SOCKET_FAMILY_UNIX			= AF_UNIX,
		SOCKET_FAMILY_INET			= AF_INET,
		SOCKET_FAMILY_IPX			= AF_IPX,
		SOCKET_FAMILY_APPLETALK		= AF_APPLETALK,
		SOCKET_FAMILY_INET6			= AF_INET6,
		SOCKET_FAMILY_DECNET		= AF_DECnet,
		SOCKET_FAMILY_ROUTE			= AF_ROUTE,
		SOCKET_FAMILY_SNA			= AF_SNA,
		SOCKET_FAMILY_MAX			= AF_MAX,   
		SOCKET_FAMILY_LINK			= AF_LINK, 
    #else

		#ifdef REASON_PLATFORM_CYGWIN		
		SOCKET_FAMILY_UNSPEC		= AF_UNSPEC,
		SOCKET_FAMILY_LOCAL			= AF_LOCAL,
		SOCKET_FAMILY_UNIX			= AF_UNIX,
		SOCKET_FAMILY_INET			= AF_INET,

		#else
		SOCKET_FAMILY_UNSPEC		= AF_UNSPEC,
		SOCKET_FAMILY_LOCAL			= AF_LOCAL,
		SOCKET_FAMILY_UNIX			= AF_UNIX,
		SOCKET_FAMILY_FILE			= AF_FILE,
		SOCKET_FAMILY_INET			= AF_INET,
		SOCKET_FAMILY_AX25			= AF_AX25,
		SOCKET_FAMILY_IPX			= AF_IPX,
		SOCKET_FAMILY_APPLETALK		= AF_APPLETALK,
		SOCKET_FAMILY_NETROM		= AF_NETROM,
		SOCKET_FAMILY_BRIDGE		= AF_BRIDGE,
		SOCKET_FAMILY_ATMPVC		= AF_ATMPVC,
		SOCKET_FAMILY_X25			= AF_X25,
		SOCKET_FAMILY_INET6			= AF_INET6,
		SOCKET_FAMILY_ROSE			= AF_ROSE,
		SOCKET_FAMILY_DECNET		= AF_DECnet,
		SOCKET_FAMILY_NETBEUI		= AF_NETBEUI,
		SOCKET_FAMILY_SECURITY		= AF_SECURITY,
		SOCKET_FAMILY_KEY			= AF_KEY,
		SOCKET_FAMILY_NETLINK		= AF_NETLINK,
		SOCKET_FAMILY_ROUTE			= AF_ROUTE,
		SOCKET_FAMILY_PACKET		= AF_PACKET,
		SOCKET_FAMILY_ASH			= AF_ASH,
		SOCKET_FAMILY_ECONET		= AF_ECONET,
		SOCKET_FAMILY_ATMSVC		= AF_ATMSVC,
		SOCKET_FAMILY_SNA			= AF_SNA,
		SOCKET_FAMILY_IRDA			= AF_IRDA,
		SOCKET_FAMILY_PPPOX			= AF_PPPOX,
		SOCKET_FAMILY_WANPIPE		= AF_WANPIPE,
		SOCKET_FAMILY_BLUETOOTH		= AF_BLUETOOTH,
		SOCKET_FAMILY_MAX			= AF_MAX,
		#endif

    #endif

		#endif

		#endif
	};

	enum SocketModes
	{
		SOCKET_MODE_SYNCHRONOUS,
		SOCKET_MODE_ASYNCHRONOUS,
	};

	enum SocketTypes
	{
		SOCKET_TYPE_STREAM		= SOCK_STREAM,	
		SOCKET_TYPE_DATAGRAM	= SOCK_DGRAM,	
		SOCKET_TYPE_RAW			= SOCK_RAW,		
	};

	enum SocketProtocols
	{
		SOCKET_PROTOCOL_ICMP	= IPPROTO_ICMP,	
		SOCKET_PROTOCOL_IGMP	= IPPROTO_IGMP,	
		SOCKET_PROTOCOL_TCP		= IPPROTO_TCP,	
		SOCKET_PROTOCOL_UDP		= IPPROTO_UDP,	
	};

	#ifdef REASON_PLATFORM_WINDOWS
	enum SocketOperations
	{
		SOCKET_OPERATION_READ		= FD_READ,
		SOCKET_OPERATION_WRITE		= FD_WRITE,
		SOCKET_OPERATION_CONNECT	= FD_CONNECT,
		SOCKET_OPERATION_ACCEPT		= FD_ACCEPT,
	};
	#else
	enum SocketOperations
	{
		SOCKET_OPERATION_READ,
		SOCKET_OPERATION_WRITE,
		SOCKET_OPERATION_CONNECT,
		SOCKET_OPERATION_CLOSE,
		SOCKET_OPERATION_ACCEPT
	};
	#endif

	enum SocketSelectors
	{
		SOCKET_SELECT_READ		=1,
		SOCKET_SELECT_WRITE		=2,
		SOCKET_SELECT_ERROR		=4,
		SOCKET_SELECT_MUTABLE	=SOCKET_SELECT_READ|SOCKET_SELECT_WRITE,
		SOCKET_SELECT_ALL		=SOCKET_SELECT_READ|SOCKET_SELECT_WRITE|SOCKET_SELECT_ERROR,
	};

	#ifdef REASON_PLATFORM_WINDOWS

	static const int SocketError = -1;
	static const int SocketInvalid = ~(0);

	enum SocketErrors
	{
		SOCKET_ERROR_INVALID_DESCRIPTOR		=EBADF,
		SOCKET_ERROR_INVALID_ARGUMENT		=WSAEINVAL,
		SOCKET_ERROR_INVALID_SOCKET			=WSAENOTSOCK,
		SOCKET_ERROR_BLOCKING				=WSAEWOULDBLOCK,
		SOCKET_ERROR_PENDING				=WSAEWOULDBLOCK,
		SOCKET_ERROR_CONNECTION_REFUSED		=WSAECONNREFUSED,
		SOCKET_ERROR_TIMEDOUT				=WSAETIMEDOUT,
		SOCKET_ERROR_NOT_CONNECTED			=WSAENOTCONN,

	};
	#endif

	#ifdef REASON_PLATFORM_POSIX

	static const int SocketError = -1;
	static const int SocketInvalid = -1;

	enum SocketErrors
	{
		SOCKET_ERROR_INVALID_DESCRIPTOR		=EBADF,
		SOCKET_ERROR_INVALID_ARGUMENT		=EINVAL,
		SOCKET_ERROR_INVALID_SOCKET			=ENOTSOCK,
		SOCKET_ERROR_BLOCKING				=EAGAIN,
		SOCKET_ERROR_PENDING				=EINPROGRESS,
		SOCKET_ERROR_CONNECTION_REFUSED		=ECONNREFUSED,
		SOCKET_ERROR_TIMEDOUT				=ETIMEDOUT,
		SOCKET_ERROR_NOT_CONNECTED			=ENOTCONN,

	};
	#endif

	static const unsigned int TimeoutDefault = 30000;
	static const unsigned int TimeoutImmediate = 0;
	static const unsigned int TimeoutIndefinate = 0xFFFFFFFF;

	class SocketsSentinel
	{
	public:
		SocketsSentinel()
		{
			Sockets::Startup();
		}

		~SocketsSentinel()
		{
			Sockets::Shutdown();
		}
	};

	static const SocketsSentinel Library;

	static void Startup();
	static void Shutdown();

	static bool Active;

	static const char * ResolveHostname(const unsigned long address);
	static const char * ResolveHostname(const char * address);
	static const char * ResolveHostname(const sockaddr_in & address);
	static const char * ResolveHostname(const sockaddr & address);
	static const char * ResolveHostname(Address &address);
	static const char * ResolveHostname();

	static unsigned long ResolveAddress(const char * hostname);
	static unsigned long ResolveAddress();

	static const char * ResolveIp(const char * hostname);
	static const char * ResolveIp();

	static unsigned short ResolveService(const char * service, const char * protocol=0);

	static const char * ResolveService(const int port, const char * protocol=0);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Address : public sockaddr
{
public:

	static const int Sizeof;

	enum NetworkType
	{
		NETWORK_TYPE_CLASS_A,
		NETWORK_TYPE_CLASS_B,
		NETWORK_TYPE_CLASS_C,
		NETWORK_TYPE_LOCALHOST,		
		NETWORK_TYPE_PRIVATE,		
	};

	enum AddressType
	{
		ADDRESS_TYPE_ANY		= INADDR_ANY,
		ADDRESS_TYPE_LOOPBACK	= INADDR_LOOPBACK,
		ADDRESS_TYPE_BRAODCAST	= INADDR_BROADCAST,
		ADDRESS_TYPE_NONE		= INADDR_NONE,
	};

	Address();
	Address(const Url &url);
	Address(const sockaddr & address);
	Address(const sockaddr_in & address);
	Address(const unsigned long address, const unsigned short port=0);
	Address(const Sequence &host, const unsigned short port=0);
	Address(const char* host, const unsigned short port=0);
	Address(const Address & address);
	~Address();

	bool Construct();
	bool Construct(const Url &url);
	bool Construct(const Sequence & host, const int port=0);
	bool Construct(const char * host, const int port=0);
	bool Construct(const unsigned long host,const int port = 0);
	bool Construct(const unsigned long host,const unsigned short port = 0);
	bool Construct(Address & address) {return Construct(address.Host(),address.Port());}

	#ifdef REASON_PLATFORM_WINDOWS
	int *  Size() const {return (int * ) &Sizeof;};
	#else
	socklen_t * Size() const {return (socklen_t*) &Sizeof;};
	#endif

	bool IsEmpty(){ return ((sockaddr_in*)this)->sin_addr.s_addr ==  0; };

	int Family()
	{
		return ((sockaddr*)this)->sa_family;
	}

	int Port()
	{

		return ntohs(((sockaddr_in*)this)->sin_port);
	}

	void Port(const int port)
	{

		((sockaddr_in*)this)->sin_port = htons(port);
	}

	void Port(const unsigned short port)
	{

		((sockaddr_in*)this)->sin_port = port;
	}

	void Port(const char * port)
	{
		((sockaddr_in*)this)->sin_port = htons(atoi(port));
	}

	void Print(String & string)
	{
		string = Print();
	}

	char * Print()
	{

		return inet_ntoa(((sockaddr_in*)this)->sin_addr);
	}

	char * Ip()
	{
		return inet_ntoa(((sockaddr_in*)this)->sin_addr);
	}

	unsigned long  Host()
	{

		return ((sockaddr_in*)this)->sin_addr.s_addr;
	}

	void Host(const char * host);
	void Host(const unsigned long host)
	{

		((sockaddr_in*)this)->sin_addr.s_addr = host;
	}

	#ifdef REASON_PLATFORM_POSIX
	#ifdef REASON_PLATFORM_CYGWIN
	#else
	void Host(const in_addr_t address)
	{
		((sockaddr_in*)this)->sin_addr.s_addr = address;
	}
	#endif
	#endif

	unsigned long ResolveAddress() {return Host();}
	char * ResolveHostname();

	int Network();

	const Address& operator=(const sockaddr & sa)		{memcpy((sockaddr *)this,&sa,sizeof(sockaddr));return *this;};
	const Address& operator=(const sockaddr_in & sin)	{memcpy((sockaddr_in *)this,&sin,sizeof(sockaddr_in));return *this;};
	const Address& operator=(Address & address)			{memcpy((sockaddr *)this,(sockaddr*)&address,sizeof(sockaddr));return *this;};

	bool operator == (const sockaddr & sa)		{return memcmp((sockaddr *)this,&sa,sizeof(sockaddr)) == 0;}
	bool operator == (const sockaddr_in & sin)	{return memcmp((sockaddr_in *)this,&sin,sizeof(sockaddr_in)) == 0;}
	bool operator == (Address & address)		{return memcmp((sockaddr*)this,(sockaddr*)&address,sizeof(sockaddr)) == 0;}

	bool operator != (const sockaddr & sa)		{return !operator == (sa);}
	bool operator != (const sockaddr_in & sin)	{return !operator == (sin);}
	bool operator != (Address & address)		{return !operator == (address);}

	#ifdef REASON_PLATFORM_WINDOWS
		operator SOCKADDR()			{return *((LPSOCKADDR) this);}		
		operator LPSOCKADDR()		{return (LPSOCKADDR) this;}			
		operator LPSOCKADDR_IN()	{return (LPSOCKADDR_IN) this;}		
	#endif

	#ifdef REASON_PLATFORM_POSIX
		operator sockaddr()			{return *((sockaddr *) this);}		
		operator sockaddr *()		{return (sockaddr *) this;}			
		operator sockaddr_in *()	{return (sockaddr_in *) this;}		
	#endif

};

class Ipv4Address : public Address
{

};

class Ipv6Address : public Address
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Host
{
public:

	String Hostname;

	Reason::Structure::Array<String> Alias;
	Reason::Structure::Array<Reason::Network::Address> Address;

	Host();
	Host(const Url &url);
	Host(const Sequence & sequence);
	Host(const char * data);
	Host(char * data, int size);
	Host(const Host & host);
	~Host();

	bool Construct(const Url &url);
	bool Construct(const Sequence & sequence) {return Construct(sequence.Data,sequence.Size);}
	bool Construct(const char * data) {return Construct((char*)data,String::Length(data));}
	bool Construct(char * data, int size);

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

class Socket;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SocketFilter
{
public:
	enum Filter
	{
		FILTER_INPUT,
		FILTER_OUTPUT,
	};

	SocketFilter():Sequence(0){}

	virtual void Process(String &string){};

	bool IsSingleton();		
	bool IsSequence();		

	SocketFilter *Sequence;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Socket
{
public:

	Bitmask16			State;
	char				Mode;

	unsigned int Handle;

	Socket(const Socket & socket);
	Socket(char mode);

	Socket();
	~Socket();

	Socket & operator = (const Socket & socket);

	bool IsAsynchronous()	{return Mode == Sockets::SOCKET_MODE_ASYNCHRONOUS;}
	bool IsSynchronous()	{return Mode == Sockets::SOCKET_MODE_SYNCHRONOUS;}

	bool IsReadable(unsigned int timeout);
	bool IsWriteable(unsigned int timeout);

	bool IsReadable()		{return IsReadable(Sockets::TimeoutImmediate);}
	bool IsWriteable()		{return IsWriteable(Sockets::TimeoutImmediate);}

	bool IsError()		{return State.Is(Sockets::SOCKET_STATE_ERROR);}
	bool IsTimeout()		{return State.Is(Sockets::SOCKET_STATE_TIMEOUT);}

	bool IsCreated()		{return Handle != Sockets::SocketInvalid;}
	bool IsDisconnected()	{return !IsConnected();}
	bool IsBound()			{return State.Is(Sockets::SOCKET_STATE_BOUND);}
	bool IsListening()		{return State.Is(Sockets::SOCKET_STATE_LISTENING);}

	bool IsConnected(unsigned int timeout);
	bool IsConnected()		{return IsConnected(Sockets::TimeoutImmediate);}

	virtual bool Create(int family = Sockets::SOCKET_FAMILY_INET, int type = Sockets::SOCKET_TYPE_STREAM, int protocol = Sockets::SOCKET_PROTOCOL_TCP);
	virtual bool Release();
	virtual bool Destroy();

	virtual bool Connect(const Address &address) {return Connect((sockaddr&)address);}
	virtual bool Connect(const sockaddr_in & address) {return Connect((sockaddr&)address);}
	virtual bool Connect(const sockaddr & address);

	virtual bool Accept(Socket &socket, Address &address) {return Accept(socket,(sockaddr&)address);}
	virtual bool Accept(Socket &socket, const sockaddr_in & address) {return Accept(socket,(sockaddr&)address);}
	virtual bool Accept(Socket &socket, const sockaddr & address);

	virtual bool Bind(const Address &address) {return Bind((sockaddr&)address);}
	virtual bool Bind(const sockaddr_in & address) {return Bind((sockaddr&)address);}
	virtual bool Bind(const sockaddr & address);

	virtual bool Disconnect();
	virtual bool Listen();

	virtual bool Select(int selector, unsigned int timeout);
	virtual bool Select(int selector) {return Select(selector,Sockets::TimeoutImmediate);};

	virtual int Send(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);
	virtual int Receive(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);

	int Error();

	int Read(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);
	int Write(char* data, int size, unsigned int timeout=Sockets::TimeoutDefault);

	void RemoteAddress(const Address &address) {return RemoteAddress((sockaddr&)address);}
	void RemoteAddress(const sockaddr_in & address) {return RemoteAddress((sockaddr&)address);}
	void RemoteAddress(const sockaddr & address);	

	void LocalAddress(const Address &address) {return LocalAddress((sockaddr&)address);}
	void LocalAddress(const sockaddr_in & address) {return LocalAddress((sockaddr&)address);}
	void LocalAddress(const sockaddr & address);	

	void Modulate(char mode);		

	void Set(int level, int option, bool value) {Set(level,option,(char*)&value,sizeof(bool));}
	void Set(int level, int option, int value) {Set(level,option,(char*)&value,sizeof(int));}
	void Set(int level, int option, char * data, const int size);

	void Get(int level, int option, bool &value) {int size=sizeof(bool);Get(level,option,(char*)&value,size);}
	void Get(int level, int option, int &value) {int size=sizeof(int);Get(level,option,(char*)&value,size);}
	void Get(int level, int option, char * data, int & size);	

	int Type();
	int Protocol();
	int Family();

protected:

	void Readable(bool read);
	void Writeable(bool write);
	void Error(bool error);
	void Timeout(bool timeout);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SocketSet : public fd_set
{
public:

	SocketSet();
	~SocketSet();

	void Append(Socket &socket);
	void Remove(Socket &socket);
	void Release();

	bool Contains(Socket &socket);	
	int Size();

	operator fd_set &()	{ return *((fd_set *)this); };
	operator fd_set *()	{ return (fd_set *) this; };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Connection
{
public:

	Reason::Network::Socket Socket;
	Reason::Network::Address Address;

	Connection(char mode):Socket(mode) {}
	Connection(const Connection & connection):Address(connection.Address),Socket(connection.Socket) {}
	Connection(const Reason::Network::Address & address, const Reason::Network::Socket & socket):Address(address),Socket(socket) {}
	Connection(const Reason::Network::Address & address):Address(address){}
	Connection() {}
	virtual ~Connection() {}

	void Construct(Reason::Network::Address & address)
	{
		Address.Construct(address);
		Socket.Destroy();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SocketOperation
{
public:

	Bitmask8 Type;

	char *		Data;
	const int	Size;
	const int	Timeout;

	Reason::System::Timer	Timer;

	SocketOperation();

	SocketOperation(char *data, const int size, const unsigned int timeout);

	virtual ~SocketOperation();
	virtual bool Process(Connection & connection);
	virtual bool Compare(SocketOperation *operation);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SocketSession :  protected Reason::Structure::List<SocketOperation*>
{
public:

	Reason::Network::Connection * Connection;

	SocketSession(Reason::Network::Connection * connection):Connection(connection){};
	SocketSession():Connection(0){};
	~SocketSession(){};

	bool Process();	
	bool Pending() {return !IsEmpty();};
	bool Pending(SocketOperation *operation);

	void Enqueue(SocketOperation *operation, bool immediate = false);
	void Dequeue(SocketOperation *operation);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SocketManager
{
public:

	Reason::Network::Socket	*Socket;	

	SocketSet Readable;
	SocketSet Writeable;

	timeval		Timeout;

	SocketManager();
	~SocketManager();

	void Connect(Address &from, Address &to);
	void Process();

	void Read();
	void Print();

private:

	bool Update();	
	bool Select();	

	 Reason::Structure::List<SocketSession*>	Sessions;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TcpSocket : public Socket
{

};

class Datagram
{

};

class UdpSocket : public Socket
{
public:

	UdpSocket();
	~UdpSocket();

	int Send(Address & dst, const char * data, const int size);
	int Receive(Address & src, char * data, const int size);

	void Broadcast(bool enable = true);
	void MulticastTimeToLive(unsigned char timeToLive);
	void MulticastJoinGroup(const char * multicastGroup);
	void MulticastLeaveGroup(const char * multicastGroup);
};

class SslSocket : public Socket
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Channel
{
};

class Receiver
{

};

class Listener
{

};

class SocketTimed : public Socket
{
public:

	Timer SendTimer;			
	Timer ReceiveTimer;		

	bool Timeout();

	virtual int Send(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);
	virtual int Receive(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);

	int Read(char * data, int size, unsigned int timeout);
	int Write(char * data, int size, unsigned int timeout);

};

class SocketRestricted : public Socket
{
public:

	SocketRestricted();
	~SocketRestricted();

	long SendBytesCount;     
	long ReceiveBytesCount;  

	virtual int Send(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);
	virtual int Receive(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);

};

class SocketObserver
{
public:

	void OnCreate();
	void OnDestroy();

	void OnConnect();
	void OnDisconnect();

	void OnAccept();

	void OnSend(const int bytes);
	void OnReceive(const int bytes);

};

class SocketWatched : protected Socket
{
public:

	Reason::Structure::List<SocketObserver*> Observers;

	SocketWatched();
	~SocketWatched();

	void Register(SocketObserver *observer);
	void Deregister(SocketObserver *observer);

	virtual bool Create(int type = Sockets::SOCKET_TYPE_STREAM, int protocol = Sockets::SOCKET_PROTOCOL_TCP);
	virtual bool Destroy();
	virtual bool Connect(Address &address);
	virtual bool Disconnect();
	virtual bool Accept(Socket &socket, Address &address);
	virtual bool Bind(Address &address);
	virtual bool Select();
	virtual bool Listen();

	virtual int Send(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);
	virtual int Receive(char * data, int size, unsigned int timeout=Sockets::TimeoutDefault);

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

class Interface
{
public:

	struct IfAddress : public String 
	{

	};

	struct Ipv4Address : public IfAddress
	{

	};

	struct Ipv6Address : public IfAddress
	{

	};

	struct MacAddress : public IfAddress
	{

	};

	struct Adapter
	{

	};

	struct Configuration
	{
		int Index;
		int Family;
		String Name;

		String Mac;			

		String Netmask;		
		String Broadcast;	

		Reason::Structure::Array<String> Ip;		

		Configuration():Index(-1),Family(0)
		{

		}

	};

	Reason::Structure::Array<Configuration> Configs;

	static char * MacAddress(int index=0);
	static char * IpAddress(int index=0);

	static int MacAddress(Reason::Structure::Array<String> & array, int length=0, int index=0);
	static int IpAddress(Reason::Structure::Array<String> & array, int length=0, int index=0);

	static Configuration Config(int index=0);
	static int Config(Reason::Structure::Array<Configuration> & array, int length=0, int index=0);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

