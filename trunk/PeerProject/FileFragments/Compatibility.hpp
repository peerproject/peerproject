//
// FileFragments/Compatibility.hpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2007.
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


namespace Fragments
{

// move to a common (private) base/mixin for block transfer (BT/ED2K)
// selects an available block, either unaligned blocks or if none is available
// a random aligned block
template< class list_type, typename available_type >
typename list_type::range_type selectBlock(const list_type& src, 
	typename list_type::range_size_type block_size, const available_type* available)
{
	typedef typename list_type::range_type range_type;
	typedef typename list_type::range_size_type range_size_type;
	typedef typename list_type::const_iterator const_iterator;

	if ( src.empty() )
		return range_type( 0, 0 );

	std::deque< range_size_type > blocks;
	//range_size_type range_size = 0;
	//range_size_type range_begin = 0;
	//range_size_type range_block = 0;
	//range_size_type range_total = 0;
	//range_size_type best_range_size = 0;
	//range_size_type best_range_begin = 0;
	//range_size_type best_range_total = 0;

	for ( const_iterator select = src.begin(); select != src.end(); ++select )
	{
		range_size_type block_begin = select->begin() / block_size;
		if ( select->begin() % block_size )
		{
			// the start of a block is complete, but part is missing
			++block_begin;
			if ( !available || available[ block_begin ] )
			{
				return range_type( select->begin(),
					min( select->end(), block_size * block_begin ) );
			}
		}
		range_size_type block_end = ( select->end() - 1 ) / block_size + 1;
		if ( block_begin != block_end && select->end() % block_size )
		{
			// the end of a block is complete, but part is missing
			--block_end;
			if ( !available || available[ block_end ] )
				return range_type( block_end * block_size, select->end() );
		}
		// this fragment contains one or more aligned empty blocks
		for ( ; block_begin != block_end; ++block_begin )
		{
			if ( !available || available[ block_begin ] )
				blocks.push_back( block_begin );
		}
	}

	if ( blocks.empty() )
		return range_type( 0, 0 );

	range_size_type block = blocks[ GetRandomNum( 0ui64, (uint64)blocks.size() - 1 ) ] * block_size;

	return range_type( block, min( block + block_size, src.limit() ) );
}

inline void SerializeOut(CArchive& ar, const Ranges::Range< uint64 >& out)
{
	ar << out.begin() << out.size();
}

inline Ranges::Range< uint64 > SerializeIn(CArchive& ar, int /*version*/)
{
	try
	{
	//	if ( version >= 29 )
	//	{
			uint64 begin, length;
			ar >> begin >> length;
			if ( begin + length < begin )
				AfxThrowArchiveException( CArchiveException::genericException );
			return Ranges::Range< uint64 >( begin, begin + length );
	//	}
	//	else
	//	{
	//		uint32 begin, length;
	//		ar >> begin >> length;
	//		if ( begin + length < begin )
	//			AfxThrowArchiveException( CArchiveException::genericException );
	//		return Ranges::Range< uint64 >( begin, begin + length );
	//	}
	}
	catch ( Exception& )
	{
		AfxThrowArchiveException( CArchiveException::genericException );
	}
}

// used in FragmentedFile.cpp
inline void SerializeOut1(CArchive& ar,
	const Ranges::List< Ranges::Range< uint64 >, ListTraits >& out)
{
	uint64 nTotal = out.limit();
	uint64 nRemaining = out.length_sum();
	uint32 nFragments = static_cast< uint32 >( out.size() );
	ar << nTotal << nRemaining << nFragments;

	for ( Ranges::List< Ranges::Range< uint64 >, ListTraits >::const_iterator i = out.begin();
		i != out.end(); ++i )
	{
		SerializeOut( ar, *i );
	}
}

inline void SerializeIn1(CArchive& ar,
	Ranges::List< Ranges::Range< uint64 >, ListTraits >& in, int version)
{
	if ( version >= 29 )
	{
		try
		{
			uint64 nTotal, nRemaining;
			uint32 nFragments;
			ar >> nTotal >> nRemaining >> nFragments;
			{
				Ranges::List< Ranges::Range< uint64 >, ListTraits > oNewRange( nTotal );
				in.swap( oNewRange );
			}
			for ( ; nFragments--; )
			{
				const Ranges::Range< uint64 >& fragment = SerializeIn( ar, version );
				if ( fragment.end() > nTotal )
					AfxThrowArchiveException( CArchiveException::genericException );
				in.insert( in.end(), fragment );
			}
			// Sanity check
			if ( in.length_sum() != nRemaining )
				AfxThrowArchiveException( CArchiveException::genericException );
		}
		catch ( Exception& )
		{
			AfxThrowArchiveException( CArchiveException::genericException );
		}
	}
	else
	{
		try
		{
			uint32 nTotal, nRemaining;
			uint32 nFragments;
			ar >> nTotal >> nRemaining >> nFragments;
			{
				Ranges::List< Ranges::Range< uint64 >, ListTraits > oNewRange( nTotal );
				in.swap( oNewRange );
			}
			for ( ; nFragments--; )
			{
				const Ranges::Range< uint64 >& fragment = SerializeIn( ar, version );
				if ( fragment.end() > nTotal )
					AfxThrowArchiveException( CArchiveException::genericException );
				in.insert( in.end(), fragment );
			}
			// Sanity check
			if ( in.length_sum() != nRemaining )
				AfxThrowArchiveException( CArchiveException::genericException );
		}
		catch ( Exception& )
		{
			AfxThrowArchiveException( CArchiveException::genericException );
		}
	}
}

// used in DownloadSource.cpp
inline void SerializeOut2(CArchive& ar,
	const Ranges::List< Ranges::Range< uint64 >, ListTraits >& out)
{
	ar.WriteCount( out.size() );

	for ( Ranges::List< Ranges::Range< uint64 >, ListTraits >::const_iterator i	= out.begin();
		i != out.end(); ++i )
	{
		SerializeOut( ar, *i );
	}
}

inline void SerializeIn2(CArchive& ar,
	Ranges::List< Ranges::Range< uint64 >, ListTraits >& in, int version)
{
	try
	{
		if ( version >= 20 )
		{
			for ( DWORD_PTR count = ar.ReadCount(); count--; )
			{
				const Ranges::Range< uint64 >& fragment = SerializeIn( ar, version );
				if ( fragment.end() > in.limit() )
					AfxThrowArchiveException( CArchiveException::genericException );
				in.insert( in.end(), fragment );
			}
		}
		else if ( version >= 5 )
		{
			while ( ar.ReadCount() )
			{
				const Ranges::Range< uint64 >& fragment = SerializeIn( ar, version );
				if ( fragment.end() > in.limit() )
					AfxThrowArchiveException( CArchiveException::genericException );
				in.insert( in.end(), fragment );
			}
		}
	}
	catch ( Exception& )
	{
		AfxThrowArchiveException( CArchiveException::genericException );
	}
}

} // namespace Fragments
