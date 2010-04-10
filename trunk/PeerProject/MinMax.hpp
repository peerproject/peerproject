//
// MinMax.hpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2005-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

//! \file	MinMax.hpp
//! \brief	Defines min und max template funtions.

#pragma once

#undef min
#undef max

//#if defined(_MSC_VER) && (_MSC_FULL_VER > 150030000)	// VS2008 SP1 for tr1
//  #include <type_traits>
//#else	// Boost fallback
//  #include <Boost/tr1/type_traits.hpp>
//#endif

#include <Boost/type_traits.hpp>	// ToDo: Use tr1 above
#include <Boost/mpl/apply_wrap.hpp>
#include <Boost/mpl/arg.hpp>
#include <Boost/mpl/if.hpp>

const bool PEERPROJECT_RESTRICT_WP64 = true;
const bool PEERPROJECT_ADVANCED_MIN_TEMPLATE = true;
// Allow min to return the smaller type if called with unsigned arguments ?

namespace min_max_detail
{
	//! \brief	Helper template to remove __w64 qualifier from a type
	template<typename T> struct RemoveWarning64 { typedef T type; };
	template<> struct RemoveWarning64< int >
	{
		typedef int type;
	};
	template<> struct RemoveWarning64< const int >
	{
		typedef const int type;
	};
	template<> struct RemoveWarning64< volatile int >
	{
		typedef volatile int type;
	};
	template<> struct RemoveWarning64< const volatile int >
	{
		typedef const volatile int type;
	};
	template<> struct RemoveWarning64< long >
	{
		typedef long type;
	};
	template<> struct RemoveWarning64< const long >
	{
		typedef const long type;
	};
	template<> struct RemoveWarning64< volatile long >
	{
		typedef volatile long type;
	};
	template<> struct RemoveWarning64< const volatile long >
	{
		typedef const volatile long type;
	};

	//! \brief	Helper template applies all cv qualifiers of two given types to the first type.
	//! 	// tr1 fix:  Use std::tr1 for all boost below except mpl (fall back to Boost/tr1)
	template<typename T, typename U>
	struct UniteCvQualifiers
	{
		typedef typename boost::mpl::apply_wrap4< boost::mpl::arg<
			boost::is_volatile< U >::value
				? boost::is_const< U >::value
					? 4
					: 3
				: boost::is_const< U >::value
					? 2
					: 1
			>, typename RemoveWarning64< T >::type,
			typename RemoveWarning64< const T >::type,
			typename RemoveWarning64< volatile T >::type,
			typename RemoveWarning64< const volatile T >::type >::type type;
	};

	//! \brief	Computes the correct result type (if any) for min and max
	template<typename T, typename U, bool opt = false>
	struct MinMaxResult
	{
	private:
		typedef typename boost::remove_const< T >::type T_;
		typedef typename boost::remove_const< U >::type U_;
		struct VolatilesTypesAreNotAllowedForMinMaxFunctions {};
		struct CannotIntermixSignedAndUnsignedTypesInForMinMax {};
		struct ReturnTypeForMinMaxCannotBeDeduced {};
		static T_ makeT();
		static U_ makeU();

		template<bool>
		struct Result
		{
			char dummy;
		};

		template<>
		struct Result< true >
		{
			char dummy[ 2 ];
		};

		template<typename X>
		static Result< boost::is_same< X, T_ >::value >
			testT(X);

		template<typename X>
		static Result< boost::is_same< X, U_ >::value >
			testU(X);

		static const bool conditionalIsT =
			sizeof( testT( true ? makeT() : makeU() ).dummy ) > sizeof( char );
		static const bool conditionalIsU =
			sizeof( testU( true ? makeT() : makeU() ).dummy ) > sizeof( char );
	public:
		typedef typename boost::mpl::apply_wrap5< boost::mpl::arg<
			boost::is_volatile< T >::value || boost::is_volatile< U >::value
				? 3
				:
			boost::is_same< T_, U_ >::value
				? 1
				:
			boost::is_integral< T_ >::value && boost::is_integral< U_ >::value
				? boost::is_same< T_, char >::value
											|| boost::is_same< U_, char >::value
					? 4
					:
				std::numeric_limits< T_ >::is_signed
										== std::numeric_limits< U_ >::is_signed
					? conditionalIsT
						? std::numeric_limits< T_ >::is_signed || !opt
							? 1
							: 2
						:
					conditionalIsU
						? std::numeric_limits< T_ >::is_signed || !opt
							? 2
							: 1
						: 5
					: 4
				:
			boost::is_float< T_ >::value && boost::is_float< U_ >::value
					|| boost::is_pointer< T_ >::value
							&& boost::is_pointer< U_ >::value
				? conditionalIsT
					? 1
					:
				conditionalIsU
					? 2
					: 5
				: 5
			>, T_, U_,
			VolatilesTypesAreNotAllowedForMinMaxFunctions,
			CannotIntermixSignedAndUnsignedTypesInForMinMax,
			ReturnTypeForMinMaxCannotBeDeduced
		>::type primary;
		typedef typename boost::mpl::if_c<
			( ! PEERPROJECT_RESTRICT_WP64 && sizeof( primary ) < 8 ),
			primary, typename RemoveWarning64< primary >::type >::type type;
	};

	template<typename T, typename U, bool opt = false>
	struct MinMaxResultRef
	{
	private:
		typedef typename boost::remove_const< T >::type T_;
		typedef typename boost::remove_const< U >::type U_;
	public:
		typedef typename boost::mpl::apply_wrap3< boost::mpl::arg<
			boost::is_volatile< T >::value || boost::is_volatile< U >::value
				? 1
				: boost::is_same< T_, U_ >::value
					? 2
					: boost::is_base_and_derived< T_, U_ >::value
						? 2
						: boost::is_base_and_derived< U_, T_ >::value
							? 3
							: 1
				>,
				typename MinMaxResult< T, U, opt >::type,
				typename UniteCvQualifiers< T, U >::type&,
				typename UniteCvQualifiers< U, T >::type&
			>::type type;
	};
} // namespace min_max_detail

template<typename T, typename U>
typename min_max_detail
	::MinMaxResultRef< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(T& lhs, U& rhs)
{
	typedef min_max_detail
		::MinMaxResultRef< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( rhs < lhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail
	::MinMaxResultRef< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(T& lhs, U& rhs, const Pred& f)
{
	typedef min_max_detail
		::MinMaxResultRef< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( f( rhs, lhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U>
typename min_max_detail
	::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(const T& lhs, U& rhs)
{
	typedef min_max_detail
		::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( rhs < lhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail
	::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(const T& lhs, U& rhs, const Pred& f)
{
	typedef min_max_detail
		::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( f( rhs, lhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U>
typename min_max_detail
	::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(T& lhs, const U& rhs)
{
	typedef min_max_detail
		::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( rhs < lhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail
	::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(T& lhs, const U& rhs, const Pred& f)
{
	typedef min_max_detail
		::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( f( rhs, lhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U>
typename min_max_detail
	::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(const T& lhs, const U& rhs)
{
	typedef min_max_detail
		::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( rhs < lhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail
	::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type
inline min(const T& lhs, const U& rhs, const Pred& f)
{
	typedef min_max_detail
		::MinMaxResult< T, U, PEERPROJECT_ADVANCED_MIN_TEMPLATE >::type Result;
	if ( f( rhs, lhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U>
typename min_max_detail::MinMaxResultRef< T, U >::type
inline max(T& lhs, U& rhs)
{
	typedef min_max_detail::MinMaxResultRef< T, U >::type Result;
	if ( lhs < rhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail::MinMaxResultRef< T, U >::type
inline max(T& lhs, U& rhs, const Pred& f)
{
	typedef min_max_detail::MinMaxResultRef< T, U >::type Result;
	if ( f( lhs, rhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U>
typename min_max_detail::MinMaxResult< T, U >::type
inline max(const T& lhs, U& rhs)
{
	typedef min_max_detail::MinMaxResult< T, U >::type Result;
	if ( lhs < rhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail::MinMaxResult< T, U >::type
inline max(const T& lhs, U& rhs, const Pred& f)
{
	typedef min_max_detail::MinMaxResult< T, U >::type Result;
	if ( f( lhs, rhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U>
typename min_max_detail::MinMaxResult< T, U >::type
inline max(T& lhs, const U& rhs)
{
	typedef min_max_detail::MinMaxResult< T, U >::type Result;
	if ( lhs < rhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail::MinMaxResult< T, U >::type
inline max(T& lhs, const U& rhs, const Pred& f)
{
	typedef min_max_detail::MinMaxResult< T, U >::type Result;
	if ( f( lhs, rhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U>
typename min_max_detail::MinMaxResult< T, U >::type
inline max(const T& lhs, const U& rhs)
{
	typedef min_max_detail::MinMaxResult< T, U >::type Result;
	if ( lhs < rhs )
		return Result( rhs );
	else
		return Result( lhs );
}

template<typename T, typename U, typename Pred>
typename min_max_detail::MinMaxResult< T, U >::type
inline max(const T& lhs, const U& rhs, const Pred& f)
{
	typedef min_max_detail::MinMaxResult< T, U >::type Result;
	if ( f( lhs, rhs ) )
		return Result( rhs );
	else
		return Result( lhs );
}
