#ifndef BONJOUR_ENDIAN_HPP__
#define BONJOUR_ENDIAN_HPP__

#include <ddnspp/common/check_arch.hpp>

#include <boost/version.hpp>
#include <boost/cstdint.hpp>

#if BOOST_VERSION >= 105500
#include <boost/predef.h>
#endif

namespace air{namespace common {

	inline boost::uint16_t	swap_2_bytes(boost::uint16_t val)
	{
		return (val<<8 | val>>8);
	}

	inline boost::uint32_t	swap_4_bytes(boost::uint32_t val)
	{
		return (val<<24 | (val<<8)&0x00FF0000 | (val>>8)&0x0000FF00 | val>>24 );
	}

	inline boost::uint64_t	swap_8_bytes(boost::uint64_t val)
	{
		val = ((val & 0x00000000000000FFULL) << 56) | 
			((val & 0x000000000000FF00ULL) << 40) | 
			((val & 0x0000000000FF0000ULL) << 24) | 
			((val & 0x00000000FF000000ULL) <<  8) | 
			((val & 0x000000FF00000000ULL) >>  8) | 
			((val & 0x0000FF0000000000ULL) >> 24) | 
			((val & 0x00FF000000000000ULL) >> 40) | 
			((val & 0xFF00000000000000ULL) >> 56);
		return val;
	}

	inline bool		is_big_endianess()
	{
#if BOOST_VERSION >= 105500
#ifdef BOOST_ARCH_X86
		return false;		
#endif 	
#else
		boost::uint16_t n16=0x1234;
		return ((boost::uint8_t*)(&n16))[0] == 0x12 ;
#endif
	}

	inline	boost::uint16_t	as_be16(boost::uint16_t val)
	{
		if(!is_big_endianess()){ return swap_2_bytes(val);}
		return val;
	}
	inline	boost::uint16_t	as_le16(boost::uint16_t val)
	{
		if(is_big_endianess()){ return swap_2_bytes(val);}
		return val;
	}
	inline	boost::uint32_t	as_be32(boost::uint32_t val)
	{
		if(!is_big_endianess()){ return swap_4_bytes(val);}
		return val;
	}
	inline	boost::uint32_t	as_le32(boost::uint32_t val)
	{
		if(is_big_endianess()){ return swap_4_bytes(val);}
		return val;
	}

	inline	boost::uint64_t	as_be64(boost::uint64_t val)
	{
		if(!is_big_endianess()){ return swap_8_bytes(val);}
		return val;
	}
	inline	boost::uint64_t	as_le64(boost::uint64_t val)
	{
		if(is_big_endianess()){ return swap_8_bytes(val);}
		return val;
	}
}}

#endif // BONJOUR_ENDIAN_HPP__