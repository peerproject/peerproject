//
// VersionChecker.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "HttpRequest.h"


class CVersionChecker :
	public CThreadImpl
{
// Construction
public:
	CVersionChecker();
	virtual ~CVersionChecker();

// Attributes
public:
	CString		m_sMessage;
	CString		m_sUpgradePath;

protected:
	bool			m_bVerbose;
	CHttpRequest	m_pRequest;
	CMap< CString, const CString&, CString, CString& >	m_pResponse;

// Operations
public:
	BOOL		Start();
	void		Stop();
	static void ClearVersionCheck();
	void		ForceCheck();
	void		SetNextCheck(int nDays);
	BOOL		CheckUpgradeHash(const Hashes::Sha1Hash& oHash, LPCTSTR pszPath);
	BOOL		CheckUpgradeHash();

	inline bool	IsUpgradeAvailable() const throw()
	{
		return ! Settings.VersionCheck.UpgradePrompt.IsEmpty();
	}

	inline bool IsVerbose() const throw()
	{
		return m_bVerbose;
	}

protected:
	BOOL		NeedToCheck();
	void		OnRun();
	BOOL		ExecuteRequest();
	void		ProcessResponse();
};

extern CVersionChecker VersionChecker;

enum VERSION_CHECK	// WM_VERSIONCHECK message wParam argument
{
	VC_MESSAGE_AND_CONFIRM = 0, // Show message and then ask to download new version
	VC_CONFIRM = 1,				// Ask to download new version
	VC_UPGRADE = 2				// Ask then start upgrading of already downloaded installer
};