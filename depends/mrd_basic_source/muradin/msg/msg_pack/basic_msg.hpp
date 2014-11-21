#ifndef MURADIN_MSGPACK_BASIC_MSG_HPP__
#define MURADIN_MSGPACK_BASIC_MSG_HPP__

#include <muradin/msg/msg_pack/detail/msg.hpp>


namespace muradin{namespace msg_pack{

struct	basic_msg_traits{

	// ����ֵ�ķָ���,ֻ���ǵ����ַ�
	static std::string	GetKeyValDelimiter(){return std::string(":");}

	// ��ֵ��֮��ķָ���,ֻ���ǵ����ַ�
	static std::string	GetRecordDelimiter(){return std::string(";");}

	// ������Ϣ�ı�־�ַ�����,ֻ����8�ֽڵ�ASCII�ַ�
	static std::string	GetReqLine(){return std::string("REQ-V1.0");}

	// ��Ӧ��Ϣ�ı�־�ַ�����,ֻ����8�ֽڵ�ASCII�ַ�
	static std::string	GetRspLine(){return std::string("RSP-V1.0");}

};


 
typedef req_base<basic_msg_traits>	BasicReqMsg;
typedef rsp_base<basic_msg_traits>	BasicRspMsg;
	
}}

#endif//MURADIN_MSGPACK_BASIC_MSG_HPP__