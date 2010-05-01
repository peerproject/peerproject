//
// RichElement.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

class CRichDocument;
class CRichFragment;
class CRichViewCtrl;


class CRichElement
{
// Construction
public:
	CRichElement(int nType = 0, LPCTSTR pszText = NULL, LPCTSTR pszLink = NULL, DWORD nFlags = 0, int nGroup = 0);
	virtual ~CRichElement();

// Attributes
public:
	CRichDocument*	m_pDocument;
	int				m_nType;
	int				m_nGroup;
	DWORD			m_nFlags;
	CString			m_sText;
	CString			m_sLink;
	HANDLE			m_hImage;
	int				m_nImageIndex;
	COLORREF		m_cColor;

// Operations
public:
	void	Show(BOOL bShow = TRUE);
	void	SetText(LPCTSTR pszText);
	void	SetFlags(DWORD nFlags, DWORD nMask = 0xFFFFFFFF);
	void	Delete();
protected:
	void	PrePaint(CDC* pDC, BOOL bHover);
	void	PrePaintBitmap(CDC* pDC);
	void	PrePaintIcon(CDC* pDC);
	CSize	GetSize();

	friend class CRichFragment;
	friend class CRichViewCtrl;
};

enum
{
	retNull, retNewline, retGap, retAlign,
	retBitmap, retIcon, retAnchor, retCmdIcon, retEmoticon,
	retText, retLink, retHeading
};

enum
{
	retfNull		= 0x00,
	retfBold		= 0x01,
	retfItalic		= 0x02,
	retfUnderline	= 0x04,
	retfHeading		= 0x08,
	retfMiddle		= 0x10,
	retfColor		= 0x20,
	retfHidden		= 0x80
};

enum
{
	reaLeft, reaCenter, reaRight
};
