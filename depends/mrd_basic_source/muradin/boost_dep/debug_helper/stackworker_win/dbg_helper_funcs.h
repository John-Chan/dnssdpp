#ifndef MURADIN_DBG_HELPER_WIN_DBG_FUNCS_H__
#define MURADIN_DBG_HELPER_WIN_DBG_FUNCS_H__


#include <muradin/boost_dep/debug_helper/stackworker_win/data_types.h>


namespace	muradin{namespace dbg{


	/************************************************************************
	//  Non-charset funcs
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	/// SymCleanup
	typedef BOOL (__stdcall *FuncSymCleanup)( IN HANDLE hProcess );


	/// SymFunctionTableAccess64
	typedef PVOID (__stdcall *FuncSymFunctionTableAccess64)( HANDLE hProcess, DWORD64 AddrBase );


	/// SymGetSymFromAddr64
	/// @note PIMAGEHLP_SYMBOL64 have Ansi and Unicode version,but SymGetSymFromAddr64 function does not
	typedef BOOL (__stdcall *FuncSymGetSymFromAddr64)( IN HANDLE hProcess, IN DWORD64 dwAddr,
		OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol );

	/// SymGetModuleBase64
	typedef DWORD64 (__stdcall *FuncSymGetModuleBase64)( IN HANDLE hProcess, IN DWORD64 dwAddr );


	/// SymGetOptions
	typedef DWORD (__stdcall *FuncSymGetOptions)( VOID );

	/// SymSetOptions
	typedef DWORD (__stdcall *FuncSymSetOptions)( IN DWORD SymOptions );

	/// StackWalk64
	typedef BOOL (__stdcall *FuncStackWalk64)( 
		DWORD MachineType, 
		HANDLE hProcess,
		HANDLE hThread, 
		LPSTACKFRAME64 StackFrame, 
		PVOID ContextRecord,
		PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
		PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
		PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
		PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );

	/************************************************************************
	//  Ansi version
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/

	/// SymFromAddr
	typedef BOOL (__stdcall *FuncSymFromAddr)( HANDLE hProcess,DWORD64 Address, PDWORD64 Displacement,PSYMBOL_INFO Symbol);

	/// SymGetLineFromAddr64
	typedef BOOL (__stdcall *FuncSymGetLineFromAddr64)( IN HANDLE hProcess, IN DWORD64 dwAddr,
		OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );

	/// SymGetModuleInfo64
	typedef BOOL (__stdcall *FuncSymGetModuleInfo64)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT V_IMAGEHLP_MODULE64 *ModuleInfo );


	/// SymInitialize
	typedef BOOL (__stdcall *FuncSymInitialize)( IN HANDLE hProcess, IN PCSTR UserSearchPath, IN BOOL fInvadeProcess );


	/// SymLoadModule64
	/// @note There is no unicode version of this function ,we can use SymLoadModuleEx instead,but need 6.0 uper of dbghelp.dll
	typedef DWORD64 (__stdcall *FuncSymLoadModule64)( IN HANDLE hProcess, IN HANDLE hFile,
		IN PSTR ImageName, IN PSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll );

	/// UnDecorateSymbolName
	typedef DWORD (__stdcall WINAPI *FuncUnDecorateSymbolName)( PCSTR DecoratedName, PSTR UnDecoratedName,
		DWORD UndecoratedLength, DWORD Flags );


	/// SymGetSearchPath
	typedef BOOL (__stdcall WINAPI *FuncSymGetSearchPath)(HANDLE hProcess, PSTR SearchPath, DWORD SearchPathLength);


	/************************************************************************
	//  UnicodeVersion
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/
	
	/// SymFromAddr
	typedef BOOL (__stdcall *FuncSymFromAddrW)( HANDLE hProcess,DWORD64 Address, PDWORD64 Displacement,PSYMBOL_INFOW Symbol);

	/// SymGetLineFromAddrW64
	typedef BOOL (__stdcall *FuncSymGetLineFromAddrW64)( IN HANDLE hProcess, IN DWORD64 dwAddr,
		OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );

	/// SymGetModuleInfoW64
	typedef BOOL (__stdcall *FuncSymGetModuleInfoW64)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT V_IMAGEHLP_MODULE64 *ModuleInfo );

	/// SymInitializeW
	typedef BOOL (__stdcall *FuncSymInitializeW)( IN HANDLE hProcess, IN PCWSTR UserSearchPath, IN BOOL fInvadeProcess );

	/// UnDecorateSymbolNameW
	typedef DWORD (__stdcall WINAPI *FuncUnDecorateSymbolNameW)( PCWSTR DecoratedName, PWSTR UnDecoratedName,
		DWORD UndecoratedLength, DWORD Flags );

	/// SymGetSearchPathW
	typedef BOOL (__stdcall WINAPI *FuncSymGetSearchPathW)(HANDLE hProcess, PWSTR SearchPath, DWORD SearchPathLength);


	/************************************************************************
	//  Others
	//  --------------------------------------------------------------------
	//                                                                  
	************************************************************************/


	/// FuncReadProcessMemoryRoutine64
	/// @para
	/// _1 A handle to the process for which the stack trace is generated
	/// _2 The base address of the memory to be read.
	/// _3 A pointer to a buffer that receives the memory to be read
	/// _4 The size of the memory to be read, in bytes.
	/// _5 A pointer to a variable that receives the number of bytes actually read
	/// @see PREAD_PROCESS_MEMORY_ROUTINE64 for StackWalk64
	typedef BOOL (__stdcall *FuncReadProcessMemoryRoutine64)(
		HANDLE      hProcess,
		DWORD64     qwBaseAddress,
		PVOID       lpBuffer,
		DWORD       nSize,
		LPDWORD     lpNumberOfBytesRead
		);
}}
#endif