#ifndef __BASE_NET_SOCKET_HTTP_DELEGATE_H__
#define __BASE_NET_SOCKET_HTTP_DELEGATE_H__

#include "cocos2d.h"
#include "../messagequeue/message_queue.h"
#include "net-common.h"

namespace net {

    /* 
     * 使用时与给定的缓冲区(buffer)一起使用，用于 PeekMessage 返回， 
     *  @code: curl code 
     *  @tag: 在缓冲区(buffer)的起始地址 
     *  @tag_len: tag 字符串的长度 
     *  @data: 在缓冲区(buffer)的起始地址
     *        code = 0 时，data 为 HTTP 请求收到的响应消息;
     *        code != 0 时，data 为 HTTP 请求的错误信息. 
     *  @data_len: data 的长度 
     */
    struct http_result_t
    {
        int             code;
        const char      *tag;   // tag should not be too long
        unsigned int    tag_len;
        const char      *data;
        unsigned int    data_len;
    };

class HttpManager : public cocos2d::Ref
{
public:
    HttpManager();
    ~HttpManager();

    bool init(const std::vector<std::string> *hosts = NULL);

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

    // @input: szDstBuff  allocated buffer; len: length of szDstBuff
    // @output: peeked data length
    int PeekMessage(char *szDstBuff, size_t len, http_result_t &http_rel);

    bool isInited() const { return m_bInited; }

    int receivedMsgCount() const { return m_receivedMsgCount; }

protected:
    void process_http_response(cocos2d::Node *pSender, void *data);

    void install_message_queue();
    
private:
    std::vector<std::string>    m_default_headers;
    CMessageQueue               m_msgQueue;
    int     m_iRecvBufferLen;
    char*   m_pRecvBuffer;      // recv temporary buffer
    bool    m_bInited;
    int     m_receivedMsgCount; // left unprocessed received message count
};

}

#endif