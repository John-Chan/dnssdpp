#ifndef MURADIN_MSGPACK_CHECK_VALUE_HPP__
#define MURADIN_MSGPACK_CHECK_VALUE_HPP__

#include <muradin/string_helper/string_piece.hpp>
#include <muradin/string_helper/string_covert.hpp>
#include <muradin/msg/msg_pack/detail/predef.hpp>
#include <muradin/msg/msg_pack/detail/msg_buff.hpp>


#include <stdio.h>

namespace muradin{
	namespace msg_pack
	{
		inline	bool	got_min_pack_size(size_t n)
		{
			return n>= kPackLenMin;
		}
		template<typename pack_traits>
		inline	bool	is_req_line(const muradin::string_helper::string_piece& str )
		{
			return str.bytes()>=kStartLineLen && memcmp(str.ptr(),pack_traits::GetReqLine().c_str(),kStartLineLen)==0;
		}

		template<typename pack_traits>
		inline	bool	is_rsp_line(const muradin::string_helper::string_piece& str )
		{
			return str.bytes()>=kStartLineLen && memcmp(str.ptr(),pack_traits::GetRspLine().c_str(),kStartLineLen)==0;
		}

		template<typename pack_traits>
		inline	bool	is_start_line(const muradin::string_helper::string_piece& str )
		{
			return is_rsp_line< pack_traits>(str) || is_req_line< pack_traits>(str);
		}

		inline	bool	all_number(const muradin::string_helper::string_piece& str )
		{
			for (size_t i=0;i<str.bytes();++i){
				if(str.ptr()[i] >'9' || str.ptr()[i] <'0') return false;
			}
			return true;
		}

		static	bool	check_flow_datas(const msg_piece& msp,size_t flow_bytes,size_t& save,size_t& discard,std::string& what_wrong) 
		{

			assert(flow_bytes >= kHeadLengthLen);
			muradin::string_helper::string_piece str=msp.to_string();
			if( !all_number(str.sub_piece(kStartLineLen+kFlowLengthLen,kHeadLengthLen) )){
				discard=kStartLineLen+kFlowLengthLen+flow_bytes;
				what_wrong="charactors not numerical  :"+ str.sub_piece(kStartLineLen+kFlowLengthLen,kHeadLengthLen).std_string() ;
				return false;
			}else{
				size_t head_bytes=msp.header_length_str().to_uint_def<size_t>(10,0);
				if (head_bytes+8 <= flow_bytes){
					save= kStartLineLen+kFlowLengthLen+flow_bytes;
					if(str.bytes() >= save){
						return true;
					}else{
						save=0;
						what_wrong="need more data(flow_bytes)";
						return false;
					}
				}else{
					discard=kStartLineLen+kFlowLengthLen+flow_bytes;
					what_wrong="header length incorrect" ;
					return false;
				}
			}
		}

		// 检查数据
		// 如果是完整的消息,则返回成功,并且由save传出消息的大小.
		// 如果数据含有错误,则返回失败,并且由discard传出脏数据的大小.
		// 如果包未接受完整,则返回失败,并且save和discard都是0
		template<typename pack_traits>
		static	bool	good_pack(const msg_piece& msp,size_t& save,size_t& discard,std::string& what_wrong) 
		{
			discard=0;save=0;
			muradin::string_helper::string_piece str=msp.to_string();
			assert(str.bytes() >= kPackLenMin);
			if( !is_start_line<pack_traits>( str)) {
				discard=kStartLineLen;
				what_wrong="bad start_line";
				return false;
			}
			if( !all_number(str.sub_piece(kStartLineLen,kFlowLengthLen) )){
				discard=kStartLineLen+kFlowLengthLen;
				what_wrong="charactors not numerical :"+ str.sub_piece(kStartLineLen,kFlowLengthLen).std_string() ;
				return false;
			}
			size_t flow_bytes=msp.flow_length_str().to_uint_def<size_t>(10,0);
			
			if(flow_bytes == 0  ){
				save= kStartLineLen+kFlowLengthLen;
				return true;
			}else if(flow_bytes >= kHeadLengthLen){
				if(str.bytes() >= kStartLineLen+kFlowLengthLen+flow_bytes ){
					return check_flow_datas(msp,flow_bytes,save,discard,what_wrong);
				}else{
					// need more data
					what_wrong="need more data(flow_bytes)";
					save=0;
					return false;
				}
			}else{
				discard=kPackLenMin;
				what_wrong="flow_bytes too small :"+ msp.flow_length_str().std_string() ;
				return false;
			}

		}

		
	}
}
#endif//MURADIN_MSGPACK_CHECK_VALUE_HPP__