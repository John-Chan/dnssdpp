

#ifndef LOGV_LOG_INIT_FROM_SETTING_H__
#define LOGV_LOG_INIT_FROM_SETTING_H__

#include "boost_lg.hpp"
#include "log_constant.h"
#include "log_severity_level.hpp"
#include "log_more_functional.hpp"

#include <boost/log/utility/setup/from_settings.hpp>


namespace logV{

	/// open,prase config file,and create global log-core object automatically 
	/// usage:
	// @code
	//    logging::settings setts;
	//
	//    setts["Core"]["Filter"] = "%Severity% >= warning";
	//    setts["Core"]["DisableLogging"] = false;
	//
	//    setts["Sinks.Console"]["Destination"] = "Console";
	//    setts["Sinks.Console"]["Filter"] = "%Severity% >= fatal";
	//    setts["Sinks.Console"]["AutoFlush"] = true;
	//
	//    setts["Sinks.File.Destination"] = "TextFile";
	//    setts["Sinks.File.FileName"] = "MyApp_%3N.log";
	//    setts["Sinks.File.AutoFlush"] = true;
	//    setts["Sinks.File.RotationSize"] = 10 * 1024 * 1024; 
	//    
	//    init_log_env(setts);
	// @endcode
	static void	init_log_env(const logging::settings& setts)
	{
		user_keyword_support();

		logging::init_from_settings(setts);

		add_more_attrib_support();
	}
}

#endif//LOGV_LOG_INIT_FROM_SETTING_H__