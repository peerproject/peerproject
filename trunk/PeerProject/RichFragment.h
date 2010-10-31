//
// RichFragment.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

class CRichElement;
class CRichViewCtrl;


class CRichFragment
{
// Construction
public:
	CRichFragment(CRichElement* pElement, int nOffset, int nLength, CPoint* pPoint, CSize* pSize);
	CRichFragment(CRichElement* pElement, CPoint* pPoint);
	virtual ~CRichFragment();

// Attributes
public:
	CRichElement*	m_pElement;
	CPoint			m_pt;
	CSize			m_sz;
	WORD			m_nOffset;
	WORD			m_nLength;

// Operations
public:
	void	Add(int nLength, CSize* pSize);
	void	Paint(CDC* pDC, CRichViewCtrl* pCtrl, int nFragment);
};
