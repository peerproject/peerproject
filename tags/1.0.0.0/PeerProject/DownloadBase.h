//
// DownloadBase.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "SharedFile.h"
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

protected:
	int				m_nCookie;
	int				m_nSaveCookie;
	CDownloadTask*	m_pTask;

public:
	void			SetModified();
	bool			IsModified() const;

	dtask			GetTaskType() const;
	void			SetTask(CDownloadTask* pTask);
	bool			CheckTask(CDownloadTask* pTask) const;
	void			AbortTask();

	virtual bool	Rename(const CString& strName) = 0;		// Set download new name safely (DownloadWithFile)
	virtual bool	IsMoving() const;
	virtual bool	IsTasking() const;	// Check if a task is already running
	virtual bool	IsTrying() const = 0;
	virtual bool	IsCompleted() const = 0;
	virtual bool	IsPaused(bool bRealState = false) const = 0;
	virtual BOOL	OnVerify(const CLibraryFile* pFile, TRISTATE bVerified) = 0;	// File was hashed and verified in the Library
	virtual void	OnTaskComplete(const CDownloadTask* pTask) = 0;	// Task callback

protected:
	virtual void	Serialize(CArchive& ar, int nVersion);
};
