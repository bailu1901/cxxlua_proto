#ifndef _MSG_PROCESS_HADNLER_H_
#define _MSG_PROCESS_HADNLER_H_

#include "socket/IMessageProtocolDelegate.h"
#include "../Base/singlton/Singleton.h"


class NetManager;
class MsgProcessHandler : public Base::Singleton<MsgProcessHandler>,
	public IMessageProtocolDelegate {
protected:
	friend class Base::Singleton<MsgProcessHandler>;
	MsgProcessHandler();

public:
	virtual ~MsgProcessHandler(){}

	virtual void init();
	virtual void unInit();
	virtual void onCocos2dExit();

	NetManager* netManager() {
		initNetManager();
		return m_netInstance_;
	}

	bool initNetManager();
	virtual void process(unsigned int connIndex, const char *szBuff, size_t size) ;
	virtual void onConnectSuccess(unsigned int connIndex) ;
	virtual void onConnectFail(unsigned int connIndex);
	virtual void onConnectBroken(unsigned int connIndex);
	virtual void onDownloaded(const net::stDownloadUrl &downloaded) ;
	virtual void onException(const std::string &e_what);

	void registerScriptHandler(int handler);
	void unregisterScriptHandler();

	void setUserId(unsigned int uId) {
		userId_ = uId;
	}
	void setSessionId(unsigned int sId) {
		sessionId_ = sId;
	}

	void showNetworkLog(bool show);
	bool connectToServer(unsigned int connIndex, const char* ip, unsigned short port);
	bool reconnectToServer(unsigned int connIndex);
	bool removeConnect(unsigned int connIndex);

	bool sendMessage(unsigned int connIndex, unsigned int msgId, 
		const char *szBuff, unsigned int len);
	bool sendMessageToDefault(unsigned int msgId, const char* szBuffer, unsigned int len);
	//bool sendMessageToDefault(PHDR header, const google::protobuf::Message& body );

	void setAutoConnect(bool autoConnect);
	bool isAutoConnect();

	inline bool supportProtocalCompress() {
		return true;
	}
protected:
	void onScriptNetMsg(const char* eventID, uint32_t connIndex, 
		const char* msg = "", uint32_t len = 0,
		uint32_t msgId = 0);

	virtual int doSerializeMsg(unsigned int msgId, const char* szBuff, size_t len);
	virtual const char* unSerializeMsg(unsigned int& msgId, const char* szBuff, size_t& len);
	void initBuffer();

	NetManager*			m_netInstance_;
	int					m_nScriptHandler;

	uint32_t			userId_;
	uint32_t			sessionId_;
	bool				showLog_;

	unsigned long		uncompressBufferLen_ = 0;
	char*				uncompressBuffer_ = nullptr;
	char *m_pSendBuffer;
	int m_iSendBufferLen;
	bool m_bInited;
	unsigned int m_defaultServerId;
};

#endif