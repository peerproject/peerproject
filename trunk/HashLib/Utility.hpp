//
// Utility.hpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions Copyright Shareaza Development Team, 2008.
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

#pragma once

//#include <stdlib.h>

// Intrinsics  (Workaround for Microsoft double declaration in legacy Visual Studio 2005)
#if defined(_MSC_VER) && (_MSC_VER < 1500)
	#define _interlockedbittestandset _ms_set
	#define _interlockedbittestandreset _ms_reset
	#define _interlockedbittestandset64 _ms_set64
	#define _interlockedbittestandreset64 _ms_reset64
	#include <intrin.h>
	#undef _interlockedbittestandset
	#undef _interlockedbittestandreset
	#undef _interlockedbittestandset64
	#undef _interlockedbittestandreset64
#else
	#include <intrin.h>
#endif

// Alternate Boost platform independent integer types:
// #include <boost/cstdint.hpp>
// typedef boost::int8_t int8;
// typedef boost::int16_t int16;
// typedef boost::int32_t int32;
// typedef boost::int64_t int64;
// typedef boost::uint8_t uint8;
// typedef boost::uint16_t uint16;
// typedef boost::uint32_t uint32;
// typedef boost::uint64_t uint64;

// Signed/unsigned integer types:
typedef char int8;
typedef short int16;
typedef long int32;
typedef __int64 int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned __int64 uint64;

// Alias for signed/unsigned char.
typedef unsigned char uchar;
typedef signed char schar;
typedef wchar_t wchar;
typedef uint16 uwchar;		// Integer type same size as a wchar_t

//! \brief enumeration to specify the byte ordering of a sequence.
enum Endianess
{
	//! \brief specifies little endian order, the least significant byte comes first.
	littleEndian,
	//! \brief specifies big endian order, the most significant byte comes first.
	bigEndian
};

//! \brief This namespace holds machine dependent definitions for the target machine.
namespace Machine
{
	//! \brief Specifies the natural byte ordering of the target machine.
	//! \todo detect endianess during compilation.
	const Endianess endianess = littleEndian; // x86

	inline bool SupportsMMX()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 1 );
		return ( CPUInfo[ 3 ] & 0x00800000 ) != 0;
	}

	inline bool SupportsSSE()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 1 );
		return ( CPUInfo[ 3 ] & 0x02000000 ) != 0;
	}

	inline bool SupportsSSE2()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 1 );
		return ( CPUInfo[ 3 ] & 0x04000000 ) != 0;
	}

	inline bool SupportsSSE3()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 1 );
		return ( CPUInfo[ 2 ] & 0x00000001 ) != 0;
	}

	inline bool SupportsSSSE3()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 1 );
		return ( CPUInfo[ 2 ] & 0x00000200 ) != 0;
	}

	inline bool SupportsSSE41()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 1 );
		return ( CPUInfo[ 2 ] & 0x00080000 ) != 0;
	}

	inline bool SupportsSSE42()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 1 );
		return ( CPUInfo[ 2 ] & 0x00100000 ) != 0;
	}

	inline bool SupportsSSE4A()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 0x80000000 );
		if ( CPUInfo[ 0 ] >= 0x80000001 )
		{
			__cpuid( CPUInfo, 0x80000001 );
			return ( CPUInfo[ 2 ] & 0x00000040 ) != 0;
		}
		return false;
	}

	inline bool SupportsSSE5()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 0x80000000 );
		if ( CPUInfo[ 0 ] >= 0x80000001 )
		{
			__cpuid( CPUInfo, 0x80000001 );
			return ( CPUInfo[ 2 ] & 0x00000800 ) != 0;
		}
		return false;
	}

	inline bool Supports3DNOW()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 0x80000000 );
		if ( CPUInfo[ 0 ] >= 0x80000001 )
		{
			__cpuid( CPUInfo, 0x80000001 );
			return ( CPUInfo[ 3 ] & 0x80000000 ) != 0;
		}
		return false;
	}

	inline bool Supports3DNOWEXT()
	{
		int CPUInfo[ 4 ] = {};
		__cpuid( CPUInfo, 0x80000000 );
		if ( CPUInfo[ 0 ] >= 0x80000001 )
		{
			__cpuid( CPUInfo, 0x80000001 );
			return ( CPUInfo[ 3 ] & 0x40000000 ) != 0;
		}
		return false;
	}
}

//! \brief Generic function to swap the byte ordering of a given type
//!
//! The byte ordering can be swapped meaningfully only for unsigned integer types
//! therefore specializations are provided only for those types.
//! We use template specialization in order to avoid automatic argument conversion.
template<typename T>
struct SwapEndianess {};

template<> struct SwapEndianess< uint8 >
{
	uint8 operator()(uint8 value) const { return value; }
};
template<> struct SwapEndianess< uint16 >
{
	uint16 operator()(uint16 value) const
	{
		return _byteswap_ushort( value );
	}
};

template<> struct SwapEndianess< uint32 >
{
	uint32 operator()(uint32 value) const
	{
		return _byteswap_ulong( value );
	}
};

template<> struct SwapEndianess< uint64 >
{
	uint64 operator()(uint64 value) const
	{
		return _byteswap_uint64( value );
	}
};

template<typename T>
inline T swapEndianess(T value)
{
	return SwapEndianess< T >()( value );
}

//! \brief Generic function object to give its char serialization a given specified byte ordering.
//!
//! The byte ordering of the argument is swapped unless
//! it matches the byte ordering of the target machine.
//! We use partial specialization to achieve this.
template<typename T, Endianess endianPolicy> struct TransformTo
{
	T operator()(T value) const { return swapEndianess< T >( value ); }
};
template<typename T > struct TransformTo< T, Machine::endianess >
{
	T operator()(T value) const { return value; }
};

//! \brief Generic function object to reconstruct a value out of its serialized form with a specified byte ordering.
//!
//! This function objects behaves the same as TransformTo does but its purpose
//! is different. Having both functions allows to make that purpose explicit in code.
template<typename T, Endianess endianPolicy> struct TransformFrom
{
	T operator()(T value) const {	return TransformTo< T, endianPolicy >()( value ); }
};

//! \brief Generic function to bring a given value into little endian order.
template<typename T> inline T transformToLE(T value)
{
	return TransformTo< T, littleEndian >()( value );
}

//! \brief Generic function to bring a given value into big endian order.
template<typename T> inline T transformToBE(T value)
{
	return TransformTo< T, bigEndian >()( value );
}

//! \brief Generic function to reconstruct a given value from little endian order.
template<typename T> inline T transformFromLE(T value)
{
	return TransformFrom< T, littleEndian >()( value );
}

//! \brief Generic function to reconstruct a given value from big endian order.
template<typename T> inline T transformFromBE(T value)
{
	return TransformFrom< T, bigEndian >()( value );
}

template<typename T, T v> struct StaticSwapEndianess;
template<uint8 v> struct StaticSwapEndianess< uint8, v >
{
	static const uint8 value = v;
};
template<uint16 v> struct StaticSwapEndianess< uint16, v >
{
	static const uint16 value = ( v << 8 ) | ( v >> 8 );
};
template<uint32 v> struct StaticSwapEndianess< uint32, v >
{
	static const uint32 value = ( v << 24 ) | ( ( v & 0xff00 ) << 8 )
			| ( ( v & 0xff0000 ) >> 8 ) | ( v >> 24 );
};
template<uint64 v> struct StaticSwapEndianess< uint64, v >
{
	static const uint64 value
			= StaticSwapEndianess< uint32, ( v >> 32 ) >::value
			| ( uint64( StaticSwapEndianess< uint32, v >::value ) << 32 );
};

template<typename T, T v, Endianess> struct StaticTransformTo
{
	static const T value = StaticSwapEndianess< T, v >::value;
};
template<typename T, T v> struct StaticTransformTo< T, v, Machine::endianess >
{
	static const T value = v;
};

//! \brief for_each with predicate.
//!
//! A generalization of the for_each algorithm that takes a predicate that
//! must be fulfilled in order to apply the given function.  This function
//! may mutate the input sequence, provided no iterators become invalid.
template<class InputIterator, class Predicate, class Function >
inline void for_each_if(InputIterator first, InputIterator last,
		Predicate pred, Function f)
{
	for ( ; first != last; ++first )
	{
		if ( pred( *first ) )
			f( *first )
	}
}

//! Helper function to rotate the bits of a given unsigned value.
template<typename T> inline T rotateLeft(T value, uint8 shift);
template<> inline uint8 rotateLeft(uint8 value, uint8 shift)
{
	return uint8( value << shift | value >> ( 8 - shift ) );
}
template<> inline uint16 rotateLeft(uint16 value, uint8 shift)
{
	return uint16( value << shift | value >> ( 16 - shift ) );
}
template<> inline uint32 rotateLeft(uint32 value, uint8 shift)
{
	return uint32( value << shift | value >> ( 32 - shift ) );
}
template<> inline uint64 rotateLeft(uint64 value, uint8 shift)
{
	return uint64( value << shift | value >> ( 64 - shift ) );
}

// Obsolete: Determine the highest/lowest bit set in the argument.
// Bits are counted from the lowest bit starting with the index zero.
//
//inline uint32 highestBitSet(uint32 value)
//{
//	uint32 index;
//	return _BitScanReverse( &index, value ) ? index : 0;
//}
//
//inline uint32 highestBitSet(uint64 value)
//{
//#ifdef _WIN64
//	uint32 index;
//	return _BitScanReverse64( &index, value ) ? index : 0;
//#else
//	return uint32( value ) != 0
//		? highestBitSet( uint32( value ) )
//		: highestBitSet( uint32( value >> 32 ) ) + 32;
//#endif
//}
//
//inline uint32 lowestBitSet(uint32 value)
//{
//	uint32 index;
//	return _BitScanForward( &index, value ) ? index : 0;
//}
//
//inline uint32 lowestBitSet(uint64 value)
//{
//#ifdef _WIN64
//	uint32 index;
//	return _BitScanForward64( &index, value ) ? index : 0;
//#else
//	return uint32( value ) != 0
//		? highestBitSet( uint32( value ) )
//		: highestBitSet( uint32( value >> 32 ) ) + 32;
//#endif
//}
