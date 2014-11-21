#ifndef MURADIN_BASE_STATIC_ASSERT_H__
#define MURADIN_BASE_STATIC_ASSERT_H__

// compile-time check ,taken form boost::static_assert

// The MRD_STATIC_ASSERT macro can be used to verify that a compile-time
// expression is true. For example, you could use it to verify the
// size of a integer-type:
//
// MRD_STATIC_ASSERT(sizeof(int) == 4);
//


#define MRD_JOIN( X, Y ) MRD_DO_JOIN( X, Y )
#define MRD_DO_JOIN( X, Y ) MRD_DO_JOIN2(X,Y)
#define MRD_DO_JOIN2( X, Y ) X##Y

//
// If the compiler issues warnings about old C style casts,
// then enable this:
//
#if defined(__GNUC__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4)))
#  define MRD_STATIC_ASSERT_BOOL_CAST( x ) ((x) == 0 ? false : true)
#else
#  define MRD_STATIC_ASSERT_BOOL_CAST(x) (bool)(x)
#endif


namespace muradin{
	// HP aCC cannot deal with missing names for template value parameters
	template <bool x> struct STATIC_ASSERTION_FAILURE;

	template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };

	// HP aCC cannot deal with missing names for template value parameters
	template<int x> struct static_assert_test{};
}


#define MRD_STATIC_ASSERT( B ) \
	typedef ::muradin::static_assert_test<\
	sizeof(::muradin::STATIC_ASSERTION_FAILURE< MRD_STATIC_ASSERT_BOOL_CAST( B ) >)>\
	MRD_JOIN(muradin_static_assert_typedef_, __LINE__)


#endif