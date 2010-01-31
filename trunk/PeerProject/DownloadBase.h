//
// DownloadBase.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "DownloadTask.h"


class CDownloadBase : public CPeerProjectFile
{
	DECLARE_DYNAMIC(CDownloadBase)

protected:
	CDownloadBase();
	virtual ~CDownloadBase();

public:
	bool			m_bSHA1Trusted;		// True if SHA1 hash is trusted
	bool			m_bTigerTrusted;	// True if TTH hash is trusted
	bool			m_bED2KTrusted;		// True if ED2K hash is trusted
	bool			m_bBTHTrusted;		// True if BTH hash is trusted
	bool			m_bMD5Trusted;		// True if MD5 hash is trusted

	void			SetModified();
	bool			IsModified() const;
	bool			Rename(const CString& strName);	// Set download safe name

	virtual bool	IsMoving() const;
	virtual bool	IsTasking() const;	// Check if a task is already running
	virtual bool	IsTrying() const = 0;
	virtual bool	IsCompleted() const = 0;
	virtual bool	IsPaused(bool bRealState = false) const = 0;
	virtual void	OnTaskComplete(const CDownloadTask* pTask) = 0;	// Task callback

	dtask			GetTaskType() const;
	void			SetTask(CDownloadTask* pTask);
	bool			CheckTask(CDownloadTask* pTask) const;
	void			AbortTask();

protected:
	int				m_nCookie;
	int				m_nSaveCookie;
	CDownloadTask*	m_pTask;

	virtual void	Serialize(CArchive& ar, int nVersion);
};
