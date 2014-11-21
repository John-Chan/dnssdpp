#ifndef MURADIN_DBG_HELPER_STACKWALKER_HPP__
#define MURADIN_DBG_HELPER_STACKWALKER_HPP__


#include <muradin/boost_dep/debug_helper/stackworker_win/modulefile_enumerator.hpp>
#include <muradin/boost_dep/debug_helper/stackworker_win/imghelp_modul64.h>
#include <muradin/boost_dep/debug_helper/stackworker_win/make_thread_context.h>
#include <muradin/boost_dep/debug_helper/stackworker_win/callstack_info.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>

namespace	muradin{namespace dbg{

	
	/// StackCallback
	/// @para
	/// _1 index,start with 0
	/// _2 CallstackEntry
	/// _3 shoudld skip
	typedef	boost::function<void(size_t,const CallstackEntry*,bool*)>		StackCallback;

	/// ReadMemoryCallback
	/// @para
	/// _1 A handle to the process for which the stack trace is generated
	/// _2 The base address of the memory to be read.
	/// _3 A pointer to a buffer that receives the memory to be read
	/// _4 The size of the memory to be read, in bytes.
	/// _5 A pointer to a variable that receives the number of bytes actually read
	/// return BOOL
	/// @see PREAD_PROCESS_MEMORY_ROUTINE64 for StackWalk64
	typedef	boost::function<BOOL(HANDLE,DWORD64,PVOID,DWORD,LPDWORD)>	ReadMemoryCallback;


	class	StackWalker:boost::noncopyable
	{
	public:
		StackWalker(
			const StackCallback& stack_callback,
			DWORD process_id, 
			HANDLE process)
			:stack_callback_(stack_callback),
			sym_search_path_(2048),
			process_id_(process_id),
			process_(process),
			need_clear_(false)
		{	
			if(!sym_search_path_.is_null()) {
				sym_search_path_.zero_memory();
			}
		}
		~StackWalker()
		{
			if(need_clear_)
				dbg_helper_api_.MrdSymCleanup(process_);
		}


		void		append_search_path_for_symbol(const TCHAR* path)
		{

			/// The path, or series of paths separated by a semicolon (;), that is used to search for symbol files.
			//The current working directory of the application 
			//The _NT_SYMBOL_PATH environment variable 
			//The _NT_ALTERNATE_SYMBOL_PATH environment variable 
			
			if(sym_search_path_.is_null()) return;

			/// TODO:....
		}

		bool		get_callstacks(HANDLE thread_handle, const CONTEXT *context)
		{
			int system_error=0;
			CONTEXT ctx;
			ThreadPauser thread_pauser(thread_handle);
			bool good=init_sym_env(system_error);
			if(good){
				if (context == NULL){
					// If no context is provided, capture the context
					// good=get_thread_ctx(thread_handle,ctx,thread_pauser,system_error);
					
					
					return false;
				}else{
					ctx = *context;
				}	
			}

			if(good) good=retrieve_stack_frames(thread_handle,ctx,system_error);
			return good;
		}
	private:

		void		setup_stack_frames(const CONTEXT& ctx,STACKFRAME64& stack_frame,DWORD& image_type)
		{

#ifdef _M_IX86
			// normally, call ImageNtHeader() and use machine info from PE header
			image_type = IMAGE_FILE_MACHINE_I386;
			stack_frame.AddrPC.Offset = ctx.Eip;
			stack_frame.AddrPC.Mode = AddrModeFlat;
			stack_frame.AddrFrame.Offset = ctx.Ebp;
			stack_frame.AddrFrame.Mode = AddrModeFlat;
			stack_frame.AddrStack.Offset = ctx.Esp;
			stack_frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
			image_type = IMAGE_FILE_MACHINE_AMD64;
			stack_frame.AddrPC.Offset = ctx.Rip;
			stack_frame.AddrPC.Mode = AddrModeFlat;
			stack_frame.AddrFrame.Offset = ctx.Rsp;
			stack_frame.AddrFrame.Mode = AddrModeFlat;
			stack_frame.AddrStack.Offset = ctx.Rsp;
			stack_frame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
			image_type = IMAGE_FILE_MACHINE_IA64;
			stack_frame.AddrPC.Offset = ctx.StIIP;
			stack_frame.AddrPC.Mode = AddrModeFlat;
			stack_frame.AddrFrame.Offset = ctx.IntSp;
			stack_frame.AddrFrame.Mode = AddrModeFlat;
			stack_frame.AddrBStore.Offset = ctx.RsBSP;
			stack_frame.AddrBStore.Mode = AddrModeFlat;
			stack_frame.AddrStack.Offset = ctx.IntSp;
			stack_frame.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif
		}


		bool		get_call_symbol_at_addr( STACKFRAME64& stack_frame,CallstackEntry&	cs_entry,int& system_error)
		{

			// SYMBOL_INFOW PSYMBOL_INFOW SYMBOL_INFO PSYMBOL_INFO
#ifdef _UNICODE
			auto_buff_c<char> sym_info_buff(sizeof(SYMBOL_INFOW) + sizeof(TCHAR)*IMAGEHLP_SYMBOL64_NAME_COUNT);
#else
			auto_buff_c<char> sym_info_buff(sizeof(SYMBOL_INFO) + sizeof(TCHAR)*IMAGEHLP_SYMBOL64_NAME_COUNT);
#endif // UNOCODE
			if(sym_info_buff.is_null()){
				system_error=ERROR_NOT_ENOUGH_MEMORY;
				return false;
			}
			sym_info_buff.zero_memory();
#ifdef _UNICODE
			PSYMBOL_INFOW sym_info_ptr=(PSYMBOL_INFOW)sym_info_buff.ptr();
			sym_info_ptr->SizeOfStruct=sizeof(SYMBOL_INFOW);
			sym_info_ptr->MaxNameLen =IMAGEHLP_SYMBOL64_NAME_COUNT; // as char count,NOT Bytes
#else
			PSYMBOL_INFO sym_info_ptr=(PSYMBOL_INFO)sym_info_buff.ptr();
			sym_info_ptr->SizeOfStruct=sizeof(SYMBOL_INFO);
			sym_info_ptr->MaxNameLen =IMAGEHLP_SYMBOL64_NAME_COUNT; // as char count,NOT Bytes
#endif // UNOCODE

			// procedure via SymGetSymFromAddr64
			if (dbg_helper_api_.SymFromAddr(process_, stack_frame.AddrPC.Offset, &(cs_entry.offsetFromSmybol), sym_info_ptr) != FALSE)
			{
				// TODO: Mache dies sicher...!
				_sntprintf_s(cs_entry.funcName,(sizeof(cs_entry.funcName)/sizeof(cs_entry.funcName[0])),
					_T("%s"),sym_info_ptr->Name);
				//strcpy_s(cs_entry.name, img_sym64_ptr->Name);
				// UnDecorateSymbolName()
				dbg_helper_api_.MrdUnDecorateSymbolName(sym_info_ptr->Name,cs_entry.undFuncName,(sizeof(cs_entry.undFuncName)/sizeof(cs_entry.undFuncName[0])),UNDNAME_NAME_ONLY);
				dbg_helper_api_.MrdUnDecorateSymbolName(sym_info_ptr->Name,cs_entry.undFullFuncName,(sizeof(cs_entry.undFullFuncName)/sizeof(cs_entry.undFullFuncName[0])),UNDNAME_COMPLETE);
				return true;
			}else{
				retrieve_sys_error(system_error);
				return false;
			}
		}

#if	0

// code will be deleted

		bool		get_call_symbol_at_addr( STACKFRAME64& stack_frame,CallstackEntry&	cs_entry,int& system_error)
		{

#ifdef _UNICODE
			auto_buff_c<char> img_sym64_buff(sizeof(IMAGEHLP_SYMBOLW64) + sizeof(TCHAR)*IMAGEHLP_SYMBOL64_NAME_COUNT);
#else
			auto_buff_c<char> img_sym64_buff(sizeof(IMAGEHLP_SYMBOL64) + sizeof(TCHAR)*IMAGEHLP_SYMBOL64_NAME_COUNT);
#endif // UNOCODE
			if(img_sym64_buff.is_null()){
				system_error=ERROR_NOT_ENOUGH_MEMORY;
				return false;
			}
			img_sym64_buff.zero_memory();
			
#ifdef _UNICODE
			PIMAGEHLP_SYMBOLW64 img_sym64_ptr=(PIMAGEHLP_SYMBOLW64)img_sym64_buff.ptr();
			img_sym64_ptr->SizeOfStruct=sizeof(IMAGEHLP_SYMBOLW64);
			img_sym64_ptr->MaxNameLength=IMAGEHLP_SYMBOL64_NAME_COUNT; // as char count,NOT Bytes
#else
			PIMAGEHLP_SYMBOL64 img_sym64_ptr=(PIMAGEHLP_SYMBOL64)img_sym64_buff.ptr();
			img_sym64_ptr->SizeOfStruct=sizeof(IMAGEHLP_SYMBOL64);
			img_sym64_ptr->MaxNameLength=IMAGEHLP_SYMBOL64_NAME_COUNT; // as char count,NOT Bytes
#endif // UNOCODE

			// procedure via SymGetSymFromAddr64
			if (dbg_helper_api_.MrdSymGetSymFromAddr64(process_, stack_frame.AddrPC.Offset, &(cs_entry.offsetFromSmybol), img_sym64_ptr) != FALSE)
			{
				// TODO: Mache dies sicher...!
				_sntprintf_s(cs_entry.funcName,(sizeof(cs_entry.funcName)/sizeof(cs_entry.funcName[0])),
					_T("%s"),img_sym64_ptr->Name);
				//strcpy_s(cs_entry.name, img_sym64_ptr->Name);
				// UnDecorateSymbolName()
				dbg_helper_api_.MrdUnDecorateSymbolName(img_sym64_ptr->Name,cs_entry.undFuncName,(sizeof(cs_entry.undFuncName)/sizeof(cs_entry.undFuncName[0])),UNDNAME_NAME_ONLY);
				dbg_helper_api_.MrdUnDecorateSymbolName(img_sym64_ptr->Name,cs_entry.undFullFuncName,(sizeof(cs_entry.undFullFuncName)/sizeof(cs_entry.undFullFuncName[0])),UNDNAME_COMPLETE);
				return true;
			}else{
				retrieve_sys_error(system_error);
				return false;
			}
		}

#endif //mark for delele
		void		fill_symbol_module_info(const V_IMAGEHLP_MODULE64&	img_mod_64,CallstackEntry&	cs_entry)
		{
			//just fill as v1 
			_sntprintf_s(cs_entry.symTypeString,(sizeof(cs_entry.symTypeString)/sizeof(cs_entry.symTypeString[0])),
				_T("%s"),symbol_str(img_mod_64.data.v1.SymType));

			// TODO: Mache dies sicher...!
			_sntprintf_s(cs_entry.moduleName,(sizeof(cs_entry.moduleName)/sizeof(cs_entry.moduleName[0])),
				_T("%s"),img_mod_64.data.v1.ModuleName);

			_sntprintf_s(cs_entry.loadedImageName,(sizeof(cs_entry.loadedImageName)/sizeof(cs_entry.loadedImageName[0])),
				_T("%s"),img_mod_64.data.v1.LoadedImageName);

			_sntprintf_s(cs_entry.imageName,(sizeof(cs_entry.imageName)/sizeof(cs_entry.imageName[0])),
				_T("%s"),img_mod_64.data.v1.ImageName);

			cs_entry.PEHeaderTimeDateStamp=img_mod_64.data.v1.TimeDateStamp;
			cs_entry.symType=img_mod_64.data.v1.SymType;
			cs_entry.baseOfImage=img_mod_64.data.v1.BaseOfImage;

		}
		bool		get_symbol_module_at_addr(DWORD64 baseAddr,V_IMAGEHLP_MODULE64&	img_mod_64,int& system_error)
		{

			zero_mem<V_IMAGEHLP_MODULE64>(&img_mod_64);

			// reserve enough memory, so the bug in v6.3.5.1 does not lead to memory-overwrites...
			auto_buff_c<char> big_enough(sizeof(V_IMAGEHLP_MODULE64)*4);
			if (big_enough.is_null()){
				system_error=ERROR_NOT_ENOUGH_MEMORY;
				return false;
			}
			big_enough.zero_memory();
			
			V_IMAGEHLP_MODULE64*	v_im64_ptr=(V_IMAGEHLP_MODULE64*)big_enough.ptr();
			v_im64_ptr->ver=imagehlp_module64_v2;
			v_im64_ptr->data.v2.SizeOfStruct=sizeof(v_im64_ptr->data.v2);

			if(dbg_helper_api_.MrdSymGetModuleInfo64(process_,baseAddr,v_im64_ptr)){

				memcpy(&(img_mod_64.data.v2), big_enough.ptr(), sizeof(img_mod_64.data.v2));
				img_mod_64.data.v2.SizeOfStruct=sizeof(img_mod_64.data.v2);
				img_mod_64.ver=imagehlp_module64_v2;
				return true;

			}else{
				retrieve_sys_error(system_error);
				return false;
			}
		}
		bool		get_source_symbol_at_addr( STACKFRAME64& stack_frame,CallstackEntry&	cs_entry,int& system_error)
		{
			IMAGEHLP_LINE64 src_line;
			zero_mem<IMAGEHLP_LINE64>(&src_line);
			src_line.SizeOfStruct=sizeof(IMAGEHLP_LINE64);

			// NT5.0-method :SymGetLineFromAddr64
			if(dbg_helper_api_.MrdSymGetLineFromAddr64(process_,stack_frame.AddrPC.Offset, &(cs_entry.offsetFromLine), &src_line) ){
				cs_entry.lineNumber	= src_line.LineNumber;
				_sntprintf_s(cs_entry.lineFileName,(sizeof(cs_entry.lineFileName)/sizeof(cs_entry.lineFileName[0])),_T("%s"),src_line.FileName);
				return true;
			}else{
				retrieve_sys_error(system_error);
				return false;
			}
		}
		bool		pull_stack_entry(size_t	index,STACKFRAME64& stack_frame,int& system_error)
		{
			V_IMAGEHLP_MODULE64	img_mod_64;
			CallstackEntry	cs_entry;
			
			cs_entry.offset = stack_frame.AddrPC.Offset;
			if (stack_frame.AddrPC.Offset == stack_frame.AddrReturn.Offset){
				//OnDbgHelpErr("StackWalk64-Endless-Callstack!", 0, s.AddrPC.Offset);
				return false;
			}

			if (stack_frame.AddrPC.Offset != 0)
			{
				// we seem to have a valid PC
				bool good=get_call_symbol_at_addr(stack_frame,cs_entry,system_error);
				good=get_source_symbol_at_addr(stack_frame,cs_entry,system_error);
				good=get_symbol_module_at_addr(stack_frame.AddrPC.Offset,img_mod_64,system_error);
				if(good){
					fill_symbol_module_info(img_mod_64,cs_entry);
					bool skip=false;
					on_get_callstack(index,&cs_entry,&skip);
					return !skip;
				}else{
					return false;
				}

			}

			return true;
		}

		bool		retrieve_stack_frames(HANDLE thread_handle,CONTEXT& ctx,int& system_error)
		{
			// init STACKFRAME for first call
			STACKFRAME64 stack_frame; // in/out stackframe
			zero_mem<STACKFRAME64>(&stack_frame);

			DWORD image_type;
			setup_stack_frames(ctx,stack_frame,image_type);

			size_t	index=0;
			for (;;++index){
				// get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
				// if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
				// assume that either you are done, or that the stack is so hosed that the next
				// deeper frame could not be found.
				// CONTEXT need not to be suplied if imageTyp is IMAGE_FILE_MACHINE_I386!
				BOOL good=dbg_helper_api_.MrdStackWalk64(
					image_type,
					process_,
					thread_handle,
					&stack_frame,
					&ctx,
					ReadProcessMemory64,
					dbg_helper_api_.func_tab().SymFunctionTableAccess64,
					dbg_helper_api_.func_tab().SymGetModuleBase64,
					NULL
					);

				
				if(!good){
					//If the StackWalk64 fails, the return value is FALSE. Note that StackWalk64 generally does not set the last error code
					return false;
				}
				if(!pull_stack_entry(index,stack_frame,system_error))
					break;
				
			}
			return true;
		}
		bool		get_thread_ctx(HANDLE thread_handle,CONTEXT& ctx,ThreadPauser thread_pauser,int& system_error)
		{
			static int kContextFlags = CONTEXT_FULL;
			if (thread_handle == GetCurrentThread()){
				//get_current_thread_context(&ctx, kContextFlags);
				get_current_thread_context_nt(&ctx, kContextFlags);
				return true;
			}else{
				thread_pauser.pause();
				memset(&ctx, 0, sizeof(CONTEXT));
				ctx.ContextFlags = kContextFlags;
				if (::GetThreadContext(thread_handle, &ctx) == FALSE){
					retrieve_sys_error(system_error);
					thread_pauser.resume();
					return false;
				}
				return true;
			}
		}
		bool		make_sym_search_path(int& system_error)
		{
			//dbg_helper_api_.func_tab().SymGetSearchPath
			return true;
		}
		bool		init_sym_env(int& system_error)
		{
			//DBGHELP_TRANSLATE_TCHAR

			bool good=true;
			good=init_sym_funcs(system_error);
			if(!good) return good;
			const TCHAR* search_path_list=(_tcslen(sym_search_path_.ptr()) == 0 )? NULL:sym_search_path_.ptr();
			// SymInitialize will enumerates the loaded modules for the process and effectively calls the SymLoadModule64 function for each module 
			static BOOL	auto_laod_modules=TRUE;
			if(FALSE == dbg_helper_api_.MrdSymInitialize(process_,search_path_list,auto_laod_modules)){
				retrieve_sys_error(system_error);
				return false;
			}
			need_clear_=true;

			DWORD symOptions=dbg_helper_api_.MrdSymGetOptions();
			symOptions |= SYMOPT_LOAD_LINES;
			symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
			//symOptions |= SYMOPT_NO_PROMPTS; 
			//symOptions |= SYMOPT_UNDNAME;
			symOptions=dbg_helper_api_.MrdSymSetOptions(symOptions);
			return true;
		}
		//
		bool		init_sym_funcs(int& system_error)
		{
			dbg_helper_api_.load();
			if(!dbg_helper_api_.all_func_good()){
				system_error=ERROR_INVALID_FUNCTION;
				return false;
			}
			return true;
		}
	private:

		void	on_get_callstack(size_t index,const CallstackEntry* cs,bool* skip)
		{
			if(NULL == stack_callback_){
				CallstackConsolPrinter::print(index,cs,skip);
			}else{
				stack_callback_(index,cs,skip);
			}
		}

		// for StackWalk64 call
		static BOOL __stdcall ReadProcessMemory64(
			HANDLE      process,
			DWORD64     base_address,
			PVOID       buff_ptr,
			DWORD       bytes_want_read,
			LPDWORD     actually_read
			)
		{
			SIZE_T want_read=(SIZE_T)bytes_want_read;
			SIZE_T have_read=0;
			BOOL good = ::ReadProcessMemory(process, (LPCVOID) base_address, buff_ptr, want_read, &have_read);
			*actually_read=(DWORD)(have_read);
			return good;
			/*
			if(NULL == read_mem_callback_ ){
				SIZE_T want_read=(SIZE_T)bytes_want_read;
				SIZE_T have_read=0;
				BOOL good = ::ReadProcessMemory(process, (LPCVOID) base_address, buff_ptr, want_read, &have_read);
				actually_read=(DWORD)(have_read);
				//printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, (DWORD) st, (DWORD) bRet);
				return good;
			}else{
				return read_mem_callback_(process,base_address,buff_ptr,bytes_want_read,actually_read);
			}
			*/
		}
	private:
		HANDLE					process_;
		DWORD					process_id_;
		ReadMemoryCallback		read_mem_callback_;
		StackCallback			stack_callback_;
		DbgHelperAPI			dbg_helper_api_;
		auto_buff_c<TCHAR>		sym_search_path_;
		bool					need_clear_;
	};
}}
#endif