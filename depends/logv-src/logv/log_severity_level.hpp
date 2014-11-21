
/**
 * 
 * @file       log_severity_level.hpp 		
 * @author     cj@sunnysec.com.cn
 * @date       2014-3-28
 * @e Copyright (c) <2014> sunnysec.com.cn
 */




#ifndef LOGV_LOG_SEVERITY_LEVEL_H__
#define LOGV_LOG_SEVERITY_LEVEL_H__

#include <string>
#include <iostream>

namespace logV{

	enum log_severity_level
	{
		trace		=0,
		debug		=1,
		info		=2,
		warning		=3,
		error		=4,
		fatal		=5
	};

	static const char* k_severity_name[] = {
		"TRACE",
		"DEBUG",
		"INFOR",
		"WARNG",
		"ERROR",
		"FATAL"
	};

	// boost.log needs this interface
	static std::ostream& operator<< (std::ostream& strm, log_severity_level level)
	{

		if (static_cast< size_t >(level) < sizeof(k_severity_name) / sizeof(k_severity_name[0]))
			strm << k_severity_name[level];
		else
			strm << static_cast< size_t >(level);

		return strm;
	}

	// boost.log needs this interface
	template< typename CharT, typename TraitsT >
	static std::basic_istream< CharT, TraitsT >& operator>> (std::basic_istream< CharT, TraitsT >& strm, log_severity_level& level)
	{
		if (strm.good()) 
		{ 
			std::string lv_str; 
			strm >> lv_str; 

			for (size_t index=0;index<sizeof(k_severity_name) / sizeof(k_severity_name[0]);++index ){
				if(lv_str == k_severity_name[index]){
					level = static_cast<log_severity_level>(index); 
					break;
				}
			}
		} 
		return strm; 
	}
}



#endif//LOGV_LOG_SEVERITY_LEVEL_H__