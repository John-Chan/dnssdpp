#ifndef MURADIN_MSGPACK_BASIC_MSG_WRITER_HPP__
#define MURADIN_MSGPACK_BASIC_MSG_WRITER_HPP__

#include <muradin/msg/msg_pack/basic_msg.hpp>
#include <muradin/msg/msg_pack/detail/convert.hpp>

namespace muradin{namespace msg_pack{

	static	muradin::base::bytebuffer&		basic_msg_serialize(const BasicReqMsg& whole_msg,muradin::base::bytebuffer& buf)
	{
		return msg_serialize<BasicReqMsg>(whole_msg,buf);
	}
	static	std::string&					basic_msg_serialize(const BasicReqMsg& whole_msg,std::string& buf)
	{
		muradin::base::bytebuffer tmp_buf;
		basic_msg_serialize(whole_msg,tmp_buf);
		buf.append(tmp_buf.rd_ptr(),tmp_buf.readable_bytes());
		return buf;
	}
	static	muradin::base::bytebuffer&		basic_msg_serialize(const BasicRspMsg& whole_msg,muradin::base::bytebuffer& buf)
	{
		return msg_serialize<BasicRspMsg>(whole_msg,buf);
	}
	static	std::string&					basic_msg_serialize(const BasicRspMsg& whole_msg,std::string& buf)
	{
		muradin::base::bytebuffer tmp_buf;
		basic_msg_serialize(whole_msg,tmp_buf);
		buf.append(tmp_buf.rd_ptr(),tmp_buf.readable_bytes());
		return buf;
	}


}}

#endif//MURADIN_MSGPACK_BASIC_MSG_WRITER_HPP__