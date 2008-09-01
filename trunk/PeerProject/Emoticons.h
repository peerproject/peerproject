//
// Emoticons.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(AFX_EMOTICONS_H__0EE665E3_3AD7_4BFF_B2D8_000C806D9D08__INCLUDED_)
#define AFX_EMOTICONS_H__0EE665E3_3AD7_4BFF_B2D8_000C806D9D08__INCLUDED_

#pragma once

class CImageFile;
class CRichDocument;


class CEmoticons
{
// Construction
public:
	CEmoticons();
	virtual ~CEmoticons();

// Attributes
public:
	CImageList			m_pImage;
	CArray< CString >	m_pIndex;
	LPTSTR				m_pTokens;
	CArray< UINT >		m_pButtons;

// Operations
public:
	LPCTSTR	FindNext(LPCTSTR pszText, int* pnIndex);
	int		Lookup(LPCTSTR pszText, int nLen = -1) const;
	LPCTSTR	GetText(int nIndex) const;
	void	Draw(CDC* pDC, int nIndex, int nX, int nY, COLORREF crBack = CLR_NONE);
	CMenu*	CreateMenu();
	void	FormatText(CRichDocument* pDocument, LPCTSTR pszBody, BOOL bNewlines = FALSE);
public:
	BOOL	Load();
	void	Clear();
protected:
	int		AddEmoticon(LPCTSTR pszText, CImageFile* pImage, CRect* pRect, COLORREF crBack, BOOL bButton);
	void	BuildTokens();
	BOOL	LoadTrillian(LPCTSTR pszFile);


};

extern CEmoticons Emoticons;

#endif // !defined(AFX_EMOTICONS_H__0EE665E3_3AD7_4BFF_B2D8_000C806D9D08__INCLUDED_)
