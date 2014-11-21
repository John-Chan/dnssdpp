
/**
 * 
 * @file       logv.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-3-28
 * @e Copyright (c) <2014> sunnysec.com.cn
 */


#ifndef LOGV_LOGV_HPP__
#define LOGV_LOGV_HPP__

#include "boost_lg.hpp"
#include "loger.hpp"
#include "log_severity_level.hpp"
#include "loging_utility.hpp"



/************************************************************************
//  Helper Macros
//  --------------------------------------------------------------------
//                                                                  
************************************************************************/

// make code-level trace point(trace function name,line.ect..)
// usage: call SOURCE_CODE_TRACE_POINT() directly
#define		SOURCE_CODE_TRACE_POINT BOOST_LOG_FUNCTION

// make trace point and set a msg
// usage: MAKE_TRACE_POINT("operation return -1")
#define		MAKE_TRACE_POINT(MSG)					BOOST_LOG_NAMED_SCOPE(MSG)

// make scoped tag
#define		MAKE_SCOPED_THREAD_TAG(NAME,VALUE)		BOOST_LOG_SCOPED_THREAD_TAG(NAME,VALUE)

/************************************************************************
//  Log with source code infomations(source file name,line number)
//  --------------------------------------------------------------------
//  e.g.   SC_LOG_TRACE<< "this is log message";                                                              
************************************************************************/

#define SC_LOG(SEVERITY_LEVEL)	RUNTIME_SEV_LOG_WITH_SRC_INFO(SEVERITY_LEVEL)
#define SC_LOG_TRACE		SC_LOG(logV::trace)
#define SC_LOG_DEBUG		SC_LOG(logV::debug)
#define SC_LOG_INFO			SC_LOG(logV::info)
#define SC_LOG_WARNING		SC_LOG(logV::warning)
#define SC_LOG_ERROR		SC_LOG(logV::error)
#define SC_LOG_FATAL		SC_LOG(logV::fatal)
/************************************************************************
//  normally Log 
//  --------------------------------------------------------------------
//  e.g.   LOG_TRACE<< "this is log message";                                                                       
************************************************************************/
#define	PUT_LOG(SEVERITY_LEVEL)		RUNTIME_SEV_LOG(SEVERITY_LEVEL)
#define LOG_TRACE			PUT_LOG(logV::trace)
#define LOG_DEBUG			PUT_LOG(logV::debug)
#define LOG_INFO			PUT_LOG(logV::info)
#define LOG_WARNING			PUT_LOG(logV::warning)
#define LOG_ERROR			PUT_LOG(logV::error)
#define LOG_FATAL			PUT_LOG(logV::fatal)

/************************************************************************
//  log with tag
//  --------------------------------------------------------------------
//  e.g.   TG_LOG_TRACE("ImportantMsg")<< "this is log message";                                                                 
************************************************************************/
#define	TAGED_LOG(SEVERITY_LEVEL,TAG_STR)		RUNTIME_TAGED_LOG(SEVERITY_LEVEL,TAG_STR)
#define TG_LOG_TRACE(TAG_STR)					TAGED_LOG(logV::trace,TAG_STR)
#define TG_LOG_DEBUG(TAG_STR)					TAGED_LOG(logV::debug,TAG_STR)
#define TG_LOG_INFO(TAG_STR)					TAGED_LOG(logV::info,TAG_STR)
#define TG_LOG_WARNING(TAG_STR)					TAGED_LOG(logV::warning,TAG_STR)
#define TG_LOG_ERROR(TAG_STR)					TAGED_LOG(logV::error,TAG_STR)
#define TG_LOG_FATAL(TAG_STR)					TAGED_LOG(logV::fatal,TAG_STR)

#endif//LOGV_LOGV_HPP__