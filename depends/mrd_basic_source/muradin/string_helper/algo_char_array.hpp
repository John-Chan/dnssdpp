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
	//  �ַ��Ƚ���
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
	//  �ַ�������
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	/// ��[start_pos,end_pos)��Χ�������ַ��� sub_string ���г��ֵ�λ��
	/// ����ҵ���sub_string����������λ��,��������ȡֵ��Χ��[start_pos,end_pos-1]
	/// ���û���ҵ�,����end_pos
	static size_t	search_sub_string(const string_piece& str,size_t start_pos,size_t end_pos,const string_piece& sub_string)
	{
		const char* found=std::search(str.ptr()+start_pos,str.ptr()+end_pos,sub_string.ptr(),sub_string.ptr()+sub_string.count());
		assert( (found >= str.ptr()+ start_pos) && (found <= str.ptr()+ end_pos));
		return (found==str.ptr()+end_pos)?end_pos:(found - str.ptr());
	}
	/// ��[0,str_.count()��Χ�������ַ��� sub_string ��һ�γ��ֵ�λ��
	/// ����ҵ���sub_string����������λ��,��������ȡֵ��Χ��[start_pos,end_pos-1]
	/// ���û���ҵ�,����str_.count()
	static size_t	search_sub_string_first(const string_piece& str,const string_piece& sub_string)
	{
		return search_sub_string(str,0,str.count(),sub_string);
	}


	/// ��to_scan����һ��������ɨ��,�����ַ��� sub_string ���г��ֵ�λ��
	/// scan_ret����ÿһ��sub_string���ֵ�λ��
	/// max_match_limit ��ʾ���ƥ����ٸ��������,������������0,���ʾ������
	/// count_notfound ��ʾ�Ƿ���scan_ret.scan_counter��ͳ��û�����������ַ�(���ǵĴ�������0)
	/// ���ط��������ļ�¼����
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
	//  �ַ��ָ���
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	/// �ַ����ָ�,sub_str ��Ϊ�ָ���
	/// max_match_limit ��ʾ��ౣ����ٸ��ָ���,������������0,���ʾ������
	/// include_token ��ʾ�ָ���ַ����Ƿ�����ָ���(�ָ�����������β��)
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