//
// GraphBase.h
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

#if !defined(AFX_GRAPHBASE_H__63FD4BDF_BEBE_4526_94FC_746EE0641321__INCLUDED_)
#define AFX_GRAPHBASE_H__63FD4BDF_BEBE_4526_94FC_746EE0641321__INCLUDED_

#pragma once


class CGraphBase
{
// Construction
public:
	CGraphBase();
	virtual ~CGraphBase();

// Attributes
protected:
	CDC			m_pDC;
	CBitmap		m_pImage;
	HBITMAP		m_hOldImage;
	CSize		m_szImage;

// Operations
public:
	virtual void	CreateDefaults();
	virtual void	Serialize(CArchive& ar);
	virtual BOOL	Update();
	virtual void	Clear();
	virtual void	Paint(CDC* pDC, CRect* pRect);
	virtual void	BufferedPaint(CDC* pDC, CRect* pRect);

};

#endif // !defined(AFX_GRAPHBASE_H__63FD4BDF_BEBE_4526_94FC_746EE0641321__INCLUDED_)
