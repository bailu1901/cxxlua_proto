#include "BSDSocket.h"
#include <stdio.h>

#ifdef WIN32
	#pragma comment(lib, "wsock32")
#endif


BSDSocket::BSDSocket(SOCKET sock)
	: is_ipv4(true)
{
	m_sock = sock;
}

BSDSocket::~BSDSocket()
{
}

int BSDSocket::Init()
{
#ifdef WIN32
	/*
	http://msdn.microsoft.com/zh-cn/vstudio/ms741563(en-us,VS.85).aspx

	typedef struct WSAData { 
		WORD wVersion;								//winsock version
		WORD wHighVersion;							//The highest version of the Windows Sockets specification that the Ws2_32.dll can support
		char szDescription[WSADESCRIPTION_LEN+1]; 
		char szSystemStatus[WSASYSSTATUS_LEN+1]; 
		unsigned short iMaxSockets; 
		unsigned short iMaxUdpDg; 
		char FAR * lpVendorInfo; 
	}WSADATA, *LPWSADATA; 
	*/
	WSADATA wsaData;
	//#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 
	WORD version = MAKEWORD(2, 0);
	int ret = WSAStartup(version, &wsaData);//win sock start up
	if ( ret ) {
//		cerr << "Initilize winsock error !" << endl;
		return -1;
	}
#endif
	
	return 0;
}
//this is just for windows
int BSDSocket::Clean()
{
#ifdef WIN32
		return (WSACleanup());
#endif
		return 0;
}

BSDSocket& BSDSocket::operator = (SOCKET s)
{
	m_sock = s;
	return (*this);
}

BSDSocket::operator SOCKET ()
{
	return m_sock;
}
//create a socket object win/lin is the same
// af:
bool BSDSocket::Create(int af, int type, int protocol)
{
	m_sock = socket(af, type, protocol);
	if ( m_sock == INVALID_SOCKET ) {
		return false;
	}
	return true;
}

bool BSDSocket::CreateEx(const char* ip, int type, int protocol) {
	addrinfo* answer = parseIPV46(ip);
	if (!answer)
		return false;

	if (answer->ai_family == AF_UNSPEC) {
		freeaddrinfo(answer);
		return false;
	}

	m_sock = socket(answer->ai_family, type, protocol);
	if (answer) {
		freeaddrinfo(answer);
	}
	if (m_sock == INVALID_SOCKET) {
		return false;
	}

	return true;
}

addrinfo* BSDSocket::parseIPV46(const char* ip) {
	if (!ip)
		return NULL;

	struct addrinfo *answer, hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(ip, NULL, &hint, &answer);
	if (ret != 0) {
		return NULL;
	}

	switch (answer->ai_family){
		case AF_UNSPEC:
			return NULL;
			break;
		case AF_INET:
			is_ipv4 = true;
			break;
		case AF_INET6:
			is_ipv4 = false;
			break;
	}

	return answer;
}

bool BSDSocket::Connect(const char* ip, unsigned short port)
{
	addrinfo* answer = parseIPV46(ip);
	if (!answer)
		return false;

	int ret = SOCKET_ERROR;
	if (is_ipv4) {
		struct sockaddr_in  *sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in *>(answer->ai_addr);
		sockaddr_ipv4->sin_port = htons(port);
		ret = connect(m_sock, (struct sockaddr*)sockaddr_ipv4, sizeof(sockaddr));
	}
	else {
		struct sockaddr_in6  *sockaddr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(answer->ai_addr);
		sockaddr_ipv6->sin6_port = htons(port);
		ret = connect(m_sock, (struct sockaddr*)sockaddr_ipv6, sizeof(sockaddr_in6));
	}
	if (answer) {
		freeaddrinfo(answer);
		answer = NULL;
	}
	if ( ret == SOCKET_ERROR ) {
		return false;
	}
	return true;
}

bool BSDSocket::Bind(unsigned short port)
{
	if (is_ipv4) {
		struct sockaddr_in svraddr;
		svraddr.sin_family = AF_INET;
		svraddr.sin_addr.s_addr = INADDR_ANY;
		svraddr.sin_port = htons(port);

		int opt = 1;
		if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
			return false;

		int ret = bind(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));
		if (ret == SOCKET_ERROR) {
			return false;
		}
	}
	else {
		struct sockaddr_in6 svraddr;
		svraddr.sin6_family = AF_INET6;
		svraddr.sin6_addr = in6addr_any;
		svraddr.sin6_port = htons(port);

		int opt = 1;
		if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
			return false;

		int ret = bind(m_sock, (struct sockaddr*)&svraddr, sizeof(sockaddr_in6));
		if (ret == SOCKET_ERROR) {
			return false;
		}
	}
	return true;
}
//for server
bool BSDSocket::Listen(int backlog)
{
	int ret = listen(m_sock, backlog);
	if ( ret == SOCKET_ERROR ) {
		return false;
	}
	return true;
}

bool BSDSocket::Accept(BSDSocket& s, char* fromip)
{
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	SOCKET sock = accept(m_sock, (struct sockaddr*)&cliaddr, &addrlen);
	if ( sock == SOCKET_ERROR ) {
		return false;
	}

	s = sock;
	if ( fromip != NULL )
		sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));

	return true;
}

int BSDSocket::Send(const char* buf, int len, int flags)
{
	int bytes;
	int count = 0;

	while ( count < len ) {

		bytes = send(m_sock, buf + count, len - count, flags);
		if ( bytes == -1 || bytes == 0 )
			return -1;
		count += bytes;
	} 

	return count;
}

int BSDSocket::Recv(char* buf, int len, int flags)
{
	return (recv(m_sock, buf, len, flags));
}

int BSDSocket::Close()
{
#ifdef WIN32
	return (closesocket(m_sock));
#else
	return (close(m_sock));
#endif
}

int BSDSocket::GetError()
{
#ifdef WIN32
	return (WSAGetLastError());
#else
	return (errno);
#endif
}

bool BSDSocket::DnsParse(const char* domain, char* ip)
{
	struct hostent* p;
	if ( (p = gethostbyname(domain)) == NULL )
		return false;
		
	sprintf(ip, 
		"%u.%u.%u.%u",
		(unsigned char)p->h_addr_list[0][0], 
		(unsigned char)p->h_addr_list[0][1], 
		(unsigned char)p->h_addr_list[0][2], 
		(unsigned char)p->h_addr_list[0][3]);
	
	return true;
}
int BSDSocket::Select( int nfds,fd_set* readSet,fd_set* writeSet,fd_set* exceptSet, struct timeval * timeout )
{
	int result;
	result=select(nfds,readSet,writeSet,exceptSet,timeout);
	return result;
}

bool BSDSocket::SetNoBlock( bool on )
{
#ifndef WIN32 
	int flags = fcntl( m_sock , F_GETFL , 0 );
	if ( on )
		// make nonblocking fd
		flags |= O_NONBLOCK;
	else
		// make blocking fd
		flags &= ~O_NONBLOCK;
	fcntl(m_sock, F_SETFL, flags);  // set to non-blocking
#endif
	return true;
}
