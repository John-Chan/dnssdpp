#ifndef MURADIN_BASE_STRING_ALGO_INCL_HPP_
#define MURADIN_BASE_STRING_ALGO_INCL_HPP_


#include <vector>
#include <map>
#include <utility>
#include <muradin/string_helper/string_piece.hpp>

namespace muradin
{
	namespace string_helper{

		typedef std::pair<char,size_t>			CharAndPos;
		typedef std::pair<std::string,size_t>	StringAndPos;
		typedef	std::vector<CharAndPos>			CharAndPosVec;
		typedef	std::vector<StringAndPos>		StringAndPosVec;
		typedef std::map<char,size_t>			CharAndCountMap;
		typedef std::map<std::string,size_t>	StringAndCountMap;

		template<class PosContainer,class CounterContainer>
		struct scan_result
		{
			// 搜索结果序列,按照查找顺序存储
			PosContainer		scan_saver;
			// 搜索结果分组统计
			CounterContainer	scan_counter;
		};

		typedef  scan_result<CharAndPosVec,CharAndCountMap>		CharScanResult;
		typedef  scan_result<StringAndPosVec,StringAndCountMap>	StringScanResult;

		typedef  scan_result<StrPieceAndPosVec,StringPieceAndCountMap>		ScanResult;


	}
}
#endif//MURADIN_BASE_STRING_ALGO_INCL_HPP_