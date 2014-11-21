#include <muradin/net/endian.h>
#include <muradin/config/predef.h>





namespace muradin{
namespace net{

	uint16_t	uint16_hton(uint16_t val)
	{
		return be16toh(val);
	}

	uint16_t	uint16_ntoh(uint16_t val)
	{
		return be16toh(val);
	}

	uint32_t	uint32_hton(uint32_t val)
	{
		return be32toh(val);
	}

	uint32_t	uint32_ntoh(uint32_t val)
	{
		return be32toh(val);
	}

	uint64_t	uint64_hton(uint32_t val)
	{
		return be64toh(val);
	}

	uint64_t	uint64_ntoh(uint32_t val)
	{
		return be64toh(val);
	}
}//net
}//muradin
