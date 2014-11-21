#ifndef LOGV_BOOST_LOG_HPP__
#define LOGV_BOOST_LOG_HPP__

#include <boost/log/sinks.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/record_ordering.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup/settings.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>

namespace logV{

	namespace logging		= boost::log;
	namespace log_attrib	= boost::log::attributes;
	namespace log_src		= boost::log::sources;
	namespace log_sinks		= boost::log::sinks;
	namespace log_expr		= boost::log::expressions;
	namespace log_keywords	= boost::log::keywords;
	
}



#endif//LOGV_BOOST_LOG_HPP__