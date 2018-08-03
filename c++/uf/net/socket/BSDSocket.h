/*
 * define file about portable socket class. 
 * description:this sock is suit both windows and linux
 * design:odison
 * e-mail:odison@126.com>
 * 
 */

#ifndef _ODSOCKET_H_
#define _ODSOCKET_H_

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	typedef int				socklen_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
    #include <string.h>
    #include <errno.h>
	typedef int				SOCKET;

	//#pragma region define win32 const variable in linux
	#define INVALID_SOCKET	-1
	#define SOCKET_ERROR	-1
	//#pragma endregion
#endif


class BSDSocket {

public:
	BSDSocket(SOCKET sock = INVALID_SOCKET);
	~BSDSocket();

	// Create socket object for snd/recv data
	bool Create(int af, int type, int protocol = 0);

	bool CreateEx(const char* ip, int type, int protocal = 0);
	// Connect socket
	bool Connect(const char* ip, unsigned short port);
	//#region server
	// Bind socket
	bool Bind(unsigned short port);

	// Listen socket
	bool Listen(int backlog = 5); 

	// Accept socket
	bool Accept(BSDSocket& s, char* fromip = NULL);
	//#endregion
	
	// Send socket
	int Send(const char* buf, int len, int flags = 0);

	// Recv socket
	int Recv(char* buf, int len, int flags = 0);
	//Select socket
	int Select(int nfds,fd_set* readSet,fd_set* writeSet,fd_set* exceptSet, struct timeval * timeout);
	//noBlock
	bool SetNoBlock(bool on);	
	// Close socket
	int Close();

	// Get errno
	static int GetError();
	
	//#pragma region just for win32
	// Init winsock DLL 
	static int Init();	
	// Clean winsock DLL
	static int Clean();
	//#pragma endregion

	// Domain parse
	static bool DnsParse(const char* domain, char* ip);

	BSDSocket& operator = (SOCKET s);

	operator SOCKET ();

protected:
	addrinfo* parseIPV46(const char* ip);

protected:
	SOCKET m_sock;
	bool	is_ipv4;

};

#endif
