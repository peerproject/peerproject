//
// VendorCache.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#if !defined(AFX_VENDORCACHE_H__D5534D6B_0819_4C8F_B62B_9DD5CB3468AD__INCLUDED_)
#define AFX_VENDORCACHE_H__D5534D6B_0819_4C8F_B62B_9DD5CB3468AD__INCLUDED_

#pragma once

class CVendor;
class CXMLElement;


class CVendorCache
{
public:
	CVendorCache();
	virtual ~CVendorCache();

public:
	CVendor*		m_pNull;

public:
	// Lookup 4-bytes vendor code (ASCII without terminating null)
	inline CVendor* Lookup(LPCSTR pszCode) const
	{
		ASSERT( pszCode );
		if ( pszCode )
		{
			WCHAR szCode[5] = { pszCode[0], pszCode[1], pszCode[2], pszCode[3], 0 };
			return Lookup( szCode );
		}
		return NULL;
	}

	// Lookup 4-chars vendor code (with terminating null)
	inline CVendor* Lookup(LPCWSTR pszCode) const
	{
		ASSERT( pszCode );
		if ( pszCode && pszCode[0] && pszCode[1] && pszCode[2] && pszCode[3] && ! pszCode[4] )
		{
			CVendor* pVendor;
			if ( m_pCodeMap.Lookup( pszCode, pVendor ) )
				return pVendor;
			else
				return NULL;
		}
		return NULL;
	}

	// Lookup by code or by name
	CVendor*		LookupByName(LPCTSTR pszName) const;

	// Load data from Vendors.xml
	BOOL			Load();

	// Is specified vendor a PeerProject-powered vendor?
	bool			IsExtended(LPCTSTR pszCode) const;

protected:
	// Vendor code map
	CMap< CString, const CString&, CVendor*, CVendor* > m_pCodeMap;
	// Name map (lowercased)
	CMap< CString, const CString&, CVendor*, CVendor* > m_pNameMap;

	void			Clear();
	BOOL			LoadFrom(CXMLElement* pXML);
};


class CVendor
{
public:
	CVendor();
	CVendor(LPCTSTR pszCode);
	virtual ~CVendor();

public:
	CString		m_sCode;
	CString		m_sName;
	CString		m_sLink;
	bool		m_bChatFlag;
	bool		m_bHTMLBrowse;
	bool		m_bExtended;		// PeerProject-powered

protected:
	BOOL		LoadFrom(CXMLElement* pXML);

	friend class CVendorCache;
};

extern CVendorCache VendorCache;


#endif // !defined(AFX_VENDORCACHE_H__D5534D6B_0819_4C8F_B62B_9DD5CB3468AD__INCLUDED_)
