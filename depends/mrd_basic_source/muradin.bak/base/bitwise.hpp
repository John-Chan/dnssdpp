#ifndef MURADIN_BASE_BITWISE_HPP__
#define MURADIN_BASE_BITWISE_HPP__

#include <limits.h> // for CHAR_BIT

namespace muradin
{
namespace base{

	template <class UnsignedT>
	static UnsignedT circular_shift_l(UnsignedT value, int shift) {
		return (value << shift) | (value >> (sizeof(value) * CHAR_BIT - shift));
	}

	template <class UnsignedT>
	static UnsignedT circular_shift_r(UnsignedT value, int shift) {
		return (value >> shift) | (value << (sizeof(value) * CHAR_BIT - shift));
	}
}
}
#endif