#ifndef __MESSAGE_QUEUE_MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_MESSAGE_QUEUE_H__

#include "circle_queue.h"

typedef struct tagPackage {
	//uint32_t    message;
	char*       buffer;
	uint32_t    length;
	uint32_t    tv_sec;
    uint32_t    tv_usec;
	//POINT       pt;
} PACKAGE, *PPACKAGE, *LPPACKAGE;

#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001

#define PM_COPY				0x10000

#define INNER_QUEUE_DEFAULT_LEN				(32*1024*1024)
enum
{
    queue_flag_enable_post_to_client_recv = 1,
    queue_flag_enable_post_to_server_recv = 2,
    queue_flag_enable_post_to_recv = 3,
};

class CMessageQueue
{
public:
	CMessageQueue(void);
	virtual ~CMessageQueue(void);
	enum msg_queue_type
	{
		enm_queue_undefine = -1,
		enm_queue_all_recv = 0,
		enm_queue_inner,
		enm_queue_client_recv,
		enm_queue_client_send,
		enm_queue_server_recv,
		enm_queue_server_send
	};
	enum
	{
		invalid_offset = -1, /**< 无效的偏移量 */
		reserved_length = 8, /**< 预留的空间长度*/

		queue_buffer_is_not_enough = 2,   /**< 队列空间不够*/
		queue_is_empty			   = 3,   /**< 队列为空*/
		recv_buffer_is_not_enough  = 4,   /**< 队列空间不够*/
		queue_type_error		   = 5,		//读写队列类型错误
		queue_type_unknown		   = 6,		//未知的队列类型

	};

public:

	virtual int32_t		PeekMessage(msg_queue_type type, LPPACKAGE lpMsg, uint32_t wMsgFilterMin, uint32_t wRemoveMsg);
	virtual int32_t		PostMessage(msg_queue_type type, char* buffer, uint32_t length);

	int32_t				Install(msg_queue_type type, char* pAddr, uint32_t nLen);
	int32_t				InstallDefaultInner(uint32_t nLen = INNER_QUEUE_DEFAULT_LEN);
	msg_queue_type		GetLeastMessageType();

	int32_t				ModifyFlag(uint32_t iAdd, uint32_t iRemove);

	void clear();
	void clear(msg_queue_type type);
protected:
	int32_t				PeekMessage(CCircleQueue* pQueue, LPPACKAGE lpMsg, uint32_t wMsgFilterMin, uint32_t wRemoveMsg);

protected:
	uint32_t	 m_iFlag;
	CCircleQueue m_stContainerClientRecv;
	CCircleQueue m_stContainerClientSend;
	CCircleQueue m_stContainerServerRecv;
	CCircleQueue m_stContainerServerSend;
	CCircleQueue m_stContainerInner;

	msg_queue_type m_nLeastActiveSend;
	msg_queue_type m_nLeastActiveRecv;

};

#endif

