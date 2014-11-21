#ifndef LOGV_LOGING_MORE_FUNCTIONAL_HPP__
#define LOGV_LOGING_MORE_FUNCTIONAL_HPP__

#include "boost_lg.hpp"
#include "log_constant.h"
#include "log_severity_level.hpp"

namespace logV{

	// call this function before log-librare init
	static void	user_keyword_support()
	{
		logging::register_simple_formatter_factory<log_severity_level, char>("Severity"); 
		logging::register_simple_filter_factory<log_severity_level, char>("Severity");

		logging::register_simple_formatter_factory<std::string, char>("logtag"); 
		logging::register_simple_filter_factory<std::string, char>("logtag");
	}

	// call this function after log-librare init
	static void	add_more_attrib_support(bool use_local_clock=true)
	{
		
		if(use_local_clock){
			logging::core::get()->add_global_attribute("TimeStamp", log_attrib::local_clock());
		}else{
			logging::core::get()->add_global_attribute("TimeStamp", log_attrib::utc_clock());
		}
		logging::core::get()->add_global_attribute("ProcessID",log_attrib::current_process_id());
		logging::core::get()->add_global_attribute("ThreadID",log_attrib::current_thread_id());
		logging::core::get()->add_global_attribute("RecordID", log_attrib::counter< unsigned int >());
		logging::core::get()->add_global_attribute("Scope", log_attrib::named_scope());
	}

}
#endif//LOGV_LOGING_MORE_FUNCTIONAL_HPP__