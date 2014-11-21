#ifndef MURADIN_ALGO_SINGLE_CHAR_HPP__
#define MURADIN_ALGO_SINGLE_CHAR_HPP__

#include <muradin/string_helper/string_piece.hpp>
#include <muradin/string_helper/string_algo_inl.hpp>
#include <muradin/string_helper/string_algo_utility.hpp>

namespace muradin{namespace string_helper{

	/************************************************************************
	//  字符比较类
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	inline	bool	is_decimal_char(char ch)
	{
		return (ch>='0' && ch<='9');
	}

	inline	bool	is_letter_char(char ch)
	{
		return (ch>='A' && ch<='Z')||(ch>='a' && ch<='z');
	}
	inline	bool	is_hexadecimal_char(char ch)
	{
		return (ch>='A' && ch<='F')||(ch>='a' && ch<='f') || is_decimal_char(ch);
	}
	/************************************************************************
	//  字符搜索类
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/


	/// 在[start_pos,end_pos)范围内搜索字符 token 第一次出现的位置
	/// 如果找到了token返回其索引位置,该索引的取值范围是[start_pos,end_pos-1]
	/// 如果没有找到,返回end_pos
	static size_t	search_char(const string_piece& str,size_t start_pos,size_t end_pos,char token)
	{
		while(start_pos < end_pos ){
			if (str.at(start_pos) == token){
				return start_pos;
			}
			++start_pos;
		}
		return start_pos;
	}

	/// 在[0,str_.count())范围内搜索字符 token 第一次出现的位置
	/// 如果找到了token返回其索引位置,该索引的取值范围是[0,str_.count()]
	/// 如果没有找到,返回str_.count()
	static size_t	search_char_first(const string_piece& str,char token)
	{
		return search_char(str,0,str.count(),token);
	}
	

	/// 在[start_pos,end_pos)范围内搜索字符 token 第一次出现的位置,但搜索的顺序是从尾部开始向头部搜索
	/// 如果找到了token返回其索引位置,该索引的取值范围是[start_pos,end_pos-1]
	/// 如果没有找到,返回end_pos
	static size_t	reverse_search_char(const string_piece& str,size_t start_pos,size_t end_pos,char token)
	{
		size_t pos=end_pos;
		while( --pos !=start_pos ){
			if (str.at(pos) == token){
				return pos;
			}
		}
		return end_pos;
	}

	/// 在[start_pos,end_pos)范围内进行一次完整的扫描,并保存字符token出现的位置,扫描结果保存在内部容器中
	/// max_match_limit 表示最多保存多少个搜索结果,如果这个参数是0,则表示不限制
	/// 返回符合条件的记录数量
	static size_t	scan_for_single_char(
		const string_piece&	to_scan,const char& token,
		ScanResult& scan_ret,
		size_t max_match_limit,
		bool count_notfound)
	{
		size_t matched_count=0;
		size_t start_pos=0;
		size_t end_pos=to_scan.count();

		if (count_notfound)
			scan_ret.scan_counter.insert(StringPieceAndCountMap::value_type( string_piece(&token,1),0));

		while(end_pos !=(start_pos=search_char(to_scan,start_pos,end_pos,token))){
			scan_ret.scan_saver.push_back(StrPieceAndPos(string_piece(&token,1),start_pos));
			utility::increment_count<StringPieceAndCountMap>(string_piece(&token,1),scan_ret.scan_counter);

			++matched_count;
			if (max_match_limit !=0 && matched_count >= max_match_limit){
				break;
			}
			start_pos+=1;
		}
		return matched_count;

	}

	/// 对to_search进行一次完整的扫描,搜索tkoens中出现的任意字符
	/// scan_ret保存每一个token出现的位置
	/// max_match_limit 表示最多匹配多少个搜索结果,如果这个参数是0,则表示不限制
	/// count_notfound 表示是否在scan_ret.scan_counter中统计没有搜索到的字符(它们的次数将是0)
	/// 返回符合条件的记录数量
	static size_t	scan_for_any_char(
		const string_piece&	to_scan,const string_piece& tkoens,
		ScanResult& scan_ret,
		size_t max_match_limit,
		bool count_notfound)
	{
		size_t matched_count=0;
		size_t start_pos=0;
		size_t end_pos=to_scan.count();

		if (count_notfound){
			for (size_t i=0;i<tkoens.count();++i){
				scan_ret.scan_counter.insert(StringPieceAndCountMap::value_type( string_piece(tkoens.ptr() + i,1),0));
			}
		}
		while(end_pos != start_pos){
			size_t token_pos=search_char_first(tkoens,to_scan.at(start_pos));
			if (token_pos != tkoens.count()){
				scan_ret.scan_saver.push_back(StrPieceAndPos(string_piece(tkoens.ptr() + token_pos,1),start_pos));
				utility::increment_count<StringPieceAndCountMap>(string_piece(tkoens.ptr() + token_pos,1),scan_ret.scan_counter);

				++matched_count;
				if (max_match_limit !=0 && matched_count >= max_match_limit){
					break;
				}
			}
			++start_pos;
		}
		return matched_count;
	}

	/************************************************************************
	//  字符分割类
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/


	template<typename ContainerType>
	static void		split_to_container(
		const string_piece&	src,const ScanResult& scan_ret,
		ContainerType& container,
		size_t max_split_limit=0,
		bool include_token=true)
	{
		size_t start_copy_pos=0;
		size_t copy_length=0;
		size_t copy_length_min=0;
		size_t has_add=0;

		for (size_t i=0;i<scan_ret.scan_saver.size();++i){
			const string_piece&	current_piece(scan_ret.scan_saver[i].first);
			size_t	current_pos=scan_ret.scan_saver[i].second;

			copy_length=current_pos - start_copy_pos ;
			const char* ptr=src.ptr() + start_copy_pos;
			if (include_token){
				copy_length+=current_piece.count();
			}else{
				copy_length_min=current_piece.count();
			}
			if(copy_length> copy_length_min ){
				container.push_back(string_piece(ptr,copy_length));
				++has_add;
			}
			start_copy_pos=current_pos + current_piece.count();

		}

		/// looking for remaining 
		if (
			(start_copy_pos< src.count() && max_split_limit==0) ||
			(start_copy_pos< src.count() && has_add < max_split_limit)
			)
		{
			container.push_back(string_piece(src.ptr() + start_copy_pos,src.count()-start_copy_pos));
		}
	}
	
	/// 字符串分割,token 作为分隔符
	/// max_match_limit 表示最多保存多少个分割结果,如果这个参数是0,则表示不限制
	/// include_token 表示分割的字符串是否包含分隔符(分隔符将出现在尾部)
	static StringPieceVec		split_by_char(
		const string_piece&	src,const char& token,
		size_t max_split_limit=0,
		bool include_token=true)
	{
		ScanResult scan_ret;
		StringPieceVec ret_list;
		size_t copy_length_min=include_token?0:1;

		if (scan_for_single_char(src,token,scan_ret,max_split_limit,false) > 0){
			split_to_container<StringPieceVec>(src,scan_ret,ret_list,max_split_limit,include_token);
		}
		return ret_list;
	}


	/// 字符串分割,token 中的任意字符都作为分隔符
	/// max_match_limit 表示最多保存多少个分割结果,如果这个参数是0,则表示不限制
	/// include_token 表示分割的字符串是否包含分隔符(分隔符将出现在尾部)
	static StringPieceVec		split_by_any_char(
		const string_piece&	src,const string_piece& tokens,
		size_t max_split_limit=0,
		bool include_token=true)
	{
		ScanResult scan_ret;
		StringPieceVec ret_list;
		size_t copy_length_min=include_token?0:1;

		if (scan_for_any_char(src,tokens,scan_ret,max_split_limit,include_token) > 0){
			split_to_container<StringPieceVec>(src,scan_ret,ret_list,max_split_limit,include_token);
		}
		return ret_list;
	}
		


}}
#endif//MURADIN_ALGO_SINGLE_CHAR_HPP__