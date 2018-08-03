/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __BASE_NET_NETMANAGER_H__
#define __BASE_NET_NETMANAGER_H__

#include "socket/SocketManager.h"
#include "socket/IMessageProtocolDelegate.h"
#include "socket/Downloader.h"
#include "socket/HttpManager.h"
#include "IHttpMessageDelegate.h"
#include "socket/net-common.h"
#include "cocos2d.h"
#include <vector>


typedef std::vector<IHttpMessageDelegate*>  HttpMessageDelegateList_t;

class NetManager : public cocos2d::Ref
{
public:
    NetManager();
    ~NetManager();

    // @[input] protocolDelegate: handler that will process recieved data
    void init(IMessageProtocolDelegate *protocolDelegate, bool autoReconnect = false);

    bool run();

    // @[input] protocolDelegate: handler that will process recieved data
    void registerSocketProtocolDelegate(IMessageProtocolDelegate *protocolDelegate);
    // @[input] protocolDelegate: handler that will process recieved data
    void unregisterSocketProtocolDelegate(IMessageProtocolDelegate *protocolDelegate);

    // @[input] protocolDelegate: handler that will process recieved data
    void registerHttpProtocolDelegate(IHttpMessageDelegate *protocolDelegate);
    // @[input] protocolDelegate: handler that will process recieved data
    void unregisterHttpProtocolDelegate(IHttpMessageDelegate *protocolDelegate);

    void Update(float dt);

    bool addConnect(unsigned int connIndex, const std::string &ip, unsigned short port);

    // reconnect after delaied seconds 
    void reConnect(unsigned int connIndex, unsigned int delaySecs);

	bool removeConnect(unsigned int connIndex);

    bool SendTo(unsigned int connIndex, const char *szBuff, size_t len);

    bool Download(const char *szIndex, const char *szUrl, const char *szOutFileName);

    net::ClientSocket* getConnectByIndex(unsigned int connIndex);

	void setAutoConnect(bool autoConnect);
	bool isAutoConnect();

    std::string IPforConnIndex(unsigned int connIndex);
    unsigned short PortForConnIndex(unsigned int connIndex);

	bool isNetworkConnected() {
		return m_networkConnect;
	}
	bool isWifiConnected() {
		return isNetworkConnected() && m_networkConnectType == 1;
	}
    // for http request
    // @szTag: string tag that you can mark each http request
    // @url: http url
    // @szData: POST(or GET) data
    // @eType: kHTTP_GET | kHTTP_POST
    // @hosts: http headers that would direct other host when the url 
    //     cannot access.
    bool RequestHTTP(const char *szTag, const std::string &url, const char *szData, size_t len, http_request_type_e eType, const std::vector<std::string> *hosts = NULL);

    // for http request
    // @szTag: string tag that you can mark each http request
    // @url: http url
    // @szData: POST(or GET) data
    // @eType: kHTTP_GET | kHTTP_POST
    // @hosts: http headers that would direct other host when the url 
    //     cannot access.
    bool RequestHTTP_GET(const char *szTag, const std::string &url, const URLVariables *urlVars, const std::vector<std::string> *hosts = NULL);

// private:
// 	void onReceiveNetworkStateChange(cocos2d::Ref* lparam, cocos2d::Ref* rparam);

private:
    net::SocketManager  m_socketManager;
    char* m_pRecvBuffer;    // recv temporary buffer
    int m_iRecvBufferLen;
    IMessageProtocolDelegate *m_pProcesserDelegate;
    HttpMessageDelegateList_t   m_httpMsgDelegate;
    bool m_bIsRunning;
   // net::Downloader     *m_pDownloader;
    bool m_bInited;
    net::HttpManager   m_httpManager;

    unsigned int        m_tmpConnIndex;
    int                 m_tmpConnStatus;
    int                 m_tmpPeekLen;
    net::http_result_t  m_tmpHttpResult;
	bool		m_networkConnect;
	int			m_networkConnectType;
};


#endif

