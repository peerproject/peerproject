//
// DlgDonkeyServers.h
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

#include "ThreadImpl.h"
#include "DlgSkinDialog.h"


class CDonkeyServersDlg :
	public CSkinDialog,
	public CThreadImpl
{
// Construction
public:
	CDonkeyServersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDonkeyServersDlg();

// Dialog Data
public:
	//{{AFX_DATA(CDonkeyServersDlg)
	enum { IDD = IDD_DONKEY_SERVERS };
	CEdit	m_wndURL;
	CButton	m_wndOK;
	CProgressCtrl	m_wndProgress;
	CString	m_sURL;
	//}}AFX_DATA

// Attributes
public:
	HINTERNET	m_hInternet;

// Operations
protected:
	void			OnRun();

// Overrides
public:
	//{{AFX_VIRTUAL(CDonkeyServersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDonkeyServersDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeURL();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
