
#ifndef MURADIN_MSGPACK_CONVERT_HPP__
#define MURADIN_MSGPACK_CONVERT_HPP__

#include <muradin/msg/msg_pack/detail/predef.hpp>
#include <muradin/msg/msg_pack/detail/msg.hpp>
#include <muradin/msg/msg_pack/detail/msg_buff.hpp>
#include <muradin/string_helper/algo_single_char.hpp>

namespace muradin{
	namespace msg_pack
	{
		static	std::string	length_str_8bytes(size_t len)
		{
			static const size_t max_len=99999999;
			if(len > max_len){
				throw std::range_error("data too lage,must less than 99999999");
			}
			char tmp[64]={0};
			sprintf(tmp,"%08d",static_cast<unsigned int>(len));
			return std::string(tmp);
		}
		static	std::string	length_str_8bytes_truncate(size_t len)
		{
			static const size_t max_len=99999999;
			if(len > max_len){
				len=max_len;
			}
			char tmp[64]={0};
			sprintf(tmp,"%08d",static_cast<unsigned int>(len));
			return std::string(tmp);
		}

		template <typename pack_traits>
		static	void		kv_string(const std::string& key,const std::string& val,std::string& str)
		{
			std::string KeyValDelimiter=pack_traits::GetKeyValDelimiter();
			std::string RecordDelimiter=pack_traits::GetRecordDelimiter();
			str.append(key.c_str(),key.length());
			str.append(KeyValDelimiter.c_str(),KeyValDelimiter.length());
			str.append(val.c_str(),val.length());
			str.append(RecordDelimiter.c_str(),RecordDelimiter.length());
		}

		template <typename pack_traits>
		static	size_t		kv_serialize(const kv_table& kvs,muradin::base::bytebuffer& buf)
		{
			StrStrMap::const_iterator it=kvs.container().begin();
			StrStrMap::const_iterator eof=kvs.container().end();
			size_t len=buf.readable_bytes();
			while(it != eof){
				std::string one_line;
				kv_string<pack_traits>(it->first,it->second,one_line);
				buf.append(one_line.c_str(),one_line.length());
				++it;
			}
			return buf.readable_bytes() - len;
		}

		template <typename pack_traits>
		static	size_t		kv_serialize(const kv_table& kvs,std::string& buf)
		{
			StrStrMap::const_iterator it=kvs.container().begin();
			StrStrMap::const_iterator eof=kvs.container().end();
			size_t len=buf.length();
			while(it != eof){
				std::string one_line;
				kv_string<pack_traits>(it->first,it->second,one_line);
				buf.append(one_line.c_str(),one_line.length());
				++it;
			}
			return buf.length() - len;
		}


		template <class MsgType>
		static muradin::base::bytebuffer&		msg_serialize(const MsgType& whole_msg,muradin::base::bytebuffer& buf)
		{
			muradin::base::bytebuffer head_buff;
			kv_serialize<MsgType::PackTraits>(whole_msg.headers(),head_buff);
			std::string flow_length=length_str_8bytes(head_buff.readable_bytes() +whole_msg.body().readable_bytes() +  8 );
			std::string head_length=length_str_8bytes(head_buff.readable_bytes());
			buf.append(whole_msg.start_line().c_str(),whole_msg.start_line().length());
			buf.append(flow_length.c_str(),kFlowLengthLen);
			buf.append(head_length.c_str(),kHeadLengthLen);
			buf.append(head_buff.rd_ptr(),head_buff.readable_bytes());
			buf.append(whole_msg.body().rd_ptr(),whole_msg.body().readable_bytes());
			return buf;

		}
		

		template<typename pack_traits>
		static void		make_str_map(const muradin::string_helper::string_piece& str,kv_table& dest)
		{

			std::string delimiter=pack_traits::GetRecordDelimiter();
			muradin::string_helper::StringPieceVec key_and_v=
				muradin::string_helper::split_by_char(
				str,*delimiter.c_str(),0,false
				);

			delimiter=pack_traits::GetKeyValDelimiter();
			muradin::string_helper::StringPieceVec::const_iterator it=key_and_v.begin();
			muradin::string_helper::StringPieceVec::const_iterator over=key_and_v.end();
			while (it != over){
				muradin::string_helper::StringPieceVec pair_2=
					muradin::string_helper::split_by_char(
					*it,*delimiter.c_str(),0,false
					);
				if(pair_2.size()==2){
					dest.put_or_replace(pair_2[0].std_string(),pair_2[1].std_string());
				}
				++it;
			}
		}

		// convert will lost start_line,FlowLengthLen,HeadLengthLen infomation
		template <class MsgType>
		static		MsgType&							convert_to_msg(const msg_piece& mp,MsgType& dest)
		{
			if(mp.have_body()) dest.body().append(mp.body().ptr(),mp.body().bytes());
			if(mp.have_header()) make_str_map<MsgType::PackTraits>(mp.header(),dest.headers());
			return dest;
		}

		
	}
}
#endif//MURADIN_MSGPACK_CONVERT_HPP__