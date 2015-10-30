
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

#include "reason/system/output.h"
#include "reason/network/socket.h"
#include "reason/network/url.h"
#include "reason/system/encoding/encoding.h"

using namespace Reason::System;
using namespace Reason::System::Encoding;

#include <stdio.h>

#include <fcntl.h>

#ifdef REASON_PLATFORM_WINDOWS

#include <io.h>
#include <ws2tcpip.h>

#include <iphlpapi.h>
#include <ipifcons.h>

#endif

#ifdef REASON_PLATFORM_POSIX

#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>

#ifdef REASON_PLATFORM_LINUX
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <ifaddrs.h>s

#include <linux/netdevice.h>

#endif

#ifdef REASON_PLATFORM_APPLE
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#endif

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char * Interface::MacAddress(int index)
{
	Reason::Structure::Array<String> array;
	MacAddress(array,1,index);
	return (array.Length() > 0)?Printer::Load(array[0]):0;
}

int Interface::MacAddress(Reason::Structure::Array<String> & array, int length, int index)
{

	int offset = array.Length();

	Reason::Structure::Array<Configuration> config;
	Config(config,length,index);

	Iterand<Configuration> iterand = config.Forward();
	while(iterand != 0)
	{
		array.Append(iterand().Mac);
		++iterand;
	}

	return array.Length()-offset;
}

char * Interface::IpAddress(int index)
{
	Reason::Structure::Array<String> array;
	IpAddress(array,1,index);
	return (array.Length() > 0)?Printer::Load(array[0]):0;
}

int Interface::IpAddress(Reason::Structure::Array<String> & array, int length, int index)
{
	int offset = array.Length();

	Reason::Structure::Array<Configuration> config;
	Config(config,length,index);

	Iterand<Configuration> iterand = config.Forward();
	while(iterand != 0)
	{
		if (iterand().Ip.Length() > 0)
			array.Append(iterand().Ip[0]);
		++iterand;
	}

	return array.Length()-offset;
}

int Interface::Config(Reason::Structure::Array<Configuration> & array, int length, int index)
{

	int offset = array.Length();

	#ifdef REASON_PLATFORM_WINDOWS

	unsigned long size = 0;
	IP_ADAPTER_INFO * info = 0;

	if (GetAdaptersInfo(info,&size) != ERROR_BUFFER_OVERFLOW)
		return 0;

	info = (IP_ADAPTER_INFO*) new char[size];

	if (GetAdaptersInfo(info,&size) == NO_ERROR)
	{
		IP_ADAPTER_INFO * adapter = info;
		while(adapter)
		{
			if (--index < 0)
			{

				Configuration config;

				config.Name = adapter->AdapterName;		

				Hex::Encode((char*)adapter->Address,adapter->AddressLength,config.Mac);

				IP_ADDR_STRING * addr = &adapter->IpAddressList;
				while (addr)
				{
					String ip = addr->IpAddress.String;
					config.Ip.Append(ip);
					addr = addr->Next;
				}

				array.Append(config);

				if (--length == 0)
					break;
			}

			adapter = adapter->Next;
		}

		delete [] info;
		return array.Length()-offset;
	}

	delete [] info;
	return 0;

	#endif

	#ifdef REASON_PLATFORM_POSIX

	#if defined(REASON_PLATFORM_LINUX) || defined(REASON_PLATFORM_APPLE)
	#define REASON_NETWORK_INTERFACE_GETIFADDRS
	#elif defined(REASON_PLATFORM_BSD) || defined(REASON_PLATFORM_APPLE)
	#define REASON_NETWORK_INTERFACE_SYSCTL
	#else
	#define REASON_NETWORK_INTERFACE_IOCTL
	#endif

	struct Ioctl
	{
		static bool Name(String & name, int sock, int ifindex)
		{

			#ifdef SIOCGIFNAME
			struct ifreq ifr;
			ifr.ifr_ifindex = ifindex+1;
			if (ioctl(sock,SIOCGIFNAME,&ifr) < 0)
				return false;

			name = ifr.ifr_name;
			return true;	
			#endif
			return false;	
		}

		static bool Mac(String & mac, int sock, const String & ifname)
		{

			mac.Release();
			struct ifreq ifr;
			strcpy(ifr.ifr_name,ifname.Data);

			#ifdef IFHWADDRLEN
			const int size = IFHWADDRLEN;
			#else
			const int size = 6;
			#endif

			#ifdef SIOCGIFHWADDR

			if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
			{

				Hex::Encode(ifr.ifr_hwaddr.sa_data,6,mac);
			}

			#endif

			#ifdef SIOCGIFMAC

			if (ioctl(sock, SIOCGIFMAC, &ifr) == 0)
			{
				Hex::Encode(ifr.ifr_addr.sa_data,6,mac);
			}

			#endif

			OutputConsole("Ifreq::Mac - Using ioctl\n");
			OutputConsole("Ifreq::Mac - Address = %s\n",mac.Print());

			return !mac.IsEmpty();
		}

		static bool Ip(String & ip, int sock, const String & ifname)
		{

			ip.Release();
			struct ifreq ifr;
			strcpy(ifr.ifr_name,ifname.Data);

			#ifdef SIOCGIFADDR
			if (ioctl(sock, SIOCGIFADDR, &ifr) == 0)
			{				
				ip = inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
			}	
			#endif

			return !ip.IsEmpty();
		}

	};

	struct Ifaddrs
	{

		static bool Address(String & addr, sockaddr * sa)
		{
			addr.Release();

			#ifdef AF_PACKET
			if (sa->sa_family == AF_PACKET)
			{

				OutputConsole("Ifaddrs::Address - Using sockaddr_dl\n");
				struct sockaddr_ll* sll = (struct sockaddr_ll*)sa;
				Hex::Encode(sll->sll_addr,sll->sll_halen,addr);
				OutputConsole("Ifaddrs::Address - Mac = %s\n",addr.Print());			
				return true;
			}
			#endif 

			#ifdef AF_LINK
			if (sa->sa_family == AF_LINK)
			{

				OutputConsole("Ifaddrs::Address - Using sockaddr_dl\n");
            	struct sockaddr_dl* sdp = (struct sockaddr_dl*)sa;
				Hex::Encode(sdp->sdl_data + sdp->sdl_nlen, sdp->sdl_alen,addr);
				OutputConsole("Ifaddrs::Address - Mac = %s\n",addr.Print());
				return true;
			}
			#endif			

            if (sa->sa_family == AF_INET)
			{

				struct sockaddr_in * sin = (struct sockaddr_in*)sa;
				char * data = (char *)&sin->sin_addr.s_addr;

				addr = inet_ntoa(sin->sin_addr);
				return true;
			}

			return false;

		}
	};

	int sock = socket(AF_INET,SOCK_DGRAM,0);
	if (sock == Sockets::SocketInvalid)
		return 0;		

	#ifdef REASON_NETWORK_INTERFACE_GETIFADDRS

	Reason::Structure::Map<String,Configuration> map;
	String name;

	int ifindex = 0;

    struct ifaddrs *ifa;
    if (getifaddrs(&ifa) == 0) 
	{
        struct ifaddrs *ifap;
        for (ifap = ifa;ifap != 0;ifap = ifap->ifa_next) 
		{
			name = ifap->ifa_name;

 			Configuration & config = map.Update(name)().Value();

			OutputConsole("Interface::Config - Adapter %d = %s [%s]\n",config.Index,config.Name.Print(),config.Mac.Print());

			if (config.Index == -1)
			{
				config.Index = ifindex++;

				if (--index >= 0)
					continue;

				if (--length == 0)
					break;
			}

			config.Family = ifap->ifa_addr->sa_family;

			OutputConsole("ifa_name = %s\n",ifap->ifa_name);
			OutputConsole("ifa_addr->sa_family = %d\n",ifap->ifa_addr->sa_family);	

			#ifdef AF_PACKET
			if (ifap->ifa_addr->sa_family == AF_PACKET)
			{
				Ifaddrs::Address(config.Mac,ifap->ifa_addr);
			}
			#endif 
			#ifdef AF_LINK
			if (ifap->ifa_addr->sa_family == AF_LINK)
			{
				Ifaddrs::Address(config.Mac,ifap->ifa_addr);
			}
			#endif			

            if (ifap->ifa_addr->sa_family == AF_INET)
			{

				String ip;
				Ifaddrs::Address(ip,ifap->ifa_addr);
				config.Ip.Append(ip);

				Ifaddrs::Address(config.Broadcast,ifap->ifa_broadaddr);					
				Ifaddrs::Address(config.Netmask,ifap->ifa_netmask);					

				#if !defined(AF_LINK) && !defined(AF_PACKET)											

				Ioctl::(config.Mac,sock,name);				

				#endif			
            }
        }

		OutputConsole("Interface::Config - Sorting getifaddrs() results...\n");

		Reason::Structure::Map<int,Configuration> sort;
		Iterand< Mapped<String,Configuration> > mapped = map.Forward();
		while (mapped != 0)
		{
			Configuration & config = mapped().Value();
			sort.Insert(config.Index,config);			
			++mapped;
		}

		Iterand< Mapped<int,Configuration> > sorted = sort.Forward();
		while(sorted != 0)
		{
			Configuration & config = sorted().Value();

			OutputConsole("Interface::Config - Adapter %d = %s [%s]\n",config.Index,config.Name.Print(),config.Mac.Print());

			array.Append(config);			
			++sorted;
		}

        freeifaddrs(ifa);
		return array.Length()-offset;
    }	

	close (sock);
	return array.Length()-offset;

	#endif

	#ifdef REASON_NETWORK_INTERFACE_IOCTL

	int ifrmax = sizeof(ifreq)*256;

	struct ifreq ifr;

	#ifdef SIOCGIFNAME

	int ifindex = 0;

	do
	{

		++ifindex;

		if (--index < 0)
		{	
			Config config;

			OutputConsole("Interface::Config - Using get interface name with index %d\n",ifindex);

			ifr.ifr_ifindex = ifindex;
			if (ioctl (sock, SIOCGIFNAME, &ifr) < 0)
				break;

			String name = ifr->ifr_name;
			config.Name = name;

			String ip;
			Ioctl::Ip(ip,sock,name);
			config.Ip.Append(ip);

			OutputConsole("Name = %s\n",name.Print());
			OutputConsole("Ip = %s\n",ip.Print());

			String mac;
			Ioctl::Mac(mac,sock,name);
			config.Mac = mac;

			array.Append(config);

			if (--length == 0)
				break;	
		}

	}
	while(length != 0);

	#else

    struct ifreq * ifrp;
    struct ifconf ifc;
	int ifrnum = 8;
	int ifrlen = 0;

	bool size = false;

	#ifdef SIOCGSIZIFCONF

	if (ioctl(fd, SIOCGSIZIFCONF, &ifrlen) >= 0)
	{
		ifc.ifc_buf = (void*) new char[ifrlen];
		ifc.ifc_len = ifrlen;

		size = true;
	}	

	#endif

	while (!size)	
	{
		ifc.ifc_buf = (void*) new ifreq[ifrnum];
		ifc.ifc_len = sizeof(ifreq)*ifrnum;	

		if (ioctl(sock, SIOCGIFCONF, &ifc) == 0)
			break;

		OutputConsole("ifc.ifc_len = %d, ifrlen = %d\n",ifc.ifc_len,ifrlen);

		if (ifc.ifc_len == ifrlen && ifc.ifc_len > 0)
			break;

		ifrlen = ifc.ifc_len;
		delete [] ifc.ifc_buf;					
		ifrnum *= 1.25;

		if ((sizeof(ifreq)*ifrnum) > ifrmax)
			return 0;
	}

	ifrp = ifc.ifc_req;
	while(ifc.ifc_len > 0)
	{		
		if (--index < 0)
		{	
			Config config;	

			String name = ifrp->ifr_name;
			config.Name = name;	

			String ip = inet_ntoa(((struct sockaddr_in *) &ifrp->ifr_addr)->sin_addr);
			config.Ip.Append(ip);

			OutputConsole("Interface::Config - Ip = %s\n",ip.Print());				

			strcpy(ifr.ifr_name, name.Data);

			if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
			{

			}

			Ioctl::Mac(config.Mac,sock,name);

			array.Append(config);

			if (--length == 0)
				break;	
		}

		++ifrp;
		ifc.ifc_len -= sizeof(struct ifreq);
	}

	delete [] ifc.ifc_buf;

	#endif

	close (sock);
	return array.Length()-offset;
	#endif

	#ifdef REASON_NETWORK_INTERFACE_SYSCTL

	OutputConsole("Interface::Config - Getting mac address from sysctl\n");

	int mib[6];
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;		
	mib[2] = 0;
	mib[3] = AF_LINK;		
	mib[4] = NET_RT_IFLIST;	
	mib[5] = 0;

	char * data;
	int size = 0;

	if (sysctl(mib,6,0,&size,0,0) < 0)
		return 0;

	data = new char[size];		
	if (sysctl(mib,6,data,&size,0,0) < 0)
		return 0;

	struct if_msghdr * ifm = 0;
	struct if_msghdr * ifmn = 0;
	struct ifa_msghdr * ifam = 0;

	struct sockaddr_dl * sdl = 0;

	int flags = 0;

	OutputConsole("Interface::Config - Querying sysctl results...\n");

	char * end = data+size;
	while (data < end)
	{
		ifm = (struct if_msghdr *)data;
		if (ifm->ifm_type != RTM_IFINFO) 
			return 0;

		sdl = (struct sockaddr_dl *)(ifm+1);
		flags = ifm->ifm_flags;

		data += ifm->ifm_msglen;
		ifam = 0;

		int count = 0;
		while (data < end)
		{
			ifmn = (struct if_msghdr *)data;
			if (ifmn->ifm_type != RTM_NEWADDR)
				break;

			if (!ifam)
				ifam = (struct ifa_msghdr *)data;

			++count;
			data += ifmn->ifm_msglen;
		}

		if (sdl->sdl_type != IFT_ETHER)
			continue;

		if (--index < 0)
		{
			Config config;

			String name(sdl->sdl_data,sdl->sdl_nlen);

			config.Name = name;

			String mac;
			Hex::Encode((char*)(sdl->sdl_data + sdl->sdl_nlen),sdl->sdl_alen,mac);
			config.Mac = mac;

			OutputConsole("Interface::Config - %s, %s\n",name.Print(),mac.Print());

			array.Append(config;

			if (--length == 0)
				break;	
		}
	}

	delete data;

	close (sock);
	return array.Length()-offset;	
	#endif

	#endif

	return 0;	

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int Address::Sizeof = sizeof(sockaddr);

Address::Address()
{ 
	Construct();
}

Address::Address(const sockaddr &address)
{
	memset(this,0,sizeof(sockaddr));
	memcpy(this, &address, sizeof(sockaddr));
}

Address::Address(const sockaddr_in &address) 
{
	memset(this,0,sizeof(sockaddr));
	memcpy(this, &address, sizeof(sockaddr_in));
}

Address::Address(const Url &url)
{
	memset(this,0,sizeof(sockaddr));
	Construct(url);
}

Address::Address(const unsigned long address, const unsigned short port) 
{ 
	memset(this,0,sizeof(sockaddr));
	Construct(address,port);
}

Address::Address(const Sequence & host, const unsigned short port)
{ 
	memset(this,0,sizeof(sockaddr));
	Construct(host,port);
}

Address::Address(const char * host, const unsigned short port)
{
	memset(this,0,sizeof(sockaddr));
	Construct(host,port);
}

Address::Address(const Address & address)
{
	memset(this,0,sizeof(sockaddr));
	Construct(((Address&)address).Host(),((Address&)address).Port());
}

Address::~Address()
{

}

bool Address::Construct()
{

	((sockaddr_in*)this)->sin_family = AF_INET;
	((sockaddr_in*)this)->sin_port = 0;
	((sockaddr_in*)this)->sin_addr.s_addr = 0; 

	memset(&(((sockaddr_in*)this)->sin_zero),0,8);

	return true;
}

bool Address::Construct(const unsigned long host,const int port)
{

	return Construct(host,htons(port));
}

bool Address::Construct(const unsigned long host,const unsigned short port)
{
	Construct();

	Host(host);
	Port(port);

	return true;
}

bool Address::Construct(const Url &url)
{
	if ( ! Construct(*url.Host,(url.Port->IsEmpty())?UrlLibrary::ResolvePort(url):url.Port->Integer()) )
	{
		OutputError("Address::Construct - Could not use \"%s\".\n",((Url&)url).Print());
		return false;
	}

	return true;
}

bool Address::Construct(const char *host, const int port)
{
	Construct();

	if (!host)
	{
		OutputError("Address::Construct - Hostname was null.\n");
		return false;
	}

	Host(host);
	Port(port);

	return true;
}

bool Address::Construct(const Sequence & host, const int port)
{
	String string = Superstring::Literal(host);
	return Construct(string.Data,port);
}

void Address::Host(const char * host)	
{
	if (!host)
	{
		OutputError("Address::Address - Hostname was null.\n");
		return;
	}

	Host(inet_addr(host));

	if (Host() == ADDRESS_TYPE_NONE && Sequences::Compare(host,"255.255.255.255") != 0)
	{

		Host(Sockets::ResolveAddress(host));
		if (Host() == ADDRESS_TYPE_NONE || Host() == 0)
		{
			OutputError("Address::Address -  Could not resolve hostname.\n");
		}
	}
}

char * Address::ResolveHostname()			
{
	return (char *) Sockets::ResolveHostname(Host());
}

int Address::Network()
{

	unsigned int mask = (((sockaddr_in*)this)->sin_addr.s_addr >> 24);
	int network=0;

	if ( mask < 127 )
	{
		network = NETWORK_TYPE_CLASS_A;
	}
	else
	if ( mask == 127 )
	{
		network = NETWORK_TYPE_LOCALHOST;
	}
	else
	if ( (mask > 127) && (mask < 192) )
	{
		network = NETWORK_TYPE_CLASS_B;
	}
	else	
	{
		network = NETWORK_TYPE_CLASS_C;
	}

	return network;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Host::Host()
{

	char hostname[255];
	if (gethostname(hostname,255) == 0)
		Construct(hostname,String::Length(hostname));
}

Host::Host(const Url &url)
{
	Construct(url);
}

Host::Host(const Sequence & sequence)
{
	Construct(sequence);
}

Host::Host(const char * data)
{
	Construct((char*)data,String::Length(data));
}

Host::Host(char * data, int size)
{
	Construct(data,size);
}

Host::Host(const Host & host)
{
	Hostname = host.Hostname;
	Alias = host.Alias;
	Address = host.Address;
}

Host::~Host()
{

}

bool Host::Construct(const Url &url)
{
	if ( ! Construct(*url.Host) )
	{
		OutputError("Host::Construct - Could not use \"%s\".\n",((Url&)url).Print());
		return false;
	}

	return true;
}

bool Host::Construct(char * data, int size)
{

	String name(data,size);

	#ifdef REASON_PLATFORM_CYGWIN
	hostent *entry = gethostbyname(name.Data);
	if (entry)
	{
		OutputMessage("Host::Construct - Family: %d\n",entry->h_addrtype);

		Hostname = entry->h_name;

		char ** ptr = 0;
		for (ptr = entry->h_aliases; *ptr != 0; ++ptr)
			Alias.Append(*ptr);

		for (ptr = entry->h_addr_list; *ptr != 0; ++ptr)
			Address.Append(((in_addr *)*ptr)->s_addr);

		return true;
	}
	else
	{
		OutputError("Host::Construct - gethostbyname() failed.\n");
		return 0;
	}

	#else

	struct addrinfo hint, *info, *next;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = PF_UNSPEC;

	int error = 0;
	if ((error = getaddrinfo(name.Data,0,&hint,&info)) != 0)
	{
		OutputError("Host::Construct -  Could not resolve hostname, error: %s.\n",gai_strerror(error));
		return false;
	}

	if (info)
	{

		Hostname = name;

		for (next = info;next != 0;next = next->ai_next)
		{
			OutputMessage("Host::Construct - Family: %d, Type: %d, Protocol: %d \n",next->ai_family,next->ai_socktype,next->ai_protocol);
			OutputMessage("Host::Construct - Address: %s\n",Reason::Network::Address(*next->ai_addr).Print());

			Alias.Append(next->ai_canonname);

			if (next->ai_family == AF_INET)
				Address.Append(*next->ai_addr);
		}

		freeaddrinfo(info);
	}

	#endif

	return true;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sockets::Active = false;

const Sockets::SocketsSentinel Sockets::Library;

void Sockets::Startup()
{
	#ifdef REASON_PLATFORM_WINDOWS
		if (! Active)
		{
			WORD wVersionRequested = MAKEWORD(2,2);
			WSADATA wsaData;
			if ( WSAStartup(wVersionRequested,&wsaData) != 0 ) 
			{
				OutputFailure("Sockets::Startup","Winsock startup failed");
				WSACleanup();
			}

			Active = true;
		}
	#endif

	#ifdef REASON_PLATFORM_POSIX

		{

			Active = true;
		}	
	#endif
}

void Sockets::Shutdown()
{
	#ifdef REASON_PLATFORM_WINDOWS

		if (Active )
		{
			if ( WSACleanup() == Sockets::SocketError  )
			{
				if (WSAGetLastError() == WSAEINPROGRESS)
				{	
					WSACancelBlockingCall();
					WSACleanup();
				}
			}
		}

	#endif

	#ifdef REASON_PLATFORM_POSIX

	#endif

}

unsigned short Sockets::ResolveService(const char * service, const char * protocol)
{

	struct servent *serviceEntry;	

	if ((serviceEntry = getservbyname(service, protocol)) == 0)
	{

		return htons(atoi(service));
	}
	else 
	{

		return serviceEntry->s_port;	
	}

}

const char * Sockets::ResolveService(const int port, const char * protocol)
{

	struct servent *serviceEntry;	

	if ((serviceEntry = getservbyport(port, protocol)) == 0)
	{
		return 0;
	}
	else 
	{

		return serviceEntry->s_name;	
	}
}

unsigned long Sockets::ResolveAddress(const char* hostname)
{

	Startup();

	hostent *entry = gethostbyname(hostname);

	if (entry)
	{

		return ((in_addr *)entry->h_addr_list[0])->s_addr;

	}
	else
	{
		OutputError("Sockets::Resolve - gethostbyname() failed.\n");
		return 0;
	}
}

unsigned long Sockets::ResolveAddress()
{

	return ResolveAddress(ResolveHostname());
}

const char * Sockets::ResolveIp(const char * hostname)
{
	in_addr in;
	in.s_addr = ResolveAddress(hostname);
	return inet_ntoa(in);
}

const char * Sockets::ResolveIp()
{
	unsigned long address = ResolveAddress();
	if (address != 0)
	{
		in_addr in;
		in.s_addr = address;
		return inet_ntoa(in);
	}
	else
	{

		return Interface::IpAddress();
	}
}

const char * Sockets::ResolveHostname(Address &address)
{
	return ResolveHostname((sockaddr&)address);
}

const char * Sockets::ResolveHostname(const char * address)
{

	return ResolveHostname(inet_addr(address));
}

const char * Sockets::ResolveHostname(const sockaddr & address)
{
	return ResolveHostname(((sockaddr_in*) &address)->sin_addr.s_addr);
}

const char * Sockets::ResolveHostname(const sockaddr_in & address)
{
	return ResolveHostname(((sockaddr_in*) &address)->sin_addr.s_addr);
}

const char * Sockets::ResolveHostname(const unsigned long address)
{

	unsigned long host = htonl(ntohl(address));
	hostent * entry = gethostbyaddr((char*) &host, sizeof(host), AF_INET);
	if(entry == 0) 
	{
		OutputError("Sockets::Resolve - Failed to resolve IP address.\n");
		return "";
	}

	OutputAssert(entry->h_name!=0);
	return entry->h_name; 

}

const char * Sockets::ResolveHostname()
{

	static char name[255];
	gethostname(name,255);
	return name;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

Socket::Socket(const Socket & socket):
	Handle(Sockets::SocketInvalid),State(0),Mode(socket.Mode)
{

}

Socket::Socket(char mode):
	Handle(Sockets::SocketInvalid),State(0),Mode(0)
{
	Modulate(mode);
	OutputAssert(Sockets::Active);
}

Socket::Socket():
	Handle(Sockets::SocketInvalid),State(0),Mode(0)
{
	Modulate(Sockets::SOCKET_MODE_SYNCHRONOUS);
	OutputAssert(Sockets::Active);
}

Socket::~Socket()
{
	if (IsCreated())
		Destroy();
}

Socket & Socket::operator = (const Socket & socket)
{
	Destroy();

	Handle = Sockets::SocketInvalid;
	State = 0;

	Mode = socket.Mode;

	return *this;
}

bool Socket::Create(int family, int type, int protocol)
{
	Error(false);
	Timeout(false);

	if (IsCreated() && !Destroy())
	{
		OutputError("Socket::Create - Could not re-create socket because destroy failed\n");
		return false;		
	}

	Handle = socket(family,type,protocol);

	if(Handle == Sockets::SocketInvalid) 
	{	
		Error(true);
		OutputError("Socket::Create - Invalid socket\n");
		return false;
	}

	int on = 1;
    setsockopt(Handle, SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on));

	Modulate(Mode);

	return true;
}

bool Socket::Disconnect()
{

	if (IsConnected())
	{

		#ifdef REASON_PLATFORM_WINDOWS

		if(shutdown(Handle,SD_BOTH) == Sockets::SocketError)
		#endif
		#ifdef REASON_PLATFORM_POSIX

		if(shutdown(Handle,SHUT_RDWR) == Sockets::SocketError)
		#endif
		{
			Error(true);
			OutputError("Socket::Disconnect - Could not shutdown socket\n");

			return false;
		}

		State.Off(Sockets::SOCKET_STATE_ALL);
		State.On(Sockets::SOCKET_STATE_DISCONNECTED);
	}

	Destroy();

	return true;
}

bool Socket::Release()
{

	if (IsCreated())
	{

		if (!Destroy())
		{
			OutputError("Socket::Release - Could not release socket because destroy failed\n");
			return false;		
		}

		Create(Family(),Type(),Protocol());
	}

	return true;
}

bool Socket::Destroy()
{

	if (IsCreated())
	{
		int result=0;	
		#ifdef REASON_PLATFORM_WINDOWS
		if(closesocket(Handle) == Sockets::SocketError) 		
		#endif
		#ifdef REASON_PLATFORM_POSIX

		if (close(Handle) == Sockets::SocketError)
		#endif
		{
			Error(true);
			OutputError("Socket::Destroy - Could not close socket\n");
			return false;
		}
		else
		{
			Handle = Sockets::SocketInvalid;
		}

		State.Off(Sockets::SOCKET_STATE_ALL);
	}

	return true;
}

void Socket::Modulate(char mode)
{
	Mode = mode;

	if (IsCreated())
	{
		#ifdef REASON_PLATFORM_WINDOWS

		unsigned long property;
		if (Mode == Sockets::SOCKET_MODE_ASYNCHRONOUS)
			property=1;
		else
			property=0;

		ioctlsocket( Handle, FIONBIO, &property);

		#endif

		#ifdef REASON_PLATFORM_POSIX

		int fflags = fcntl(Handle, F_GETFL);
		if (Mode == Sockets::SOCKET_MODE_ASYNCHRONOUS)
			fflags |= O_NONBLOCK;
		else
			fflags &=~ O_NONBLOCK;

		fcntl(Handle, F_SETFL, fflags);

		#endif
	}
}

void Socket::Set(int level, int option, char * data, const int size)
{

    setsockopt(Handle,level,option,data,size);
}

void Socket::Get(int level, int option, char * data, int & size)
{

    getsockopt(Handle,level,option,data,&size);
}

int Socket::Type()
{
	int type;
    int length = sizeof(type);
    getsockopt(Handle, SOL_SOCKET, SO_TYPE, (char*)&type, &length);
    return type;
}

int Socket::Protocol()
{
	#ifdef REASON_PLATFORM_WINDOWS
	WSAPROTOCOL_INFO info;
	int length = sizeof(info);
	getsockopt(Handle, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&info, &length);
	return info.iProtocol;
	#endif

	#ifdef REASON_PLATFORM_POSIX
	int protocol;
    int length = sizeof(protocol);
    getsockopt(Handle, SOL_SOCKET, SO_PROTOCOL, (char*)&protocol, &length);
    return protocol;
	#endif
}

int Socket::Family()
{	
	#ifdef REASON_PLATFORM_WINDOWS
	WSAPROTOCOL_INFO info;
	int length = sizeof(info);
	getsockopt(Handle, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&info, &length);
	return info.iAddressFamily;
	#endif

	#ifdef REASON_PLATFORM_POSIX
	int domain;
    int length = sizeof(domain);
    getsockopt(Handle, SOL_SOCKET, SO_DOMAIN, (char*)&domain, &length);
    return domain;
    #endif
}

int Socket::Error()
{

	#ifdef REASON_PLATFORM_WINDOWS
		int error;

		error = WSAGetLastError();
	#endif
	#ifdef REASON_PLATFORM_POSIX

		int error = errno;
	#endif

    return error;
}

void Socket::Readable(bool read)
{
	if (read)
	{
		if (State.Is(Sockets::SOCKET_STATE_LISTENING))
		{

			State.On(Sockets::SOCKET_STATE_ACCEPTING);
		}
		else
		{
			State.On(Sockets::SOCKET_STATE_READABLE);
		}
	}
	else
	{
		State.Off(Sockets::SOCKET_STATE_READABLE);
	}
}

void Socket::Writeable(bool write)
{
	if (write)
	{
		if (State.Is(Sockets::SOCKET_STATE_NOT_CONNECTED))
		{

			State.Off(Sockets::SOCKET_STATE_NOT_CONNECTED);
			State.On(Sockets::SOCKET_STATE_CONNECTED);
		}
		else
		{
			State.On(Sockets::SOCKET_STATE_WRITEABLE);
		}
	}
	else
	{
		State.Off(Sockets::SOCKET_STATE_WRITEABLE);
	}
}

void Socket::Error(bool error)
{
	if (error)
	{
		State.On(Sockets::SOCKET_STATE_ERROR);

		#ifdef REASON_PLATFORM_WINDOWS
		OutputError("Socket::Error\n\tWSA Error number: %d\n\tError number: %d, Error description: %s\n", WSAGetLastError(), errno, strerror(errno));
		OutputConsole("Socket::Error\n\tWSA Error number: %d\n\tError number: %d, Error description: %s\n", WSAGetLastError(), errno, strerror(errno));
		#endif
		#ifdef REASON_PLATFORM_POSIX
		OutputError("Socket::Error\n\tError number: %d, Error description: %s\n", errno, strerror(errno));
		OutputConsole("Socket::Error\n\tError number: %d, Error description: %s\n", errno, strerror(errno));
		#endif

	}
	else
	{
		State.Off(Sockets::SOCKET_STATE_ERROR);
	}
}

void Socket::Timeout(bool timeout)
{
	if (timeout)
	{
		State.On(Sockets::SOCKET_STATE_TIMEOUT);
	}
	else
	{
		State.Off(Sockets::SOCKET_STATE_TIMEOUT);
	}
}

bool Socket::Select(int selector, const unsigned int timeout)
{

	if (!IsCreated())
	{
		Error(false);
		Readable(false);
		Writeable(false);
		return false;
	}

	Nullable<fd_set> read;
	Nullable<fd_set> write;
	Nullable<fd_set> error;

	FD_ZERO(&read);
	FD_ZERO(&write);
	FD_ZERO(&error);

	if ((selector&Sockets::SOCKET_SELECT_READ) || !(read.Null=true)) FD_SET(Handle,&read);
	if ((selector&Sockets::SOCKET_SELECT_WRITE) || !(write.Null=true)) FD_SET(Handle,&write);
	if ((selector&Sockets::SOCKET_SELECT_ERROR) || !(error.Null=true)) FD_SET(Handle,&error);

	Nullable<timeval> wait;

	if (timeout == Sockets::TimeoutIndefinate) 
	{
		wait.Nullify();
	}
	else
	{

		int seconds = timeout/Time::MillisecondsPerSecond;
		timeval time = {seconds, ((timeout-(seconds*Time::MillisecondsPerSecond))*Time::MicrosecondsPerMillisecond)};
		wait = time;
	}

	int result = select(1+1, &read, &write, &error, &wait);

 	if(result == Sockets::SocketError) 
	{
		Error(true);
		OutputError("Socket::Select - Could not select socket\n");		
		if (!read.IsNull()) Readable(false);
		if (!write.IsNull()) Writeable(false);

		return false;
	}
	else
	if(result == 0)
	{

		Timeout(true);
		OutputError("Socket::Select - Timeout during select\n");

		if (!read.IsNull()) Readable(false);
		if (!write.IsNull()) Writeable(false);

		return false;
	}

	if (!read.IsNull()) Readable(FD_ISSET(Handle,read.Pointer()));
	if (!write.IsNull()) Writeable(FD_ISSET(Handle,write.Pointer()));
	if (!error.IsNull()) Error(FD_ISSET(Handle,error.Pointer()));

	return true;
}

bool Socket::Bind(const sockaddr &address)
{
	Error(false);
	Timeout(false);

	if (!IsCreated() && ! Create())
	{
		OutputError("Socket::Bind - Attempt to create and bind to socket failed.\n");
		return false;
	}

	if( bind(Handle, &address, sizeof(sockaddr)) == Sockets::SocketError)
	{

		Error(true);
		OutputError("Socket::Bind - Could not bind to socket, address %s", ((Address &)address).Print());	
		return false;
	}

	State.On(Sockets::SOCKET_STATE_BOUND);
	return true;
}

bool Socket::Listen()
{
	Error(false);
	Timeout(false);

	if(!IsBound() || listen(Handle, 5) == Sockets::SocketError) 
	{
		OutputError("Socket::Listen - Could not listen on socket.\n");	
		Error(true);
		return false;
	}

	State.On(Sockets::SOCKET_STATE_LISTENING);
	return true;
}

bool Socket::Accept(Socket &socket, const sockaddr &address)
{
	Error(false);
	Timeout(false);

	if (socket.IsCreated())
	{
		OutputError("Socket::Accept - Destination socket is already created.\n");
		return false;
	}

	if(!IsBound() || !IsListening()) 
	{
		OutputError("Socket::Accept - Could not accept on socket that is not bound or listening.\n");	
		return false;
	}

	if (IsAsynchronous())
	{

	}

	int size = sizeof(sockaddr);
	#ifdef REASON_PLATFORM_WINDOWS
	if((socket.Handle = accept(Handle,(sockaddr*)&address, (int *)&size))== Sockets::SocketInvalid)
	#else
	if((socket.Handle = accept(Handle,(sockaddr*)&address, (unsigned int *)&size))== Sockets::SocketInvalid) 
	#endif
	{
		if (IsAsynchronous() && Error() == Sockets::SOCKET_ERROR_BLOCKING)
		{

		}
		else
		{
			OutputError("Socket::Accept - Invalid socket used, perhaps it didn't initialise ?\n");
			Error(true);
		}

		socket.Handle = Sockets::SocketInvalid;
		return false;
	}

	State.Off(Sockets::SOCKET_STATE_ACCEPTING);
	socket.State.On(Sockets::SOCKET_STATE_CONNECTED);
	return true;
}

bool Socket::IsConnected(unsigned int timeout)		
{
	if (State.Is(Sockets::SOCKET_STATE_CONNECTED)) return true;

	if (State.Is(Sockets::SOCKET_STATE_NOT_CONNECTED))
	{		
		if (IsAsynchronous())
		{

			Select(Sockets::SOCKET_SELECT_WRITE);
		}
		else
		{
			Select(Sockets::SOCKET_SELECT_WRITE,timeout);
		}
	}

	return State.Is(Sockets::SOCKET_STATE_CONNECTED);
}

bool Socket::Connect(const sockaddr & address)
{
	Error(false);
	Timeout(false);

	if (IsConnected() || State.Is(Sockets::SOCKET_STATE_NOT_CONNECTED))
	{
		if (!Destroy())
		{
			OutputError("Socket::Connect - Could not re-connect socket because destroy failed\n");
			return false;		
		}
	}

	if (!IsCreated() && ! Create())
	{
		OutputError("Socket::Connect - Attempt to create and connect to socket failed.\n");
		return false;
	}

	if (connect(Handle, (sockaddr*)&address, sizeof(sockaddr)) == Sockets::SocketError)
	{

		int error = Error();
		if (IsAsynchronous() && (error == Sockets::SOCKET_ERROR_BLOCKING || error == Sockets::SOCKET_ERROR_PENDING))
		{

			State.On(Sockets::SOCKET_STATE_NOT_CONNECTED);
			return true;
		}
		else
		if (error == Sockets::SOCKET_ERROR_TIMEDOUT)
		{
			Timeout(true);	
			OutputError("Socket::Connect - Timeout during connect\n");
			return false;
		}
		else
		{
			Error(true);
			OutputError("Socket::Connect - Could not connect to %s\n", ((Address &)address).Print());
			return false;
		}
	}

	OutputMessage("Socket::Connect - Connection successful %s\n",((Address &)address).Print());

	State.On(Sockets::SOCKET_STATE_CONNECTED);

	return true;
}

bool Socket::IsReadable(unsigned int timeout)
{
	if (State.Is(Sockets::SOCKET_STATE_NOT_READABLE)) return false;
	if (State.Is(Sockets::SOCKET_STATE_READABLE)) return true;

	if (IsAsynchronous())
	{

		if (IsConnected())
			Readable(true);
	}
	else
	if (IsConnected())
	{

		Select(Sockets::SOCKET_SELECT_READ,timeout);
	}

	return State.Is(Sockets::SOCKET_STATE_READABLE);
}

int Socket::Read(char * data, int size, unsigned int timeout)
{

	if (data == 0 || size <= 0) return 0;

	int received = 0;
	int bytes = 0;
	do 
	{
		bytes = Receive(data, size - received, timeout);
		received += bytes;
		data += bytes;
	} 
	while(bytes > 0);

	return received;
}

int Socket::Receive(char * data, const int size, unsigned int timeout)
{
	Error(false);
	Timeout(false);

	if (data == 0 || size <= 0) return 0;

	int received=0;
	if ( IsReadable(timeout) )
	{
		OutputAssert(IsConnected());

		if ( (received = recv(Handle, data, size, 0)) == Sockets::SocketError )
		{
			if (IsAsynchronous() && Error() == Sockets::SOCKET_ERROR_BLOCKING)
			{

			}
			else
			{
				OutputError("Socket::Receive - Failed\n");
				Error(true);
				Readable(false);

				Destroy();				
			}

			return 0;
		}

		if (received == 0 && size > 0)
		{

			State.On(Sockets::SOCKET_STATE_NOT_READABLE);
		}

		if (!IsAsynchronous())
			Readable(false);
	}

	return received;
}

bool Socket::IsWriteable(unsigned int timeout)
{
	if (State.Is(Sockets::SOCKET_STATE_NOT_WRITABLE)) return false;
	if (State.Is(Sockets::SOCKET_STATE_WRITEABLE)) return true;

	if (IsAsynchronous())
	{

		if (IsConnected())
			Writeable(true);
	}
	else
	if (IsConnected())
	{
		Select(Sockets::SOCKET_SELECT_WRITE,timeout);	
	}

	return State.Is(Sockets::SOCKET_STATE_WRITEABLE);
}

int Socket::Write(char * data, int size, unsigned int timeout)
{
	if (data == 0 || size <= 0) return 0;

	int sent = 0;
	int bytes = 0;
	do 
	{
		bytes = Send(data, size - sent, timeout);
		sent += bytes;
		data += bytes;	
	} 
	while(bytes > 0);

	return sent;
}

int Socket::Send(char * data, int size, unsigned int timeout)
{
	Error(false);
	Timeout(false);

	if (data == 0 || size <= 0) return 0;

	int sent=0;
	if( IsWriteable(timeout) )
	{
		OutputAssert(IsConnected());

		if ((sent = send(Handle, data, size, 0)) == Sockets::SocketError)
		{
			if (IsAsynchronous() && Error() == Sockets::SOCKET_ERROR_BLOCKING)
			{

			}
			else
			{
				OutputError("Socket::Send - Failed\n");
				Error(true);
				Writeable(false);

				Destroy();				
			}

			return 0;
		}

		if (sent == 0 && size > 0)
		{

			OutputWarning("Socket::Send - Socket has entered non writable state.\n");

			State.On(Sockets::SOCKET_STATE_NOT_WRITABLE);
		}

		if (!IsAsynchronous())
			Writeable(false);
	}

	return sent;
}

void Socket::RemoteAddress(const sockaddr & address)
{
	Error(false);
	Timeout(false);

	OutputAssert(Handle != Sockets::SocketInvalid);

	int size = sizeof(sockaddr);
	#ifdef REASON_PLATFORM_WINDOWS
	if(getpeername(Handle,(sockaddr*)&address, (int *)&size) == Sockets::SocketError) 
	#else
	if(getpeername(Handle,(sockaddr*)&address, (unsigned int *)&size) == Sockets::SocketError) 
	#endif
	{
		OutputError("Socket::RemoteAddress - Could not get peer address\n");
		Error(true);
	}
}

void Socket::LocalAddress(const sockaddr & address)
{
	Error(false);
	Timeout(false);

	OutputAssert(Handle != Sockets::SocketInvalid);

	int size = sizeof(sockaddr);
	#ifdef REASON_PLATFORM_WINDOWS
	if(getpeername(Handle,(sockaddr*)&address, (int *)&size) == Sockets::SocketError) 
	#else
	if(getsockname(Handle,(sockaddr*)&address, (unsigned int *)&size) == Sockets::SocketError) 
	#endif
	{
		OutputError("Socket::LocalAddress - Could not get socket address\n");
		Error(true);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SocketOperation::SocketOperation(char *data, const int size, const unsigned int timeout)
	:Data(data),Size(size),Timeout(timeout)
{

}

SocketOperation::SocketOperation()
	:Data(0),Size(0),Timeout(0)
{

}

SocketOperation::~SocketOperation()
{

}

bool SocketOperation::Compare(SocketOperation *operation)
{
	return Type == operation->Type;
}

bool SocketOperation::Process(Connection & connection)
{
	switch(Type.Bits)
	{
	case Sockets::SOCKET_OPERATION_ACCEPT:
		return false;	
	case Sockets::SOCKET_OPERATION_CONNECT:
		return (connection.Socket.Connect(connection.Address));
	case Sockets::SOCKET_OPERATION_READ:
		return (connection.Socket.Receive(Data,Size,Timeout)!=0);
	case Sockets::SOCKET_OPERATION_WRITE:
		return (connection.Socket.Send((char*)Data,Size,Timeout)!=0);
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool SocketSession::Process()
{

	Structure::Iterator<SocketOperation*> iterator(*this);
	for(iterator.Forward();iterator.Has();)
	{
		if (iterator()->Process(*Connection))
		{
			Structure::Iterator<SocketOperation*> iter = iterator;
			iterator.Move();

			OutputBreak();
			iter.Destroy();
		}
		else
		{
			iterator.Move();
		}
	}

	return true;
}

void SocketSession::Enqueue(SocketOperation *operation, bool immediate)
{
	if (Pending(operation))
	{
		delete operation;
	}
	else
	{
		if (immediate)
			Prepend(operation);
		else
			Append(operation);
	}
}

void SocketSession::Dequeue(SocketOperation *operation)
{
	Remove(operation);
}

bool SocketSession::Pending(SocketOperation *operation)
{
	Reason::Structure::Abstract::LinkedList<SocketOperation*>::Entry * entry = First;

	while (entry != 0)
	{
		if (entry->Reference().Compare(operation))
			return true;

		entry = entry->Next;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SocketSet::SocketSet()
{
    FD_ZERO(this);
}

SocketSet::~SocketSet()
{

}

bool SocketSet::Contains(Socket &socket)
{

	return FD_ISSET(socket.Handle,this);
}

int SocketSet::Size()
{
	#ifdef REASON_PLATFORM_WINDOWS
	return fd_count;
	#else
	return NFDBITS;
	#endif
}

void SocketSet::Release()
{
	FD_ZERO(this);
}

void SocketSet::Append(Socket &socket)
{

	if (Size() < FD_SETSIZE-1)
	{
		FD_SET(socket.Handle,this);
	}
	else
	{
		OutputError("SocketSet::Append - The socket set is full.\n");
	}
}

void SocketSet::Remove(Socket &socket)
{
	if (Size() > 0)
	{
		FD_CLR(socket.Handle,this);
	}
	else
	{
		OutputError("SocketSet::Remove - The socket set is empty.\n");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SocketManager::SocketManager()
{

	memset(&Timeout,0,sizeof(timeval));

}

SocketManager::~SocketManager()
{
	if (Socket)
		delete Socket;
}

void SocketManager::Connect(Address &from, Address &to)
{
	Socket = new Reason::Network::Socket();
	Socket->Bind(from);
	Socket->Listen();

}

bool SocketManager::Update()
{
	Readable.Release();
	Writeable.Release();

	Readable.Append(*Socket);
	Writeable.Append(*Socket);

	for (int i=0;i<Sessions.Length();++i)
	{
		Readable.Append(Sessions[i]->Connection->Socket);
		Writeable.Append(Sessions[i]->Connection->Socket);
	}

	return true;

}

bool SocketManager::Select()
{
	if (select(0, &Readable, &Writeable, 0, &Timeout) > 0) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SocketManager::Process()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UdpSocket::UdpSocket()
{
	Create(Sockets::SOCKET_FAMILY_INET,Sockets::SOCKET_TYPE_DATAGRAM,Sockets::SOCKET_PROTOCOL_UDP);
}

UdpSocket::~UdpSocket()
{

}

void UdpSocket::Broadcast(bool enable) 
{

	int opt = enable?1:0;
	setsockopt(Handle, SOL_SOCKET, SO_BROADCAST,(char *) &opt, sizeof(opt));
}

int UdpSocket::Send(  Address & dst,  const char * data,  const int size) 
{

	unsigned long maxMsg;
	int maxMsgLength = sizeof(maxMsg);

	#ifdef REASON_PLATFORM_WINDOWS
	if ( getsockopt(Handle,SOL_SOCKET,SO_MAX_MSG_SIZE,(char *) &maxMsg,&maxMsgLength) == Sockets::SocketError)
	{

		OutputError("UdpSocket::Send - Error whilst retrieving maximum message size.\n");
	}
	#else
		OutputError("UdpSocket::Send - Not implemented under linux.\n");
	#endif

	if (maxMsg < size)
	{
		OutputError("UdpSocket::Send - The message size exceeds the allowable size for this protocol.\n");
	}

	int bytes;
	if ((bytes = sendto(Handle,data,size,0,(sockaddr*)dst,*dst.Size())) != size) 
	{ 
		OutputError("UdpSocket::Send - Transmission failed.\n");
	}

	return bytes;
}

int UdpSocket::Receive(  Address & src,  char * data,  const int size )
{
	int len=*src.Size();

	int bytes;	
	if ( (bytes = recvfrom(Handle,data,size,0,(sockaddr*)src,(socklen_t*)&len)) < 0 )
	{
		OutputError("UdpSocket::Receive - Transmission failed.\n");
	}

	return bytes;
}

void UdpSocket::MulticastTimeToLive(unsigned char timeToLive)
{
	if (setsockopt(Handle, IPPROTO_IP, IP_MULTICAST_TTL,(char*) &timeToLive, sizeof(timeToLive)) < 0) 
	{
		OutputError("UdpSocket::MulticastTimeToLive - Multicast TTL could not be assigned.\n");
	}
}

void UdpSocket::MulticastJoinGroup(const char * multicastGroup)
{
	struct ip_mreq multicastRequest;
	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup);
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(Handle, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*) &multicastRequest, sizeof(multicastRequest)) < 0) 
	{
		OutputError("UdpSocket::MulticastJoinGroup - Multicast join group failed.\n");
	}
}

void UdpSocket::MulticastLeaveGroup(const char * multicastGroup) 
{
	struct ip_mreq multicastRequest;
	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup);
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(Handle, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &multicastRequest,sizeof(multicastRequest)) < 0) 
	{
		OutputError("UdpSocket::MulticastLeaveGroup - Multicast leave group failed.\n");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SocketRestricted::SocketRestricted()
{
	SendBytesCount		= 0;
	ReceiveBytesCount	= 0;
}

SocketRestricted::~SocketRestricted()
{
}

 int SocketRestricted::Receive(char * data, int size, unsigned int timeout)
{
	int bytesReceived = Socket::Receive(data,size,timeout);

	if (bytesReceived)
	{
		ReceiveBytesCount += bytesReceived;
	}

	return bytesReceived;
}

 int SocketRestricted::Send(char * data, int size,  unsigned int timeout)
{

	int bytesSent = Socket::Send(data,size,timeout);

	if (bytesSent)
	{
		SendBytesCount += bytesSent;
	}

	return bytesSent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int SocketTimed::Read(char * data, int size, unsigned int timeout)
{

	return 0;

}

int SocketTimed::Receive(char* data, int size, unsigned int timeout)
{

	int bytesReceived = Socket::Receive(data,size,timeout);

	if (bytesReceived)
	{
		ReceiveTimer.Start();
	}

	return bytesReceived;

}

int SocketTimed::Write(char * data, int size, unsigned int timeout)
{
	return 0;
}

int SocketTimed::Send(char* data, int size, unsigned int timeout)
{

	int bytesSent = Socket::Send(data,size,timeout);

	if (bytesSent)
	{
		SendTimer.Start();
	}

	return bytesSent;
}

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

