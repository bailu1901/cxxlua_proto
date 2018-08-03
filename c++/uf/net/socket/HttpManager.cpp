#include "HttpManager.h"
#include "cocos-ext.h"
// #include "network/HttpRequest.h"
// #include "network/HttpResponse.h"
// #include "network/HttpClient.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define _HTTP_DEBUG1(_x_)   _x_

namespace net {

HttpManager::HttpManager()
    : m_iRecvBufferLen(0)
    , m_pRecvBuffer(NULL)
    , m_bInited(false)
    , m_receivedMsgCount(0)
{

}

HttpManager::~HttpManager()
{
    if (m_pRecvBuffer)
    {
        delete []m_pRecvBuffer;
        m_pRecvBuffer = NULL;
    }
}

bool HttpManager::init( const std::vector<std::string> *hosts /*= NULL*/ )
{
    if (hosts)
    {
        m_default_headers = *hosts;
    }

    if (!m_bInited)
    {
        install_message_queue();
        if (m_pRecvBuffer)
        {
            delete []m_pRecvBuffer;
        }

        m_iRecvBufferLen = kRECV_BUFF_LEN;
        m_pRecvBuffer = new char[m_iRecvBufferLen];

        m_bInited = true;
    }

    return m_bInited;
}

bool HttpManager::RequestHTTP( const char *szTag, const std::string &url, const char *szData, size_t len, http_request_type_e eType, const std::vector<std::string> *hosts /*= NULL*/ )
{
    if (!m_bInited)
    {
        init();
    }

    bool bRet = false;
//     do 
//     {
//         CC_BREAK_IF(!szTag);
// 
//         CCHttpRequest *pRequest = new CCHttpRequest();
//         CC_BREAK_IF(!pRequest);
// 
//         if (strlen(szTag) > kHTTP_TAG_MAXLEN)
//         {
//             CCLOG("Error: http request marked tag better not bigger than %d",
//                 kHTTP_TAG_MAXLEN);
//         }
// 
//         if (eType == kHTTP_GET)
//         {
//             pRequest->setRequestType(CCHttpRequest::kHttpGet);
//         }
//         else if (eType == kHTTP_POST)
//         {
//             pRequest->setRequestType(CCHttpRequest::kHttpPost);
//         }
//         else
//         {
//             break;
//         }
// 
//         pRequest->setTag(szTag);
//         pRequest->setUrl(url.c_str());
//         pRequest->setRequestData(szData, len);
//         pRequest->setResponseCallback(this, callfuncND_selector(HttpManager::process_http_response));
// 
//         if (hosts && !(*hosts).empty())
//         {
//             pRequest->setHeaders(*hosts);
//         }
//         else if (!m_default_headers.empty())
//         {
//             pRequest->setHeaders(m_default_headers);
//         }
// 
 //       CCHttpClient::getInstance()->send(pRequest);
//         pRequest->release();
// 
//         _HTTP_DEBUG1(CCLOG("HttpManager::RequestHTTP tag:[%s] url:[%s]", szTag,
//             url.c_str());
//         )
// 
//         bRet = true;
//     } while ( 0 );

    return bRet;
}

bool HttpManager::RequestHTTP_GET( const char *szTag, const std::string &url, const URLVariables *urlVars, const std::vector<std::string> *hosts /*= NULL*/ )
{
    if (!m_bInited)
    {
        init();
    }

    bool bRet = false;
//     do 
//     {
//         CC_BREAK_IF(!szTag);
// 
//         CCHttpRequest *pRequest = new CCHttpRequest();
//         CC_BREAK_IF(!pRequest);
// 
//         if (strlen(szTag) > kHTTP_TAG_MAXLEN)
//         {
//             CCLOG("Error: http request marked tag better not bigger than %d",
//                 kHTTP_TAG_MAXLEN);
//         }
// 
//         pRequest->setRequestType(CCHttpRequest::kHttpGet);
// 
//         pRequest->setTag(szTag);
// 
//         std::string url_var = url;
//         if (urlVars)
//         {
//             url_var += "?";
//             url_var += urlVars->decode();
//         }
//         
//         pRequest->setUrl(url_var.c_str());
//         //pRequest->setRequestData(szData, len);
//         pRequest->setResponseCallback(this, callfuncND_selector(HttpManager::process_http_response));
// 
//         if (hosts && !(*hosts).empty())
//         {
//             pRequest->setHeaders(*hosts);
//         }
//         else if (!m_default_headers.empty())
//         {
//             pRequest->setHeaders(m_default_headers);
//         }
// 
 //       CCHttpClient::getInstance()->send(pRequest);
//         pRequest->release();
// 
//         _HTTP_DEBUG1(CCLOG("HttpManager::RequestHTTP tag:[%s] url:[%s]", szTag,
//             url.c_str());
//         )
// 
//         bRet = true;
//     } while ( 0 );

    return bRet;
}

void HttpManager::process_http_response( cocos2d::Node *pSender, void *data )
{
//     CCHttpResponse *pResponse = (CCHttpResponse*)data;
//     if (!pResponse)
//     {
//         return;
//     }
// 
//     int code = 0;
//     const char *szTag = pResponse->getHttpRequest()->getTag();
//     unsigned int tag_len = strlen(szTag);
//     if (!pResponse->isSucceed())    // http success
//     {
//         code = pResponse->getResponseCode();
//         CCLOG("http request:%s error code:%d error:%s", 
//             szTag, code, pResponse->getErrorBuffer());
//     }
// 
//     _HTTP_DEBUG1(CCLOG("HttpManager::process_http_response tag:[%s]", szTag);)
// 
//     char *ptr = m_pRecvBuffer;
//     // response code
//     *((int*)ptr) = code;
//     ptr += sizeof(code);
//     memcpy(ptr, szTag, tag_len);
//     ptr[tag_len] = '\0';
//     ptr += tag_len + 1;
// 
//     unsigned int total_len = 0;
//     unsigned int data_len = 0;
//     const char *_data = NULL;
//     if (pResponse->isSucceed())
//     {
//         std::vector<char> *responsedata = pResponse->getResponseData();
//         if (responsedata->size() > m_iRecvBufferLen - (ptr - m_pRecvBuffer) - sizeof(unsigned int) - 1)
//         {
//             CCLOG("process_http_response recv buffer left_size:[%d] less than"
//                 "recved response data_len:[%d] ,thow this pkg, tag:[%s]", 
//                 m_iRecvBufferLen - (ptr - m_pRecvBuffer), responsedata->size(),
//                 szTag);
//             return;
//         }
//         
//         data_len = responsedata->size();
//         _data =responsedata->data();
//     }
//     else
//     {
//         _data = pResponse->getErrorBuffer();
//         data_len = strlen(_data);
//         if (data_len > m_iRecvBufferLen - (ptr - m_pRecvBuffer) - sizeof(unsigned int) - 1)
//         {
//             data_len = m_iRecvBufferLen - (ptr - m_pRecvBuffer) - sizeof(unsigned int) - 1;
//         }
//     }
// 
//     // data len
//     *((unsigned int*)ptr) = data_len;
//     ptr += sizeof(unsigned int);
// 
//     // data
//     memcpy(ptr, _data, data_len);
//     ptr[data_len] = '\0';
// 
//     total_len = ptr - m_pRecvBuffer + data_len + 1;
// 
//     if (m_msgQueue.PostMessage(CMessageQueue::enm_queue_client_recv, 
//         m_pRecvBuffer, total_len) != 0)
//     {
//         CCLOG("Error: process_http_response couldn't post %s code:[%d] to msgQueue", 
//             szTag, code);
//         return ;
//     }
// 
//     ++m_receivedMsgCount;
}

void HttpManager::install_message_queue()
{
    int len = kRECV_QUEUE_BUFF_LEN;
    char* p = new char[len];
    m_msgQueue.Install(CMessageQueue::enm_queue_client_recv, p, len);
}

int HttpManager::PeekMessage( char *szDstBuff, size_t len, http_result_t &http_rel )
{
    PACKAGE msg;
    int code = m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_recv, &msg, 0, PM_NOREMOVE);
    if (code == 0)
    {
        if (msg.length <= len)
        {
            memcpy(szDstBuff, msg.buffer, msg.length);

            // format http_result
            char *ptr = szDstBuff;
            http_rel.code = *((int*)ptr);
            ptr += sizeof(int);
            http_rel.tag = ptr;
            http_rel.tag_len = strlen(http_rel.tag);
            ptr += http_rel.tag_len + 1;
            http_rel.data_len = *((unsigned int*)ptr);
            ptr += sizeof(unsigned int);
            http_rel.data = ptr;

            m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_recv, &msg, 0, PM_REMOVE);

            if (m_receivedMsgCount > 0)
            {
                --m_receivedMsgCount;
            }

            return msg.length;
        }
        else
        {
            CCLOG("Error: PeekMessage(http) buffer_len:[%d] smaller than msg.length:[%d]",
                len, msg.length);
        }

        m_msgQueue.PeekMessage(CMessageQueue::enm_queue_client_recv, &msg, 0, PM_REMOVE);

        if (m_receivedMsgCount > 0)
        {
            --m_receivedMsgCount;
        }

        return -1;
    }
    return -1;
}

}   // namespace net
