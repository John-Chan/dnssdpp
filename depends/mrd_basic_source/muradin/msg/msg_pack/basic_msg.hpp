#ifndef MURADIN_MSGPACK_BASIC_MSG_HPP__
#define MURADIN_MSGPACK_BASIC_MSG_HPP__

#include <muradin/msg/msg_pack/detail/msg.hpp>


namespace muradin{namespace msg_pack{

struct	basic_msg_traits{

	// 键和值的分隔符,只能是单个字符
	static std::string	GetKeyValDelimiter(){return std::string(":");}

	// 键值对之间的分隔符,只能是单个字符
	static std::string	GetRecordDelimiter(){return std::string(";");}

	// 请求消息的标志字符序列,只能是8字节的ASCII字符
	static std::string	GetReqLine(){return std::string("REQ-V1.0");}

	// 响应消息的标志字符序列,只能是8字节的ASCII字符
	static std::string	GetRspLine(){return std::string("RSP-V1.0");}

};


 
typedef req_base<basic_msg_traits>	BasicReqMsg;
typedef rsp_base<basic_msg_traits>	BasicRspMsg;
	
}}

#endif//MURADIN_MSGPACK_BASIC_MSG_HPP__