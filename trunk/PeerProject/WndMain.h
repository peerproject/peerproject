//
// WndMain.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "WindowManager.h"
#include "CtrlCoolBar.h"
#include "CtrlCoolMenuBar.h"
#include "CtrlMainTabBar.h"
#include "CtrlWndTabBar.h"
#include "CtrlMonitorBar.h"
#include "WndMonitor.h"
#include "WndHashProgressBar.h"
#include "PeerProjectDataSource.h"

class CChildWnd;
//class CURLActionDlg;


class CMainWnd : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CMainWnd)

public:
	CMainWnd();
	virtual ~CMainWnd();

public:
	CWindowManager		m_pWindows;

//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CMDIChildWnd* MDIGetActive(BOOL* pbMaximized = NULL) const;

	void	CloseToTray();							// Hide application to tray
	void	OpenFromTray(int nShowCmd = SW_SHOW);	// Open main window from tray
	void	OpenTrayMenu(); 						// Open context menu from tray
	void	ShowTrayPopup(const CString& sText, const CString& sTitle = CLIENT_NAME, DWORD dwIcon = NIIF_INFO, UINT uTimeout = 12 /*seconds*/);
	void	SetGUIMode(int nMode, BOOL bSaveState = TRUE);	// Set mode GUI_WINDOWED, GUI_TABBED or GUI_BASIC

	inline BOOL	IsForegroundWindow() const			// Test if window foreground or not
	{
		return ! m_bTrayHide && ! IsIconic() && IsWindowVisible();
	}

	void	OnUpdateCmdUI();						// Update tab and navigation bars

	// Dispatch command messages also to monitor bar and media frame (if opened)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	CCoolMenuBarCtrl	m_wndMenuBar;
	CMainTabBarCtrl		m_wndNavBar;
	CCoolBarCtrl		m_wndToolBar;
	CWndTabBar			m_wndTabBar;
	CStatusBar			m_wndStatusBar;
	CMonitorBarCtrl		m_wndMonitorBar;
	CRemoteWnd			m_wndRemoteWnd;
	CHashProgressBar	m_wndHashProgressBar;

#ifdef __ITaskbarList3_INTERFACE_DEFINED__	// VS2010+
	CComPtr< ITaskbarList3 > m_pTaskbar;	// Windows task bar
#endif
	NOTIFYICONDATA		m_pTray;			// Tray icon data
	BOOL				m_bTrayHide;		// Is main window hidden to tray?
	BOOL				m_bTrayIcon;		// Is tray icon available?
	BOOL				m_bTrayNotify;		// Is temporary notification balloon present?
	BOOL				m_bTrayUpdate;		// Does tray data need to be updated?
	BOOL				m_bTimer;
	CString				m_sMsgStatus;
	CBrush				m_brDockArea;
	CSkinWindow*		m_pSkin;
	DWORD				m_nAlpha;			// Main window transparency (0-255)

	void	UpdateMessages();				// Update main window tile, status bar and tray messages
	void	LocalSystemChecks();			// Run various networks, host caches and disk checks
	void	SaveState();					// Save all window states
	void	RemoveSkin();					// Remove skins from window (primarily for shutdown)
	void	AddTray();
	void	DeleteTray();

	// Snarl notifications (getsnarl.info)
	int		SnarlCommand(LPCSTR szCommand);
	bool	SnarlRegister();
	void	SnarlUnregister();
	bool	SnarlNotify(const CString& sText, const CString& sTitle, DWORD dwIcon, UINT uTimeout);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName, DWORD dwExStyle, CCreateContext* pContext);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void GetMessageString(UINT nID, CString& rMessage) const;

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg LRESULT OnWinsock(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHandleURL(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHandleImport(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHandleTorrent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHandleCollection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnVersionCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpenChat(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpenSearch(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTray(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeAlpha(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSkinChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMediaKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDevModeChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLibrarySearch(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdatePluginRange(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSystem(CCmdUI* pCmdUI);
	afx_msg void OnViewSystem();
	afx_msg void OnUpdateViewNeighbours(CCmdUI* pCmdUI);
	afx_msg void OnViewNeighbours();
	afx_msg void OnUpdateNetworkConnect(CCmdUI* pCmdUI);
	afx_msg void OnNetworkConnect();
	afx_msg void OnUpdateNetworkDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnNetworkDisconnect();
	afx_msg void OnUpdateViewPackets(CCmdUI* pCmdUI);
	afx_msg void OnViewPackets();
	afx_msg void OnUpdateViewHosts(CCmdUI* pCmdUI);
	afx_msg void OnViewHosts();
	afx_msg void OnNetworkConnectTo();
	afx_msg void OnNetworkBrowseTo();
	afx_msg void OnNetworkChatTo();
	afx_msg void OnUpdateViewSearchMonitor(CCmdUI* pCmdUI);
	afx_msg void OnViewSearchMonitor();
	afx_msg void OnNetworkExit();
	afx_msg void OnUpdateNetworkSearch(CCmdUI* pCmdUI);
	afx_msg void OnNetworkSearch();
	afx_msg void OnUpdateViewResultsMonitor(CCmdUI* pCmdUI);
	afx_msg void OnViewResultsMonitor();
	afx_msg void OnUpdateNetworkConnectTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDownloads(CCmdUI* pCmdUI);
	afx_msg void OnViewDownloads();
	afx_msg void OnUpdateViewLibrary(CCmdUI* pCmdUI);
	afx_msg void OnViewLibrary();
	afx_msg void OnUpdateViewUploads(CCmdUI* pCmdUI);
	afx_msg void OnViewUploads();
	afx_msg void OnToolsSettings();
	afx_msg void OnHelpAbout();
	afx_msg void OnHelpVersionCheck();
	afx_msg void OnHelpHomepage();
	afx_msg void OnHelpWeb1();
	afx_msg void OnHelpWeb2();
	afx_msg void OnHelpWeb3();
	afx_msg void OnHelpWeb4();
	afx_msg void OnHelpWeb5();
	afx_msg void OnHelpWeb6();
	afx_msg void OnHelpFaq();
	afx_msg void OnHelpConnectiontest();
	afx_msg void OnHelpGuide();
	afx_msg void OnHelpForums();
	afx_msg void OnHelpForumsLocal();
	afx_msg void OnHelpUpdate();
	afx_msg void OnHelpRouter();
	afx_msg void OnHelpSecurity();
//	afx_msg void OnHelpScheduler();
	afx_msg void OnHelpCodec();
	afx_msg void OnHelpDonate();
	afx_msg void OnHelpFakeShareaza();
	afx_msg void OnUpdateViewTraffic(CCmdUI* pCmdUI);
	afx_msg void OnViewTraffic();
	afx_msg void OnWindowCascade();
	afx_msg void OnToolsWizard();
	afx_msg void OnTrayOpen();
	afx_msg void OnUpdateNetworkAutoClose(CCmdUI* pCmdUI);
	afx_msg void OnNetworkAutoClose();
	afx_msg void OnUpdateToolsDownload(CCmdUI* pCmdUI);
	afx_msg void OnToolsDownload();
	afx_msg void OnUpdateToolsImportDownloads(CCmdUI* pCmdUI);
	afx_msg void OnToolsImportDownloads();
	afx_msg void OnUpdateOpenDownloadsFolder(CCmdUI* pCmdUI);
	afx_msg void OnOpenDownloadsFolder();
	afx_msg void OnUpdateViewSecurity(CCmdUI* pCmdUI);
	afx_msg void OnViewSecurity();
	afx_msg void OnUpdateViewScheduler(CCmdUI* pCmdUI);
	afx_msg void OnViewScheduler();
	afx_msg void OnUpdateWindowCascade(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowTileHorz(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowTileVert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTabConnect(CCmdUI* pCmdUI);
	afx_msg void OnTabConnect();
	afx_msg void OnUpdateTabNetwork(CCmdUI* pCmdUI);
	afx_msg void OnTabNetwork();
	afx_msg void OnUpdateTabLibrary(CCmdUI* pCmdUI);
	afx_msg void OnTabLibrary();
	afx_msg void OnUpdateTabTransfers(CCmdUI* pCmdUI);
	afx_msg void OnTabTransfers();
	afx_msg void OnUpdateViewTabbed(CCmdUI* pCmdUI);
	afx_msg void OnViewTabbed();
	afx_msg void OnUpdateViewWindowed(CCmdUI* pCmdUI);
	afx_msg void OnViewWindowed();
	afx_msg void OnUpdateViewDiscovery(CCmdUI* pCmdUI);
	afx_msg void OnViewDiscovery();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnUpdateTabHome(CCmdUI* pCmdUI);
	afx_msg void OnTabHome();
	afx_msg void OnToolsReskin();
	afx_msg void OnUpdateWindowTabBar(CCmdUI* pCmdUI);
	afx_msg void OnWindowTabBar();
	afx_msg void OnUpdateWindowToolBar(CCmdUI* pCmdUI);
	afx_msg void OnWindowToolBar();
	afx_msg void OnUpdateWindowMonitor(CCmdUI* pCmdUI);
	afx_msg void OnWindowMonitor();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnToolsLanguage();
	afx_msg void OnToolsSkin();
	afx_msg void OnToolsSeedTorrent();
	afx_msg void OnToolsReseedTorrent();
	afx_msg void OnToolsCreateTorrent();
	afx_msg void OnDiskSpace();
	afx_msg void OnDiskWriteFail();
	afx_msg void OnConnectionFail();
	afx_msg void OnNoDonkeyServers();
	afx_msg void OnUpdateViewMedia(CCmdUI* pCmdUI);
	afx_msg void OnViewMedia();
	afx_msg void OnUpdateTabMedia(CCmdUI* pCmdUI);
	afx_msg void OnTabMedia();
	afx_msg void OnUpdateTabIRC(CCmdUI* pCmdUI);
	afx_msg void OnTabIRC();
	afx_msg void OnUpdateTabSearch(CCmdUI* pCmdUI);
	afx_msg void OnTabSearch();
	afx_msg void OnToolsProfile();
	afx_msg void OnLibraryFolders();
	afx_msg void OnLibrarySearchBox();
	afx_msg void OnHelpWarnings();
	afx_msg void OnHelpPromote();
	afx_msg void OnUpdateNetworkG2(CCmdUI* pCmdUI);
	afx_msg void OnNetworkG2();
	afx_msg void OnUpdateNetworkG1(CCmdUI* pCmdUI);
	afx_msg void OnNetworkG1();
	afx_msg void OnUpdateNetworkED2K(CCmdUI* pCmdUI);
	afx_msg void OnNetworkED2K();
	afx_msg void OnUpdateNetworkDC(CCmdUI* pCmdUI);
	afx_msg void OnNetworkDC();
	afx_msg void OnUpdateNetworkBT(CCmdUI* pCmdUI);
	afx_msg void OnNetworkBT();
	afx_msg void OnUpdateViewBasic(CCmdUI* pCmdUI);
	afx_msg void OnViewBasic();
	afx_msg void OnUpdateLibraryHashPriority(CCmdUI* pCmdUI);
	afx_msg void OnLibraryHashPriority();
	afx_msg void OnUpdateWindowNavBar(CCmdUI *pCmdUI);
	afx_msg void OnWindowNavBar();
	afx_msg void OnUpdateWindowRemote(CCmdUI *pCmdUI);
	afx_msg void OnWindowRemote();
	afx_msg void OnRemoteClose();
	afx_msg void OnUpdateMediaCommand(CCmdUI *pCmdUI);
	afx_msg void OnMediaCommand();
	afx_msg void OnUpdateShell(CCmdUI* pCmdUI);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg LRESULT OnSanityCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNowUploading(WPARAM wParam, LPARAM lParam);
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, UINT nEventData);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);	// Windows scheduler not implemented

	DECLARE_MESSAGE_MAP()
	DECLARE_DROP()
};

#define IDW_TOOL_BAR		AFX_IDW_TOOLBAR
#define IDW_MENU_BAR		0xE810
#define IDW_NAV_BAR			0xE811
#define IDW_TAB_BAR			0xE812
#define IDW_MONITOR_BAR		0xE813
