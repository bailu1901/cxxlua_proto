#include "MsgProcessHandler.h"
#include "CCLuaEngine.h"
#include "base/CCScriptSupport.h"
#include "../base/singlton/SingletonManager.h"
#include "NetManager.h"
#include "package.head.h"
#include "zlib.h"

USING_NS_CC;


MsgProcessHandler::MsgProcessHandler() 
: m_netInstance_(NULL)
, m_nScriptHandler(0)
, m_pSendBuffer(NULL)
, m_iSendBufferLen(0)
, m_bInited(false)
, showLog_(true)
, m_defaultServerId(0)
, userId_(0) 
, sessionId_(0)
, uncompressBufferLen_() {

}

void MsgProcessHandler::init() {
	Base::Singleton<MsgProcessHandler>::init();
}

void MsgProcessHandler::unInit() {
	Base::Singleton<MsgProcessHandler>::unInit();

	
}

void MsgProcessHandler::onCocos2dExit() {
if (m_netInstance_ != NULL) {
		m_netInstance_->release();
		m_netInstance_ = NULL;
	}

	if (m_pSendBuffer)
	{
		delete []m_pSendBuffer;
		m_pSendBuffer = NULL;
	}

	if (uncompressBuffer_) {
		delete[] uncompressBuffer_;
		uncompressBuffer_ = nullptr;
	}
}

bool MsgProcessHandler::initNetManager() {
	if (m_bInited)
	{
		return false;
	}

	if (m_netInstance_ != NULL)
		return false;

	m_netInstance_ = new NetManager();
	m_netInstance_->init(this, false);

	if (m_pSendBuffer)
	{
		delete []m_pSendBuffer;
	}
	m_iSendBufferLen = kSEND_BUFF_LEN;
	m_pSendBuffer = new char[m_iSendBufferLen];

	m_bInited = true;

	return true;
}

void MsgProcessHandler::registerScriptHandler(int handler) {
	unregisterScriptHandler();
	m_nScriptHandler = handler;
}

void MsgProcessHandler::unregisterScriptHandler() {
	if (m_nScriptHandler)
	{
		ScriptEngineManager::getInstance()->getScriptEngine()->removeScriptHandler(m_nScriptHandler);
		m_nScriptHandler = 0;
	}
}

bool MsgProcessHandler::connectToServer(unsigned int connIndex, const char* ip, unsigned short port)
{
	if (initNetManager()) {
		m_netInstance_->run();
	}

	if (ip != NULL)
	{
		if (showLog_)
			CCLOG("[c++][MsgProcessHandler] add connect [%s:%d] begin", ip, port);
		if (! m_netInstance_->addConnect(connIndex, ip, port))
		{
			if (showLog_)
				CCLOG("[c++][MsgProcessHandler] add connect [%s:%d] failed", ip, port);
			m_defaultServerId = 0;
		}
		else
		{
			m_defaultServerId = connIndex;
			return true;
		}
	}

	return false;
}

bool MsgProcessHandler::reconnectToServer(unsigned int connIndex)
{
	if (!m_bInited)
	{
		CC_ASSERT(0);
		return false;
	}
	m_netInstance_->reConnect(connIndex, 1);
	return true;
}

bool MsgProcessHandler::removeConnect(unsigned int connIndex) {
	if (!m_bInited)
	{
		//CC_ASSERT(0);
		return false;
	}

	return m_netInstance_->removeConnect(connIndex);
}

bool MsgProcessHandler::sendMessage( unsigned int connIndex, unsigned int msgId, const char *szBuff, unsigned int len )
{
	if (m_netInstance_ == NULL)
		return false;

	int totalLen = doSerializeMsg(msgId, szBuff, len);
	if (totalLen > 0) {
		std::string sendStr;
		sendStr.append(m_pSendBuffer, 0, totalLen);
		if (showLog_)
			CCLOG("[c++][MsgProcessHandler] sendMessage:id=%d, msg=%s", msgId, sendStr.c_str());
		return m_netInstance_->SendTo(connIndex, m_pSendBuffer, totalLen);
	}

	return false;
}

bool MsgProcessHandler::sendMessageToDefault(unsigned int msgId, const char* szBuffer, unsigned int len) {
	if (m_netInstance_ == NULL)
		return false;

	return sendMessage(m_defaultServerId, msgId, szBuffer, len);
}


void MsgProcessHandler::setAutoConnect(bool autoConnect) {
	if (m_netInstance_)
		m_netInstance_->setAutoConnect(autoConnect);
}

bool MsgProcessHandler::isAutoConnect() {
	if (m_netInstance_)
		return m_netInstance_->isAutoConnect();

	return true;
}

void MsgProcessHandler::showNetworkLog(bool show) {
	showLog_ = show;
}

void MsgProcessHandler::onScriptNetMsg(const char* eventID, uint32_t connIndex, 
									   const char* msg, uint32_t len, uint32_t msgId) {
	if (eventID == NULL || !m_nScriptHandler)
		return ;

	ScriptEngineProtocol* pEngine = ScriptEngineManager::getInstance()->getScriptEngine();
	if (pEngine == NULL)
		return ;

	if (pEngine->getScriptType() == kScriptTypeLua)
	{
		LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
		if (stack == NULL)
			return ;

		stack->pushString(eventID);
		stack->pushInt(connIndex);
		if (len > 0)
			stack->pushString(msg, len);
		else
			stack->pushString("");
		stack->pushInt(len);
		stack->pushInt(msgId);

		int ret = stack->executeFunctionByHandler(m_nScriptHandler, 5);
		stack->clean();
		return ;
	}
}

void MsgProcessHandler::process(unsigned int connIndex, const char *szBuff, size_t size) {
	unsigned int msgId = 0;
	size_t msgLen = 0;

	initBuffer();
	const char* pStart = unSerializeMsg(msgId, szBuff, msgLen);
	if (pStart != NULL) {
		if (showLog_)
			CCLOG("[c++][MsgProcessHandler][process] receive data: , len=%d, msgId=%d",  msgLen, msgId);
		onScriptNetMsg("netmsg", connIndex, pStart, msgLen, msgId);
	} else {
		if (showLog_)
			CCLOG("[c++][MsgProcessHandler][process] receive invalid data!");
	}
}

void MsgProcessHandler::onConnectSuccess(unsigned int connIndex)  {
	onScriptNetMsg("connect_success", connIndex);
}

void MsgProcessHandler::onConnectFail(unsigned int connIndex) {
	onScriptNetMsg("connect_fail", connIndex);
}

void MsgProcessHandler::onConnectBroken(unsigned int connIndex) {
	onScriptNetMsg("connect_broken", connIndex);
}

void MsgProcessHandler::onDownloaded(const net::stDownloadUrl &downloaded) {
	onScriptNetMsg("download", 0, downloaded.index.c_str(), downloaded.index.length());
}

void MsgProcessHandler::onException(const std::string &e_what) {
	onScriptNetMsg("exception", 0, e_what.c_str(), e_what.length());
}

int MsgProcessHandler::doSerializeMsg( unsigned int msgId, const char* szBuff, size_t len )
{
	if (szBuff != NULL && len > 0) {
		if (len > m_iSendBufferLen - PHDR_LEN)
			len = m_iSendBufferLen - PHDR_LEN;
		memcpy(m_pSendBuffer + PHDR_LEN, szBuff, len);
	}

	PHDR hdr;
	hdr.cmd = msgId;
	hdr.len = len + PHDR_LEN;
	hdr.uid = userId_;
	hdr.sid = sessionId_;
	push_pack_head(m_pSendBuffer, hdr);

	return hdr.len;
}

const char* MsgProcessHandler::unSerializeMsg( unsigned int& msgId, const char* szBuff, size_t& len )
{
	if (szBuff != NULL) {
		PHDR hdr;
		pop_pack_head(szBuff, hdr);
		msgId = hdr.cmd;
		len = hdr.len - PHDR_LEN;

		if (msgId >= 100000000) {
			msgId -= 100000000;

			if (!uncompressBuffer_) {
				uncompressBufferLen_ = len * 5;
				if (showLog_)
					CCLOG("unSerializeMsg: extend uncompress buffer size to %d", uncompressBufferLen_);
				uncompressBuffer_ = new char[uncompressBufferLen_];
			}
			unsigned long compressRetLen = uncompressBufferLen_;
			int ret = uncompress((Byte*)uncompressBuffer_, &compressRetLen, (const Byte*)(szBuff + PHDR_LEN), len);
			int retryMaxCount = 0;
			while (ret != Z_OK && retryMaxCount < 5) {
				if (ret == Z_BUF_ERROR) {
					delete[] uncompressBuffer_;
					uncompressBufferLen_ *= 3;
					if (showLog_)
						CCLOG("unSerializeMsg: extend uncompress buffer size to %d", uncompressBufferLen_);
					uncompressBuffer_ = new char[uncompressBufferLen_];
					if (!uncompressBuffer_) {
						CCLOG("Alloc memory failed! msgId:%d, len:%d, count=%d", msgId, len, uncompressBufferLen_);
						return nullptr;
					}
					memset(uncompressBuffer_, 0, uncompressBufferLen_);
					compressRetLen = uncompressBufferLen_;
					ret = uncompress((Byte*)uncompressBuffer_, &compressRetLen, (const Byte*)(szBuff + PHDR_LEN), len);
					retryMaxCount++;
				}
				else {
					CCLOG("uncompress failed! msgId:%d, len:%d, error code=%d", msgId, len, ret);
					return nullptr;
				}
			}

			if (ret == Z_OK) {
				len = compressRetLen;
				return uncompressBuffer_;
			}
		}
	}

	return szBuff + PHDR_LEN;
}

void MsgProcessHandler::initBuffer()
{
	memset(m_pSendBuffer, 0, m_iSendBufferLen);
}
