//
// DownloadBase.h
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

#include "PeerProjectFile.h"

class CDownloadTask;


class CDownloadBase : public CPeerProjectFile
{
// Construction
protected:
	CDownloadBase();
	virtual ~CDownloadBase();

// Attributes
public:
	bool		m_bSHA1Trusted;
	bool		m_bTigerTrusted;
	bool		m_bED2KTrusted;
	bool		m_bBTHTrusted;
	bool		m_bMD5Trusted;
	int			m_nCookie;
public:
	CString		m_sSearchKeyword;			// Search keyword to override G1 keyword search.
	CString		m_sSafeName;				// The name, with invalid characters removed. (A meaningful local disk name)
	CString		m_sPath;					// The name and path of the incomplete file on disk (the .partial). 
											// The .sd will be the same as above with ".sd" on the end
protected:
	CDownloadTask*	m_pTask;

// Operations
public:
	BOOL			IsTasking() const { return m_pTask != NULL; }
	BOOL			SetNewTask(CDownloadTask* pTask);
	void			SetModified();
protected:
	virtual BOOL	IsCompleted() const = 0;
	virtual BOOL	IsMoving() const = 0;
	virtual BOOL	IsPaused(BOOL bRealState = FALSE) const = 0;
	virtual BOOL	IsTrying() const = 0;
	void			GenerateDiskName(bool bTorrent = false);
	virtual void	Serialize(CArchive& ar, int nVersion);
};
