#ifndef	STRACK_WORKER_CRUSH_RPT_EXAMPLE_HPP__
#define STRACK_WORKER_CRUSH_RPT_EXAMPLE_HPP__

#include <muradin/boost_dep/debug_helper/stackworker_win/stackworker_win.h>


static int report_to_console(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{	
	// Generate error report
	DWORD	process_id=::GetCurrentProcessId();
	muradin::dbg::Win32Handle process(::OpenProcess( PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, process_id ));

	muradin::dbg::ModuleFileEnumerator mod_em(NULL);
	int sys_error=0;
	bool good=mod_em.enum_proc_modules(process.get(),process_id,sys_error);

	muradin::dbg::StackWalker sw(NULL,::GetCurrentProcessId(),::GetCurrentProcess());
	sw.get_callstacks(GetCurrentThread(),ep->ContextRecord);
	// Execute exception handler
	return EXCEPTION_EXECUTE_HANDLER;
}

static int report_to_file(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{	
	muradin::dbg::generate_report_files(_T("crush_report"),ep);
	
	return EXCEPTION_EXECUTE_HANDLER;
	//return EXCEPTION_CONTINUE_SEARCH;
}

static int bug_func3(int n)
{
	int val=1;
	int val2=::_tstoi(_T("000000"));
	return val/val2;
}
static int bug_func2(int n)
{
	return bug_func3(n);
}
static int bug_func1(const TCHAR* msg,int n)
{
	return bug_func2(n);
}
static	size_t throw_a_bad_alloc(size_t MB)
{
	size_t total=0,loop=0xFFFFFFFF;
	while(--loop > 0 ){
		char* p=new char[MB*1024*1024];
		++total;
	}
	return total*MB;
}
static	size_t bug_func4(size_t MB)
{
	try{
		return throw_a_bad_alloc(MB);
	}catch (std::bad_alloc& ){
		_tprintf_s(_T("std::bad_alloc\r\n"));
		throw;
	}catch (...){
		_tprintf_s(_T("unknow exception\r\n"));
		throw;
	}
}
static int	example_1(const TCHAR* msg,int n)
{
	__try
	{
		bug_func1(msg,n);
		return 0;
	}
	__except(report_to_console(GetExceptionCode(), GetExceptionInformation()))
	{    
		return 1;
	}
}
static int	example_2(const TCHAR* msg,int n)
{
	__try
	{
		bug_func1(msg,n);
		return 0; 
	}
	__except(report_to_file(GetExceptionCode(), GetExceptionInformation()))
	{   
		return 1;
	}
}

static int	example_3(size_t alloc_MB)
{
	__try
	{
		bug_func4(alloc_MB);
		return 0; 
	}
	__except(report_to_file(GetExceptionCode(), GetExceptionInformation()))
	{   
		return 1;
	}
}

#endif