
/**
 * 
 * @file       stackworker_win.h 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-7-23 
 */


#ifndef MURADIN_DBG_HELPER_STACKWALK_WIN_H__
#define MURADIN_DBG_HELPER_STACKWALK_WIN_H__

/************************************************************************
//  README 
//  --------------------------------------------------------------------
//  - TO generate  module info 
//      -# need psapi.dll or tlhelp32.dll, it dynamic loaded    
//  - TO generate  callstack info 
//      -# need psapi.dll or dbghelp.dll, it dynamic loaded  
//  - If dll(s) can not be loaded via LoadLibrary,nothing happen
************************************************************************/


/// grab module info
#include <muradin/boost_dep/debug_helper/stackworker_win/modulefile_enumerator.hpp>

/// grab callstack infor
#include <muradin/boost_dep/debug_helper/stackworker_win/stackwalker.hpp>

/// some helper stuff for ouput
#include <muradin/boost_dep/debug_helper/stackworker_win/printer.hpp>


/// some helper stuff for system infomation
#include <muradin/boost_dep/debug_helper/stackworker_win/system_helper.hpp>


/// some helper stuff for error report
#include <muradin/boost_dep/debug_helper/stackworker_win/error_rpt.hpp>

#endif