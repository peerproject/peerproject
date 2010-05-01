//
// HttpRequest.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#include "ThreadImpl.h"

class CBuffer;


class CHttpRequest :
	public CThreadImpl
{
// Constructions
public:
	CHttpRequest();
	virtual ~CHttpRequest();

// Operations
public:
	void		Clear();
	CString		GetURL() const;
	BOOL		SetURL(LPCTSTR pszURL);
	void		SetUserAgent(LPCTSTR pszUserAgent);
	void		AddHeader(LPCTSTR pszKey, LPCTSTR pszValue);
	void		SetPostData(LPCVOID pBody, DWORD nBody);
	void		LimitContentLength(DWORD nLimit);
	void		SetNotify(HWND hWnd, UINT nMsg, WPARAM wParam = 0);
	int			GetStatusCode() const;
	bool		GetStatusSuccess() const;
	CString		GetStatusString() const;
	CString		GetHeader(LPCTSTR pszName) const;
	CString		GetResponseString(UINT nCodePage = CP_UTF8) const;
	CBuffer*	GetResponseBuffer() const;
	BOOL		InflateResponse();
	bool		Execute(bool bBackground);
	BOOL		IsPending() const;
	BOOL		IsFinished() const;
	void		EnableCookie(bool bEnable);
	void		Cancel();

// Data
public:
	CString		m_sURL;
protected:
	HINTERNET	m_hInternet;
	CString		m_sUserAgent;
	CString		m_sRequestHeaders;
	bool		m_bUseCookie;
	DWORD		m_nLimit;
	int			m_nStatusCode;
	CString		m_sStatusString;
//	CBuffer*	m_pPost;
	CBuffer*	m_pResponse;
	HWND		m_hNotifyWnd;
	UINT		m_nNotifyMsg;
	WPARAM		m_nNotifyParam;
	typedef CMap< CString, const CString&, CString, CString& > Map;
	Map			m_pResponseHeaders;

	void		OnRun();

private:
	CHttpRequest(const CHttpRequest&);
	CHttpRequest& operator=(const CHttpRequest&);
};
