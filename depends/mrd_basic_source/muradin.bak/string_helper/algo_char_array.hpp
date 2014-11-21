#ifndef MURADIN_ALGO_CHAR_ARRAY_HPP__
#define MURADIN_ALGO_CHAR_ARRAY_HPP__

#include <muradin/string_helper/string_piece.hpp>
#include <muradin/string_helper/string_algo_inl.hpp>
#include <muradin/string_helper/string_algo_utility.hpp>
#include <muradin/string_helper/algo_single_char.hpp>

#include <algorithm>

#include <assert.h>

namespace muradin{namespace string_helper{

	/************************************************************************
	//  字符比较类
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	inline	bool	is_decimal_str(const string_piece& str)
	{
		for (size_t n=0;n<str.bytes();++n){
			if(!is_decimal_char(str.ptr()[n]))
				return false;
		}
		return true;
	}

	inline	bool	is_letter_str(const string_piece& str)
	{
		for (size_t n=0;n<str.bytes();++n){
			if(!is_letter_char(str.ptr()[n]))
				return false;
		}
		return true;
	}
	inline	bool	is_hexadecimal_str(const string_piece& str)
	{
		for (size_t n=0;n<str.bytes();++n){
			if(!is_hexadecimal_char(str.ptr()[n]))
				return false;
		}
		return true;
	}

	/************************************************************************
	//  字符搜索类
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	/// 在[start_pos,end_pos)范围内搜索字符串 sub_string 所有出现的位置
	/// 如果找到了sub_string返回其索引位置,该索引的取值范围是[start_pos,end_pos-1]
	/// 如果没有找到,返回end_pos
	static size_t	search_sub_string(const string_piece& str,size_t start_pos,size_t end_pos,const string_piece& sub_string)
	{
		const char* found=std::search(str.ptr()+start_pos,str.ptr()+end_pos,sub_string.ptr(),sub_string.ptr()+sub_string.count());
		assert( (found >= str.ptr()+ start_pos) && (found <= str.ptr()+ end_pos));
		return (found==str.ptr()+end_pos)?end_pos:(found - str.ptr());
	}
	/// 在[0,str_.count()范围内搜索字符串 sub_string 第一次出现的位置
	/// 如果找到了sub_string返回其索引位置,该索引的取值范围是[start_pos,end_pos-1]
	/// 如果没有找到,返回str_.count()
	static size_t	search_sub_string_first(const string_piece& str,const string_piece& sub_string)
	{
		return search_sub_string(str,0,str.count(),sub_string);
	}


	/// 对to_scan进行一次完整的扫描,保存字符串 sub_string 所有出现的位置
	/// scan_ret保存每一个sub_string出现的位置
	/// max_match_limit 表示最多匹配多少个搜索结果,如果这个参数是0,则表示不限制
	/// count_notfound 表示是否在scan_ret.scan_counter中统计没有搜索到的字符(它们的次数将是0)
	/// 返回符合条件的记录数量
	static size_t	scan_for_sub_string(
		const string_piece&	to_scan,const string_piece& sub_string,
		ScanResult& scan_ret,
		size_t max_match_limit=0,
		bool count_notfound=true)
	{
		size_t matched_count=0;
		size_t start_pos=0;
		size_t end_pos=to_scan.count();
		if (count_notfound)
			scan_ret.scan_counter.insert(StringPieceAndCountMap::value_type(sub_string,0));

		while(end_pos !=(start_pos=search_sub_string(to_scan,start_pos,end_pos,sub_string))){
			scan_ret.scan_saver.push_back(StrPieceAndPos(sub_string,start_pos));
			utility::increment_count<StringPieceAndCountMap>(sub_string,scan_ret.scan_counter);

			++matched_count;
			if (max_match_limit !=0 && matched_count >= max_match_limit){
				break;
			}
			start_pos+=sub_string.count();
		}
		return matched_count;
	}
	
	/************************************************************************
	//  字符分割类
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	/// 字符串分割,sub_str 作为分隔符
	/// max_match_limit 表示最多保存多少个分割结果,如果这个参数是0,则表示不限制
	/// include_token 表示分割的字符串是否包含分隔符(分隔符将出现在尾部)
	static StringPieceVec		split_by_substr(
		const string_piece&	src,const string_piece& sub_str,
		size_t max_split_limit=0,
		bool include_token=true)
	{
		ScanResult scan_ret;
		StringPieceVec ret_list;
		size_t copy_length_min=include_token?0:1;

		if (scan_for_sub_string(src,sub_str,scan_ret,max_split_limit,include_token) > 0){
			split_to_container<StringPieceVec>(src,scan_ret,ret_list,max_split_limit,include_token);
		}
		return ret_list;
	}

}}
#endif//MURADIN_ALGO_CHAR_ARRAY_HPP__