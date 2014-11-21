#ifndef MURADIN_CONFIG_PROCESSOR_ARH_H__
#define MURADIN_CONFIG_PROCESSOR_ARH_H__
#include <muradin/config/predef.h>




/***************************************************************************
*
*  [processors]
*
* PREDEF_CPU_X86                 Intel i386/i486/Pentium (little endian)
* PREDEF_CPU_X86_64              AMD64(x86)/Intel64(x86) (little endian)
* PREDEF_CPU_IA_64               Itanium Processor Family 64-bit processors
* PREDEF_CPU_ALPHA               Compaq/Digital Alpha (little endian)
* PREDEF_CPU_SPARC               Sun Sparc
* PREDEF_CPU_HPPA                HP PA-RISC
* PREDEF_CPU_MIPS                MIPS
* PREDEF_CPU_PPC                 PowerPC
* PREDEF_CPU_POWER               POWER Processors.
*
***************************************************************************/



#if (defined(PREDEF_COMPILER_VISUALC))

#ifdef _M_IX86 /* i386 architecture */
#define PREDEF_CPU_X86
#endif

#if ( defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) )
#define PREDEF_CPU_X86_64
#endif

#ifdef _M_ALPHA /* Alpha architecture */
#define PREDEF_CPU_ALPHA
#endif

#elif (defined(PREDEF_COMPILER_SUNPRO))

#ifdef __sparc /* Sparc architecture - 32-bit compilation mode */
#define PREDEF_CPU_SPARC
#define PREDEF_CPU_SUN_SPARC
#define PREDEF_CPU_SUN_SPARC32
#endif
#ifdef __sparcv9 /* Sparc architecture - 64-bit compilation mode */
#define PREDEF_CPU_SPARC
#define PREDEF_CPU_SUN_SPARC
#define PREDEF_CPU_SUN_SPARC64
#endif
#ifdef __i386 /* I386 architecture */
#define PREDEF_CPU_X86
#define PREDEF_CPU_SUN_I386
#endif

#elif (defined(PREDEF_COMPILER_DECC))

#ifdef __alpha /* Alpha architecture */
#define PREDEF_CPU_ALPHA
#endif

#elif (defined(PREDEF_COMPILER_GCC))

#if (defined(__alpha) || defined(__alpha__))
#define PREDEF_CPU_ALPHA
#elif (defined(__i386__))
#define PREDEF_CPU_X86
#elif (defined(__sparc))
#define PREDEF_CPU_SUN_SPARC32
#elif (defined(__hppa__))
#define PREDEF_CPU_HPPA
#elif (defined(__mips__))
#define PREDEF_CPU_MIPS
#elif (defined(__PPC__))
#define PREDEF_CPU_PPC
#elif ( defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) )
#define PREDEF_CPU_X86_64
#else
#warning  "Unknown processor architecture!"
#endif

#elif (defined(PREDEF_COMPILER_HPCC))

#if defined(__hppa)
#define PREDEF_CPU_HPPA
#endif

#elif (defined(__mips) || defined(__mips__) || defined(__MIPS__))
#define PREDEF_CPU_MIPS

#elif (defined(PREDEF_COMPILER_XLC))
#if defined(_POWER)
#define PREDEF_CPU_POWER
#endif

#elif (defined(PREDEF_COMPILER_MPW))

#define PREDEF_CPU_PPC

#else
#warning "Unknown processor architecture!"

#endif
#endif // MURADIN_CONFIG_PROCESSOR_ARH_H__
