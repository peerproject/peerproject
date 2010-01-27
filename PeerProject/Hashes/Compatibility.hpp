//
// Hashes/Compatibility.hpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
// See the See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

//! \file	Hashes/Compatibility.hpp
//! \brief	Defines functions to interface with legacy and MFC code.
//!
//! This file contains function definitions to emulate the old serialization functions.
//! ToDo: This should be replaced once a new serialization method has been adopted.

#pragma once


namespace Hashes
{

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy,
		template<typename> class ValidationPolicy
	>
	inline void SerializeOut(CArchive& ar, const Hash< Descriptor, StoragePolicy,
			CheckingPolicy, ValidationPolicy >& out);

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy
	>
	inline void SerializeOut(CArchive& ar, const Hash< Descriptor, StoragePolicy,
			CheckingPolicy, Policies::NoValidation >& out)
	{
		ASSERT( ar.IsStoring() );
		ar.Write( out.begin(), out.byteCount );
	}

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy
	>
	inline void SerializeOut(CArchive& ar, const Hash< Descriptor, StoragePolicy,
			CheckingPolicy, Policies::BasicValidation >& out)
	{
		ASSERT( ar.IsStoring() );
		uint32 bValid = bool( out );
		ar << bValid;
		if ( bValid ) ar.Write( out.begin(), out.byteCount );
	}

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy
	>
	inline void SerializeOut(CArchive& ar, const Hash< Descriptor, StoragePolicy,
			CheckingPolicy, Policies::ExtendedValidation >& out)
	{
		ASSERT( ar.IsStoring() );
		uint32 bValid = bool( out );
		ar << bValid;
		if ( bValid ) ar.Write( out.begin(), out.byteCount );
		uint32 bTrusted = out.isTrusted();
		ar << bTrusted;
	}

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy,
		template<typename> class ValidationPolicy
	>
	inline void SerializeIn(CArchive& ar, Hash< Descriptor, StoragePolicy,
			CheckingPolicy, ValidationPolicy >& in, int version);

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy
	>
	inline void SerializeIn(CArchive& ar, Hash< Descriptor, StoragePolicy,
			CheckingPolicy, Policies::NoValidation >& in, int /*version*/)
	{
		ASSERT( ar.IsLoading() );
		ReadArchive( ar, in.begin(), in.byteCount );
	}

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy
	>
	inline void SerializeIn(CArchive& ar, Hash< Descriptor, StoragePolicy,
			CheckingPolicy, Policies::BasicValidation >& in, int /*version*/)
	{
		ASSERT( ar.IsLoading() );
		uint32 bValid;
		ar >> bValid;
		if ( bValid )
		{
			ReadArchive( ar, in.begin(), in.byteCount );
			in.validate();
		}
		else
		{
			in.clear();
		}
	}

	template
	<
		typename Descriptor,
		template<typename> class StoragePolicy,
		template<typename> class CheckingPolicy
	>
	inline void SerializeIn(CArchive& ar, Hash< Descriptor, StoragePolicy,
			CheckingPolicy, Policies::ExtendedValidation >& in, int version)
	{
		ASSERT( ar.IsLoading() );
		uint32 bValid;
		ar >> bValid;
		if ( bValid )
		{
			ReadArchive( ar, in.begin(), in.byteCount );
			in.validate();
		}
		else
		{
			in.clear();
		}
		uint32 bTrusted = bValid;
		if ( version >= 31 ) ar >> bTrusted;
		if ( bTrusted ) in.signalTrusted();
	}

} // namespace Hashes
