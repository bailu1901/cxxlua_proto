#include "lua_cocos2dx_uf_manual.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/CCLuaValue.h"

#include "uf/net/MsgProcessHandler.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"MsgProcessHandler");
 tolua_usertype(tolua_S,"SingletonDelegate");
}

/* method: getInstance of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_getInstance00
static int tolua_UIInterface_luabinding_MsgProcessHandler_getInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   MsgProcessHandler* tolua_ret = (MsgProcessHandler*)  MsgProcessHandler::getInstance();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"MsgProcessHandler");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: registerScriptHandler of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_registerScriptHandler00
static int tolua_UIInterface_luabinding_MsgProcessHandler_registerScriptHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !toluafix_isfunction(tolua_S,2,"LUA_FUNCTION",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  LUA_FUNCTION handler = (  toluafix_ref_function(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'registerScriptHandler'", NULL);
#endif
  {
   self->registerScriptHandler(handler);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'registerScriptHandler'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: unregisterScriptHandler of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_unregisterScriptHandler00
static int tolua_UIInterface_luabinding_MsgProcessHandler_unregisterScriptHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'unregisterScriptHandler'", NULL);
#endif
  {
   self->unregisterScriptHandler();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'unregisterScriptHandler'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setUserId of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_setUserId00
static int tolua_UIInterface_luabinding_MsgProcessHandler_setUserId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int uId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setUserId'", NULL);
#endif
  {
   self->setUserId(uId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setUserId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSessionId of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_setSessionId00
static int tolua_UIInterface_luabinding_MsgProcessHandler_setSessionId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int sId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setSessionId'", NULL);
#endif
  {
   self->setSessionId(sId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setSessionId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: connectToServer of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_connectToServer00
static int tolua_UIInterface_luabinding_MsgProcessHandler_connectToServer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connIndex = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* ip = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned short port = ((unsigned short)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'connectToServer'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->connectToServer(connIndex,ip,port);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'connectToServer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reconnectToServer of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_reconnectToServer00
static int tolua_UIInterface_luabinding_MsgProcessHandler_reconnectToServer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connIndex = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reconnectToServer'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->reconnectToServer(connIndex);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reconnectToServer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeConnect of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_removeConnect00
static int tolua_UIInterface_luabinding_MsgProcessHandler_removeConnect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connIndex = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeConnect'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeConnect(connIndex);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeConnect'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendMessage of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_sendMessage00
static int tolua_UIInterface_luabinding_MsgProcessHandler_sendMessage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connIndex = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int msgId = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  const char* szBuff = ((const char*)  tolua_tostring(tolua_S,4,0));
  unsigned int len = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendMessage'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->sendMessage(connIndex,msgId,szBuff,len);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendMessage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendMessageToDefault of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_sendMessageToDefault00
static int tolua_UIInterface_luabinding_MsgProcessHandler_sendMessageToDefault00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MsgProcessHandler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MsgProcessHandler* self = (MsgProcessHandler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int msgId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* szBuffer = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned int len = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendMessageToDefault'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->sendMessageToDefault(msgId,szBuffer,len);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendMessageToDefault'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setAutoConnect of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_setAutoConnect00
static int tolua_UIInterface_luabinding_MsgProcessHandler_setAutoConnect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "MsgProcessHandler", 0, &tolua_err) ||
		!tolua_isboolean(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		MsgProcessHandler* self = (MsgProcessHandler*)tolua_tousertype(tolua_S, 1, 0);
		bool autoConnect = ((bool)tolua_toboolean(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setAutoConnect'", NULL);
#endif
		{
			self->setAutoConnect(autoConnect);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror :
	tolua_error(tolua_S, "#ferror in function 'setAutoConnect'.", &tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: isAutoConnect of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_isAutoConnect00
static int tolua_UIInterface_luabinding_MsgProcessHandler_isAutoConnect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "MsgProcessHandler", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		MsgProcessHandler* self = (MsgProcessHandler*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'isAutoConnect'", NULL);
#endif
		{
			bool tolua_ret = (bool)self->isAutoConnect();
			tolua_pushboolean(tolua_S, (bool)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
tolua_lerror :
	tolua_error(tolua_S, "#ferror in function 'isAutoConnect'.", &tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: showNetworkLog of class  MsgProcessHandler */
#ifndef TOLUA_DISABLE_tolua_UIInterface_luabinding_MsgProcessHandler_showNetworkLog00
static int tolua_UIInterface_luabinding_MsgProcessHandler_showNetworkLog00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "MsgProcessHandler", 0, &tolua_err) ||
		!tolua_isboolean(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		MsgProcessHandler* self = (MsgProcessHandler*)tolua_tousertype(tolua_S, 1, 0);
		bool show = ((bool)tolua_toboolean(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'showNetworkLog'", NULL);
#endif
		{
			self->showNetworkLog(show);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror :
	tolua_error(tolua_S, "#ferror in function 'showNetworkLog'.", &tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_UIInterface_luabinding_open(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	tolua_reg_types(tolua_S);
	tolua_module(tolua_S, NULL, 0);
	tolua_beginmodule(tolua_S, NULL);
	tolua_cclass(tolua_S, "MsgProcessHandler", "MsgProcessHandler", "SingletonDelegate", NULL);
	tolua_beginmodule(tolua_S, "MsgProcessHandler");
	tolua_function(tolua_S, "getInstance", tolua_UIInterface_luabinding_MsgProcessHandler_getInstance00);
	tolua_function(tolua_S, "registerScriptHandler", tolua_UIInterface_luabinding_MsgProcessHandler_registerScriptHandler00);
	tolua_function(tolua_S, "unregisterScriptHandler", tolua_UIInterface_luabinding_MsgProcessHandler_unregisterScriptHandler00);
	tolua_function(tolua_S, "setUserId", tolua_UIInterface_luabinding_MsgProcessHandler_setUserId00);
	tolua_function(tolua_S, "setSessionId", tolua_UIInterface_luabinding_MsgProcessHandler_setSessionId00);
	tolua_function(tolua_S, "connectToServer", tolua_UIInterface_luabinding_MsgProcessHandler_connectToServer00);
	tolua_function(tolua_S, "reconnectToServer", tolua_UIInterface_luabinding_MsgProcessHandler_reconnectToServer00);
	tolua_function(tolua_S, "removeConnect", tolua_UIInterface_luabinding_MsgProcessHandler_removeConnect00);
	tolua_function(tolua_S, "sendMessage", tolua_UIInterface_luabinding_MsgProcessHandler_sendMessage00);
	tolua_function(tolua_S, "sendMessageToDefault", tolua_UIInterface_luabinding_MsgProcessHandler_sendMessageToDefault00);
	tolua_function(tolua_S, "setAutoConnect", tolua_UIInterface_luabinding_MsgProcessHandler_setAutoConnect00);
	tolua_function(tolua_S, "isAutoConnect", tolua_UIInterface_luabinding_MsgProcessHandler_isAutoConnect00);
	tolua_function(tolua_S, "showNetworkLog", tolua_UIInterface_luabinding_MsgProcessHandler_showNetworkLog00);
	tolua_endmodule(tolua_S);
	tolua_endmodule(tolua_S);
	return 1;
}

#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
TOLUA_API int lua_cocos2dx_uf_luabinding (lua_State* tolua_S) {
	return tolua_UIInterface_luabinding_open(tolua_S);
};
#endif