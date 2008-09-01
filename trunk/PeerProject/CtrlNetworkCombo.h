//
// CtrlNetworkCombo.h
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


class CNetworkCombo : public CComboBox
{
// Construction
public:
	CNetworkCombo();
	virtual ~CNetworkCombo();

	DECLARE_DYNAMIC(CNetworkCombo)

// Operations
public:
	virtual BOOL Create(DWORD dwStyle, CWnd* pParentWnd, UINT nID);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnSkinChange();
public:
	DWORD_PTR GetNetwork();
	void	SetNetwork(int nProtocol);

// Attributes
protected:
	CImageList	m_gdiImageList;

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


