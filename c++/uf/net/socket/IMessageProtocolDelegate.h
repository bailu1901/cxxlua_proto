/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __BASE_NET_MESSAGE_PROTOCOL_DELEGATE_H__
#define __BASE_NET_MESSAGE_PROTOCOL_DELEGATE_H__

#include "cocos2d.h"
#include <sys/stat.h>
#include "Downloader.h"

class IMessageProtocolDelegate
{
public:
    IMessageProtocolDelegate() {}
    ~IMessageProtocolDelegate() {}

    virtual void process(unsigned int connIndex, const char *szBuff, size_t size) 
    { CC_UNUSED_PARAM(connIndex);CC_UNUSED_PARAM(szBuff);CC_UNUSED_PARAM(size); }

    virtual void onConnectSuccess(unsigned int connIndex) 
    { CC_UNUSED_PARAM(connIndex); }

    virtual void onConnectFail(unsigned int connIndex)
    { CC_UNUSED_PARAM(connIndex); }

    virtual void onConnectBroken(unsigned int connIndex)
    { CC_UNUSED_PARAM(connIndex); }

    // 解析缓冲区内数据的包长 
    // 默认处理过程是取缓冲区的头4字节，由网络字节序转成本机字节序，作为包长 
    // 存入 @len 返回 true
    //     通常我们设定的包头结构头四字节是包长，当然，不同协议可能包头结构不 
    //     一样，这就需要重写这个方法，读到正确的包长。如果返回false，则将会 
    //     将一次select取到的所有数据放入circle queue，这将有导致收到不完整包 
    //     时出现错乱的可能。 
    virtual bool parseBufferLen(unsigned int connIndex, 
        const char *szBuff, size_t size, unsigned int &len) 
    { 
        if (size > 4) 
        {
            len = ntohl(*reinterpret_cast<const unsigned int*>(szBuff));
            return true;
        }
        return false;
    }

    virtual void onException(const std::string &e_what) 
    { CC_UNUSED_PARAM(e_what); }

    virtual void onDownloaded(const net::stDownloadUrl &downloaded) 
    { CC_UNUSED_PARAM(downloaded); }
};

#endif
