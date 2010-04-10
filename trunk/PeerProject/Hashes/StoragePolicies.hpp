//
// Hashes/StoragePolicies.hpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2005.
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

//! \file       Hashes/StoragePolicies.hpp
//! \brief      Defines storage policies.

#pragma once

namespace Hashes
{
	namespace Policies
	{

		//! \brief A model of \ref hashstoragepoliciespage "Storage Policy"
		//! This policy zero-initializes the hash value at default construction and when clearing the hash.
		template<typename DescriptorT>
		struct ZeroInit : public DescriptorT
		{
		public:
			// types
			//! \brief  Simple typedef forwarding the template parameter
			typedef DescriptorT Descriptor;
			typedef typename Descriptor::WordType WordType;
			typedef typename Descriptor::RawStorage RawStorage;
			typedef typename Descriptor::AlignedStorage AlignedStorage;

			//! \brief  Defines an STL style random access iterator to access the hash content.
			typedef typename AlignedStorage::iterator iterator;
			//! \brief  Defines an STL style random access const iterator to access the hash content.
			typedef typename AlignedStorage::const_iterator const_iterator;
			//! \brief  tr1 fix: Defines a TR1 style pointer to access the hash content.
			//typedef typename AlignedStorage::pointer pointer;
			//! \brief  tr1 fix: Defines a TR1 style const pointer to access the hash content.
			//typedef typename AlignedStorage::const_pointer const_pointer;

			// Constructors
			ZeroInit() : Descriptor(), m_storage() {}
			ZeroInit(iterator input)
				: Descriptor()
			{
				std::copy( input, input + wordCount, begin() );
			}
			ZeroInit(const RawStorage& rhs)
				: Descriptor(), m_storage( rhs )
			{}
			template<template<typename> class OtherStoragePolicy>
			ZeroInit(const OtherStoragePolicy< Descriptor >& rhs)
				: Descriptor(), m_words( rhs.alignedStorage() )
			{}
			template<template<typename> class OtherStoragePolicy>
			ZeroInit& operator=(const OtherStoragePolicy< Descriptor >& rhs)
			{
				alignedStorage() = rhs.alignedStorage();
				return *this;
			}

			void clear() { memset( &m_storage[ 0 ], 0, byteCount ); }

			uchar& operator[](size_t index) { return m_storage[ index ]; }
			const uchar& operator[](size_t index) const
			{
				return m_storage[ index ];
			}

			RawStorage& storage() { return m_storage; }
			const RawStorage& storage() const { return m_storage; }

			AlignedStorage& alignedStorage() { return m_words; }
			const AlignedStorage& alignedStorage() const { return m_words; }

			iterator       begin()       { return m_words.begin(); }
			const_iterator begin() const { return m_words.begin(); }
			iterator       end()         { return m_words.end(); }
			const_iterator end()   const { return m_words.end(); }

		private:
			union
			{
				RawStorage m_storage;
				AlignedStorage m_words;
			};
		};

		//! \brief A model of \ref hashstoragepoliciespage "Storage Policy"
		//! This policy does not initialize the hash value upon default
		//! construction and leaves the hash unchanged when clearing.
		template<typename DescriptorT>
		struct NoInit : public DescriptorT
		{
		public:
			typedef DescriptorT Descriptor;
			typedef typename Descriptor::WordType WordType;
			static const size_t wordCount = Descriptor::wordCount;
			static const size_t byteCount = Descriptor::byteCount;
			typedef typename Descriptor::RawStorage RawStorage;
			typedef typename Descriptor::AlignedStorage AlignedStorage;

			typedef typename AlignedStorage::iterator iterator;
			typedef typename AlignedStorage::const_iterator const_iterator;
			//typedef typename AlignedStorage::pointer pointer;
			//typedef typename AlignedStorage::const_pointer const_pointer;

			NoInit() : Descriptor() {}
			NoInit(iterator input)
				: Descriptor()
			{
				memcpy( begin(), input, byteCount );
			}
			NoInit(const RawStorage& rhs)
				: Descriptor(), m_storage( rhs )
			{}
			template<template<typename> class OtherStoragePolicy>
			NoInit(const OtherStoragePolicy< Descriptor >& rhs)
				: Descriptor(), m_words( rhs.alignedStorage() )
			{}
			template<template<typename> class OtherStoragePolicy>
			NoInit& operator=(const OtherStoragePolicy< Descriptor >& rhs)
			{
				alignedStorage() = rhs.alignedStorage();
				return *this;
			}
			void clear() {}

			uchar& operator[](size_t index) { return m_storage[ index ]; }
			const uchar& operator[](size_t index) const
			{
				return m_storage[ index ];
			}

			RawStorage& storage() { return m_storage; }
			const RawStorage& storage() const { return m_storage; }

			AlignedStorage& alignedStorage() { return m_words; }
			const AlignedStorage& alignedStorage() const { return m_words; }

			iterator		begin()		  { return m_words.begin(); }
			const_iterator	begin()	const { return m_words.begin(); }
			iterator		end()		  { return m_words.end(); }
			const_iterator	end()	const { return m_words.end(); }
			//pointer 		data()		  { return m_words.data(); }	// tr1 fix:
			//const_pointer	data()	const { return m_words.data(); }	// tr1 fix:

		private:
			union
			{
				RawStorage m_storage;
				AlignedStorage m_words;
			};
		};

	} // namespace Policies

} // namespace Hashes
