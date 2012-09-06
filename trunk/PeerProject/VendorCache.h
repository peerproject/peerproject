//
// VendorCache.h
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

class CVendor;
class CXMLElement;


class CVendorCache
{
public:
	CVendorCache();
	virtual ~CVendorCache();

public:
	CVendor*	m_pNull;

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

	BOOL		Load(); 								// Load data from Vendors.xml
	CVendor*	LookupByName(LPCTSTR pszName) const;	// Lookup by code or by name
	bool		IsExtended(LPCTSTR pszCode) const;		// Is specified vendor a PeerProject/Shareaza-powered vendor?

protected:
	CMap< CString, const CString&, CVendor*, CVendor* > m_pCodeMap;		// Vendor code map
	CMap< CString, const CString&, CVendor*, CVendor* > m_pNameMap;		// Vendor name map (lowercased)

	void		Clear();
	BOOL		LoadFrom(CXMLElement* pXML);
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
	bool		m_bBrowseFlag;
	bool		m_bExtended;		// PeerProject/Shareaza-powered

protected:
	BOOL		LoadFrom(CXMLElement* pXML);

	friend class CVendorCache;
};

extern CVendorCache VendorCache;
