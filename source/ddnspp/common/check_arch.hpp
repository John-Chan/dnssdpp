#ifndef SM_COMMON_CHECK_ARCH_HPP__
#define SM_COMMON_CHECK_ARCH_HPP__

#if BOOST_VERSION >= 105500
#include <boost/predef.h>

	#ifndef BOOST_ARCH_X86
		#error "this library support X86/X86_64 only"		
	#endif 
	
#endif

#endif
