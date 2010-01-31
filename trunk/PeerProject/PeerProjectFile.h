//
// PeerProjectFile.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "PeerProjectOM.h"

typedef CMap< CString, CString&, FILETIME, FILETIME& > CMapStringToFILETIME;

class CPeerProjectFile : public CComObject
{
	DECLARE_DYNAMIC(CPeerProjectFile)

public:
	CPeerProjectFile();
	CPeerProjectFile(const CPeerProjectFile& pFile);
	CPeerProjectFile& operator=(const CPeerProjectFile& pFile);

	CString				m_sName;	// Filename only
	QWORD				m_nSize;	// Size if any: (no size if equal to 0 or SIZE_UNKNOWN)
	Hashes::Sha1Hash	m_oSHA1;	// SHA1 (Base32)
	Hashes::TigerHash	m_oTiger;	// TigerTree Root Hash (Base32)
	Hashes::Ed2kHash	m_oED2K;	// ED2K (MD4, Base16)
	Hashes::Md5Hash		m_oMD5;		// MD5 (Base16)
	Hashes::BtHash		m_oBTH;		// BitTorrent Info Hash (Base32)
	CString				m_sURL;		// Host if any
	CString				m_sPath;	// Usage:
									// CPeerProjectURL : Path part of URL
									// CLibraryFile : Local path without filename
									// CBTFile      : Relative path inside .torrent
									// CDownload    : Path of .pd file (.sd)
									// CUploadFile  : Path of requested file	// ToDo: Fix This?!
	
	// Returns "urn:bitprint:SHA1.TIGER" or "urn:sha1:SHA1" or "urn:tree:tiger/:TIGER"
	CString GetBitprint() const;

	// Returns "sha1_SHA1", "ttr_TIGER" etc.
	CString GetFilename() const;

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
	inline QWORD GetSize() const throw()
	{
		return ( ( m_nSize == SIZE_UNKNOWN ) ? 0 : m_nSize );
	}

	// Get file name suitable for searching
	virtual CString GetSearchName() const
	{
		CString sName = m_sName;
		ToLower( sName );
		return sName;
	}

// Automation
protected:
	BEGIN_INTERFACE_PART(PeerProjectFile, IPeerProjectFile)
		DECLARE_DISPATCH()
		STDMETHOD(get_Path)(BSTR FAR* psPath);
		STDMETHOD(get_Name)(BSTR FAR* psName);
		STDMETHOD(get_Size)(ULONGLONG FAR* pnSize);
		STDMETHOD(get_URN)(BSTR sURN, BSTR FAR* psURN);
		STDMETHOD(get_Hash)(URN_TYPE nType, ENCODING nBase, BSTR FAR* psURN);
		STDMETHOD(get_URL)(BSTR FAR* psURL);
	END_INTERFACE_PART(PeerProjectFile)

	DECLARE_INTERFACE_MAP()
};
