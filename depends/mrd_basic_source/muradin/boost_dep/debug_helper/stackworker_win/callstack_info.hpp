#ifndef MURADIN_DBG_HELPER_CALLSTACK_INFO_HPP__
#define MURADIN_DBG_HELPER_CALLSTACK_INFO_HPP__


#include <muradin/boost_dep/debug_helper/stackworker_win/data_types.h>
//#include <WinBase.h>
#include <Windows.h>
#include <tchar.h>

namespace	muradin{namespace dbg{

	static const TCHAR*		symbol_str(SYM_TYPE sym_type)
	{
		switch(sym_type){
		case SymNone:return _T("SymNone");
		case SymCoff:return _T("SymCoff");
		case SymCv:return _T("SymCv");
		case SymPdb:return _T("SymPdb");
		case SymExport:return _T("SymExport");
		case SymDeferred:return _T("SymDeferred");
		case SymSym:return _T("SymSym");
			//SymVirtual:
		case 8: return _T("SymVirtual");
			// SymDia:
		case 9: return _T("SymDia");
		default:return _T("UNKONW");
		}
	}

	static const TCHAR*		exception_code_str(DWORD exception_code)
	{
		static  TCHAR	unknow_code[128]={0};// for unknow exception_code
		switch(exception_code){
		case EXCEPTION_ACCESS_VIOLATION:		return _T("EXCEPTION_ACCESS_VIOLATION");
		case EXCEPTION_DATATYPE_MISALIGNMENT:	return _T("EXCEPTION_DATATYPE_MISALIGNMENT");
		case EXCEPTION_BREAKPOINT:				return _T("EXCEPTION_BREAKPOINT");
		case EXCEPTION_SINGLE_STEP:				return _T("EXCEPTION_SINGLE_STEP");
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	return _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		case EXCEPTION_FLT_DENORMAL_OPERAND:	return _T("EXCEPTION_FLT_DENORMAL_OPERAND");
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:		return _T("EXCEPTION_FLT_DIVIDE_BY_ZERO");
		case EXCEPTION_FLT_INEXACT_RESULT:		return _T("EXCEPTION_FLT_INEXACT_RESULT");
		case EXCEPTION_FLT_INVALID_OPERATION:	return _T("EXCEPTION_FLT_INVALID_OPERATION");
		case EXCEPTION_FLT_OVERFLOW:			return _T("EXCEPTION_FLT_OVERFLOW");
		case EXCEPTION_FLT_STACK_CHECK:			return _T("EXCEPTION_FLT_STACK_CHECK");
		case EXCEPTION_FLT_UNDERFLOW:			return _T("EXCEPTION_FLT_UNDERFLOW");
		case EXCEPTION_INT_DIVIDE_BY_ZERO:		return _T("EXCEPTION_INT_DIVIDE_BY_ZERO");
		case EXCEPTION_INT_OVERFLOW:			return _T("EXCEPTION_INT_OVERFLOW");
		case EXCEPTION_PRIV_INSTRUCTION:		return _T("EXCEPTION_PRIV_INSTRUCTION");
		case EXCEPTION_IN_PAGE_ERROR:			return _T("EXCEPTION_IN_PAGE_ERROR");
		case EXCEPTION_ILLEGAL_INSTRUCTION:		return _T("EXCEPTION_ILLEGAL_INSTRUCTION");
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:return _T("EXCEPTION_NONCONTINUABLE_EXCEPTION");
		case EXCEPTION_STACK_OVERFLOW:			return _T("EXCEPTION_STACK_OVERFLOW");
		case EXCEPTION_INVALID_DISPOSITION:		return _T("EXCEPTION_INVALID_DISPOSITION");
		case EXCEPTION_GUARD_PAGE:				return _T("EXCEPTION_GUARD_PAGE");
		case EXCEPTION_INVALID_HANDLE:			return _T("EXCEPTION_INVALID_HANDLE");
		//case EXCEPTION_POSSIBLE_DEADLOCK:		return _T("EXCEPTION_POSSIBLE_DEADLOCK");
		default:
			_sntprintf_s(unknow_code, (sizeof(unknow_code)/sizeof(unknow_code[0])),_T("UNKONW EXCEPTION(0x%08x)"),exception_code);
			return unknow_code;
		}
	}

	//Entry for each Callstack-Entry
	class  CallstackEntry 
	{
	public:
		CallstackEntry()
		{
			reset();
		}
		void	reset()
		{
			offset=0;
			offsetFromSmybol=0;
			offsetFromLine=0;
			lineNumber=0;
			symType=0;
			baseOfImage=0;
			PEHeaderTimeDateStamp=0;
			::ZeroMemory(funcName,sizeof(funcName));
			::ZeroMemory(undFuncName,sizeof(undFuncName));
			::ZeroMemory(undFullFuncName,sizeof(undFullFuncName));
			::ZeroMemory(lineFileName,sizeof(lineFileName));
			::ZeroMemory(symTypeString,sizeof(symTypeString));
			::ZeroMemory(moduleName,sizeof(moduleName));
			::ZeroMemory(loadedImageName,sizeof(loadedImageName));
			::ZeroMemory(imageName,sizeof(imageName));
		}

		// if 0, we have no valid entry
		DWORD64 offset;  

		// Function name with C++ decoration
		TCHAR	funcName[IMAGEHLP_SYMBOL64_NAME_COUNT];			
		
		// Function name,Crack only the name for primary declaration. Returns [scope::]name. Does expand template parameters
		TCHAR	undFuncName[IMAGEHLP_SYMBOL64_NAME_COUNT];		
		
		// Function name,fully undecoration.
		TCHAR	undFullFuncName[IMAGEHLP_SYMBOL64_NAME_COUNT];	
		DWORD64 offsetFromSmybol;
		DWORD	offsetFromLine;
		DWORD	lineNumber;
		TCHAR	lineFileName[IMAGEHLP_SYMBOL64_NAME_COUNT];
		DWORD	symType;
		TCHAR	symTypeString[IMAGEHLP_SYMBOL64_NAME_COUNT];

		// The module name.
		TCHAR	moduleName[IMAGEHLP_SYMBOL64_NAME_COUNT];	

		// The base virtual address where the image is loaded
		DWORD64 baseOfImage;

		// The image name. The name may or may not contain a full path.
		TCHAR	imageName[IMAGEHLP_SYMBOL64_NAME_COUNT];

		// The full path and file name of the file from which symbols were loaded.
		TCHAR	loadedImageName[IMAGEHLP_SYMBOL64_NAME_COUNT];

		// The date and timestamp value from PE-Header. 
		// The value is represented in the number of seconds elapsed since midnight (00:00:00), January 1, 1970, Universal Coordinated Time, according to the system clock. 
		// The timestamp can be printed using the C run-time (CRT) function ctime.
		DWORD	PEHeaderTimeDateStamp;
	} ;
}}
#endif