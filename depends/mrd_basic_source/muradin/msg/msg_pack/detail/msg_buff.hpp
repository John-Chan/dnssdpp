
/**
 * 
 * @file       msg_buff.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-2-8
 *  
 * @brief      底层消息对象
 * 
 * @e Copyright (c) <year> <copyright holders>
 */


#ifndef MURADIN_MSGPACK_MSG_BUFF_HPP__
#define MURADIN_MSGPACK_MSG_BUFF_HPP__

#include <muradin/string_helper/string_piece.hpp>
#include <muradin/string_helper/string_covert.hpp>
#include <muradin/base/bytebuffer.hpp>
#include <vector>
#include <list>

namespace muradin{
	namespace msg_pack
	{


		//msg_piece is a view of msg_buff
		class msg_piece
		{
		public:

			msg_piece(const std::string& txt)
				:data_(txt.c_str(),txt.length())
			{
			}
			msg_piece(muradin::string_helper::string_piece data)
				:data_(data.ptr(),data.bytes())
			{
				
			}
			msg_piece(const void* ptr,size_t bytes)
				:data_((const char*)ptr,bytes)
			{
				
			}
			
			bool	have_header()const
			{
				if(data_.bytes() < kStartLineLen + kFlowLengthLen + kHeadLengthLen) return false;
				size_t len=0;
				if (header_length_str().try_to_uint<size_t>(len,10)){
					return len>0;
				}else{
					return false;
				}
			}
			bool	have_body()const
			{
				if ( !have_header() ){
					return false;
				}else{
					return flow_length_str().to_uint<size_t>(10) - kHeadLengthLen > header_length_str().to_uint<size_t>(10) ;
				}
			}
			muradin::string_helper::string_piece	start_line()const
			{
				return muradin::string_helper::string_piece(data_.ptr(),kStartLineLen);
			}
			muradin::string_helper::string_piece	flow_length_str()const
			{
				return muradin::string_helper::string_piece(data_.ptr() + kStartLineLen,kFlowLengthLen);
			}
			muradin::string_helper::string_piece	header_length_str()const
			{
				return muradin::string_helper::string_piece(data_.ptr() + kStartLineLen +kFlowLengthLen,kHeadLengthLen);
			}

			muradin::string_helper::string_piece	header()const
			{
				return muradin::string_helper::string_piece(
					data_.ptr() + kStartLineLen +kFlowLengthLen+kHeadLengthLen,
					header_length_str().to_uint<size_t>(10) );
			}
			muradin::string_helper::string_piece	body()const
			{
				size_t bytes= flow_length_str().to_uint<size_t>(10) - kHeadLengthLen - header_length_str().to_uint<size_t>(10) ;
				return muradin::string_helper::string_piece(
					data_.ptr() + kStartLineLen +kFlowLengthLen+kHeadLengthLen+header_length_str().to_uint<size_t>(10),
					bytes);
			}

			std::string								to_string_copy()
			{
				return data_.std_string();
			}
			muradin::string_helper::string_piece	to_string()const
			{
				return data_;
			}

		private:

		private:
			muradin::string_helper::string_piece data_;
		};

		typedef	std::vector<msg_piece>			MsgPicecVec;

		class msg_buff
		{
		public:
			msg_buff()
			{
				reset(NULL,0);
			}
			msg_buff(const void* ptr,size_t bytes)
			{
				reset(ptr,bytes);
			}
			msg_buff& reset(const void* ptr,size_t bytes)
			{
				buff_.discard_all();
				buff_.append(ptr,bytes);
				return *this;
			}
			msg_piece	as_msg_piece()const
			{
				return msg_piece(buff_.rd_ptr(),buff_.readable_bytes());
			}
			void	swap(msg_buff& rh)
			{
				buff_.swap(rh.buff_);
			}
			size_t	bytes()const
			{
				return buff_.readable_bytes();
			}
			size_t	append_to(muradin::base::bytebuffer& buf)const
			{
				size_t copy_bytes=buff_.readable_bytes();
				buf.append(buff_.rd_ptr(),copy_bytes);
				return copy_bytes;
			}
		private:
			muradin::base::bytebuffer		buff_;
		};

		typedef	std::list<msg_buff>			MsgBuffList;
	}
}
#endif//MURADIN_MSGPACK_MSG_BUFF_HPP__