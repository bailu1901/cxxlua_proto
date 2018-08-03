
#ifndef __BASE_NET_HTTP_MESSAGE_DELEGATE_H__
#define __BASE_NET_HTTP_MESSAGE_DELEGATE_H__

#include "cocos2d.h"
#include <sys/stat.h>

class IHttpMessageDelegate
{
public:
    IHttpMessageDelegate() {}
    ~IHttpMessageDelegate() {}

    // HTTP Response processer 
    // @szTag: user self set tag when request, this tag will let user know 
    //     the current response belongs to witch request
    // @code: code == 0: success, code != 0: curl error code
    // @szBuff: if code == 0: points to the received response data
    //          if code != 0: points to the error message of curl request
    // @size: szBuff length
    virtual void onHttpResponse(const char *szTag, int code, 
        const char *szBuff, size_t size)
    {}
};

#endif
