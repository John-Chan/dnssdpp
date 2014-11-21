#ifndef MURADIN_DBG_HELPER_ERRREPORT_HPP__
#define MURADIN_DBG_HELPER_ERRREPORT_HPP__

#include <muradin/boost_dep/debug_helper/stackworker_win/modulefile_enumerator.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/stackwalker.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/printer.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/system_helper.hpp>
#include <Windows.h>
#include <tchar.h>

namespace	muradin{namespace dbg{


	static int generate_report_files(const TCHAR* a_short_name, struct _EXCEPTION_POINTERS* ep)
	{	
		static const size_t char_count=1024;
		muradin::dbg::Win32Path this_module_path=muradin::dbg::Win32Path::path_of_module(NULL);

		TCHAR	report_file[char_count]={0};

		muradin::dbg::make_base_file_name(a_short_name,report_file,char_count);
		muradin::dbg::Win32Path abs_path=this_module_path.new_file_path(report_file);

		muradin::dbg::ModuleInfoFilePrinter	moudel_report(abs_path.full_path(report_file),_T("a"));
		muradin::dbg::CallstackFilePrinter	callstack_report(abs_path.full_path(report_file),_T("a"));
		
		DWORD	process_id=::GetCurrentProcessId();
		muradin::dbg::Win32Handle process(::OpenProcess( PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, process_id ));

		//////////////////////////////////////////////////////////////////////////
		/// generate module info for current process
		muradin::dbg::ModuleFileEnumerator mod_em(boost::bind(&muradin::dbg::ModuleInfoFilePrinter::print,&moudel_report,_1,_2,_3));
		moudel_report.open();
		int sys_error=0;
		bool good=mod_em.enum_proc_modules(process.get(),process_id,sys_error);
		moudel_report.close();


		//////////////////////////////////////////////////////////////////////////
		/// output exception info 
		ExceptionPointerFormator epfmt;
		callstack_report.open();
		callstack_report.print(epfmt.fmt(ep));
		callstack_report.close();

		//////////////////////////////////////////////////////////////////////////
		/// generate callstack
		muradin::dbg::StackWalker sw(boost::bind(&muradin::dbg::CallstackFilePrinter::print,&callstack_report,_1,_2,_3),
			::GetCurrentProcessId(),::GetCurrentProcess());
		callstack_report.open();
		sw.get_callstacks(GetCurrentThread(),ep->ContextRecord);
		callstack_report.close();

		//////////////////////////////////////////////////////////////////////////
		/// put a message to say we miss dbghelp.dll
		muradin::dbg::DbgHelperAPI dbghelpdll;
		if(!dbghelpdll.load()){
			callstack_report.open();
			callstack_report.print_ln(_T("cant general full information for callstack,miss dbghelp.dll"));
			callstack_report.close();
		}
		return 0;
	}


}}
#endif