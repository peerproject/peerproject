//
// Remote.h
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

#pragma once

#include "Buffer.h"
#include "Transfer.h"

class CMatchFile;


class CRemote : public CTransfer
{
// Construction
public:
	CRemote(CConnection* pConnection);
	~CRemote();

// Attributes
protected:
	CString				m_sHandshake;
	CString				m_sRedirect;
	CString				m_sHeader;
	CString				m_sResponse;
	CBuffer				m_pResponse;
	CMap< CString, const CString&, CString, const CString& >	m_pKeys;
	static CList<int>	m_pCookies;

// Operations
public:
	virtual BOOL	OnRun();
	virtual void	OnDropped();
	virtual BOOL	OnRead();
	virtual BOOL	OnHeadersComplete();
protected:
	CString			GetKey(LPCTSTR pszName);
	BOOL			CheckCookie();
	BOOL			RemoveCookie();
	void			Prepare(LPCTSTR pszPrefix = NULL);
	void			Add(LPCTSTR pszKey, LPCTSTR pszValue);
	void			Output(LPCTSTR pszName);

// Page Handlers
protected:
	void	PageSwitch(CString& strPath);
	void	PageLogin();
	void	PageLogout();
	void	PageHome();
	void	PageSearch();
	void	PageNewSearch();
	void	PageDownloads();
	void	PageNewDownload();
	void	PageUploads();
	void	PageNetwork();
	void	PageBanner(CString& strPath);
	void	PageImage(CString& strPath);

// Utilities
protected:
	void	PageSearchHeaderColumn(int nColumnID, LPCTSTR pszCaption, LPCTSTR pszAlign);
	void	PageSearchRowColumn(int nColumnID, CMatchFile* pFile, LPCTSTR pszValue, LPCTSTR pszAlign = _T("center"));
	void	PageNetworkNetwork(int nID, bool* pbConnect, LPCTSTR pszName);
};
