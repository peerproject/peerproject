//
// PeerProjectFile.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

typedef CMap< CString, CString&, FILETIME, FILETIME& > CMapStringToFILETIME;

class CPeerProjectFile : boost::noncopyable
{
public:
	CPeerProjectFile() :
		m_nSize( SIZE_UNKNOWN )
	{
	}

	CString				m_sName;	// Filename only
	QWORD				m_nSize;	/*
									Size if any
									 (there is no size if it equal to 0 or SIZE_UNKNOWN)
									*/
	Hashes::Sha1Hash	m_oSHA1;	// SHA1 (Base32)
	Hashes::TigerHash	m_oTiger;	// TigerTree Root Hash (Base32)
	Hashes::Ed2kHash	m_oED2K;	// ED2K (MD4, Base16)
	Hashes::BtHash		m_oBTH;		// BitTorrent Info Hash (Base32)
	Hashes::Md5Hash		m_oMD5;		// MD5 (Base16)
	CString				m_sPath;	/*
									Use:
									 CDownloadBase : Full local path (.partial)
									 CPeerProjectURL  : Path part of URL
									 CLibraryFile  : Local path without filename
									 CBTFile       : Relative path inside .torrent
									 CUploadFile   : Full local path
									*/
	CString				m_sURL;		// Host if any

	// Returns "urn:bitprint:SHA1.TIGER" or "urn:sha1:SHA1" or "urn:tree:tiger/:TIGER"
	CString GetBitprint() const;

	// Returns "http://nAddress:nPort/uri-res/N2R?{SHA1|TIGER|ED2K|MD5|BTH}"
	CString GetURL(const IN_ADDR& nAddress, WORD nPort) const;

	// Split string of URLs delimited by commas to URL list
	bool SplitStringToURLs(LPCTSTR pszURLs, CMapStringToFILETIME& oUrls) const;

	// Is some of hashes present?
	inline bool IsHashed() const throw()
	{
		return m_oSHA1 || m_oTiger || m_oED2K || m_oBTH || m_oMD5;
	}
	
	// Printable file size
	inline QWORD GetSize() const
	{
		return ( ( m_nSize == SIZE_UNKNOWN ) ? 0 : m_nSize );
	}
};
