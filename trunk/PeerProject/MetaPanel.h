//
// MetaPanel.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "MetaList.h"


class CMetaPanel : public CMetaList
{
// Construction
public:
	CMetaPanel();
	virtual ~CMetaPanel();

// Operations
public:
	int		Layout(CDC* pDC, int nWidth);
	void	Paint(CDC* pDC, const CRect* prcArea);
	BOOL	OnClick(const CPoint& point);

	virtual BOOL IsWorking() const { return FALSE; }
	virtual void Start() {}
	virtual void Stop() {}
	virtual void Clear() { CMetaList::Clear(); }

	CMetaPanel*		m_pChild;
	CString			m_sThumbnailURL;

// Attributes
public:
	int		m_nHeight;

protected:
	CBitmap m_bmMusicBrainz;
};
