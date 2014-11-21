///
/// this file taken from muduo::timestamp
///
#ifndef MURADIN_BASE_TIMESTAMP_H__
#define MURADIN_BASE_TIMESTAMP_H__

#include <muradin/config/predef.h>
#include <muradin/config/integer_define.h>
#include <string>
#include <boost/operators.hpp>

#if (defined(PREDEF_COMPILER_VISUALC)) 
#include <time.h>
#else
#include <sys/time.h>
#endif

namespace muradin
{
namespace base
{


///
/// Time stamp in UTC, in microseconds resolution.
///
/// This class is immutable.
/// It's recommended to pass it by value, since it's passed in register on x64.
///
class timestamp :public boost::less_than_comparable<timestamp>
{
public:

	///
	/// Constucts an invalid timestamp.
	///
	timestamp()
	: microSecondsSinceEpoch_(0)
	{
	}

	/// Constucts a timestamp at specific time
	explicit timestamp(int64_t microSecondsSinceEpoch);

	void swap(timestamp& that)
	{
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
	}

    /// got integer string
	std::string to_string() const;
    /// got yyyymmdd hh:nn:ss.ms
	std::string to_formatted_string() const;
    /// got yyyymmdd-hhnnss-ms
	std::string to_formatted_string1() const;

	bool valid() const { return microSecondsSinceEpoch_ > 0; }

	// for internal usage.
	int64_t micro_seconds_since_epoch() const { return microSecondsSinceEpoch_; }
	time_t seconds_since_epoch() const
	{ return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }

	///
	/// Get time of now.
	///
	static timestamp now();
	static timestamp invalid();

	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	int64_t microSecondsSinceEpoch_;
};

inline bool operator<(timestamp lhs, timestamp rhs)
{
  return lhs.micro_seconds_since_epoch() < rhs.micro_seconds_since_epoch();
}

inline bool operator==(timestamp lhs, timestamp rhs)
{
  return lhs.micro_seconds_since_epoch() == rhs.micro_seconds_since_epoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microseciond
/// resolution for next 100 years.
inline double time_difference(timestamp high, timestamp low)
{
  int64_t diff = high.micro_seconds_since_epoch() - low.micro_seconds_since_epoch();
  return static_cast<double>(diff) / timestamp::kMicroSecondsPerSecond;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as timestamp
///
inline timestamp add_time(timestamp tmstamp, double seconds)
{
  int64_t delta = static_cast<int64_t>(seconds * timestamp::kMicroSecondsPerSecond);
  return timestamp(tmstamp.micro_seconds_since_epoch() + delta);
}

}//namespace base
}//muradin
#endif // MURADIN_BASE_TIMESTAMP_H__
