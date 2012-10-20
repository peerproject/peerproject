//
// FileFragments.hpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#ifndef FILEFRAGMENTS_HPP_INCLUDED
#define FILEFRAGMENTS_HPP_INCLUDED
#endif

#include "PeerProject.h"

#include "FileFragments/Ranges.hpp"

namespace Fragments
{

template< class RangeT, class ContainerT >
class ListTraits
{
public:
	typedef RangeT range_type;
	typedef ContainerT container_type;
	typedef typename range_type::size_type range_size_type;
	typedef typename range_type::payload_type payload_type;
	typedef Ranges::RangeCompare< payload_type, range_size_type > compare_type;
	typedef typename container_type::iterator iterator;
	typedef std::pair< iterator, iterator > iterator_pair;

public:
	range_size_type limit() const { return m_limit; }
	range_size_type length_sum() const { return m_length_sum; }
	range_size_type missing() const { return limit() - length_sum(); }

	void ensure(range_size_type limit)
	{
		m_limit = ( m_limit == SIZE_UNKNOWN ) ? limit : max( m_limit, limit );
	}

// Following functions have to be declared
protected:
	typedef range_size_type ctor_arg_type;
	explicit ListTraits(ctor_arg_type limit) : m_limit( limit ), m_length_sum( 0 ) { }
	void clear() { m_length_sum = 0; }
	void swap(ListTraits& other)
	{
		std::swap( m_limit, other.m_limit );
		std::swap( m_length_sum, other.m_length_sum );
	}
	range_size_type erase(const iterator where)
	{
		m_length_sum -= where->size();
		return where->size();
	}
	template< class container_type >
	range_size_type merge_and_replace(container_type& set,
		iterator_pair sequence, const range_type& new_range)
	{
		ASSERT( sequence.first != sequence.second );
		if ( sequence.first->begin() <= new_range.begin()
			&& sequence.first->end() >= new_range.end() ) return 0;
		range_size_type old_sum = m_length_sum;
		range_size_type low = min( sequence.first->begin(), new_range.begin() );
		range_size_type high = max( ( --sequence.second )->end(), new_range.end() );
		for ( ++sequence.second; sequence.first != sequence.second; )
		{
			range_size_type length = sequence.first->size();
			set.erase( sequence.first++ );
			m_length_sum -= length;
		}
		set.insert( sequence.second, range_type( low, high ) );
		m_length_sum += high - low;
		return m_length_sum - old_sum;
	}
	template< class container_type >
	range_size_type simple_merge(container_type& set, iterator where, const range_type& new_range)
	{
		set.insert( where, new_range );
		m_length_sum += new_range.size();
		return new_range.size();
	}

private:
	range_size_type m_limit;
	range_size_type m_length_sum;
};

using Ranges::Exception;

typedef Ranges::Range< uint64 > Fragment;
typedef Ranges::RangeError< Fragment > FragmentError;
typedef Ranges::ListError< Fragment > ListError;
typedef Ranges::List< Fragment, ListTraits > List;
typedef Ranges::Queue< Fragment > Queue;

//template Fragment;
//template FragmentError;
//template ListError;
//template List;
//template Queue;

} // namespace Fragments


#include "FileFragments/Compatibility.hpp"
