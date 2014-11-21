#ifndef MURADIN_MSGPACK_BASIC_MSG_READER_HPP__
#define MURADIN_MSGPACK_BASIC_MSG_READER_HPP__

#include <muradin/msg/msg_pack/basic_msg.hpp>
#include <muradin/msg/msg_pack/detail/convert.hpp>
#include <muradin/msg/msg_pack/detail/msg_reader.hpp>

namespace muradin{namespace msg_pack{

	/// convert string-data(msg_piece) to BasicReqMsg
	/// convert will lost start_line,FlowLengthLen,HeadLengthLen infomation
	static	BasicReqMsg&			convert_to_basic_msg(const msg_piece& mp,BasicReqMsg& dest)
	{
		return convert_to_msg<BasicReqMsg>(mp,dest);
	}

	/// convert string-data(msg_piece) to BasicRspMsg
	/// convert will lost start_line,FlowLengthLen,HeadLengthLen infomation
	static	BasicRspMsg&			convert_to_basic_msg(const msg_piece& mp,BasicRspMsg& dest)
	{
		return convert_to_msg<BasicRspMsg>(mp,dest);
	}


	/// basic_msg_reader used to prase msg-object form stream
	typedef	reader<basic_msg_traits>		basic_msg_reader;

}}

#endif//MURADIN_MSGPACK_BASIC_MSG_READER_HPP__