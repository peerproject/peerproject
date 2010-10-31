//
// LiveListSizer.h
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

class CLiveListSizer
{
// Construction
public:
	CLiveListSizer(CListCtrl* pCtrl = NULL);
	virtual ~CLiveListSizer();

// Attributes
protected:
	CListCtrl*	m_pCtrl;
	int			m_nWidth;
	int			m_nColumns;
	int*		m_pWidth;
	float*		m_pTake;

// Operations
public:
	void	Attach(CListCtrl* pCtrl, BOOL bScale = FALSE);
	void	Detach();
	BOOL	Resize(int nWidth = 0, BOOL bScale = FALSE);
};
