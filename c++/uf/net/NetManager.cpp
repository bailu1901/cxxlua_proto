/************************************************************************/
/*                                                                      */
/************************************************************************/
#include "NetManager.h"
#include "cocos2d.h"

USING_NS_CC;


NetManager::NetManager()
    : m_pRecvBuffer(NULL)
    , m_iRecvBufferLen(kRECV_BUFF_LEN)
    , m_pProcesserDelegate(NULL)
    , m_bIsRunning(false)
    //, m_pDownloader(NULL)
    , m_bInited(false)
    , m_tmpConnIndex(0)
    , m_tmpConnStatus(-1)
    , m_tmpPeekLen(0)
	, m_networkConnect(true)
	, m_networkConnectType(0)
{
    m_pRecvBuffer = new char[m_iRecvBufferLen];

	Director::getInstance()->getScheduler()->schedule(CC_SCHEDULE_SELECTOR(NetManager::Update), this, 0, false);
}

NetManager::~NetManager()
{
	if (m_bIsRunning) {
		m_socketManager.stopThread();
	}

    //Director::getInstance()->getScheduler()->unscheduleSelector(CC_SCHEDULE_SELECTOR(NetManager::Update), this);
    if (NULL != m_pRecvBuffer)
    {
        delete [] m_pRecvBuffer;
        m_pRecvBuffer = NULL;
    }
}

void NetManager::init(IMessageProtocolDelegate *protocolDelegate, bool autoReconnect/* = false */)
{
    if (m_bInited)
    {
        return;
    }
    m_socketManager.init(autoReconnect);

    m_pProcesserDelegate = protocolDelegate;
    m_socketManager.registerConnectProcessDelegate(protocolDelegate);
    m_bInited = true;
}

bool NetManager::run()
{
    if (m_bIsRunning)
    {
        return false;
    }
    m_bIsRunning = true;
    return m_socketManager.RunThread();
}

void NetManager::registerSocketProtocolDelegate(IMessageProtocolDelegate *protocolDelegate)
{
    if (m_pProcesserDelegate)
    {
        CCASSERT(0, "Do not register multi socket protocol delegate!");
        return;
    }
    m_pProcesserDelegate = protocolDelegate;
}

void NetManager::unregisterSocketProtocolDelegate(IMessageProtocolDelegate *protocolDelegate)
{
    if (m_pProcesserDelegate == protocolDelegate)
    {
        m_pProcesserDelegate = NULL;
        m_socketManager.registerConnectProcessDelegate(m_pProcesserDelegate);
    }
}

void NetManager::registerHttpProtocolDelegate(IHttpMessageDelegate *delegate)
{
    if (NULL == delegate)
    {
        return;
    }
    
    HttpMessageDelegateList_t::iterator iter = m_httpMsgDelegate.begin();
    for (; iter != m_httpMsgDelegate.end(); ++iter)
    {
        if (*iter == delegate)
        {
            return;
        }
    }
    
    m_httpMsgDelegate.push_back(delegate);
}

void NetManager::unregisterHttpProtocolDelegate(IHttpMessageDelegate *delegate)
{
    HttpMessageDelegateList_t::iterator iter = m_httpMsgDelegate.begin();
    for (; iter != m_httpMsgDelegate.end(); ++iter)
    {
        if (*iter == delegate)
        {
            m_httpMsgDelegate.erase(iter);
            break;
        }
    }
}

void NetManager::Update(float dt)
{
    if (m_socketManager.isRuning())
    {
        m_tmpPeekLen = m_socketManager.PeekMessage(m_pRecvBuffer, 
            m_iRecvBufferLen, m_tmpConnIndex, m_tmpConnStatus);
        if (net::kSockStatus_OK == m_tmpConnStatus)
        {
            if (m_pProcesserDelegate)
            {
                m_pProcesserDelegate->process(
                    m_tmpConnIndex, m_pRecvBuffer, m_tmpPeekLen);
            }
        }
        else if (m_pProcesserDelegate)
        {
            switch (m_tmpConnStatus)
            {
            case net::kSockStatus_ConnFailed:
                m_pProcesserDelegate->onConnectFail(m_tmpConnIndex);
                break;
            case net::kSockStatus_Broken:
                m_pProcesserDelegate->onConnectBroken(m_tmpConnIndex);
                break;
			case net::kSockStatus_Connect:
				m_pProcesserDelegate->onConnectSuccess(m_tmpConnIndex);
				break;
            }
        }
    }

    if (m_httpManager.receivedMsgCount() > 0)
    {
        m_tmpPeekLen = m_httpManager.PeekMessage(m_pRecvBuffer, 
            m_iRecvBufferLen, m_tmpHttpResult);
        if (m_tmpPeekLen > 0)
        {
            for (HttpMessageDelegateList_t::iterator iter = m_httpMsgDelegate.begin();
                iter != m_httpMsgDelegate.end(); ++iter)
            {
                if (*iter)
                {
                    (*iter)->onHttpResponse(
                        m_tmpHttpResult.tag, m_tmpHttpResult.code, 
                        m_tmpHttpResult.data, m_tmpHttpResult.data_len);
                }
            }
            //if (m_pProcesserDelegate)
            //{
            //    m_pProcesserDelegate->onHttpResponse(
            //        m_tmpHttpResult.tag, m_tmpHttpResult.code, 
            //        m_tmpHttpResult.data, m_tmpHttpResult.data_len);
            //}
        }
    }

//     if (m_pDownloader)
//     {
//         if (m_pDownloader->getDownloadedSize() > 0) {
//             net::stDownloadUrl result;
//             if (m_pDownloader->peekDownloaded(result)) {
//                 if (m_pProcesserDelegate)
//                 {
//                     m_pProcesserDelegate->onDownloaded(result);
//                 }
// 				
//             }
//         }
//         else if (m_pDownloader->allDownloaded())
//         {
//             delete m_pDownloader;
//             m_pDownloader = NULL;
//         }
//     }
}

bool NetManager::addConnect( unsigned int connIndex, const std::string &ip, unsigned short port )
{
    return m_socketManager.addConnect(connIndex, ip, port);
}

void NetManager::reConnect( unsigned int connIndex, unsigned int delaySecs )
{
    m_socketManager.reConnect(connIndex, delaySecs);
}

bool NetManager::removeConnect(unsigned int connIndex) {
	return m_socketManager.removeConnect(connIndex);
}

bool NetManager::SendTo( unsigned int connIndex, const char *szBuff, size_t len )
{
    return m_socketManager.SendMessage(connIndex, szBuff, len);
}

bool NetManager::Download( const char *szIndex, const char *szUrl, const char *szOutFileName )
{
//     if (NULL == szIndex || NULL == szUrl || NULL == szOutFileName)
//     {
//         return false;
//     }
// 
//     if (NULL == m_pDownloader)
//     {
//         m_pDownloader = new net::Downloader();
//     }
//     
//     return m_pDownloader->append(szIndex, szUrl, szOutFileName);
	return false;
}

net::ClientSocket* NetManager::getConnectByIndex( unsigned int connIndex )
{
    return m_socketManager.getConnectByIndex(connIndex);
}

void NetManager::setAutoConnect(bool autoConnect) {
	m_socketManager.setAutoConnect(autoConnect);
}

bool NetManager::isAutoConnect() {
	return m_socketManager.isAutoConnect();
}

std::string NetManager::IPforConnIndex( unsigned int connIndex )
{
    return m_socketManager.IPforConnIndex(connIndex);
}

unsigned short NetManager::PortForConnIndex( unsigned int connIndex )
{
    return m_socketManager.PortForConnIndex(connIndex);
}

bool NetManager::RequestHTTP( const char *szTag, const std::string &url, const char *szData, size_t len, http_request_type_e eType, const std::vector<std::string> *hosts /*= NULL*/ )
{
    return m_httpManager.RequestHTTP(szTag, url, szData, len, eType, hosts);
}

bool NetManager::RequestHTTP_GET( const char *szTag, const std::string &url, const URLVariables *urlVars, const std::vector<std::string> *hosts /*= NULL*/ )
{
    return m_httpManager.RequestHTTP_GET(szTag, url, urlVars, hosts);
}

// void NetManager::onReceiveNetworkStateChange( cocos2d::Ref* lparam, cocos2d::Ref* rparam )
// {
// 	int connect = CCObjectToInt(rparam);
// 	m_networkConnectType = CCObjectToInt(lparam);

// 	bool can_connect = connect > 0;

// 	CCLOG("[c++][NetManager]onReceiveNetworkStateChange: can_connect=%d, state=%d", can_connect, m_networkConnectType);
// 	if ( (can_connect && m_networkConnect) ||
// 		(!can_connect && !m_networkConnect) )
// 		return ;

// 	CCLOG("[c++][NetManager]onReceiveNetworkStateChange: can_connect changed to %d", can_connect);
// 	m_networkConnect = can_connect;
// }
