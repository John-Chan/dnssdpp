#ifndef	MURADIN_MISC_STOPWATCH_HPP__
#define MURADIN_MISC_STOPWATCH_HPP__

#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp> 

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include <stdio.h> // sprintf

namespace muradin{namespace misc{
	class stopwatch
	{
	public:
		typedef boost::chrono::steady_clock				clock_type;
		typedef clock_type::duration					dura_type;
		typedef boost::chrono::time_point<clock_type>	time_point_type;
		typedef std::pair<std::string,time_point_type>	label_and_time_point;
		typedef std::vector<label_and_time_point>		time_point_vec;

		//typedef boost::chrono::duration<int, boost::ratio<86400>> days;
		typedef boost::chrono::duration<long long, boost::ratio<86400> > days;

		typedef boost::chrono::duration<long long, boost::centi> centiseconds;
	private:
		clock_type	clock_;
		time_point_vec	check_points_;
	private:
		time_point_vec::const_iterator	find_by_label(const std::string& lable)const
		{
			time_point_vec::const_iterator found=check_points_.begin();
			while (found != check_points_.end()){
				if(found->first == lable) return found;
				++found;
			}
			return check_points_.end();
		}
	public:
		void	clear(){check_points_.clear();}
		size_t	count()const {return check_points_.size();}
		size_t	mark(const std::string& label)
		{
			time_point_type tp=clock_.now();
			if(label.length()==0){
				std::string	lb=boost::lexical_cast<std::string>(check_points_.size());
				check_points_.push_back(label_and_time_point(lb,tp));
			}else{
				check_points_.push_back(label_and_time_point(label,tp));
			}
			return check_points_.size();
		}

		const time_point_vec&	marked()const{return check_points_;}
		time_point_vec&			marked(){return check_points_;}

		// example: stopwatch.duration(0,1);
		dura_type duration(size_t checkpoint_1,size_t checkpoint_2)const
		{
			return check_points_[checkpoint_2].second - check_points_[checkpoint_1].second;
		}
		dura_type duration()const
		{
			size_t count=check_points_.size();
			return check_points_[count -1 ].second - check_points_[count -2].second;
		}

		// example: 
		// stopwatch watch;
		// watch.mark("one");
		// sleep(1000);
		// watch.mark("two");
		// std::cout << watch.duration("one","two");
		// sleep(1000);
		// watch.mark("three");
		// std::cout << watch.duration("one","three");
		dura_type duration(const std::string& checkpoint_1,const std::string& checkpoint_2)const
		{
			time_point_vec::const_iterator tp1=find_by_label(checkpoint_1);
			time_point_vec::const_iterator tp2=find_by_label(checkpoint_2);
			return tp2->second - tp1->second;
		}

		//example : stopwatch::to_nanoseconds(stopwatch.duration(0,1));
		static boost::int64_t	count_nanoseconds(const dura_type& dur)
		{
			return boost::chrono::duration_cast<boost::chrono::nanoseconds>(dur).count();
		}
		static boost::int64_t	count_microseconds(const dura_type& dur)
		{
			return boost::chrono::duration_cast<boost::chrono::microseconds>(dur).count();
		}

		static boost::int64_t	count_milliseconds(const dura_type& dur)
		{
			return boost::chrono::duration_cast<boost::chrono::milliseconds>(dur).count();
		}

		static boost::int64_t	count_seconds(const dura_type& dur)
		{
			return boost::chrono::duration_cast<boost::chrono::seconds>(dur).count();
		}

		static size_t	count_minutes(const dura_type& dur)
		{
			return boost::chrono::duration_cast<boost::chrono::minutes>(dur).count();
		}

		static size_t	count_hours(const dura_type& dur)
		{
			return boost::chrono::duration_cast<boost::chrono::hours>(dur).count();
		}
		static size_t	count_days(const dura_type& dur)
		{
			return static_cast<size_t>(boost::chrono::duration_cast<days>(dur).count()) ;
		}

		// day_numb is the number of days
		// hour_numb is the number of hours
		// min_numb is the number of minutes
		// sec_numb is the number of seconds 
		// cc_numb is  the nearest hundreth of a second
		static		void	count_dhmsc(dura_type dur,
			size_t& day_numb,
			size_t& hour_numb,
			size_t& min_numb,
			size_t& sec_numb,
			size_t& cc_numb)
		{
			if (dur < dura_type(0)){
				dur = -dur;
			}
			// round dur to nearest centiseconds, to even on tie
			centiseconds cs = boost::chrono::duration_cast<centiseconds>(dur);
			if (dur - cs > boost::chrono::milliseconds(5) || (dur - cs == boost::chrono::milliseconds(5) && cs.count() & 1))
				++cs;
			// separate seconds from centiseconds
			boost::chrono::seconds s = boost::chrono::duration_cast<boost::chrono::seconds>(cs);
			cs -= s;
			// separate minutes from seconds
			boost::chrono::minutes m = boost::chrono::duration_cast<boost::chrono::minutes>(s);
			s -= m;
			// separate hours from minutes
			boost::chrono::hours h = boost::chrono::duration_cast<boost::chrono::hours>(m);
			m -= h;
			// separate days from hours
			days dy = boost::chrono::duration_cast<days>(h);
			h -= dy;

			day_numb	=static_cast<size_t>(dy.count());
			hour_numb	=static_cast<size_t>(h.count());
			min_numb	=static_cast<size_t>(m.count());  
			sec_numb	=static_cast<size_t>(s.count());  
			cc_numb		=static_cast<size_t>(cs.count());
		}
		
		static std::string	good_fmt(const dura_type& dur)
		{
			std::string negative ;
			char buff[128];
			size_t day_numb,hour_numb,min_numb,sec_numb,cc_numb;
			count_dhmsc(dur,day_numb,hour_numb,min_numb,sec_numb,cc_numb);
			if(dur < dura_type(0)){negative="-";}
			//d/hh:mm:ss.cc
			sprintf(buff,"%s%d/%02d:%02d:%02d.%02d",negative.c_str(),day_numb,hour_numb,min_numb,sec_numb,cc_numb);
			return std::string(buff);
		}

	};
}
}
#endif//MURADIN_MISC_STOPWATCH_HPP__