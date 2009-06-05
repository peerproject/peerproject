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
protected:
	CDownloadBase();
	virtual ~CDownloadBase();

public:
	bool			m_bSHA1Trusted;		// True if SHA1 hash is trusted
	bool			m_bTigerTrusted;
	bool			m_bED2KTrusted;
	bool			m_bBTHTrusted;
	bool			m_bMD5Trusted;
	int				m_nCookie;
	CString			m_sSearchKeyword;	// Search keyword to override G1 keyword search.
private:
	CDownloadTask*	m_pTask;

// Operations
public:
	bool		IsTasking() const;						// Check if a task is already running
	void		SetTask(CDownloadTask* pTask);
	DWORD		GetTaskType() const;
	bool		CheckTask(CDownloadTask* pTask) const;
	void		AbortTask();
	void		SetModified();

protected:
	virtual bool	IsCompleted() const = 0;
	virtual bool	IsMoving() const = 0;
	virtual bool	IsPaused(bool bRealState = false) const = 0;
	virtual bool	IsTrying() const = 0;
	virtual void	Serialize(CArchive& ar, int nVersion);
};
