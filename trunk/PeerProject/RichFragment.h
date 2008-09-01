//
// RichFragment.h
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

#if !defined(AFX_RICHFRAGMENT_H__B52AE063_BDD3_41D8_BE1D_D78C47A634A6__INCLUDED_)
#define AFX_RICHFRAGMENT_H__B52AE063_BDD3_41D8_BE1D_D78C47A634A6__INCLUDED_

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

#endif // !defined(AFX_RICHFRAGMENT_H__B52AE063_BDD3_41D8_BE1D_D78C47A634A6__INCLUDED_)
