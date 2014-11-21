
/**
 * 
 * @file       string_piece.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-17
 *  
 * @brief      字符串切片
 * @remark     string_piece 不保存数据的拷贝,由使用方管理数据的有效性 
 */


#ifndef MURADIN_BASE_STRING_PIECE_HPP_
#define MURADIN_BASE_STRING_PIECE_HPP_

#include <muradin/base/buff_holder.hpp>
#include <muradin/string_helper/string_covert.hpp>

#include <string>
#include <vector>
#include <map>


#include <assert.h>

namespace muradin
{
namespace string_helper{

	class string_piece:public muradin::base::ConstBuff
	{
	public:
		string_piece(const char* begin,size_t length)
			:muradin::base::ConstBuff(begin,length)
		{
			//
		}
		string_piece(const char* c_str)
			:muradin::base::ConstBuff(c_str,::strlen(c_str))
		{
			//
		}
		string_piece(const std::string& std_str)
			:muradin::base::ConstBuff(std_str.c_str(),std_str.length())
		{
			//
		}
		string_piece(const string_piece& rh)
			:muradin::base::ConstBuff(rh.ptr(),rh.count())
		{
			//
		}
		virtual bool operator<(const string_piece& rh)const
		{
			return rh.std_string() < this->std_string();
		}
		virtual bool operator==(const string_piece& rh)const
		{
			return rh.std_string() == this->std_string();
		}
		virtual bool operator!=(const string_piece& rh)const
		{
			return !(*this == rh);
		}
		string_piece		sub_piece(size_t start_pos,size_t char_count)const
		{
			// char_count 可以是 0
			assert_good_index(start_pos);
			if (char_count == 0){
				char_count= count() - start_pos;
			}else{
				assert_good_range(start_pos + char_count);
			}
			return string_piece(ptr()+start_pos, char_count);
		}


		///@ warn dont call like this : const char* p=string_piece.std_string().c_str();
		std::string std_string(size_t start_pos,size_t char_count)const
		{
			// char_count 可以是 0
			string_piece pc=sub_piece(start_pos,char_count);
			return std::string(pc.ptr(),pc.bytes());
		}

		///@ warn dont call like this : const char* p=string_piece.std_string().c_str();
		std::string std_string(size_t start_pos)const
		{
			return std_string(start_pos,0 );
		}

		///@ warn dont call like this : const char* p=string_piece.std_string().c_str();
		std::string std_string()const
		{
			return std_string(0,0);
		}

		void string_copy(size_t start_pos,size_t char_count,std::string& a_copy)const
		{
			a_copy= std_string(start_pos,char_count);
		}
		void string_copy(size_t start_pos,std::string& a_copy)const
		{
			a_copy= std_string(start_pos);
		}
		void string_copy(std::string& a_copy)const
		{
			a_copy= std_string();
		}
		// pos 必须>= 0并且 <count()
		char at(size_t pos)const
		{
			assert_not_null();
			assert_good_index(pos);
			return ptr()[pos];
		}
		template <class IntType>
		bool	try_to_uint(IntType& number,int radix)const
		{
			return str_to_uint<IntType>(std_string(),radix,number);
		}
		template <class IntType>
		bool	try_to_int(IntType& number,int radix)const
		{
			return str_to_int<IntType>(std_string(),radix,number);
		}
		template <class IntType>
		IntType	to_uint(int radix)const
		{
			IntType number=0;
			bool b=str_to_uint<IntType>(std_string(),radix,number);
			assert(b);
			return number;
			(void)b;
		}
		template <class IntType>
		IntType	to_int(int radix)const
		{
			IntType number=0;
			bool b=str_to_int<IntType>(std_string(),radix,number);
			assert(b);
			return number;
			(void)b;
		}

		template <class IntType>
		IntType	to_uint_def(int radix,IntType default_val)const
		{
			IntType number=default_val;
			bool b=str_to_uint<IntType>(std_string(),radix,number);
			assert(b);
			return number;
			(void)b;
		}
		template <class IntType>
		IntType	to_int_def(int radix,IntType default_val)const
		{
			IntType number=default_val;
			bool b=str_to_int<IntType>(std_string(),radix,number);
			assert(b);
			return number;
			(void)b;
		}
	private:
		inline void	assert_good_index(size_t pos)const
		{
			assert(/*pos>=0 && */pos< count() );
			(void)pos;
		}
		inline void	assert_good_range(size_t pos)const
		{
			assert(pos<= count() );
			(void)pos;
		}
		inline void	assert_not_null()const
		{
			assert(not_null());
		}
	};

	

	typedef std::vector<string_piece>		StringPieceVec;

	typedef std::pair<string_piece,size_t>	StrPieceAndPos;
	typedef std::vector<StrPieceAndPos>		StrPieceAndPosVec;
	typedef std::map<string_piece,size_t>	StringPieceAndCountMap;
}
}
#endif//MURADIN_BASE_STRING_PIECE_HPP_