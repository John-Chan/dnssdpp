
/**
 * 
 * @file       predef.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-2-8
 *  
 * @brief      一些前置定义
 * @remark     <some issue>  
 * @note    
 * @e<somthing important>
 * 
 * @e Copyright (c) <year> <copyright holders>
 */
#ifndef MURADIN_MSGPACK_PREDEF_HPP__
#define MURADIN_MSGPACK_PREDEF_HPP__

#include <map>
#include <string>

namespace muradin{
	namespace msg_pack
	{
	
		/*
		 data-structure:

		 +----------------+---------------------+----------------------+-----------------+-----------------+
		 |   start line   |     flow_length     |       head_length    |   head  zone    |   body zone     |
		 +----------------+---------------------+----------------------+-----------------+-----------------+
		 <    8 bytes   >   <     8 bytes     >   <     8 bytes     >   < head_length  >   <    N bytes    >
		 
		 */
	

		static const size_t kStartLineLen=8;
		static const size_t kFlowLengthLen=8;
		static const size_t kHeadLengthLen=8;
		//static const size_t kPackLenMin= kStartLineLen + kFlowLengthLen + kHeadLengthLen;
		static const size_t kPackLenMin= kStartLineLen + kFlowLengthLen ;

		typedef std::map<std::string,std::string>	StrStrMap;

		
	}
}
#endif//MURADIN_MSGPACK_PREDEF_HPP__