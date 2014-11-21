#include <muradin/boost_dep/misc/timestamp.h>
#include <muradin/base/muradin_utility.hpp>

#include <stdio.h>


#if (defined(PREDEF_COMPILER_VISUALC)) 
#define snprintf	_snprintf
#else

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#endif // _ENV_MSVCPP


MRD_STATIC_ASSERT( sizeof(muradin::base::timestamp) == sizeof(int64_t) );

namespace muradin
{
namespace base
{


timestamp::timestamp(int64_t microseconds)
  : microSecondsSinceEpoch_(microseconds)
{
}

std::string timestamp::to_string() const
{
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;

#if (defined(PREDEF_COMPILER_VISUALC)) 
  snprintf(buf, sizeof(buf)-1, "%I64d.%06I64d", seconds, microseconds);
#else
  BOOST_STATIC_ASSERT(sizeof(long long int) == sizeof(int64_t));
  snprintf(buf, sizeof(buf)-1, "%lld.%06lld", (long long int)seconds, (long long int)microseconds);
#endif // _ENV_MSVCPP

  return std::string(buf);
}

std::string timestamp::to_formatted_string() const
{
	char buf[32] = {0};
	struct tm tm_time;
#if (defined(PREDEF_COMPILER_VISUALC)) 

	__time64_t seconds = static_cast<__time64_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);

	_gmtime64_s(&tm_time,&seconds);
#else
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
  gmtime_r(&seconds, &tm_time);
#endif // _ENV_MSVCPP

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);

  return std::string(buf);
}

std::string timestamp::to_formatted_string1() const
{
	char buf[32] = {0};
	struct tm tm_time;
#if (defined(PREDEF_COMPILER_VISUALC)) 

	__time64_t seconds = static_cast<__time64_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);

	_gmtime64_s(&tm_time,&seconds);
#else
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
	gmtime_r(&seconds, &tm_time);
#endif // _ENV_MSVCPP

	snprintf(buf, sizeof(buf), "%4d%02d%02d-%02d%02d%02d-%06d",
		tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
		tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
		microseconds);

	return std::string(buf);
}

timestamp timestamp::now()
{
#if (defined(PREDEF_COMPILER_VISUALC)) 
	int64_t seconds = 0;
	_time64( &seconds );
    
	return timestamp(seconds*kMicroSecondsPerSecond);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t seconds = tv.tv_sec;
	return timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
#endif // _ENV_MSVCPP
}

timestamp timestamp::invalid()
{
  return timestamp();
}


}//base
}//muradin
