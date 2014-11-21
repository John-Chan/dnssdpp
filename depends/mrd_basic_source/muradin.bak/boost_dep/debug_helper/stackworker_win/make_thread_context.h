#ifndef MURADIN_DBG_HELPER_MAKE_THREAD_CONTEXT_H__
#define MURADIN_DBG_HELPER_MAKE_THREAD_CONTEXT_H__

#include <Windows.h>

// Set contextFlags to CONTEXT_FULL is good enough
// you can find those macro in winnt.h
#if defined(_M_IX86)
	#ifdef CURRENT_THREAD_VIA_EXCEPTION

	// dont use this function,because the callstack is only valid in the "__except" block
	inline	void	get_current_thread_context(CONTEXT* ctx_ptr,int contextFlags)
	{
		memset(ctx_ptr,0,sizeof CONTEXT);
		EXCEPTION_POINTERS *pExp = NULL;
		__try { 
			throw 0; 
		} __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} 
		if (pExp != NULL) {
			memcpy(ctx_ptr, pExp->ContextRecord, sizeof(CONTEXT)); 
			ctx_ptr->ContextFlags = contextFlags;
		}
	}
	#else
	
	inline	void	get_current_thread_context(CONTEXT* ctx_ptr,int contextFlags)
	{
		CONTEXT	ctx;
		memset(&ctx, 0, sizeof(CONTEXT)); 
		ctx.ContextFlags = contextFlags; 
		__asm    call x ;
		__asm x: pop eax ;
		__asm    mov (ctx.Eip), eax ;
		__asm    mov (ctx.Ebp), ebp ;
		__asm    mov (ctx.Esp), esp ;

		memcpy(ctx_ptr,&ctx,sizeof CONTEXT);// for C4537
	}
	#endif

#else
// The following is defined for x86 (XP and higher), x64 and IA64:
inline	void	get_current_thread_context(CONTEXT* ctx_ptr,int contextFlags)
{
	memset(ctx_ptr, 0, sizeof(CONTEXT)); 
	ctx_ptr->ContextFlags = contextFlags;
	::RtlCaptureContext(ctx_ptr); 
}

#endif

static	void	get_current_thread_context_nt(CONTEXT* ctx_ptr,int contextFlags)
{
	memset(ctx_ptr, 0, sizeof(CONTEXT)); 
	ctx_ptr->ContextFlags = contextFlags;
	// RtlCaptureContext works on XP and later
	::RtlCaptureContext(ctx_ptr); 
}

#endif

