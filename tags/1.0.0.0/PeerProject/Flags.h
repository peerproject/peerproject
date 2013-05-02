//
// Flags.h
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

// Load the 26x26 Flags.png source image

#pragma once

class CImageFile;

class CFlags
{
// Construction
public:
	CFlags();
	~CFlags();

// Attributes
protected:
	CImageList	m_pImage;	// Flag images

// Operations
public:
	BOOL		Load();
	void		Clear();
	int			GetCount() const;
	int			GetFlagIndex(const CString& sCountry) const;
	BOOL		Draw(int i, HDC hdcDst, int x, int y, COLORREF rgbBk, COLORREF rgbFg = CLR_NONE, UINT fStyle = ILD_NORMAL);
	HICON		ExtractIcon(int i);

protected:
	void		AddFlag(CImageFile* pImage, CRect* pRect, COLORREF crBack);

private:
	CFlags(const CFlags&);
	CFlags& operator=(const CFlags&);
};

extern CFlags Flags;
