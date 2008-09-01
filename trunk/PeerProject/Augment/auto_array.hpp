////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// augment/auto_array.hpp                                                     //
//                                                                            //
// This file is part of PeerProject (peerproject.org) © 2008                  //
// Portions Copyright Shareaza Development Team, 2002-2007.                   //
//                                                                            //
// PeerProject is free software; you can redistribute it and/or               //
// modify it under the terms of the GNU General Public License                //
// as published by the Free Software Foundation; either version 3             //
// of the License, or later version (at your option).                         //
//                                                                            //
// PeerProject is distributed in the hope that it will be useful,             //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       //
// See the See the GNU General Public License for more details.               //
//                                                                            //
// You should have received a copy of the GNU General Public License 3.0          //
// along with PeerProject; if not, write to Free Software Foundation, Inc.    //
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef AUGMENT_AUTO_ARRAY_HPP_INCLUDED
#define AUGMENT_AUTO_ARRAY_HPP_INCLUDED

namespace augment
{

template<typename T>
class auto_array
{
public:
	typedef T element_type;
private:
	struct auto_array_ref
	{
		explicit auto_array_ref(element_type** ref) throw()
			: ref_( ref )
		{}
		element_type* release() throw()
		{
			element_type* ptr = *ref_;
			*ref_ = NULL;
			return ptr;
		}
		element_type** ref_;
	};
public:
	explicit auto_array(element_type* ptr = NULL) throw()
		: ptr_( ptr )
	{}
	auto_array(auto_array& other) throw()
		: ptr_( other.release() )
	{}
	auto_array(auto_array_ref other) throw()
		: ptr_( other.release() )
	{}
	~auto_array() throw()
	{
		if ( get() != NULL )
			boost::checked_array_delete( get() );
	};

	auto_array& operator=(auto_array& other) throw()
	{
		reset( other.release() );
		return *this;
	}
	auto_array& operator=(auto_array_ref other) throw()
	{
		reset( other.release() );
		return *this;
	}

	element_type* get() const  throw()
	{
		return ptr_;
	}
	element_type& operator[](std::size_t index) const  throw()
	{
		return get()[ index ];
	}
	element_type* release() throw()
	{
		element_type* ptr = get();
		ptr_ = NULL;
		return ptr;
	}
	void reset(element_type* ptr = NULL) throw()
	{
		if ( ptr != get() && get() != NULL )
			boost::checked_array_delete( get() );
		ptr_ = ptr;
	}

	operator auto_array_ref() throw()
	{
		return auto_array_ref( &ptr_ );
	}
private:
	element_type* ptr_;
};

} // namespace augment

#endif // #ifndef AUGMENT_AUTO_ARRAY_HPP_INCLUDED
