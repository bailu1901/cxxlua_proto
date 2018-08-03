--[======================[

    单个网络连接，包含其状态和消息监听等

]======================]

local type = type
local tos = tostring

-- MsgProcessHandler唯一实例，直接写在文件里，所有socket连接共享。定义在c++，用来创建网络连接
local netMsgHandler = MsgProcessHandler:getInstance()
-- 存储连接的socket的全局表
local socketArr = {}

local onEvent = function(event, connIndex, ...)
    local strConn = tos(connIndex)
    local socket = socketArr[strConn]
    if socket then
        socket:onNetEvent(event, ...)
    end
end

netMsgHandler:registerScriptHandler(onEvent)

-- 网络连接标识号，往上递增
local globalConnIndex = 0

--===================================================

local NetSocket = class("NetSocket")

-- 网络状态常量
NetSocket.EVENT_NET_MSG 				= 0		-- 正常获取到消息
NetSocket.EVENT_NET_CONNECT_SUCCESS 	= 1		-- 网络连接成功
NetSocket.EVENT_NET_CONNECT_FAIL 		= 2		-- 网络连接失败
NetSocket.EVENT_NET_CONNECT_BROKEN 	    = 3		-- 网络连接中断
NetSocket.EVENT_NET_EXCEPTION 			= 4		-- 网络异常？？

function NetSocket:ctor(ip, port)
    
    -- 连接参数
    self._connIndex = globalConnIndex
    globalConnIndex = globalConnIndex + 1

    self._ip = ip
    self._port = port

    -- 网络状态监听接口
	self._netListener = nil

    -- 是否连接上
    self._isConnected = false

    -- 存储到全局表里
    local strConn = tos(self._connIndex)
    socketArr[strConn] = self

end

--[=================[

	socket销毁，相当于析构函数，显示调用以销毁此对象

]=================]

function NetSocket:destroy()

    if self._isConnected then
        self:close()
    end
    
	self._netListener = nil

    socketArr[tos(self._connIndex)] = nil

end

--[=================[

    设置网络状态监听接口
    
    @param listener function/table 接收消息接口，如果是table则要有onNetEvent方法

]=================]

function NetSocket:setNetListener(listener)

    local lType = type(listener)
    assert(lType == "function" or (lType == "table" and listener.onNetEvent), "Invalid net listener: "..tostring(listener))

    self._netListener = listener

end

--[=================[

    获取网络连接标识号
    
    @return int

]=================]

function NetSocket:getConnIndex()

    return self._connIndex

end

--[=================[

    连接网络，使用创建socket传递的ip和port。
    其不可中途修改。如果要建立新连接，则创建新的NetSocket

]=================]

function NetSocket:connect()

    netMsgHandler:connectToServer(self._connIndex, self._ip, self._port)

end

--[=================[

    重连网络

]=================]

function NetSocket:reconnect()
    
    netMsgHandler:reconnectToServer(self._connIndex)

end

--[=================[

    是否已经连接上
    
    @return bool

]=================]

function NetSocket:isConnected()
    
    return self._isConnected

end

--[=================[

    关闭网络连接

]=================]

function NetSocket:close()

    -- 如果没有连接上，则直接返回
    -- 这里可能还要加等待，因为连接上是异步的，可能正在连接，这个时候关闭网络，因为isConnected还没生效，所以这里判断会直接返回
    -- 现在使用情况还比较简单，后面考虑优化一下
    if not self._isConnected then
        return
    end

    netMsgHandler:removeConnect(self._connIndex)

    self._isConnected = false

end

--[=================[

    发送消息

	@param msgId int 消息id 
	@param content table/string 消息元数据（非protobuf转换过后的)

]=================]

function NetSocket:sendMsg(msgId, content)

    netMsgHandler:sendMessage(self._connIndex, msgId, content, #content)

end

--[=================[

    网络消息

	@param msgId int 消息id 
	@param content table 消息元数据（非protobuf转换过后的)

]=================]

function NetSocket:onNetEvent(event, ...)

    if event == "netmsg" then
        self:_onReceiveNetMsg(...)
    elseif event == "connect_success" then
        self:_onConnectSuccess()
    elseif event == "connect_fail" then
        self:_onConnectFailed()
    elseif event == "connect_broken" then
        self:_onConnectBroken()
    elseif event == "exception" then
        self:_onNetException()
    end

end

function NetSocket:setUserId(uid) netMsgHandler:setUserId(uid) end
function NetSocket:setSessionId(sid) netMsgHandler:setSessionId(sid) end

-- 接受到网络消息
-- @param 分别是连接标识，消息体，消息长度，消息id

function NetSocket:_onReceiveNetMsg(msgBuf, msgLen, msgId)

    local content = msgBuf

    self:_dispatchEvent(NetSocket.EVENT_NET_MSG, msgId, content)

end

-- 连接成功回调

function NetSocket:_onConnectSuccess()

    self._isConnected = true
    self:_dispatchEvent(NetSocket.EVENT_NET_CONNECT_SUCCESS)

end

-- 连接失败回调

function NetSocket:_onConnectFailed()

    self:close()
    self._isConnected = false
    self:_dispatchEvent(NetSocket.EVENT_NET_CONNECT_FAIL)

end

-- 连接断开回调

function NetSocket:_onConnectBroken()

    self:close()
    self._isConnected = false
    self:_dispatchEvent(NetSocket.EVENT_NET_CONNECT_BROKEN)

end

-- 网络异常回调

function NetSocket:_onNetException()

    self:close()
    self._isConnected = false
    self:_dispatchEvent(NetSocket.EVENT_NET_EXCEPTION)

end

-- 抛出网络事件
-- @param event 事件名，见顶部定义
-- @param connIndex 连接标识

function NetSocket:_dispatchEvent(event, ...)
    
    if not self._netListener then
        return
    end

    local listener = self._netListener
    if type(listener) == "table" then
        listener = handler(self._netListener, self._netListener.onNetEvent)
    end

    listener(self, event, ...)

end

return NetSocket