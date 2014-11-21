#ifndef MURADIN_DBG_HELPER_API_WAPPER_H__
#define MURADIN_DBG_HELPER_API_WAPPER_H__


#include <muradin/boost_dep/debug_helper/stackworker_win/dbg_helper_funcs.h>
#include <muradin/boost_dep/debug_helper/stackworker_win/dll_loader.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/misc.hpp>

#include <boost/noncopyable.hpp>
#include <boost/assert.hpp>

#include <tchar.h>

#ifdef _INC_TOOLHELP32
#error	"do not include tlhelp32.h"
#endif // _INC_TOOLHELP32

namespace	muradin{namespace dbg{

	class DbgHelperAPI:boost::noncopyable
	{

	public:
		typedef struct	SymFuncTab_
		{
			FuncSymCleanup						SymCleanup;
			FuncSymFunctionTableAccess64		SymFunctionTableAccess64;
			FuncSymGetLineFromAddr64			SymGetLineFromAddr64;
			FuncSymGetLineFromAddrW64			SymGetLineFromAddrW64;
			FuncSymGetModuleBase64				SymGetModuleBase64;
			FuncSymGetModuleInfo64				SymGetModuleInfo64;
			FuncSymGetModuleInfoW64				SymGetModuleInfoW64;
			FuncSymGetOptions					SymGetOptions;
			FuncSymGetSymFromAddr64				SymGetSymFromAddr64;
			FuncSymInitialize					SymInitialize;
			FuncSymInitializeW					SymInitializeW;
			FuncSymLoadModule64					SymLoadModule64;
			FuncSymSetOptions					SymSetOptions;
			FuncStackWalk64						StackWalk64;
			FuncUnDecorateSymbolName			UnDecorateSymbolName;
			FuncUnDecorateSymbolNameW			UnDecorateSymbolNameW;
			FuncSymGetSearchPath				SymGetSearchPath;
			FuncSymGetSearchPathW				SymGetSearchPathW;
			FuncSymFromAddr						SymFromAddr;
			FuncSymFromAddrW					SymFromAddrW;
			//ReadProcessMemoryCallBack			ReadProcessMemory;
		}SymFuncTab;

	public:
		DbgHelperAPI(/*ReadProcessMemoryCallBack	read_mem_routine*/)
			:dll_loader_(_T("dbghelp.dll"))/*,read_mem_routine_(read_mem_routine)*/
		{
			clear();
		}
		~DbgHelperAPI()
		{
			clear();
		}
		bool	load()
		{
			if(all_func_good()) return true;
			if(!dll_loader_.load())	{
				_tprintf_s(_T("cant load dll[%s]"),_T("dbghelp.dll"));
				return false;
			}

			//func_tab_.ReadProcessMemory=read_mem_routine_;
			func_tab_.SymCleanup				= dll_loader_.load_dll_func<FuncSymCleanup>("SymCleanup") ;
			func_tab_.SymFunctionTableAccess64	= dll_loader_.load_dll_func<FuncSymFunctionTableAccess64>("SymFunctionTableAccess64") ;
			func_tab_.SymGetModuleBase64		= dll_loader_.load_dll_func<FuncSymGetModuleBase64>("SymGetModuleBase64") ;
			func_tab_.SymGetLineFromAddr64		= dll_loader_.load_dll_func<FuncSymGetLineFromAddr64>("SymGetLineFromAddr64") ;
			func_tab_.SymGetLineFromAddrW64		= dll_loader_.load_dll_func<FuncSymGetLineFromAddrW64>("SymGetLineFromAddrW64") ;
			func_tab_.SymGetModuleInfo64		= dll_loader_.load_dll_func<FuncSymGetModuleInfo64>("SymGetModuleInfo64") ;
			func_tab_.SymGetModuleInfoW64		= dll_loader_.load_dll_func<FuncSymGetModuleInfoW64>("SymGetModuleInfoW64") ;
			func_tab_.SymGetOptions				= dll_loader_.load_dll_func<FuncSymGetOptions>("SymGetOptions") ;
			func_tab_.SymGetSymFromAddr64		= dll_loader_.load_dll_func<FuncSymGetSymFromAddr64>("SymGetSymFromAddr64") ;
			func_tab_.SymInitialize				= dll_loader_.load_dll_func<FuncSymInitialize>("SymInitialize") ;
			func_tab_.SymInitializeW			= dll_loader_.load_dll_func<FuncSymInitializeW>("SymInitializeW") ;
			func_tab_.SymLoadModule64			= dll_loader_.load_dll_func<FuncSymLoadModule64>("SymLoadModule64") ;
			func_tab_.SymSetOptions				= dll_loader_.load_dll_func<FuncSymSetOptions>("SymSetOptions") ;
			func_tab_.StackWalk64				= dll_loader_.load_dll_func<FuncStackWalk64>("StackWalk64") ;
			func_tab_.UnDecorateSymbolName		= dll_loader_.load_dll_func<FuncUnDecorateSymbolName>("UnDecorateSymbolName") ;
			func_tab_.UnDecorateSymbolNameW		= dll_loader_.load_dll_func<FuncUnDecorateSymbolNameW>("UnDecorateSymbolNameW") ;
			func_tab_.SymGetSearchPath			= dll_loader_.load_dll_func<FuncSymGetSearchPath>("SymGetSearchPath") ;
			func_tab_.SymGetSearchPathW			= dll_loader_.load_dll_func<FuncSymGetSearchPathW>("SymGetSearchPathW") ;

			func_tab_.SymFromAddr				= dll_loader_.load_dll_func<FuncSymFromAddr>("SymFromAddr") ;
			func_tab_.SymFromAddrW				= dll_loader_.load_dll_func<FuncSymFromAddrW>("SymFromAddrW") ;	
			
			return	all_func_good();
		}

		BOOL	MrdSymCleanup(IN HANDLE hProcess)
		{
			return func_tab_.SymCleanup(hProcess);
		}
		PVOID	MrdSymFunctionTableAccess64( HANDLE hProcess, DWORD64 AddrBase )
		{
			return	func_tab_.SymFunctionTableAccess64(hProcess,AddrBase);
		}
#ifdef _UNICODE
		BOOL	SymFromAddr( HANDLE hProcess,DWORD64 Address, PDWORD64 Displacement,PSYMBOL_INFOW Symbol)
		{
			return	func_tab_.SymFromAddrW(hProcess,Address,Displacement,Symbol);
		}
#else
		BOOL	SymFromAddr( HANDLE hProcess,DWORD64 Address, PDWORD64 Displacement,PSYMBOL_INFO Symbol)
		{
			return	func_tab_.SymFromAddr(hProcess,Address,Displacement,Symbol);
		}
#endif // UNOCODE

#ifdef _UNICODE
		BOOL	MrdSymGetSymFromAddr64(IN HANDLE hProcess, IN DWORD64 dwAddr,
			OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOLW64  Symbol)
		{
#pragma message("[Warning]SymGetSymFromAddr64 dont have Unicode version")
			return	FALSE;
		}
#else
		BOOL	MrdSymGetSymFromAddr64(IN HANDLE hProcess, IN DWORD64 dwAddr,
			OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol)
		{
			BOOST_ASSERT(NULL != func_tab_.SymGetSymFromAddr64);
			return	func_tab_.SymGetSymFromAddr64(hProcess,dwAddr,pdwDisplacement,Symbol);
		}
#endif // UNOCODE


		DWORD64	MrdSymGetModuleBase64( IN HANDLE hProcess, IN DWORD64 dwAddr)
		{
			BOOST_ASSERT(NULL != func_tab_.SymGetModuleBase64);
			return	func_tab_.SymGetModuleBase64(hProcess,dwAddr);
		}

		DWORD	MrdSymGetOptions(VOID)
		{
			BOOST_ASSERT(NULL != func_tab_.SymGetOptions);
			return	func_tab_.SymGetOptions();
		}
		DWORD	MrdSymSetOptions(IN DWORD SymOptions)
		{
			BOOST_ASSERT(NULL != func_tab_.SymSetOptions);
			return	func_tab_.SymSetOptions(SymOptions);
		}

		BOOL	MrdStackWalk64(
			DWORD MachineType, 
			HANDLE hProcess,
			HANDLE hThread, 
			LPSTACKFRAME64 StackFrame, 
			PVOID ContextRecord,
			PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
			PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
			PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
			PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress )
		{
			BOOST_ASSERT(NULL != func_tab_.StackWalk64);
			return	func_tab_.StackWalk64(MachineType,hProcess,hThread,StackFrame,ContextRecord,
				ReadMemoryRoutine,FunctionTableAccessRoutine,GetModuleBaseRoutine,TranslateAddress);
		}

		BOOL	MrdSymGetLineFromAddr64(IN HANDLE hProcess, IN DWORD64 dwAddr,OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line )
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != func_tab_.SymGetLineFromAddrW64);
			return	func_tab_.SymGetLineFromAddrW64(hProcess,dwAddr,pdwDisplacement,Line);
#else
			BOOST_ASSERT(NULL != func_tab_.SymGetLineFromAddr64);
			return	func_tab_.SymGetLineFromAddr64(hProcess,dwAddr,pdwDisplacement,Line);
#endif // UNOCODE
		}

		BOOL	MrdSymGetModuleInfo64(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT V_IMAGEHLP_MODULE64 *ModuleInfo)
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != func_tab_.SymGetModuleInfoW64);
			return	func_tab_.SymGetModuleInfoW64(hProcess,dwAddr,ModuleInfo);
#else
			BOOST_ASSERT(NULL != func_tab_.SymGetModuleInfo64);
			return	func_tab_.SymGetModuleInfo64(hProcess,dwAddr,ModuleInfo);
#endif // UNOCODE
		}

		BOOL	MrdSymInitialize(IN HANDLE hProcess, IN PCTSTR UserSearchPath, IN BOOL fInvadeProcess)
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != func_tab_.SymInitializeW);
			return	func_tab_.SymInitializeW(hProcess,UserSearchPath,fInvadeProcess);
#else
			BOOST_ASSERT(NULL != func_tab_.SymInitialize);
			return	func_tab_.SymInitialize(hProcess,UserSearchPath,fInvadeProcess);
#endif // UNOCODE
		}


		/// SymLoadModule64
		/// @note There is no unicode version of this function ,we can use SymLoadModuleEx instead,but need 6.0 uper of dbghelp.dll
		/// If the function fails, the return value is zero
		DWORD64	MrdSymLoadModule64( IN HANDLE hProcess, IN HANDLE hFile,IN PTSTR ImageName, IN PTSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll)
		{

#ifdef _UNICODE
			BOOST_ASSERT(NULL != func_tab_.SymLoadModule64);
			static const size_t	kMaxLength=512*2;
			auto_buff_c<char>	ImageNameStrA(kMaxLength);
			auto_buff_c<char>	ModuleNameStrA(kMaxLength);
			if(ImageNameStrA.is_null() || ModuleNameStrA.is_null()){
				::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
				return 0;
			}
			if(!unicode_to_ansi(ImageName,ImageNameStrA.ptr(),ImageNameStrA.bytes())){
				return 0;
			}
			if(!unicode_to_ansi(ModuleName,ModuleNameStrA.ptr(),ModuleNameStrA.bytes())){
				return 0;
			}

			return	func_tab_.SymLoadModule64(hProcess,hFile,ImageNameStrA.ptr(),ModuleNameStrA.ptr(),BaseOfDll,SizeOfDll);
#else
			BOOST_ASSERT(NULL != func_tab_.SymLoadModule64);
			return	func_tab_.SymLoadModule64(hProcess,hFile,ImageName,ModuleName,BaseOfDll,SizeOfDll);
#endif // UNOCODE
		}

		DWORD	MrdUnDecorateSymbolName( PCTSTR DecoratedName, PTSTR UnDecoratedName,DWORD UndecoratedLength, DWORD Flags)
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != func_tab_.UnDecorateSymbolNameW);
			return	func_tab_.UnDecorateSymbolNameW(DecoratedName,UnDecoratedName,UndecoratedLength,Flags);
#else
			BOOST_ASSERT(NULL != func_tab_.UnDecorateSymbolName);
			return	func_tab_.UnDecorateSymbolName(DecoratedName,UnDecoratedName,UndecoratedLength,Flags);
#endif // UNOCODE
		}

		BOOL	MrdSymGetSearchPath(HANDLE hProcess, PTSTR SearchPath, DWORD SearchPathLength)
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != func_tab_.SymGetSearchPathW);
			return	func_tab_.SymGetSearchPathW(hProcess,SearchPath,SearchPathLength);
#else
			BOOST_ASSERT(NULL != func_tab_.SymGetSearchPath);
			return	func_tab_.SymGetSearchPath(hProcess,SearchPath,SearchPathLength);
#endif // UNOCODE
		}
		bool		all_func_good()const
		{
			auto_buff_c<char> all_zero_bytes(sizeof SymFuncTab);
			all_zero_bytes.zero_memory();
			if(0 == memcmp(&func_tab_,all_zero_bytes.ptr(),sizeof SymFuncTab))
				return false;
			return (
				/*func_tab_.ReadProcessMemory			!= NULL && */
				func_tab_.SymCleanup					!= NULL &&
				func_tab_.SymFunctionTableAccess64		!= NULL &&
				func_tab_.SymGetModuleBase64			!= NULL &&
				func_tab_.SymGetLineFromAddr64			!= NULL &&
				func_tab_.SymGetLineFromAddrW64			!= NULL &&
				func_tab_.SymGetModuleInfo64			!= NULL &&
				func_tab_.SymGetModuleInfoW64			!= NULL &&
				func_tab_.SymGetOptions					!= NULL &&
				func_tab_.SymGetSymFromAddr64			!= NULL &&
				func_tab_.SymInitialize					!= NULL &&
				func_tab_.SymInitializeW				!= NULL &&
				func_tab_.SymLoadModule64				!= NULL &&
				func_tab_.SymSetOptions					!= NULL &&
				func_tab_.StackWalk64					!= NULL &&
				func_tab_.UnDecorateSymbolName			!= NULL &&
				func_tab_.UnDecorateSymbolNameW			!= NULL &&
				func_tab_.SymGetSearchPath				!= NULL &&
				func_tab_.SymGetSearchPathW				!= NULL &&
				func_tab_.SymFromAddr					!= NULL &&
				func_tab_.SymFromAddrW					!= NULL 
				);
		}
		const SymFuncTab&	func_tab()const
		{
			return	func_tab_;
		}
	private:
		
		void		clear()
		{
			memset(&func_tab_,0x0,sizeof func_tab_ );
		}

		
	private:
		DllLoader	dll_loader_;
		SymFuncTab	func_tab_;
		//ReadProcessMemoryCallBack	read_mem_routine_;

	};

	class	Tool32API:boost::noncopyable
	{
	public:
		// **************************************** ToolHelp32 ************************
		enum{
			kMaxModuleName=255,			// kMaxModuleName = MAX_MODULE_NAME32(255), defined in tlhelp32.h
			kTH32SnapModule=0x00000008	// kTH32SnapModule = TH32CS_SNAPMODULE(0x00000008), defined in tlhelp32.h
		};
		typedef struct tagMODULEENTRY32W
		{
			DWORD   dwSize;
			DWORD   th32ModuleID;       // This module
			DWORD   th32ProcessID;      // owning process
			DWORD   GlblcntUsage;       // Global usage count on the module
			DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
			BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
			DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
			HMODULE hModule;            // The hModule of this module in th32ProcessID's context
			WCHAR   szModule[kMaxModuleName + 1];
			WCHAR   szExePath[MAX_PATH];
		} MODULEENTRY32W;
		typedef MODULEENTRY32W *  PMODULEENTRY32W;
		typedef MODULEENTRY32W *  LPMODULEENTRY32W;

		typedef struct tagMODULEENTRY32A
		{
			DWORD   dwSize;
			DWORD   th32ModuleID;       // This module
			DWORD   th32ProcessID;      // owning process
			DWORD   GlblcntUsage;       // Global usage count on the module
			DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
			BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
			DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
			HMODULE hModule;            // The hModule of this module in th32ProcessID's context
			char    szModule[kMaxModuleName + 1];
			char    szExePath[MAX_PATH];
		} MODULEENTRY32A;
		typedef MODULEENTRY32A *  PMODULEENTRY32A;
		typedef MODULEENTRY32A *  LPMODULEENTRY32A;
#ifdef _UNICODE
		typedef MODULEENTRY32W   MODULEENTRY32;
		typedef MODULEENTRY32 *  PMODULEENTRY32;
		typedef MODULEENTRY32 *  LPMODULEENTRY32;
#else
		typedef MODULEENTRY32A   MODULEENTRY32;
		typedef MODULEENTRY32 *  PMODULEENTRY32;
		typedef MODULEENTRY32 *  LPMODULEENTRY32;
#endif // UNOCODE
	private:
		// CreateToolhelp32Snapshot()
		typedef HANDLE (__stdcall *FuncCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
		// Module32First()
		typedef BOOL (__stdcall *FuncModule32FirstA)(HANDLE hSnapshot, LPMODULEENTRY32A lpme);
		// Module32Next()
		typedef BOOL (__stdcall *FuncModule32NextA)(HANDLE hSnapshot, LPMODULEENTRY32A lpme);
		// Module32First()
		typedef BOOL (__stdcall *FuncModule32FirstW)(HANDLE hSnapshot, LPMODULEENTRY32W lpme);
		// Module32Next()
		typedef BOOL (__stdcall *FuncModule32NextW)(HANDLE hSnapshot, LPMODULEENTRY32W lpme);
	public:
		Tool32API()
			:dll_kernel32_(_T("kernel32.dll")),
			dll_tlhelp32_(_T("tlhelp32.dll"))
		{ 
			clear();
		}
		~Tool32API()
		{
			clear();
		}
		bool	load()
		{
			if(all_func_good()) return true;
			// try both kernel32.dll and tlhelp32.dll
			if(load_from_kernel32()) return true;
			if(load_from_tlhelp32()) return true;
			return false;
		}
		HANDLE	MrdCreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID)
		{
			BOOST_ASSERT(NULL != CreateToolhelp32Snapshot_);
			return CreateToolhelp32Snapshot_(dwFlags,th32ProcessID);
		}
		BOOL	MrdModule32First(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != Module32FirstW_);
			return Module32FirstW_(hSnapshot,lpme);
#else
			BOOST_ASSERT(NULL != Module32FirstA_);
			return Module32FirstA_(hSnapshot,lpme);
#endif // UNOCODE
		}

		BOOL	MrdModule32Next(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != Module32NextW_);
			return Module32NextW_(hSnapshot,lpme);
#else
			BOOST_ASSERT(NULL != Module32NextA_);
			return Module32NextA_(hSnapshot,lpme);
#endif // UNOCODE
		}

		BOOL	MrdModule32FirstA(HANDLE hSnapshot, LPMODULEENTRY32A lpme)
		{
			BOOST_ASSERT(NULL != Module32FirstA_);
			return Module32FirstA_(hSnapshot,lpme);
		}

		BOOL	MrdModule32NextA(HANDLE hSnapshot, LPMODULEENTRY32A lpme)
		{
			BOOST_ASSERT(NULL != Module32NextA_);
			return Module32NextA_(hSnapshot,lpme);
		}

		BOOL	MrdModule32FirstW(HANDLE hSnapshot, LPMODULEENTRY32W lpme)
		{
			BOOST_ASSERT(NULL != Module32FirstW_);
			return Module32FirstW_(hSnapshot,lpme);
		}

		BOOL	MrdModule32NextW(HANDLE hSnapshot, LPMODULEENTRY32W lpme)
		{
			BOOST_ASSERT(NULL != Module32NextW_);
			return Module32NextW_(hSnapshot,lpme);
		}

		bool		all_func_good()const
		{
			return (
				CreateToolhelp32Snapshot_	!= NULL &&
				Module32FirstA_				!= NULL &&
				Module32NextA_				!= NULL &&
				Module32FirstW_				!= NULL &&
				Module32NextW_				!= NULL 
				);
		}
	private:
		bool		load_from_kernel32()
		{
			if(dll_kernel32_.load()){
				CreateToolhelp32Snapshot_		=dll_kernel32_.load_dll_func<FuncCreateToolhelp32Snapshot>("CreateToolhelp32Snapshot") ;
				Module32FirstA_					=dll_kernel32_.load_dll_func<FuncModule32FirstA>("Module32First") ;
				Module32NextA_					=dll_kernel32_.load_dll_func<FuncModule32NextA>("Module32Next") ;
				Module32FirstW_					=dll_kernel32_.load_dll_func<FuncModule32FirstW>("Module32FirstW") ;
				Module32NextW_					=dll_kernel32_.load_dll_func<FuncModule32NextW>("Module32NextW") ;
				return all_func_good();
			}

			_tprintf_s(_T("cant load dll[%s]"),_T("kernel32.dll"));
			return false;
		}
		bool		load_from_tlhelp32()
		{
			if(dll_tlhelp32_.load()){
				CreateToolhelp32Snapshot_		=dll_tlhelp32_.load_dll_func<FuncCreateToolhelp32Snapshot>("CreateToolhelp32Snapshot") ;
				Module32FirstA_					=dll_tlhelp32_.load_dll_func<FuncModule32FirstA>("Module32First") ;
				Module32NextA_					=dll_tlhelp32_.load_dll_func<FuncModule32NextA>("Module32Next") ;
				Module32FirstW_					=dll_kernel32_.load_dll_func<FuncModule32FirstW>("Module32FirstW") ;
				Module32NextW_					=dll_kernel32_.load_dll_func<FuncModule32NextW>("Module32NextW") ;
				return all_func_good();
			}
			_tprintf_s(_T("cant load dll[%s]"),_T("tlhelp32.dll"));
			return false;
		}
		void	clear()
		{
			CreateToolhelp32Snapshot_=NULL;
			Module32FirstA_=NULL;
			Module32NextA_=NULL;
			Module32FirstW_=NULL;
			Module32NextW_=NULL;
		}
	private:
		DllLoader	dll_kernel32_;
		DllLoader	dll_tlhelp32_;
		FuncCreateToolhelp32Snapshot	CreateToolhelp32Snapshot_;
		FuncModule32FirstA				Module32FirstA_;
		FuncModule32NextA				Module32NextA_;
		FuncModule32FirstW				Module32FirstW_;
		FuncModule32NextW				Module32NextW_;
	};

	class	PSAPI:boost::noncopyable
	{
	public:
		// **************************************** PSAPI ************************
		typedef struct _MODULEINFO {
			LPVOID lpBaseOfDll;		//The load address of the module
			DWORD SizeOfImage;		//The size of the linear space that the module occupies, in bytes.
			LPVOID EntryPoint;		//The entry point of the module.
		} MODULEINFO, *LPMODULEINFO;
		// Structure for GetProcessMemoryInfo()
		typedef struct _PROCESS_MEMORY_COUNTERS {
			DWORD cb;
			DWORD PageFaultCount;
			SIZE_T PeakWorkingSetSize;
			SIZE_T WorkingSetSize;
			SIZE_T QuotaPeakPagedPoolUsage;
			SIZE_T QuotaPagedPoolUsage;
			SIZE_T QuotaPeakNonPagedPoolUsage;
			SIZE_T QuotaNonPagedPoolUsage;
			SIZE_T PagefileUsage;
			SIZE_T PeakPagefileUsage;
		} PROCESS_MEMORY_COUNTERS;
		typedef PROCESS_MEMORY_COUNTERS *PPROCESS_MEMORY_COUNTERS;
	private:
		// EnumProcessModules()
		typedef BOOL (__stdcall *FuncEnumProcessModules)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
		// GetModuleInformation()
		typedef BOOL (__stdcall *FuncGetModuleInformation)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );
		// GetModuleFileNameExA()
		typedef DWORD (__stdcall *FuncGetModuleFileNameExA)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD char_count );
		// GetModuleBaseNameA()
		typedef DWORD (__stdcall *FuncGetModuleBaseNameA)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD char_count );
		// GetModuleFileNameExW()
		typedef DWORD (__stdcall *FuncGetModuleFileNameExW)(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD char_count );
		// GetModuleBaseNameW()
		typedef DWORD (__stdcall *FuncGetModuleBaseNameW)(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD char_count );

		// GetProcessMemoryInfo()
		typedef BOOL (__stdcall *FuncGetProcessMemoryInfo)(HANDLE hProcess, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb );


	public:
		PSAPI()
			:dll_psapi_(_T("psapi.dll"))
		{
			clear();
		}
		~PSAPI()
		{
			clear();
		}
		bool	load()
		{
			if(all_func_good()) return true;
			if(dll_psapi_.load()){
				EnumProcessModules_		=dll_psapi_.load_dll_func<FuncEnumProcessModules>("EnumProcessModules") ;
				GetModuleInformation_	=dll_psapi_.load_dll_func<FuncGetModuleInformation>("GetModuleInformation") ;
				GetModuleFileNameExA_	=dll_psapi_.load_dll_func<FuncGetModuleFileNameExA>("GetModuleFileNameExA") ;
				GetModuleBaseNameA_		=dll_psapi_.load_dll_func<FuncGetModuleBaseNameA>("GetModuleBaseNameA") ;
				GetModuleFileNameExW_	=dll_psapi_.load_dll_func<FuncGetModuleFileNameExW>("GetModuleFileNameExW") ;
				GetModuleBaseNameW_		=dll_psapi_.load_dll_func<FuncGetModuleBaseNameW>("GetModuleBaseNameW") ;
				GetProcessMemoryInfo_	=dll_psapi_.load_dll_func<FuncGetProcessMemoryInfo>("GetProcessMemoryInfo") ;
				return all_func_good();
			}
			_tprintf_s(_T("cant load dll[%s]"),_T("psapi.dll"));
			return false;
		}
		BOOL	MrdEnumProcessModules(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded)
		{
			BOOST_ASSERT(NULL != EnumProcessModules_);
			return EnumProcessModules_(hProcess,  lphModule,  cb,  lpcbNeeded);
		}
		BOOL	MrdGetModuleInformation(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize )
		{
			BOOST_ASSERT(NULL != GetModuleInformation_);
			return	GetModuleInformation_(hProcess,  hModule,  pmi,  nSize);
		}
		DWORD	MrdGetModuleBaseName(HANDLE hProcess, HMODULE hModule, TCHAR* lpFilename, DWORD char_count)
		{ 
#ifdef _UNICODE
			BOOST_ASSERT(NULL != GetModuleBaseNameW_);
			return GetModuleBaseNameW_(hProcess,hModule,lpFilename,char_count);
#else
			BOOST_ASSERT(NULL != GetModuleBaseNameA_);
			return GetModuleBaseNameA_(hProcess,hModule,lpFilename,char_count);
#endif // UNOCODE
		}

		DWORD	MrdGetModuleFileNameEx(HANDLE hProcess, HMODULE hModule, TCHAR* lpFilename, DWORD char_count)
		{
#ifdef _UNICODE
			BOOST_ASSERT(NULL != GetModuleFileNameExW_);
			return	GetModuleFileNameExW_(hProcess,hModule,lpFilename,char_count);
#else
			BOOST_ASSERT(NULL != GetModuleFileNameExA_);
			return	GetModuleFileNameExA_(hProcess,hModule,lpFilename,char_count);
#endif // UNOCODE
		}

		
		BOOL	GetProcessMemoryInfo(HANDLE hProcess, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb)
		{
			BOOST_ASSERT(NULL != GetProcessMemoryInfo_);
			return	GetProcessMemoryInfo_(hProcess,ppsmemCounters,cb);
		}

		bool		all_func_good()const
		{
			return (
				EnumProcessModules_		!= NULL &&
				GetModuleFileNameExA_	!= NULL &&
				GetModuleBaseNameA_		!= NULL &&
				GetModuleFileNameExW_	!= NULL &&
				GetModuleBaseNameW_		!= NULL &&
				GetModuleInformation_	!= NULL &&
				GetProcessMemoryInfo_	!= NULL
				);
		}

		DWORD	MrdGetModuleFileNameExA(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD char_count)
		{
			BOOST_ASSERT(NULL != GetModuleFileNameExA_);
			return	GetModuleFileNameExA_(hProcess,hModule,lpFilename,char_count);
		}
		DWORD	MrdGetModuleBaseNameA(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD char_count)
		{ 
			BOOST_ASSERT(NULL != GetModuleBaseNameA_);
			return GetModuleBaseNameA_(hProcess,hModule,lpFilename,char_count);
		}
		DWORD	MrdGetModuleFileNameExW(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD char_count)
		{
			BOOST_ASSERT(NULL != GetModuleFileNameExW_);
			return	GetModuleFileNameExW_(hProcess,hModule,lpFilename,char_count);
		}
		DWORD	MrdGetModuleBaseNameW(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD char_count)
		{ 
			BOOST_ASSERT(NULL != GetModuleBaseNameW_);
			return GetModuleBaseNameW_(hProcess,hModule,lpFilename,char_count);
		}
	private:
		void	clear()
		{
			EnumProcessModules_=NULL;
			GetModuleFileNameExA_=NULL;
			GetModuleBaseNameA_=NULL;
			GetModuleInformation_=NULL;
			GetModuleFileNameExW_=NULL;
			GetModuleBaseNameW_=NULL;
			GetProcessMemoryInfo_=NULL;
		}
	private:
		DllLoader	dll_psapi_;
		FuncEnumProcessModules		EnumProcessModules_;
		FuncGetModuleInformation	GetModuleInformation_;
		FuncGetModuleFileNameExA	GetModuleFileNameExA_;
		FuncGetModuleBaseNameA		GetModuleBaseNameA_;
		FuncGetModuleFileNameExW	GetModuleFileNameExW_;
		FuncGetModuleBaseNameW		GetModuleBaseNameW_;
		FuncGetProcessMemoryInfo	GetProcessMemoryInfo_;
	};

	

}}
#endif