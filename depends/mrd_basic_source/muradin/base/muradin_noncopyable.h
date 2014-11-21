//  Private copy constructor and copy assignment ensure classes derived from
//  class noncopyable cannot be copied.

//  taken from boost::noncopyable

#ifndef MURADIN_BASE_NONCOPYABLE_H__
#define MURADIN_BASE_NONCOPYABLE_H__

namespace muradin  // protection from unintended ADL
{
	class mrd_noncopyable
	{
	protected:
		mrd_noncopyable() {}
		~mrd_noncopyable() {}
	private:  // emphasize the following members are private
		mrd_noncopyable( const mrd_noncopyable& );
		const mrd_noncopyable& operator=( const mrd_noncopyable& );
	};
}


#endif
