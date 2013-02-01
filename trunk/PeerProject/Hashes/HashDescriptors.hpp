//
// Hashes/HashDescriptors.hpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

//! \file       Hashes/HashDescriptors.hpp
//! \brief      Defines hash descriptors.

#pragma once

#if defined(_MSC_VER) && (_MSC_FULL_VER > 150030000)	// _HAS_TR1		// VS2008 SP1 for std::tr1, VS2012 for std::
#include <array>
#else	// Boost fallback
#include <Boost/tr1/array.hpp>
#endif


namespace Hashes
{
	namespace Policies
	{
		//! \brief  Array of the size of the hash.
		//!
		//! Sometimes it is necessary to access the hash as a bytestream and
		//! bypass alignment and padding restrictions mposed by the Word type.
		//! This POD structure contains an array of appropriate size
		//! and subscripting operators for convenience.
		//! \brief Defines urn signatures.
		//!
		//! This structure contains definitions for a particular urn and hash.
		//! Used to define models of \ref hashdescriptorpage "Hash Descriptor".
		//! The scheme used for a urn here is simple:
		//! - A urn consists of a prefix which identifies the urn.
		//! 	This prefix orms the first part of the urn string.
		//! - The second part of the string consists of arbitrary data,
		//! 	at a fixed position of that part a hash string can be found.
		//! Members of a UrnString structure define these properties for a given urn prefix.
		struct UrnString
		{
			//! \brief The minimum urn string length.
			//! This specifies the minimum length a given string must have in order
			//! to be valid for that string type. There must be enough space for the
			//! prefix itself and the hash string at least; possibly more if the urn
			//! specifies additional data we don't care about here.
			size_t minimumLength;

			//! \brief The offset of the hash string.
			//! This specifies the offset relative to the beginning of the string at
			//! which the hash substring can be found.
			size_t hashOffset;

			//! \brief The length of the urn prefix.
			//! This specifies the length of the urn prefix without the terminator:
			//! \a signaturleLength = \c _tcslen( *\a signature )
			size_t signatureLength;

			//! \brief The urn prefix.
			//! A pointer to the urn prefix (should be a string literal).
			const wchar* signature;
		};
		//! \brief  A model of \ref hashdescriptorpage "Hash Descriptor" for SHA1 hashes.
		//!
		//! SHA1 is used a basic hash. It is computed directly over a 32bit stream,
		//! with each 32bit word consisting of 4 bytes in big endian order. (RFC 3174)
		struct Sha1Descriptor
		{
			typedef uint32 WordType;
			static const size_t wordCount = 5;
			static const size_t byteCount = wordCount * sizeof( WordType );
			static const size_t numUrns = 4;
			static const UrnString urns[ numUrns ];
			static const Encoding encoding = base32Encoding;
			typedef std::tr1::array< uchar, byteCount > RawStorage;
			typedef std::tr1::array< WordType, wordCount > AlignedStorage;
			static std::vector< AlignedStorage > blackList;
		};

		//! \brief  A model of \ref hashdescriptorpage "Hash Descriptor" for Tiger tree hashes.
		//!
		//! A Tiger tree hash is a compound hash.  It is computed using the
		//! Tiger hash algorithm, organized in a Merkle Hash Tree.
		//! It's input is organized in 64bit words in little endian order.
		//! \sa http://open-content.net/specs/draft-jchapweske-thex-02.html (expired)
		//! \sa http://zgp.org/pipermail/p2p-hackers/2002-June/000621.html (cached)
		//! \sa http://cs.technion.ac.il/~biham/Reports/Tiger/
		struct TigerDescriptor
		{
			typedef uint64 WordType;
			static const size_t wordCount = 3;
			static const size_t byteCount = wordCount * sizeof( WordType );
			static const size_t numUrns = 7;
			static const UrnString urns[ numUrns ];
			static const Encoding encoding = base32Encoding;
			typedef std::tr1::array< uchar, byteCount > RawStorage;
			typedef std::tr1::array< WordType, wordCount > AlignedStorage;
			static std::vector< AlignedStorage > blackList;
		};

		//! \brief  A model of \ref hashdescriptorpage "Hash Descriptor" for Ed2k hashes.
		//!
		//! A Edonkey2000 hash is a compound hash based on the MD4 Message-Digest
		//! algorithm (RFC 1320). It divides the input stream into blocks of 9.25MB,
		//! the MD4 digest is generated of each block. The Ed2k hash is than
		//! computed as the MD4 digest of the digests of all individual blocks
		//! unless there is only one block. In the latter case Ed2k and MD4 are
		//! identical. Since the Edonkey network does not support files larger than
		//! 4GB this hash type shall not be used to identify such files.
		struct Ed2kDescriptor
		{
			typedef uint32 WordType;
			static const size_t wordCount = 4;
			static const size_t byteCount = wordCount * sizeof( WordType );
			static const size_t numUrns = 4;
			static const UrnString urns[ numUrns ];
			static const Encoding encoding = base16Encoding;
			typedef std::tr1::array< uchar, byteCount > RawStorage;
			typedef std::tr1::array< WordType, wordCount > AlignedStorage;
			static std::vector< AlignedStorage > blackList;
		};

		//! \brief  A model of \ref hashdescriptorpage "Hash Descriptor" MD5 hashes.
		//! MD5 is a basic hash discribed in RFC 1321.
		struct Md5Descriptor
		{
			typedef uint32 WordType;
			static const size_t wordCount = 4;
			static const size_t byteCount = wordCount * sizeof( WordType );
			static const size_t numUrns = 2;
			static const UrnString urns[ numUrns ];
			static const Encoding encoding = base16Encoding;
			typedef std::tr1::array< uchar, byteCount > RawStorage;
			typedef std::tr1::array< WordType, wordCount > AlignedStorage;
			static std::vector< AlignedStorage > blackList;
		};

		//! \brief  A model of \ref hashdescriptorpage "Hash Descriptor" for Bittorrent info hashes.
		//!
		//! The Bittorrent info hash is a compound hash based on SHA1.
		//! Similar to Ed2k the input stream is divided into blocks of equal size
		//! from which the SHA1 is taken. The info hash is then computed as the SHA1 of
		//! all individual SHA1 digests. Exact block length may vary from torrent to torrent.
		//! Although this info hash has the same properties as a SHA1 hash
		//! it does not have the same semantics and comparing or assigning between
		//! info hashes and plain SHA1 is not meaningful. In this template system,
		//! both are distinct types, thus such meaningless operations cannot take place.
		struct BthDescriptor
		{
			typedef uint32 WordType;
			static const size_t wordCount = 5;
			static const size_t byteCount = wordCount * sizeof( WordType );
			static const size_t numUrns = 2;
			static const UrnString urns[ numUrns ];
			static const Encoding encoding = base32Encoding;
			typedef std::tr1::array< uchar, byteCount > RawStorage;
			typedef std::tr1::array< WordType, wordCount > AlignedStorage;
			static std::vector< AlignedStorage > blackList;
		};

		//! \brief  A model of \ref hashdescriptorpage "Hash Descriptor" for Guids.
		//!
		//! A Guid is a placeholder used to identify varous kinds of resources,
		//! such as components and peers. It is usually unspecified, how a Guid is generated-
		//! often it is not reproducible. This makes it suitable to serve as temporary ids
		//! which become obsolete once the identified resource no longer recognizes them.
		//! There is no urn form known for this type of hash.
		struct GuidDescriptor
		{
			typedef uint32 WordType;
			static const size_t wordCount = 4;
			static const size_t byteCount = wordCount * sizeof( WordType );
			static const Encoding encoding = guidEncoding;
			static const size_t numUrns = 0;
			typedef std::tr1::array< uchar, byteCount > RawStorage;
			typedef std::tr1::array< WordType, wordCount > AlignedStorage;
			static std::vector< AlignedStorage > blackList;
		};

		//! \brief  A model of \ref hashdescriptorpage "Hash Descriptor" for Bittorent Guids.
		//!
		//! A Bittorrent Guid is similar to a plain Guid, except that it uses 160 instead of 128 bit.
		//! PeerProject uses a special transformation signature to convert its local Guid
		//! into a Bittorrent Guid. This signature is recognized by other PeerProject/Shareaza clients.
		//! There is no urn form known for this type of hash.
		struct BtGuidDescriptor
		{
			typedef uint32 WordType;
			static const size_t wordCount = 5;
			static const size_t byteCount = wordCount * sizeof( WordType );
			static const Encoding encoding = base16Encoding;
			static const size_t numUrns = 0;
			typedef std::tr1::array< uchar, byteCount > RawStorage;
			typedef std::tr1::array< WordType, wordCount > AlignedStorage;
			static std::vector< AlignedStorage > blackList;
		};
	} // namespace Polcies

} // namespace Hashes
