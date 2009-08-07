//
// RichDocument.h
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

#pragma once

class CRichElement;
class CXMLElement;


class CRichDocument
{
public:
	CRichDocument();
	virtual ~CRichDocument();

	CCriticalSection	m_pSection;
	DWORD			m_nCookie;
	CSize			m_szMargin;
	COLORREF		m_crBackground;
	COLORREF		m_crText;
	COLORREF		m_crLink;
	COLORREF		m_crHover;
	COLORREF		m_crHeading;
	CFont			m_fntNormal;
	CFont			m_fntBold;
	CFont			m_fntItalic;
	CFont			m_fntUnder;
	CFont			m_fntBoldUnder;
	CFont			m_fntHeading;

public:
	POSITION		GetIterator() const;
	CRichElement*	GetNext(POSITION& pos) const;
	CRichElement*	GetPrev(POSITION& pos) const;
	INT_PTR			GetCount() const;
	POSITION		Find(CRichElement* pElement) const;
	CRichElement*	Add(CRichElement* pElement, POSITION posBefore = NULL);
	CRichElement*	Add(int nType, LPCTSTR pszText, LPCTSTR pszLink = NULL, DWORD nFlags = 0, int nGroup = 0, POSITION posBefore = NULL);
	void			Remove(CRichElement* pElement);
	void			ShowGroup(int nGroup, BOOL bShow = TRUE);
	void			ShowGroupRange(int nMin, int nMax, BOOL bShow = TRUE);
	void			SetModified();
	void			Clear();
	BOOL			LoadXML(CXMLElement* pBase, CMap< CString, const CString&, CRichElement*, CRichElement* >* pMap = NULL, int nGroup = 0);
//	void			CreateFonts(LPCTSTR pszFaceName = NULL, int nSize = 0);

protected:
	CList< CRichElement* >	m_pElements;

	void			CreateFonts(const LOGFONT* lpLogFont = NULL, const LOGFONT* lpHeading = NULL);
	BOOL			LoadXMLStyles(CXMLElement* pParent);
	BOOL			LoadXMLColor(CXMLElement* pXML, LPCTSTR pszName, COLORREF* pColor);
};
