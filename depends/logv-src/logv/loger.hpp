
/**
 * 
 * @file       loger.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-3-28
 * @e Copyright (c) <2014> sunnysec.com.cn
 */


#ifndef LOGV_LOGV_LOGER_HPP__
#define LOGV_LOGV_LOGER_HPP__

#include "boost_lg.hpp"
#include "log_constant.h"
#include "log_severity_level.hpp"

namespace logV{
	typedef		log_src::severity_logger_mt<log_severity_level>		LogerTypeMT;
}

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(RuntimeLog, logV::LogerTypeMT);
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logV::log_severity_level);
BOOST_LOG_ATTRIBUTE_KEYWORD(log_tag, "Tag", std::string);

#define RUNTIME_LOG_STREAM_SEV(logger_obj, lvl)\
	BOOST_LOG_STREAM_WITH_PARAMS((logger_obj), (::boost::log::keywords::severity = (lvl)))

#define RUNTIME_SEV_LOG(SEVERITY_LEVEL) RUNTIME_LOG_STREAM_SEV(RuntimeLog::get(), SEVERITY_LEVEL)
#define RUNTIME_SEV_LOG_WITH_SRC_INFO(SEVERITY_LEVEL) RUNTIME_LOG_STREAM_SEV(RuntimeLog::get(), SEVERITY_LEVEL) << "(" << __FILE__ << ", " << __LINE__ << ") "

#endif//LOGV_LOGV_LOGER_HPP__