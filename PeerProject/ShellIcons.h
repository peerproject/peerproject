//
// ShellIcons.h
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


class CShellIcons
{
// Construction
public:
	CShellIcons();
	virtual ~CShellIcons();

// Operations
public:
	void	Clear();
	int		Get(LPCTSTR pszFile, int nSize);
	int		Add(HICON hIcon, int nSize);
	HICON	ExtractIcon(int nIndex, int nSize);
	BOOL	Lookup(LPCTSTR pszType, HICON* phSmallIcon, HICON* phLargeIcon, CString* psName, CString* psMIME, HICON* phHugeIcon = NULL);
	CString	GetTypeString(LPCTSTR pszFile);

// Inlines
public:
	inline CImageList* GetObject(int nSize) const
	{
		switch ( nSize )
		{
		case 16:
			return (CImageList*)&m_i16;
		case 32:
			return (CImageList*)&m_i32;
		case 48:
			return (CImageList*)&m_i48;
		default:
			return NULL;
		}
	}

	inline HIMAGELIST GetHandle(int nSize) const
	{
		switch ( nSize )
		{
		case 16:
			return m_i16.GetSafeHandle();
		case 32:
			return m_i32.GetSafeHandle();
		case 48:
			return m_i48.GetSafeHandle();
		default:
			return NULL;
		}
	}

	inline BOOL Draw(CDC* pDC, int nIcon, int nSize, int nX, int nY, COLORREF crBack = CLR_NONE, BOOL bSelected = FALSE) const
	{
		return ImageList_DrawEx( GetHandle( nSize ), nIcon, pDC->GetSafeHdc(),
			nX, nY, nSize, nSize, crBack, CLR_DEFAULT, bSelected ? ILD_SELECTED : ILD_NORMAL );
	}

// Attributes
private:
	typedef CMap< CString, const CString&, int, int > CIconMap;

	CImageList		m_i16;
	CImageList		m_i32;
	CImageList		m_i48;
	CIconMap		m_m16;
	CIconMap		m_m32;
	CIconMap		m_m48;
};

extern CShellIcons ShellIcons;

// Predefined icons
enum
{
	SHI_FILE = 0,
	SHI_EXECUTABLE = 1,
	SHI_COMPUTER = 2,
	SHI_FOLDER_CLOSED = 3,
	SHI_FOLDER_OPEN = 4,
	SHI_LOCKED = 5
};

enum
{
	SHI_O_LOCKED = 1
};
