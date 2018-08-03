
#include "message_queue.h"

CMessageQueue::CMessageQueue(void)
{
	m_nLeastActiveSend = enm_queue_undefine;
	m_nLeastActiveRecv = enm_queue_undefine;
	m_iFlag = queue_flag_enable_post_to_client_recv|queue_flag_enable_post_to_server_recv;
}

CMessageQueue::~CMessageQueue(void)
{
}

int32_t CMessageQueue::Install( msg_queue_type type, char* pAddr, uint32_t nLen )
{
	int32_t nResult = queue_type_unknown;
	switch (type)
	{
	case enm_queue_inner:
		{
			m_stContainerInner.reset(pAddr, nLen);
			break;
		}
	case enm_queue_client_recv:
		{
			m_stContainerClientRecv.reset(pAddr, nLen);
			break;
		}
	case enm_queue_client_send:
		{
			m_stContainerClientSend.reset(pAddr, nLen);
			break;
		}
	case enm_queue_server_recv:
		{
			m_stContainerServerRecv.reset(pAddr, nLen);
			break;
		}
	case enm_queue_server_send:
		{
			m_stContainerServerSend.reset(pAddr, nLen);
			break;
		}
	default:
		{
			break;
		}
	}
	return nResult;
}

int32_t CMessageQueue::PeekMessage( msg_queue_type type, LPPACKAGE lpMsg, uint32_t wMsgFilterMin, uint32_t wRemoveMsg )
{
	int32_t nResult = queue_type_error;
	CCircleQueue *pFirst = NULL;
	CCircleQueue *pSecond = NULL;
	switch (type)
	{
	case enm_queue_inner:
		{
			pFirst = &m_stContainerInner;
			m_nLeastActiveRecv = enm_queue_inner;
			break;
		}
	case enm_queue_client_recv:
		{
			pFirst = &m_stContainerClientRecv;
			m_nLeastActiveRecv = enm_queue_client_recv;
			break;
		}
	case enm_queue_server_recv:
		{
			pFirst = &m_stContainerServerRecv;
			m_nLeastActiveRecv = enm_queue_server_recv;
			break;
		}
	case enm_queue_client_send:
		{
			pFirst = &m_stContainerClientSend;
			m_nLeastActiveRecv = enm_queue_client_send;
			break;
		}
	case enm_queue_server_send:
		{
			pFirst = &m_stContainerServerSend;
			m_nLeastActiveRecv = enm_queue_server_send;
			break;
		}

	case enm_queue_all_recv:
		{
			//每次都先查看内部队列
			pFirst = &m_stContainerInner;
			m_nLeastActiveRecv = enm_queue_inner;
			nResult = PeekMessage(pFirst, lpMsg, wMsgFilterMin, wRemoveMsg);
			if(queue_is_empty == nResult)
			{
				if (m_nLeastActiveRecv == enm_queue_client_recv)
				{
					pFirst = &m_stContainerServerRecv;
					m_nLeastActiveRecv = enm_queue_server_recv;
					nResult = PeekMessage(pFirst, lpMsg, wMsgFilterMin, wRemoveMsg);
					if (queue_is_empty == nResult)
					{
						pSecond = &m_stContainerClientRecv;
						nResult = PeekMessage(pSecond, lpMsg, wMsgFilterMin, wRemoveMsg);
						m_nLeastActiveRecv = enm_queue_client_recv;
					}

				}
				else
				{
					pFirst = &m_stContainerClientRecv;
					m_nLeastActiveRecv = enm_queue_client_recv;
					nResult = PeekMessage(pFirst, lpMsg, wMsgFilterMin, wRemoveMsg);
					if (queue_is_empty == nResult)
					{
						pSecond = &m_stContainerServerRecv;
						nResult = PeekMessage(pSecond, lpMsg, wMsgFilterMin, wRemoveMsg);
						m_nLeastActiveRecv = enm_queue_server_recv;
					}
				}

			}
			pFirst = NULL;
			break;
		}
	default:
		{
			break;
		}
	}
	if (pFirst)
	{
		nResult = PeekMessage(pFirst, lpMsg, wMsgFilterMin, wRemoveMsg);
	}
	return nResult;

}

int32_t CMessageQueue::PeekMessage( CCircleQueue* pQueue, LPPACKAGE lpMsg, uint32_t wMsgFilterMin, uint32_t wRemoveMsg )
{
	int32_t nResult = queue_is_empty;

	if (pQueue)
	{
		if ((wRemoveMsg & PM_REMOVE))
		{
			if (wRemoveMsg & PM_COPY)
			{
				nResult = pQueue->pop(lpMsg->buffer, lpMsg->length);
			}
			else
			{
				lpMsg->buffer = (char*)pQueue->get(lpMsg->length);
				if (lpMsg->buffer != NULL)
				{
					nResult = SUCCESS;
					pQueue->pop();
				}
			}
		}
		else
		{
			//PM_NOREMOVE不执行PM_COPY 
			lpMsg->buffer = (char*)pQueue->get(lpMsg->length);
			if (lpMsg->buffer != NULL)
			{
				nResult = SUCCESS;
			}
		}

	}
	return nResult;

}

int32_t CMessageQueue::PostMessage( msg_queue_type type, char* buffer, uint32_t length )
{
	int32_t nResult = queue_type_error;
	CCircleQueue *pFirst = NULL;
	switch (type)
	{
	case enm_queue_inner:
		{
			pFirst = &m_stContainerInner;
			m_nLeastActiveSend = enm_queue_inner;
			break;
		}
	case enm_queue_client_send:
		{
			pFirst = &m_stContainerClientSend;
			m_nLeastActiveSend = enm_queue_client_send;
			break;
		}
	case enm_queue_server_send:
		{
			pFirst = &m_stContainerServerSend;
			m_nLeastActiveSend = enm_queue_server_send;
			break;
		}
//flag
	case enm_queue_client_recv:
		{
			if (m_iFlag & queue_flag_enable_post_to_client_recv)
			{
				pFirst = &m_stContainerClientRecv;
			}
			break;
		}
	case enm_queue_server_recv:
		{
			if (m_iFlag & queue_flag_enable_post_to_server_recv)
			{
				pFirst = &m_stContainerServerRecv;
			}
			break;
		}

	default:
		{
			break;
		}
	}
	if (pFirst)
	{
		nResult = pFirst->append(buffer, length);
	}
	return nResult;

}

CMessageQueue::msg_queue_type CMessageQueue::GetLeastMessageType()
{
	return m_nLeastActiveRecv;
}

int32_t CMessageQueue::InstallDefaultInner( uint32_t nLen )
{
	char *pBuffer = new char[nLen];
	if (pBuffer)
	{
		char *pOldBuffer = m_stContainerInner.reset(pBuffer, nLen);
		delete [] pOldBuffer;
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

int32_t CMessageQueue::ModifyFlag( uint32_t iAdd, uint32_t iRemove )
{
	m_iFlag &= ~iRemove;
	m_iFlag |= iAdd;
	return SUCCESS;
}

void CMessageQueue::clear() {
	m_stContainerClientRecv.clear();
	m_stContainerClientSend.clear();
	m_stContainerServerRecv.clear();
	m_stContainerServerSend.clear();
}

void CMessageQueue::clear(msg_queue_type type) {
	switch (type)
	{
	case enm_queue_inner:
		break;
	case enm_queue_client_send:
		m_stContainerClientSend.clear();
		break;
	case enm_queue_server_send:
		m_stContainerServerSend.clear();
		break;
	case enm_queue_client_recv:
		m_stContainerClientRecv.clear();
		break;
	case enm_queue_server_recv:
		m_stContainerServerRecv.clear();
		break;

	default:
		break;
	}
}
