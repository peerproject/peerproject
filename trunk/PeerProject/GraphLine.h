//
// GraphLine.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

	virtual void	CreateDefaults();
	virtual void	Serialize(CArchive& ar);
	virtual BOOL	Update();
	virtual void	Clear();
	virtual void	Paint(CDC* pDC, CRect* pRect);
protected:
	void	PaintGrid(CDC* pDC, CRect* pRect);
	void	PaintLegend(CDC* pDC, CRect* pRect);
};
