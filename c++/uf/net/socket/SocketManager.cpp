/************************************************************************/
/*                                                                      */
/************************************************************************/
#include "SocketManager.h"
#include "cocos2d.h"
#include <iostream>


#define _SOCK_DEBUG1(_x_)   _x_
#define _SOCK_DEBUG2(_x_)   _x_

namespace net 
{

    USING_NS_CC;

    const unsigned int kRecvBufHeaderLen = sizeof(unsigned int) + sizeof(int);

SocketManager::SocketManager()
    : m_maxsock(0)
#ifdef _WIN32
    , m_wsainited(false)
#endif
    , m_pCodeBuffer(NULL)
    , m_pRecvBuffer(NULL)
    , m_p_total_recv_buf(NULL)
    , m_iCodeBufferLen(0)
    , m_iRecvBufferLen(0)
    , m_iRecvBufferOffset(0)
    , m_connectProcessDelegate(NULL)
    , m_autoReconnect(false)
    , m_bRetry(false)
    , m_allConnectSuccess(false)
    , m_isRunning(false)
	, m_isDispatching(false)
    , m_maxReconnCount(kReconnectCount)
    , m_connCount(0)
{

    FD_ZERO(&m_rfds);
    FD_ZERO(&m_wfds);
    FD_ZERO(&m_efds);
}

SocketManager::~SocketManager()
{
    if (m_isRunning)
    {
        m_isRunning = false;
        SLEEP_MS(1000);  // wait for thread close
    }
    for (ClientSocketMap_t::iterator iter = m_clientSocketMap.begin();
        iter != m_clientSocketMap.end(); ++iter)
    {
        if (iter->second)
        {
            //m_socketHandler.Remove(iter->second);
            iter->second->Close();
            delete iter->second;
            iter->second = NULL;
        }
    }
    m_clientSocketMap.clear();

    m_connectProcessDelegate = NULL;

    if (NULL != m_pCodeBuffer)
    {
        delete [] m_pCodeBuffer;
        m_pCodeBuffer = NULL;
    }
    if (NULL != m_p_total_recv_buf)
    {
        delete [] m_p_total_recv_buf;
        m_p_total_recv_buf = NULL;
    }
#ifdef _WIN32
    if (m_wsainited)
    {
        BSDSocket::Clean();
        m_wsainited = false;
    }
#endif
}

bool SocketManager::init(bool autoReconnect/* = false */)
{
    m_autoReconnect = autoReconnect;
    //分配内存池页面大小 
    if (NULL != m_pCodeBuffer)
    {
        delete [] m_pCodeBuffer;
    }
    m_iCodeBufferLen = kSEND_BUFF_LEN;
    m_pCodeBuffer = new char[m_iCodeBufferLen];

    if (NULL != m_p_total_recv_buf)
    {
        delete [] m_p_total_recv_buf;
    }
    m_iRecvBufferLen = kRECV_BUFF_LEN;
    // this adding sizeof(uint) buffer is inorder to enough space for save current
    // received from connection index, while, to 
    m_p_total_recv_buf = new char[m_iRecvBufferLen + kRecvBufHeaderLen];
    m_pRecvBuffer = m_p_total_recv_buf + kRecvBufHeaderLen;
    m_iRecvBufferOffset = 0;

#ifdef _WIN32
    if (!m_wsainited)
    {
        BSDSocket::Init();
        m_wsainited = true;
    }
#endif

    // 
    InstallMessageQueue();

    return true;
}

void SocketManager::Run()
{
    int code = 0;
    int n = 0;
    //int nBytes;
    struct timeval _sel_tv;
    _sel_tv.tv_sec = 0;
    _sel_tv.tv_usec = 100;
    m_isRunning = true;
    while (m_isRunning)
    {
        _check_deleting();
        _check_reconnecting();
        _check_adding();

        if (m_clientSocketMap.empty())
        {
            SLEEP_MS(100);
            continue;
        }
        PACKAGE msg;
        code = m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_send, &msg, 0, PM_NOREMOVE);
        if (code == 0)
        {
            // first 4-byte save for connection index
            const unsigned int *pIndex = (unsigned int *)(msg.buffer);
            ClientSocket *sock = getConnectByIndex(*pIndex);
            if (NULL != sock && sock->IsConnected())
            {
                // sending data start after the 4-byte
                const char *sendBuff = msg.buffer + sizeof(unsigned int);
                int len = sock->Send(sendBuff, 
                    msg.length - sizeof(unsigned int),
                    kSocketFlag_NOSIGNAL);
                if (len < 0)
                {
                    _check_error_on_sock(*pIndex, sock);
                }
                m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_send, &msg, 0, PM_REMOVE);
                //CCLOG("send buff xxxxxxxx %d size.", len);
            }
        }

        n = sockes_select(&_sel_tv);

        SLEEP_MS(30);
    }
}

int SocketManager::_sendTo( unsigned int connIndex, const char *szBuff, size_t len )
{
    ClientSocketMap_t::iterator iter = m_clientSocketMap.find(connIndex);
    if (iter != m_clientSocketMap.end() && iter->second)
    {
        if (iter->second->Send(szBuff, len, kSocketFlag_NOSIGNAL) < 0)
        {
            _check_error_on_sock(connIndex, iter->second);
        }
        return len;
    }
    return -1;
}

bool SocketManager::addConnect( unsigned int connIndex, const std::string &ip, unsigned short port )
{
    ClientSocketMap_t::iterator iter = m_clientSocketMap.find(connIndex);
    if (iter != m_clientSocketMap.end())
    {
		CCLOG("client socket has find connindex[%d]", connIndex);
        return false;
    }
    iter = m_addingSocketMap.find(connIndex);
    if (iter != m_addingSocketMap.end())
    {
		CCLOG("adding socket has find connindex[%d]", connIndex);
        return false;
    }

    ClientSocket *s = new ClientSocket();
    s->SetNoBlock(false);

    s->init(kSockType_TCP, ip, port);

	iter = m_addingSocketMap.begin();
	if (iter != m_addingSocketMap.end()) {
		iter->second->Close();
		delete iter->second;
		m_addingSocketMap.erase(iter);
	}
    m_addingSocketMap.insert(std::make_pair(connIndex, s));

    return true;
}

bool SocketManager::removeConnect( unsigned int connIndex )
{
    if (m_reconnectingMap.erase(connIndex))
    {
        _SOCK_DEBUG1(
            CCLOG("removeConnect erased connIndex:%d from m_reconnectingMap",
            connIndex);
        )
    }

	ClientSocketMap_t::iterator iter = m_clientSocketMap.find(connIndex);
	if (iter != m_clientSocketMap.end())
	{
		//ClientSocketMap_t::iterator iterDisconnect = m_deletingSocketMap.find(connIndex);
		//if (iterDisconnect == m_deletingSocketMap.end()) {
			SOCKET removeS = 0;
			if (iter->second) {
				iter->second->Close();
				removeS = iter->second->getSocket();
				m_closeSocketMap[iter->first] = iter->second;
			}			
			//m_clientSocketMap.erase(iter);

			CCLOG("old maxsock:%d", m_maxsock);
			m_maxsock = 0;
			for (ClientSocketMap_t::iterator it = m_clientSocketMap.begin(); 
				it != m_clientSocketMap.end(); ++it)
			{
				SOCKET s = it->second->getSocket();
				if (s != removeS)
					m_maxsock = s > m_maxsock ? s : m_maxsock;
			}
			CCLOG("new maxsock:%d", m_maxsock);
		//} 
	}

	return true;
}

bool SocketManager::_remove_connect( unsigned int connIndex )
{
	ClientSocketMap_t::iterator iter = m_clientSocketMap.find(connIndex);
	if (iter != m_clientSocketMap.end())
	{
		if (iter->second)
		{
			set_socket_fd(iter->second->getSocket(), false, false, false);
			iter->second->Close();
			delete iter->second;
			iter->second = NULL;
		}
		m_clientSocketMap.erase(iter);

		iter = m_closeSocketMap.find(connIndex);
		if (iter != m_closeSocketMap.end()) {
			m_closeSocketMap.erase(iter);
		}

		m_maxsock = 0;
		for (ClientSocketMap_t::iterator it = m_clientSocketMap.begin(); 
			it != m_clientSocketMap.end(); ++it)
		{
			SOCKET s = it->second->getSocket();
			m_maxsock = s > m_maxsock ? s : m_maxsock;
		}
		return true;
	}
	return false;
}

void SocketManager::InstallMessageQueue()
{
    int len = kSEND_QUEUE_BUFF_LEN;
    char* p = new char[len];
    m_msgQueue.Install(CMessageQueue::enm_queue_client_send, p, len);
    len = kRECV_QUEUE_BUFF_LEN;
    p = new char[len];
    m_msgQueue.Install(CMessageQueue::enm_queue_client_recv, p, len);
}

void SocketManager::clearSocket() {
	for (ClientSocketMap_t::iterator it = m_clientSocketMap.begin(); 
		it != m_clientSocketMap.end(); ++it)
	{
		if (it->second) {
			it->second->Close();
			delete it->second;
		}
	}

	m_clientSocketMap.clear();
}

bool SocketManager::SendMessage( unsigned int connIndex, const char *szBuff, size_t len )
{
    if (getConnectByIndex(connIndex) == NULL)
    {
        return false;
    }
    // first 4-bytes save the connection index
    unsigned int *pIndex = (unsigned int*)m_pCodeBuffer;
    *pIndex = connIndex;
    // the bellow buffer save the sending data
    char *pCursor = m_pCodeBuffer + sizeof(unsigned int);
    memcpy(pCursor, szBuff, len);
    pCursor += len;

    int code = m_msgQueue.PostMessage(CMessageQueue::enm_queue_client_send,
        m_pCodeBuffer, pCursor - m_pCodeBuffer);
    return (code == 0) ? true : false;
}

int SocketManager::PeekMessage( char *szDstBuff, size_t len, unsigned int &connIndex, int &sockStatus )
{
    PACKAGE msg;
    int code = m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_recv, &msg, 0, PM_NOREMOVE);
    if (code == 0)
    {
        if (msg.length - kRecvBufHeaderLen <= len)
        {
            // real received data started from 4-byte after the msg.buffer,
            // the first 4-byte data saves the connection index
            memcpy(szDstBuff, msg.buffer + kRecvBufHeaderLen, 
                msg.length - kRecvBufHeaderLen );
            sockStatus = *((int*)msg.buffer);
            connIndex = *((unsigned int*)(msg.buffer + sizeof(int)));
            m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_recv, &msg, 0, PM_REMOVE);
            return msg.length;
        }
        else
        {
            CCLOG("Error: PeekMessage(socket) buffer_len:[%d] smaller than msg.length:[%d]",
                len, msg.length - kRecvBufHeaderLen);
        }

        m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_recv, &msg, 0, PM_REMOVE);
        sockStatus = -1;
        return -1;
    }
    sockStatus = -1;
    return -1;
}

bool SocketManager::PostRecvMessageFrom( unsigned int connIndex, ClientSocket *sock, int len )
{
    if (NULL == sock || NULL == m_pRecvBuffer)
    {
        return false;
    }

    int readLen = 0;
    unsigned int msg_len;
    int curLen = 0;
    char *pStart = m_pRecvBuffer;
    char *pEnd = m_pRecvBuffer;
    char *pPostAddr = NULL;
    // 收到的包可能过多，有可能超过临时缓冲区 m_pRecvBuffer，采取分批读取。 
    // 上次收到的包可能是不完整的，临时存在缓冲区的起始位置，读取到的数据从 
    // 缓冲区的 m_iRecvBufferOffset 位置开始开始存。将每个拆分的完整包(包长
    // msg_len)压入msgQueue 
    while (len > 0)
    {
        readLen = len > m_iRecvBufferLen - m_iRecvBufferOffset ? 
            m_iRecvBufferLen - m_iRecvBufferOffset : len;
        readLen = sock->Recv(m_pRecvBuffer + m_iRecvBufferOffset, readLen,
            kSocketFlag_NOSIGNAL);
        //CCLOG("recv buff zzzzzzzz %d size.", readLen);
        if (readLen <= 0)
        {
            _check_error_on_sock(connIndex, sock);
            return false;
        }
        if (readLen < len)
        {
            len = readLen;
        }

        pStart = m_pRecvBuffer;
        pEnd = m_pRecvBuffer + m_iRecvBufferOffset + readLen;
        while (pStart < pEnd)
        {
            curLen = pEnd - pStart;
            // curLen = readLen + m_iRecvBufferOffset - (pStart - m_pRecvBuffer);
            if (m_connectProcessDelegate 
                && m_connectProcessDelegate->parseBufferLen(
                connIndex, pStart, curLen, msg_len))
            {
                if (msg_len > (unsigned int)m_iRecvBufferLen)
                {
                    CCLOG("Fatal: parsed body len bigger than limit: parsedLen:[%d] limit:[%d]", msg_len, m_iRecvBufferLen);
                    msg_len = curLen;
                }
            }

            if (msg_len > (unsigned int)curLen)
            {
                // 此处包未收完 
                m_iRecvBufferOffset = curLen;
                memmove(m_pRecvBuffer, pStart, m_iRecvBufferOffset);
                break;
            }

            if (!_do_post_recved_buff(pStart, msg_len, kSockStatus_OK, connIndex))
            {
                return false;
            }
            //// format 4-byte connection status and 4-byte connection index
            //// on the addres before pStart, thus, the 4-bytes space is enough
            //// and dirty now before pStart, so, this operation is safe.
            //pPostAddr = pStart - kRecvBufHeaderLen;
            //*((int*)pPostAddr) = (int)kSockStatus_OK;
            //*((unsigned int*)(pPostAddr + sizeof(int))) = connIndex;

            //if (m_msgQueue.PostMessage(CMessageQueue::enm_queue_client_recv, 
            //    pPostAddr, msg_len + kRecvBufHeaderLen) != 0)
            //{
            //    return false;
            //}

            pStart += msg_len;
            if (pStart == pEnd)
            {
                m_iRecvBufferOffset = 0;
                break;
            }
        }

        len -= readLen;
        if (len > 0)
        {
            readLen = len > m_iRecvBufferLen ? m_iRecvBufferLen : len;
        }
    }
    return true;
}

ClientSocket* SocketManager::getConnectByIndex( unsigned int index )
{
    ClientSocketMap_t::iterator iter = m_clientSocketMap.find(index);
    if (iter != m_clientSocketMap.end())
    {
        return iter->second;
    }
    return NULL;
}

void SocketManager::setAutoConnect(bool autoConnect) {
	m_autoReconnect = autoConnect;
}

bool SocketManager::isAutoConnect() {
	return m_autoReconnect;
}


void SocketManager::onConnect( unsigned int connIndex, ClientSocket *sock )
{
    do 
    {
        CC_BREAK_IF(!sock);

        ClientSocketMap_t::const_iterator iter = m_clientSocketMap.find(connIndex);
        CC_BREAK_IF(iter == m_clientSocketMap.end());
        
        sock->_set_connected(true);

        _SOCK_DEBUG2(
            CCLOG("onConnect connIndex:[%d] address:[%s:%d] connected success.", 
            connIndex, sock->get_ip(), sock->get_port());
        )

        m_reconnectingMap.erase(connIndex);
        m_disconnectedMap.erase(connIndex);
        _SOCK_DEBUG1(
            CCLOG("onConnect erase connIndex from m_reconnectingMap | m_disconnectedMap", connIndex);
        )

//         if (m_connectProcessDelegate)
//         {
//             m_connectProcessDelegate->onConnectSuccess(connIndex);
//         }
		_do_post_recved_buff(m_pRecvBuffer, 0, kSockStatus_Connect, connIndex);

        ++m_connCount;

        if (m_reconnectingMap.empty())
        {
            m_allConnectSuccess = true;
            for (ClientSocketMap_t::const_iterator iter=m_clientSocketMap.begin();
                iter != m_clientSocketMap.end(); ++iter)
            {
                if (iter->second && ! iter->second->IsConnected())
                {
                    m_allConnectSuccess = false;
                    break;
                }
            }
        }
        else if (m_autoReconnect)
        {
            m_allConnectSuccess = false;
        }

        if (m_allConnectSuccess)
        {
            m_bRetry = false;
        }
    } while (0);
}

void SocketManager::onDisconnected( unsigned int connIndex, ClientSocket *sock )
{
    do 
    {
        CC_BREAK_IF(!sock);

        m_allConnectSuccess = false;

        ClientSocketMap_t::iterator iter = m_clientSocketMap.find(connIndex);
        if (iter != m_clientSocketMap.end()) {
			m_deletingSocketMap[iter->first] = iter->second;
		} else {
			iter = m_closeSocketMap.find(connIndex);
			if (iter != m_closeSocketMap.end()) {
				m_closeSocketMap.erase(iter);
				m_deletingSocketMap[iter->first] = iter->second;
			}
		}

        stReconnectSock reconn;
        reconn.ip = sock->get_ip();
        reconn.port = sock->get_port();
        reconn.waitStartTime = time(NULL);
        _SOCK_DEBUG2(
        CCLOG("onDisconnected connIndex:[%d] address:[%s:%d] disconnected.", 
            connIndex, reconn.ip.c_str(), reconn.port);
        )
        if (m_autoReconnect)
        {
            m_reconnectingMap[connIndex] = reconn;
            _SOCK_DEBUG1(
                CCLOG("onDisconnected connIndex:[%d] address:[%s:%d] add to m_reconnectingMap.", 
                connIndex, reconn.ip.c_str(), reconn.port);
            )
        }
        else
        {
            m_disconnectedMap[connIndex] = reconn;
            _SOCK_DEBUG1(
                CCLOG("onDisconnected connIndex:[%d] address:[%s:%d] add to m_disconnectedMap.", 
                connIndex, reconn.ip.c_str(), reconn.port);
            )
            //if (m_connectProcessDelegate)
            //{
            //    m_connectProcessDelegate->onConnectBroken(connIndex);
            //}
            _do_post_recved_buff(m_pRecvBuffer, 0, kSockStatus_Broken, connIndex);
        }

        if (m_connCount > 0)
        {
            --m_connCount;
        }
    } while (0);
}

void SocketManager::onConnectFailed( unsigned int connIndex, ClientSocket *sock )
{
    do 
    {
        CC_BREAK_IF(!sock);

        m_allConnectSuccess = false;
        m_deletingSocketMap[connIndex] = sock;

        bool wouldDelete = true;
        if (m_autoReconnect)
        {
            wouldDelete = false;
            IPPortMap_t::iterator ip_it = m_reconnectingMap.find(connIndex);
            if (ip_it != m_reconnectingMap.end())
            {
                if (ip_it->second.reConnectCount > 0)
                {
                    _SOCK_DEBUG2(
                    CCLOG("onConnectFailed connIndex[%d] address:[%s:%d] reconnect %dth failed", 
                        connIndex, ip_it->second.ip.c_str(), ip_it->second.port, 
                        ip_it->second.reConnectCount);
                    )
                    if (ip_it->second.reConnectCount >= m_maxReconnCount)
                    {
                        wouldDelete = true;
                    }
                }
                else
                {
                    _SOCK_DEBUG2(
                    CCLOG("onConnectFailed connIndex:[%d] address:[%s:%d] connect failed.", 
                        connIndex, ip_it->second.ip.c_str(), ip_it->second.port);
                    )
                }
            }
        }

        if (wouldDelete)
        {
            stReconnectSock reconn;
            reconn.ip = sock->get_ip();
            reconn.port = sock->get_port();
            reconn.waitStartTime = time(NULL);
            m_disconnectedMap[connIndex] = reconn;
            _SOCK_DEBUG2(
            CCLOG("onConnectFailed connIndex:[%d] address:[%s:%d] connect failed.", 
                connIndex, reconn.ip.c_str(), reconn.port);
            )

            m_reconnectingMap.erase(connIndex);
            _SOCK_DEBUG1(
                CCLOG("onConnectFailed erase connIndex:[%d] from m_reconnectingMap.", connIndex);
            )

            //if (m_connectProcessDelegate)
            //{
            //    m_connectProcessDelegate->onConnectFail(connIndex);
            //}
            _do_post_recved_buff(m_pRecvBuffer, 0, kSockStatus_ConnFailed, connIndex);
        }

    } while (0);
}

void SocketManager::registerConnectProcessDelegate( IMessageProtocolDelegate *connDelegate )
{
    m_connectProcessDelegate = connDelegate;
}

void SocketManager::_check_adding()
{
    if (!m_addingSocketMap.empty())
    {
        ClientSocket *pSock = NULL;
        for (ClientSocketMap_t::iterator iter = m_addingSocketMap.begin();
            iter != m_addingSocketMap.end(); ++iter)
        {
            pSock = iter->second;
            if (!pSock)
            {
                continue;
            }
            if(!pSock->open(pSock->sock_type(), pSock->get_ip(), pSock->get_port())) 
            {
                onConnectFailed(iter->first, pSock);
                continue;
            }

			set_socket_fd(pSock->getSocket(), true, false, true);
			m_maxsock = pSock->getSocket() > m_maxsock ? pSock->getSocket() : m_maxsock;
			m_clientSocketMap[iter->first] = pSock;	

			onConnect(iter->first, pSock);   
        }
        m_addingSocketMap.clear();
    }
}

void SocketManager::_check_deleting()
{
    if (!m_deletingSocketMap.empty())
    {
        for (ClientSocketMap_t::iterator iter = m_deletingSocketMap.begin();
            iter != m_deletingSocketMap.end(); ++iter)
        {
            if (!_remove_connect(iter->first))
            {
                if (iter->second)
                {
                    iter->second->Close();
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        m_deletingSocketMap.clear();
    }
}

void SocketManager::_check_reconnecting( )
{
    if (!m_addReconnMap.empty())
    {
        for (IPPortMap_t::iterator iter = m_addReconnMap.begin();
            iter != m_addReconnMap.end(); ++iter)
        {
            m_reconnectingMap[iter->first] = iter->second;
            _SOCK_DEBUG1(
                CCLOG("_check_reconnecting add connIndex:[%d] to m_reconnectingMap",  iter->first);
            )
        }
        m_addReconnMap.clear();
    }

    if (m_bRetry || m_autoReconnect)
    {
        if (!m_reconnectingMap.empty())
        {
            time_t curTime = time(NULL);
            for (IPPortMap_t::iterator iter = m_reconnectingMap.begin();
                iter != m_reconnectingMap.end(); ++iter)
            {
                if (iter->second.reConnectCount <= m_maxReconnCount && 
                    curTime - iter->second.waitStartTime >= iter->second.delaySecs
                    /*&& ! iter->second.isConnecting*/)
                {
                    if (NULL != getConnectByIndex(iter->first))
                    {
                        _remove_connect(iter->first);
                    }
                    iter->second.isConnecting = true;
                    iter->second.reConnectCount++;
                    iter->second.waitStartTime = curTime;

                    _SOCK_DEBUG2(
                    CCLOG("_check_reconnecting connIndex:%d %dth reconnet to %s:%d after %d secs",
                        iter->first, iter->second.reConnectCount, iter->second.ip.c_str(),
                        iter->second.port, iter->second.delaySecs);
                    )
                    addConnect(iter->first, iter->second.ip, iter->second.port);
                }
            }
        }
    }
}

void SocketManager::reConnect( unsigned int connIndex, unsigned int delaySecs )
{
    std::string ip_;
    unsigned short port_;

    _SOCK_DEBUG1(
    CCLOG("reConnect connIndex:[%d] delaySecs:[%d]", connIndex, delaySecs);
    )
    IPPortMap_t::iterator ip_it = m_reconnectingMap.find(connIndex);
    if (ip_it != m_reconnectingMap.end())
    {
        _SOCK_DEBUG1(
        CCLOG("reConnect found connIndex:[%d] from m_reconnectingMap", connIndex);
        )
        //ip_it->second.delaySecs = delaySecs;
        //ip_it->second.isConnecting = false;
        //ip_it->second.reConnectCount = 0;
        ip_ = ip_it->second.ip;
        port_ = ip_it->second.port;
        return;
    }
    else
    {
        /*IPPortMap_t::iterator */ip_it = m_disconnectedMap.find(connIndex);
        if (ip_it != m_disconnectedMap.end())
        {
            _SOCK_DEBUG1(
                CCLOG("reConnect found connIndex:[%d] from m_disconnectedMap", 
                connIndex);
            )
            ip_ = ip_it->second.ip;
            port_ = ip_it->second.port;
        }
        else
        {
            ClientSocket *pSock = getConnectByIndex(connIndex);
            if (pSock)
            {
                _SOCK_DEBUG1(
                    CCLOG("reConnect found connIndex:[%d] from m_clientSocketMap",  connIndex);
                )
                ip_ = pSock->get_ip();
                port_ = pSock->get_port();
            }
            else
            {
                return;
            }
        }
    }

    m_bRetry = true;
    //addConnect(connIndex, ip_, port_);

    stReconnectSock reconn;
    reconn.ip = ip_;
    reconn.port = port_;
    reconn.waitStartTime = time(NULL);
    reconn.delaySecs = delaySecs;
    reconn.isConnecting = false;
    reconn.reConnectCount = 0;
    m_addReconnMap[connIndex] = reconn;
    _SOCK_DEBUG1(
        CCLOG("reConnect add connIndex:[%d] to m_addReconnMap",  connIndex);
    )
}

void SocketManager::test()
{
    int connIndex = 1;
    int _test_iRecvBufferLen = 256; // = 128;
    int _test_iRecvBufferOffset = 17;
    char *_test_pRecvBuffer = new char[_test_iRecvBufferLen];
    int len = _test_iRecvBufferLen / 2; // * 2;
    char *_test_pSockBuffer = new char[len + 1];

    int _test_totalReadonLen = 0;
    uint32_t _test_onePieceLen = 50;
    char *_test_pSockStart = _test_pSockBuffer;

    // put offset part in pRecvBuffer
    if (_test_iRecvBufferOffset > 0)
    {
        if (_test_iRecvBufferOffset < sizeof(uint32_t))
        {
            _test_iRecvBufferOffset = sizeof(uint32_t);
        }
        else if (_test_iRecvBufferOffset >= (int)_test_onePieceLen)
        {
            _test_iRecvBufferOffset = _test_onePieceLen - 1;
        }
        *reinterpret_cast<uint32_t*>(_test_pRecvBuffer) = htonl(_test_onePieceLen);
        uint32_t i = 0;
        for (; i < _test_iRecvBufferOffset - sizeof(uint32_t); ++i)
        {
            _test_pRecvBuffer[i + sizeof(uint32_t)] = '0' + i % 10;
        }
        int idx = 0;
        for (; i < _test_onePieceLen - sizeof(uint32_t); ++i)
        {
            _test_pSockStart[idx++] = '0' + i % 10;
        }
        _test_pSockStart += _test_onePieceLen - _test_iRecvBufferOffset;
    }

    while (_test_pSockStart < _test_pSockBuffer + len)
    {
        // len
        if ((int)(_test_pSockStart - _test_pSockBuffer + _test_onePieceLen) > len)
        {
            _test_onePieceLen = len - (_test_pSockStart - _test_pSockBuffer);
        }

        *reinterpret_cast<uint32_t*>(_test_pSockStart) = htonl(_test_onePieceLen);
        _test_pSockStart += sizeof(uint32_t);
        
        for (uint32_t i = 0; i < _test_onePieceLen - sizeof(uint32_t); ++i)
        {
            _test_pSockStart[i] = '0' + i % 10;
        }
        _test_pSockStart[_test_onePieceLen - sizeof(uint32_t)] = 0;
        CCLOG("src msg_len:[%d] data:[%s]", _test_onePieceLen, _test_pSockStart);

        _test_pSockStart += _test_onePieceLen - sizeof(uint32_t);
    }
    CCLOG("======== start ========");

    int readLen = 0;
    unsigned int msg_len;
    int curLen = 0;
    char *pStart = _test_pRecvBuffer;
    char *pEnd = _test_pRecvBuffer;
    // 收到的包可能过多，有可能超过临时缓冲区 m_pRecvBuffer，采取分批读取。 
    // 上次收到的包可能是不完整的，临时存在缓冲区的起始位置，读取到的数据从 
    // 缓冲区的 m_iRecvBufferOffset 位置开始开始存。将每个拆分的完整包(包长
    // msg_len)压入msgQueue 
    while (len > 0)
    {
        readLen = len > _test_iRecvBufferLen - _test_iRecvBufferOffset ? 
            _test_iRecvBufferLen - _test_iRecvBufferOffset : len;
        // readLen = sock->ReadInput(_test_pRecvBuffer + _test_iRecvBufferOffset, readLen);

        CCLOG("len:[%d] readLen:[%d] offset:[%d]", 
            len, readLen, _test_iRecvBufferOffset);
        /// read
        memcpy(_test_pRecvBuffer + _test_iRecvBufferOffset, 
            _test_pSockBuffer + _test_totalReadonLen, readLen);
        _test_totalReadonLen += readLen;

        if (readLen <= 0)
        {
            return ;
        }

        pStart = _test_pRecvBuffer;
        pEnd = _test_pRecvBuffer + _test_iRecvBufferOffset + readLen;
        while (pStart < pEnd)
        {
            curLen = pEnd - pStart;
            // curLen = readLen + m_iRecvBufferOffset - (pStart - m_pRecvBuffer);
            if (m_connectProcessDelegate 
                && m_connectProcessDelegate->parseBufferLen(
                connIndex, pStart, curLen, msg_len))
            {
                if ((int)msg_len > _test_iRecvBufferLen)
                {
                    CCLOG("Fatal: parsed body len bigger than limit: parsedLen:[%d] limit:[%d]", msg_len, _test_iRecvBufferLen);
                    msg_len = curLen;
                }
            }
            else
            {
                msg_len = curLen;
            }

            if ((int)msg_len > curLen)
            {
                // 此处包未收完 
                _test_iRecvBufferOffset = curLen;
                memmove(_test_pRecvBuffer, pStart, _test_iRecvBufferOffset);
                break;
            }

            //if (m_msgQueue.PostMessage(CMessageQueue::enm_queue_client_recv, pStart, msg_len) != 0)
            //{
            //    return false;
            //}
            // output 
            int tmpBuffLen = msg_len - sizeof(uint32_t);
            char *pTmpBuff = new char[tmpBuffLen + 1];
            memcpy(pTmpBuff, pStart + sizeof(uint32_t), tmpBuffLen);
            pTmpBuff[tmpBuffLen] = 0;
            CCLOG("parsed msg_len:[%d] data:[%s]", msg_len, pTmpBuff);
            delete []pTmpBuff;

            pStart += msg_len;
            if (pStart == pEnd)
            {
                _test_iRecvBufferOffset = 0;
                break;
            }
        }

        len -= readLen;
        if (len > 0)
        {
            readLen = len > _test_iRecvBufferLen ? _test_iRecvBufferLen : len;
        }
    }

    CCLOG("======== end ========");
    delete []_test_pRecvBuffer;
    delete []_test_pSockBuffer;
}

std::string SocketManager::IPforConnIndex( unsigned int connIndex )
{
    ClientSocket *sock = getConnectByIndex(connIndex);
    if (NULL == sock)
    {
        IPPortMap_t::iterator iter = m_reconnectingMap.find(connIndex);
        if (iter != m_reconnectingMap.end())
        {
            return iter->second.ip;
        }
        else
        {
            iter = m_disconnectedMap.find(connIndex);
            if (iter != m_disconnectedMap.end())
            {
                return iter->second.ip;
            }
        }
        return "";
    }

    return sock->get_ip();
}

unsigned short SocketManager::PortForConnIndex( unsigned int connIndex )
{
    ClientSocket *sock = getConnectByIndex(connIndex);
    if (NULL == sock)
    {
        IPPortMap_t::iterator iter = m_reconnectingMap.find(connIndex);
        if (iter != m_reconnectingMap.end())
        {
            return iter->second.port;
        }
        else
        {
            iter = m_disconnectedMap.find(connIndex);
            if (iter != m_disconnectedMap.end())
            {
                return iter->second.port;
            }
        }
        return 0;
    }

    return sock->get_port();
}

void SocketManager::set_socket_fd( SOCKET s, bool bRead, bool bWrite, bool bException )
{
    if (s >= 0)
    {
        if (bRead)
        {
            if (!FD_ISSET(s, &m_rfds))
            {
                FD_SET(s, &m_rfds);
            }
        }
        else
        {
            FD_CLR(s, &m_rfds);
        }
        if (bWrite)
        {
            if (!FD_ISSET(s, &m_wfds))
            {
                FD_SET(s, &m_wfds);
            }
        }
        else
        {
            FD_CLR(s, &m_wfds);
        }
        if (bException)
        {
            if (!FD_ISSET(s, &m_efds))
            {
                FD_SET(s, &m_efds);
            }
        }
        else
        {
            FD_CLR(s, &m_efds);
        }
    }
}

void SocketManager::rebuild_fdset()
{
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    // rebuild fd_set's from active sockets list (m_sockets) here
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

	for (ClientSocketMap_t::iterator it = m_closeSocketMap.begin(); 
		it != m_closeSocketMap.end(); ++it)
	{
		ClientSocketMap_t::iterator iter = m_clientSocketMap.find(it->first);
		if (iter != m_clientSocketMap.end())
			m_clientSocketMap.erase(iter);
	}
	m_closeSocketMap.clear();

    std::vector<unsigned int> rmConnIndexVec;
    for (ClientSocketMap_t::iterator it = m_clientSocketMap.begin(); 
        it != m_clientSocketMap.end(); ++it)
    {
        SOCKET s = it->second->getSocket();;
        ClientSocket *p = it -> second;
        if (s >= 0)
        {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(s, &fds);
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            int n = select((int)s + 1, &fds, NULL, NULL, &tv);
            if (n == -1 && BSDSocket::GetError() == EBADF)
            {
                // %! bad fd, remove
                // DeleteSocket(p);
                p->_set_connected(false);
                set_socket_fd(s, false, false, false);
                rmConnIndexVec.push_back(it->first);
            }
            else
            {
                if (FD_ISSET(s, &m_rfds))
                    FD_SET(s, &rfds);
                if (FD_ISSET(s, &m_wfds))
                    FD_SET(s, &wfds);
                if (FD_ISSET(s, &m_efds))
                    FD_SET(s, &efds);
            }
        }
        else
        {
            // %! mismatch
            // DeleteSocket(p);
            p->_set_connected(false);
            set_socket_fd(s, false, false, false);
            rmConnIndexVec.push_back(it->first);
        }
    }
    m_rfds = rfds;
    m_wfds = wfds;
    m_efds = efds;
}

int SocketManager::sockes_select(struct timeval *tsel)
{
#ifdef MACOSX
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    FD_COPY(&m_rfds, &rfds);
    FD_COPY(&m_wfds, &wfds);
    FD_COPY(&m_efds, &efds);
#else
    fd_set rfds = m_rfds;
    fd_set wfds = m_wfds;
    fd_set efds = m_efds;
#endif
    int n;
    n = select( (int)(m_maxsock + 1),&rfds,&wfds,&efds,tsel);
    
	if (n == -1) // error on select
    {
        int err = BSDSocket::GetError();
		/*
			EBADF  An invalid file descriptor was given in one of the sets.
			EINTR  A non blocked signal was caught.
			EINVAL n is negative. Or struct timeval contains bad time values (<0).
			ENOMEM select was unable to allocate memory for internal tables.
		*/
#ifdef _WIN32
		switch (err)
		{
		case WSAENOTSOCK:
            rebuild_fdset();
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err: %d rebuild fdset", err);
            )
			break;
		case WSAEINTR:
        case WSAEINPROGRESS:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err WSAEINTR:%d|WSAEINPROGRESS:%d err:%d", WSAEINTR, WSAEINPROGRESS, err);
            )
			break;
        case WSAEINVAL:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err WSAEINVAL:%d", err);
            )
			//throw Exception("select(n): n is negative. Or struct timeval contains bad time values (<0).");
            break;
        case WSAEFAULT:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err WSAEFAULT:%d", err);
            )
			//LogError(NULL, "SocketHandler::Select", err, StrError(err), LOG_LEVEL_ERROR);
			break;
        case WSANOTINITIALISED:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err WSANOTINITIALISED:%d", err);
            )
			//throw Exception("WSAStartup not successfully called");
            break;
        case WSAENETDOWN:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err WSAENETDOWN:%d", err);
            )
			//throw Exception("Network subsystem failure");
            break;
		}
#else
		switch (err)
		{
		case EBADF:
            rebuild_fdset();
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err rebuild fdset EBADF:%d", err);
            )
			break;
        case EINTR:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err EINTR:%d", err);
            )
			break;
        case EINVAL:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err EINVAL:%d", err);
            )
			//throw Exception("select(n): n is negative. Or struct timeval contains bad time values (<0).");
            break;
        case ENOMEM:
            _SOCK_DEBUG2(
            CCLOG("__XX_SOCK_ERROR__ select err ENOMEM:%d", err);
            )
			//LogError(NULL, "SocketHandler::Select", err, StrError(err), LOG_LEVEL_ERROR);
			break;
		}
#endif
		//printf("error on select(): %d %s\n", Errno, StrError(err));
	}
	else
	if (!n) // timeout
	{
		rebuild_fdset();
	}
	else
	if (n > 0)
	{
		m_isDispatching = true;
		for (ClientSocketMap_t::iterator it = m_clientSocketMap.begin(); 
            it != m_clientSocketMap.end(); ++it)
		{
			SOCKET s = it->second->getSocket();
			ClientSocket *p = it -> second;
			if (FD_ISSET(s, &rfds))
			{
				//p -> OnRead();
                PostRecvMessageFrom(it->first, p, 
                    m_iRecvBufferLen - m_iRecvBufferOffset);
			}
			if (FD_ISSET(s, &wfds))
			{
				// used for non block mode
				// 				if (p->is_connecting())
				// 				{
				// 					int err = BSDSocket::GetError();
				// 					{
				// 						set_socket_fd(s, true, false, true);
				// 						p->_set_connecting(false);
				// 						onConnect(it->first, p);
				// 					}
				// 				}
			}
			if (FD_ISSET(s, &efds))
			{
				//p -> OnException();
                int err = BSDSocket::GetError();
                _SOCK_DEBUG2(
                CCLOG("__SELECT_ERROR__ connIndex:%d sock:%d "
                    "exception errorCode:%d",
                    it->first, s, err);
                )
			}
		} // m_sockets ...
		m_isDispatching = false;

	} // if (n > 0)
	return n;
}

void SocketManager::_check_error_on_sock( unsigned int connIndex, ClientSocket *sock )
{
    // normal error codes:
    // WSAEWOULDBLOCK
    //       EAGAIN or EWOULDBLOCK
    int err = BSDSocket::GetError();
#ifdef _WIN32
    if (err != WSAEWOULDBLOCK)
#else
    if (err != EWOULDBLOCK)
#endif
    {	
        //Handler().LogError(this, "send", Errno, StrError(Errno), LOG_LEVEL_FATAL);
        //OnDisconnect();
        //OnDisconnect(TCP_DISCONNECT_WRITE|TCP_DISCONNECT_ERROR, Errno);
        //SetCloseAndDelete(true);
        //SetFlushBeforeClose(false);
        //SetLost();
        set_socket_fd(sock->getSocket(), false, false, false);
        onDisconnected(connIndex, sock);

		m_msgQueue.clear(CMessageQueue::enm_queue_client_send);
    }
}

bool SocketManager::_do_post_recved_buff( char *pStart, unsigned int msgLen , int sockStatus, unsigned int connIndex )
{
    // format 4-byte connection status and 4-byte connection index
    // on the addres before pStart, thus, the 4-bytes space is enough
    // and dirty now before pStart, so, this operation is safe.
    char *pPostAddr = pStart - kRecvBufHeaderLen;
    *((int*)pPostAddr) = sockStatus;
    *((unsigned int*)(pPostAddr + sizeof(int))) = connIndex;

    if (m_msgQueue.PostMessage(CMessageQueue::enm_queue_client_recv, 
        pPostAddr, msgLen + kRecvBufHeaderLen) != 0)
    {
        return false;
    }
    return true;
}

}   // namespace net
