#ifndef	INTEGER_DEFINE_H__
#define INTEGER_DEFINE_H__

#include "predef.h"

#if (defined(PREDEF_COMPILER_GCC))

#include <stdint.h>
#include <inttypes.h>

#else

#include "inttypes.h"
#include "stdint.h"

#endif

#endif// INTEGER_DEFINE_H__
