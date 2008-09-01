//
// GraphLine.h
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

#if !defined(AFX_GRAPHLINE_H__90D244AB_D902_42D6_BD55_E8F34845EB1A__INCLUDED_)
#define AFX_GRAPHLINE_H__90D244AB_D902_42D6_BD55_E8F34845EB1A__INCLUDED_

#pragma once

#include "GraphBase.h"

class CGraphItem;


class CLineGraph : public CGraphBase
{
// Construction
public:
	CLineGraph();
	virtual ~CLineGraph();

// Attributes
public:
	BOOL		m_bShowAxis;
	BOOL		m_bShowGrid;
	BOOL		m_bShowLegend;
	COLORREF	m_crBack;
	COLORREF	m_crGrid;
	DWORD		m_nMinGridVert;
public:
	DWORD		m_nSpeed;
	DWORD		m_nScale;
	DWORD		m_nMaximum;
	DWORD		m_nUpdates;
	DWORD		m_tLastScale;
protected:
	CList< CGraphItem* > m_pItems;
	CPen		m_pGridPen;

// Operations
public:
	void		AddItem(CGraphItem* pItem);
	POSITION	GetItemIterator() const;
	CGraphItem*	GetNextItem(POSITION& pos) const;
	INT_PTR		GetItemCount() const { return m_pItems.GetCount(); }
	void		RemoveItem(CGraphItem* pItem);
	void		ClearItems();
	void		ResetMaximum(BOOL bForce = TRUE);
public:
	virtual void	CreateDefaults();
	virtual void	Serialize(CArchive& ar);
	virtual BOOL	Update();
	virtual void	Clear();
	virtual void	Paint(CDC* pDC, CRect* pRect);
protected:
	void	PaintGrid(CDC* pDC, CRect* pRect);
	void	PaintLegend(CDC* pDC, CRect* pRect);

};

#endif // !defined(AFX_GRAPHLINE_H__90D244AB_D902_42D6_BD55_E8F34845EB1A__INCLUDED_)
