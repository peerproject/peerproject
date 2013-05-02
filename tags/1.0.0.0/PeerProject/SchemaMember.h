//
// SchemaMember.h
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

class CSchema;
class CXMLElement;


class CSchemaMember
{
// Construction
public:
	CSchemaMember(CSchema* pSchema);
	virtual ~CSchemaMember();

// Attributes
public:
	CSchema*	m_pSchema;
	CString		m_sName;
	CString		m_sType;
	CString		m_sTitle;
	BOOL		m_bElement;
	BOOL		m_bNumeric;
	BOOL		m_bGUID;
	BOOL		m_bYear;
	BOOL		m_bIndexed;
	BOOL		m_bSearched;
	BOOL		m_bHidden;
	BOOL		m_bBoolean;

	int			m_nMinOccurs;
	int			m_nMaxOccurs;
	int			m_nMaxLength;

	BOOL		m_bPrompt;
	int			m_nFormat;
	int			m_nColumnWidth;
	int			m_nColumnAlign;

	CString		m_sLinkURI;
	CString		m_sLinkName;

	CList< CString > m_pItems;

// Operations
public:
	POSITION	GetItemIterator() const;
	CString		GetNextItem(POSITION& pos) const;
	INT_PTR		GetItemCount() const { return m_pItems.GetCount(); }
	CString		GetValueFrom(const CXMLElement* pElement, LPCTSTR pszDefault = NULL, BOOL bFormat = FALSE, BOOL bNoValidation = FALSE) const;
	void		SetValueTo(CXMLElement* pBase, LPCTSTR pszValue);
protected:
	BOOL		LoadSchema(CXMLElement* pRoot, CXMLElement* pElement);
	BOOL		LoadType(CXMLElement* pType);
	BOOL		LoadDescriptor(CXMLElement* pXML);
	BOOL		LoadDisplay(CXMLElement* pXML);

	friend class CSchema;

private:
	CSchemaMember(const CSchemaMember&);
	CSchemaMember& operator=(const CSchemaMember&);
};

enum
{
	smfNone, smfTimeMMSS, smfBitrate, smfFrequency, smfTimeHHMMSSdec
};
