/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __PROTOCOLS_MESSAGE_HEAD_H__
#define __PROTOCOLS_MESSAGE_HEAD_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#endif

#include "stdint.h"
#include <sys/stat.h>

#pragma pack(1)

struct PHDR
{
    PHDR():len(0), cmd(0), uid(0){}
    uint32_t len;   //! packet length
    uint32_t cmd;   //! command
    uint64_t uid;   //! user id
    //uint64_t sid;   //! session_id created by server

};

#pragma pack()

static uint64_t htonll1(uint64_t h)
{
    static const int num = 42;

    if (*(char *)&num == 42)
    {
        uint32_t low = htonl(h & 0xFFFFFFFF);
        uint32_t high = htonl((h >> 32) & 0xFFFFFFFF);
        return (((uint64_t)low) << 32) | high;
    }
    else
    {
        return h;
    }
}

static uint64_t ntohll1(uint64_t n)
{
    static const int num = 42;

    if (*(char *)&num == 42)
    {
        uint32_t low = ntohl(n & 0xFFFFFFFF);
        uint32_t high = ntohl((n >> 32) & 0xFFFFFFFF);
        return (((uint64_t)low) << 32) | high;
    }
    else
    {
        return n;
    }
}

static const unsigned int PHDR_LEN = sizeof(PHDR);

static inline void push_pack_head(char* dest_buffer , const PHDR& header)
{
    *reinterpret_cast<uint32_t*>(dest_buffer) = htonl(header.len);
    dest_buffer += sizeof(uint32_t);
    *reinterpret_cast<uint32_t*>(dest_buffer) = htonl(header.cmd);
    dest_buffer += sizeof(uint32_t);
    *reinterpret_cast<uint64_t*>(dest_buffer) = htonll1(header.uid);
    //dest_buffer += sizeof(uint64_t);
    //*reinterpret_cast<uint64_t*>(dest_buffer) = htonll1(header.sid);
}

static inline void pop_pack_head(const char* src_buffer , PHDR& header)
{
    header.len = ntohl(*reinterpret_cast<const uint32_t*>(src_buffer));
    src_buffer += sizeof(uint32_t);
    header.cmd = ntohl(*reinterpret_cast<const uint32_t*>(src_buffer));
    src_buffer += sizeof(uint32_t);
    header.uid = ntohll1(*reinterpret_cast<const uint64_t*>(src_buffer));
    //src_buffer += sizeof(uint64_t);
    //header.sid = ntohll1(*reinterpret_cast<const  uint64_t*>(src_buffer));
}


#endif
