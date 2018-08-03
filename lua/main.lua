local socket = NetSocket.new(ip, port)


local proxy = {
    

    onNetEvent = function(proxy, event, ...) 
        -- 网络状态常量 event
        -- NetSocket.EVENT_NET_MSG 				= 0		-- 正常获取到消息
        -- NetSocket.EVENT_NET_CONNECT_SUCCESS 	= 1		-- 网络连接成功
        -- NetSocket.EVENT_NET_CONNECT_FAIL 		= 2		-- 网络连接失败
        -- NetSocket.EVENT_NET_CONNECT_BROKEN 	    = 3		-- 网络连接中断
        -- NetSocket.EVENT_NET_EXCEPTION 			= 4		-- 网络异常？？

        if event == 1 then 
            -- connect ok
            socket:sendMsg(id, content)
        elseif event == 0 then 
            -- receive msg

        end
    end,


}


socket:setNetListener(proxy)
socket:connect()


--关掉socket

socket:destroy()