/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __BASE_NET_SOCKET_MANAGER_H__
#define __BASE_NET_SOCKET_MANAGER_H__

#include "BSDSocket.h"
#include "Thread_t.h"
#include "../messagequeue/message_queue.h"
#include "IMessageProtocolDelegate.h"
#include "net-common.h"

namespace net
{
    enum socket_type_e
    {
        kSockType_TCP = SOCK_STREAM,
        kSockType_UDP = SOCK_DGRAM,
    };
    enum { kSocketFlag_NOSIGNAL = 0};

    enum socket_status_e
    {
        kSockStatus_OK = 0,
		kSockStatus_Connect = -99000,
        kSockStatus_ConnFailed = -99001,
        kSockStatus_Broken = -99002,		
    };

    class ClientSocket : public BSDSocket
    {
    public:
        ClientSocket() 
            : m_port(0)
            , m_is_connected(false)
            , m_is_connecting(false)
        {}
        ~ClientSocket()
        {}

        void init(socket_type_e type,const std::string &ip,unsigned short port)
        {
            m_type = type;
            set_ip(ip, port);
        }

        bool open(socket_type_e type,const std::string &ip,unsigned short port)
        {
            //if (Create(AF_INET, type, 0))
			if (CreateEx(ip.c_str(), type, 0))
            {
                set_ip(ip, port);
                if (! Connect(m_ip.c_str(), m_port))
                {
                    Close();
                    return false;
                }
                return true;
            }
            return false;
        }

        SOCKET getSocket() const
        { return m_sock; }

        void set_ip(const std::string &ip, unsigned short port) 
        { m_ip = ip; m_port = port; }
        const char * get_ip() const
        { return m_ip.c_str(); }
        unsigned short get_port() const
        { return m_port; }

        bool IsConnected() 
        { return m_is_connected; }
        void _set_connected(bool _is_connected) 
        { m_is_connected = _is_connected; }

        bool is_connecting() 
        { return m_is_connecting; }
        void _set_connecting(bool _is_connecting) 
        { m_is_connecting = _is_connecting; }

        socket_type_e sock_type() 
        { return m_type; }

    private:
        std::string     m_ip;
        unsigned short  m_port;
        socket_type_e   m_type;
        bool            m_is_connected;
        bool            m_is_connecting;
    };

class SocketManager;

class SocketManager : public thread_ex
{
public:
    typedef std::map<unsigned int, ClientSocket*>   ClientSocketMap_t;

    struct stReconnectSock
    {
        stReconnectSock()
        {
            this->ip            = "";
            this->port          = 0;
            this->waitStartTime = 0;
            this->delaySecs     = 1;
            this->reConnectCount= 0;
            this->isConnecting  = false;
        }
        stReconnectSock(const stReconnectSock& src)
        {
            this->ip            = src.ip            ;
            this->port          = src.port          ;
            this->waitStartTime = src.waitStartTime ;
            this->delaySecs     = src.delaySecs     ;
            this->reConnectCount= src.reConnectCount;
            this->isConnecting  = src.isConnecting  ;
        }

        std::string ip;
        unsigned short port;
        time_t       waitStartTime;
        unsigned int delaySecs;
        unsigned int reConnectCount;
        bool         isConnecting;

        stReconnectSock & operator = (const stReconnectSock& src)
        {
            this->ip            = src.ip            ;
            this->port          = src.port          ;
            this->waitStartTime = src.waitStartTime ;
            this->delaySecs     = src.delaySecs     ;
            this->reConnectCount= src.reConnectCount;
            this->isConnecting  = src.isConnecting  ;
            return *this;
        }
    };
    typedef std::map<unsigned int, stReconnectSock>        IPPortMap_t;

public:
    SocketManager();
    ~SocketManager();
protected:
    virtual void Run();

public:

    bool init(bool autoReconnect = false);

    bool addConnect(unsigned int connIndex, const std::string &ip, unsigned short port);

    bool removeConnect(unsigned int connIndex);

    bool SendMessage(unsigned int connIndex, const char *szBuff, size_t len);
    // @input: szDstBuff  allocated buffer; len: length of szDstBuff
    // @output: peeked buffer length
    int PeekMessage(char *szDstBuff, size_t len, unsigned int &connIndex, int &sockStatus);

    // reconnect after delaied seconds 
    void reConnect(unsigned int connIndex, unsigned int delaySecs);

    // @[input] protocolDelegate: handler that will process recieved data
    void registerConnectProcessDelegate(IMessageProtocolDelegate *connDelegate);

    std::string IPforConnIndex(unsigned int connIndex);
    unsigned short PortForConnIndex(unsigned int connIndex);

	void setAutoConnect(bool autoConnect);
	bool isAutoConnect();
 
    ClientSocket* getConnectByIndex(unsigned int connIndex);

    bool isRuning() { return m_isRunning; }

protected:
    void InstallMessageQueue();

	void clearSocket();

    bool _remove_connect(unsigned int connIndex);

    int _sendTo(unsigned int connIndex, const char *szBuff, size_t len);

    bool PostRecvMessageFrom(unsigned int connIndex, ClientSocket *sock, int len);
    void onConnect(unsigned int connIndex, ClientSocket *sock);
    void onDisconnected(unsigned int connIndex, ClientSocket *sock);
    void onConnectFailed(unsigned int connIndex, ClientSocket *sock);

    void _check_adding();
    void _check_deleting();
    void _check_reconnecting();

    void test();
    
private:
    void set_socket_fd(SOCKET s, bool bRead, bool bWrite, bool bException);
    void rebuild_fdset();
    int sockes_select(struct timeval *tsel);
    void _check_error_on_sock(unsigned int connIndex, ClientSocket *sock);

    bool _do_post_recved_buff(char *pStart, unsigned int msgLen
        , int sockStatus, unsigned int connIndex);

private:
    SOCKET m_maxsock; ///< Highest file descriptor + 1 in active sockets list
    fd_set m_rfds; ///< file descriptor set monitored for read events
    fd_set m_wfds; ///< file descriptor set monitored for write events
    fd_set m_efds; ///< file descriptor set monitored for exceptions
    ClientSocketMap_t   m_clientSocketMap;
    ClientSocketMap_t   m_addingSocketMap;
    ClientSocketMap_t   m_deletingSocketMap;
	ClientSocketMap_t   m_closeSocketMap;
#ifdef _WIN32
    bool    m_wsainited;
#endif
    char* m_pCodeBuffer;    // send temporary buffer
    char* m_pRecvBuffer;    // recv temporary buffer
    // m_p_total_recv_buf is started from 4-byte before m_pRecvBuffer, 
    // inorder to there is enough space for format 4-byte connection 
    // index before received data, that means m_pRecvBuffer pointed
    // to the address of 4-byte after m_p_total_recv_buf
    char* m_p_total_recv_buf;
    int m_iCodeBufferLen;
    int m_iRecvBufferLen;
    int m_iRecvBufferOffset;
    CMessageQueue   m_msgQueue;
    IMessageProtocolDelegate *m_connectProcessDelegate;
    bool m_autoReconnect;
    bool m_bRetry;
    bool m_allConnectSuccess;
    bool m_isRunning;
	bool m_isDispatching;

    unsigned int m_maxReconnCount;  // reconnecting max count
    unsigned int m_connCount;
    IPPortMap_t m_reconnectingMap;  // for reconnect ip/port
    IPPortMap_t m_disconnectedMap;  // un-connected|disconnected ip/port
    IPPortMap_t m_addReconnMap;     // user added for reconnect ip/port
};

}

#endif

