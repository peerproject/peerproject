//
// DlgDownloadMonitor.h
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

#if !defined(AFX_DLGDOWNLOADMONITOR_H__46BFBDED_8DCF_4E08_B224_8718FA19E7BF__INCLUDED_)
#define AFX_DLGDOWNLOADMONITOR_H__46BFBDED_8DCF_4E08_B224_8718FA19E7BF__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"

class CDownload;
class CLineGraph;
class CGraphItem;


class CDownloadMonitorDlg : public CSkinDialog
{
// Construction
public:
	CDownloadMonitorDlg(CDownload* pDownload);
	virtual ~CDownloadMonitorDlg();

// Attributes
public:
	CDownload*		m_pDownload;
	CString			m_sName;
	CLineGraph*		m_pGraph;
	CGraphItem*		m_pItem;
	BOOL			m_bTray;
	NOTIFYICONDATA	m_pTray;
	BOOL			m_bCompleted;
public:
	static CList< CDownloadMonitorDlg* >	m_pWindows;

// Operatons
protected:
	BOOL	CreateReal(UINT nID);
	void	Update(CWnd* pWnd, LPCTSTR pszText);
	void	Update(CWnd* pWnd, BOOL bEnabled);
	void	DoPaint(CDC& dc);
	void	DrawProgressBar(CDC* pDC, CRect* pRect);
public:
	static void OnSkinChange(BOOL bSet);
	static void CloseAll();

// Dialog Data
public:
	//{{AFX_DATA(CDownloadMonitorDlg)
	enum { IDD = IDD_DOWNLOAD_MONITOR };
	CStatic	m_wndVolume;
	CStatic m_wndVolumeLabel;
	CButton	m_wndCancel;
	CButton	m_wndClose;
	CButton	m_wndStop;
	CStatic	m_wndProgress;
	CStatic	m_wndTime;
	CStatic	m_wndTimeLabel;
	CStatic	m_wndStatus;
	CStatic	m_wndSpeed;
	CStatic	m_wndSpeedLabel;
	CStatic	m_wndSources;
	CStatic	m_wndSourcesLabel;
	CButton	m_wndLaunch;
	CButton	m_wndView;
	CStatic	m_wndIcon;
	CStatic	m_wndGraph;
	CStatic	m_wndFile;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CDownloadMonitorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDownloadMonitorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDownloadCancel();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDownloadLaunch();
	afx_msg void OnDownloadView();
	afx_msg void OnDownloadStop();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg LRESULT OnTray(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNeedText(UINT nID, NMHDR* pTTT, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGDOWNLOADMONITOR_H__46BFBDED_8DCF_4E08_B224_8718FA19E7BF__INCLUDED_)
