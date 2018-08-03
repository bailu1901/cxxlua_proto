#ifndef __MESSAGE_QUEUE_COMMON_DEFINE_H__
#define __MESSAGE_QUEUE_COMMON_DEFINE_H__

enum
{
    success = 0,
    fail = 1,
    error = -1,
};

enum 
{
    max_game_tag_length				= 32,
    max_game_para_length			= 32,
    max_account_string_length		= 32,   //用户account最大长度 //
    max_passwd_string_length        = 32,
    max_nickname_length				= 64,	//昵称最大长度 //
    max_present_message_length		= 302,  //赠言最大长度，100个汉字(注：这个不要太长，有一部分赠言是放在透明数据中的，保证不溢出透明数据)  //
    max_system_message_length		= 4096,	//系统消息的最大长度 //
    max_reason_message_length		= 4096, //原因最大长度 //
    max_operate_description_length	= 64,   //操作描述最大长度 //
    max_transparent_data_size		= 4096, //透明数据最大长度 //
    max_crypt_string_length			= 128,  //加密串最大长度  //
    max_sub_message_size			= 4096,	//SubMessage的最大长度,它是序列化以后放在额外的透明数据中的 //
    max_qun_crypt_length			= 2048,	//51群组加密信息 //
    max_profile_crypt_length		= 2048,	//profile加密串最大长度 //
    max_web_qun_count				= 32,	//网站群的个数 //
    max_private_chat_length			= 3002,	//最多一次聊1000个汉字 //
    max_loudspeaker_length			= 302,	//喇叭最大100个汉字 //
    max_match_describe_length       = 128, //比赛信息描述 //
    max_rolename_length             = 32,  //角色名字 //
    max_hero_item_string_length     = 512, //用户装备字符串长度 //
    max_mail_content_length         = 2048,//邮件内容最大长度 //
    max_comment_length				= 128, //备注信息描述 //
    max_chat_content_length         = 1024,//发言内容最大长度 //
    max_big_transparent_data_size		= 32768, //透明数据最大长度 //
    max_package_size = 0xffff,		//64k SS通讯协议包的最大长度 //

};
    
enum
{
    SUCCESS                             = 0,
    FAIL                                = 1,
};

enum //seconds in xxx
{
    MINUTE                              = 60,       //one minute
    HOUR                                = 3600,     //60*MINUTE, //one hour
    DAY	                                = 86400,    //24*HOUR,	//one day
    YEAR                                = 31536000, //365*DAY	//one year
};

enum enm_desc_length
{
    MAX_DATETIME_STRING_LENGTH          = 36,       /**<日期时间字符串的最大长度 <*/
    MAX_FILE_NAME_LENGTH                = 255,		/**<文件名最大长度<*/
    MAX_PATH_LENGTH                     = 255,		/**<路径的最大长度 */ 
    MAX_IPADDR_LEN                      = 32,		//ip的地址的长度 //
    MAX_ACCOUNT_STRING_LENGTH           = 48,       //用户account最大长度 //
    MAX_PASSWD_STRING_LENGTH            = 32,       //密码长度 //
    MAX_PLAYER_NAME_LENGTH		        = 32,		/**< 最大用户名字长度 */
    MIN_PLAYER_NAME_LENGTH		        = 4,		/**< 最小用户名字长度 */
    MAX_URL_LENGTH				        = 128,		//URL地址的长度 //
    MAX_CHAT_MESSAGE_LENGTH	            = 512,	    //聊天的字符数，英文 //
    MAX_SPEAK_MESSAGE_LENGTH	        = 512,	    //喇叭字符数 //
    MAX_MAIL_CONTENT_LENGTH             = 2048,     //邮件内容最大长度 //
    MAX_SYSTEM_MESSAGE_LENGTH           = 4096,	    //系统公告消息的最大长度 //
    MAX_BILL_MESSAGE_LENGTH             = 2048,	    //系统公告消息的最大长度 //
    MAX_TINY_DESC_LENGTH                = 36,       /* */
    MAX_SHORT_DESC_LENGTH               = 128,      /* */
    MAX_LONG_DESC_LENGTH                = 512,      /* */

};

enum enm_desc_size
{
    MAX_TRANSPARENT_DATA_SIZE		    = 4096, //透明数据最大长度 //
    MAX_SETTED_TIMER_COUNT		        = 64,		/**< 每个用户最多可以设置的定时器数目 */
    MAX_COMMENT_SIZE				    = 128, //备注信息描述 //
};

#endif

