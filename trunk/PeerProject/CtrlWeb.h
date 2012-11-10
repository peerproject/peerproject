//
// CtrlWeb.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

class CCoolMenu;


class CWebCtrl : public CWnd
{
	DECLARE_DYNAMIC(CWebCtrl)

// Construction
public:
	CWebCtrl();
	virtual ~CWebCtrl();

// Operations
public:
	BOOL	Create(DWORD dwStyle, CWnd* pParentWnd, UINT nID = AFX_IDW_PANE_FIRST);
	void	EnableCoolMenu(BOOL bEnable = TRUE);
	void	EnableSandbox(BOOL bSandbox = TRUE);
	void	SetExternal(IDispatch* pDispatch);
	HRESULT	Navigate(LPCTSTR lpszURL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);

// Data Members
protected:
	CWnd					m_wndBrowser;
	CComPtr<IWebBrowser2>	m_pBrowser;
	CComPtr<IDispatch>		m_pExternal;
	BOOL					m_bSandbox;
	DWORD					m_tFrame;

// Menu Driver
protected:
	CCoolMenu*				m_pMenu;
	HWND					m_hWndThis;
	WNDPROC					m_pWndProc;
	static CWebCtrl*		m_pThis;

	void					EnterMenu(POINT* pPoint);
	static LRESULT PASCAL	WebWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

// Event Handlers
protected:
	DECLARE_EVENTSINK_MAP()
	virtual void BeforeNavigate2(LPDISPATCH pDispatch, VARIANT* pvURL, VARIANT* pvFlags, VARIANT* pvTargetFrameName, VARIANT* pvPostData, VARIANT* pvHeaders, VARIANT_BOOL* pvCancel);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, VARIANT_BOOL* bCancel);

// Message Map
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

// Control Site
public:
	class DocSite : public COleControlSite
	{
	public:
		DocSite(CWebCtrl* pCtrl, COleControlContainer* pParentWnd);
		CWebCtrl* GetCtrl() const { return m_pCtrl; }
		CWebCtrl* m_pCtrl;
	protected:
		DECLARE_INTERFACE_MAP()
		BEGIN_INTERFACE_PART(DocHostUIHandler, IDocHostUIHandler)
			STDMETHOD(ShowContextMenu)(DWORD, LPPOINT, LPUNKNOWN, LPDISPATCH);
			STDMETHOD(GetHostInfo)(DOCHOSTUIINFO*);
			STDMETHOD(ShowUI)(DWORD, LPOLEINPLACEACTIVEOBJECT, LPOLECOMMANDTARGET, LPOLEINPLACEFRAME, LPOLEINPLACEUIWINDOW);
			STDMETHOD(HideUI)();
			STDMETHOD(UpdateUI)();
			STDMETHOD(EnableModeless)(BOOL);
			STDMETHOD(OnDocWindowActivate)(BOOL);
			STDMETHOD(OnFrameWindowActivate)(BOOL);
			STDMETHOD(ResizeBorder)(LPCRECT, LPOLEINPLACEUIWINDOW, BOOL);
			STDMETHOD(TranslateAccelerator)(LPMSG, const GUID*, DWORD);
			STDMETHOD(GetOptionKeyPath)(OLECHAR **, DWORD);
			STDMETHOD(GetDropTarget)(LPDROPTARGET, LPDROPTARGET*);
			STDMETHOD(GetExternal)(LPDISPATCH*);
			STDMETHOD(TranslateUrl)(DWORD, OLECHAR*, OLECHAR **);
			STDMETHOD(FilterDataObject)(LPDATAOBJECT, LPDATAOBJECT*);
		END_INTERFACE_PART(DocHostUIHandler)
		BEGIN_INTERFACE_PART(DocHostShowUI, IDocHostShowUI)
			STDMETHOD(ShowHelp)(HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch *pDispatchObjectHit);
			STDMETHOD(ShowMessage)(HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT *plResult);
		END_INTERFACE_PART(DocHostShowUI)
		BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
			STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void **ppv);
		END_INTERFACE_PART(ServiceProvider)
		BEGIN_INTERFACE_PART(InternetSecurityManager, IInternetSecurityManager)
			STDMETHOD(GetSecurityId)(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved);
			STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite **ppSite);
			STDMETHOD(GetZoneMappings)(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags);
			STDMETHOD(MapUrlToZone)(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags);
			STDMETHOD(ProcessUrlAction)(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved);
			STDMETHOD(QueryCustomPolicy)(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved);
			STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite *pSite);
			STDMETHOD(SetZoneMapping)(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags);
		END_INTERFACE_PART(InternetSecurityManager)
	};

	virtual BOOL CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT nID, REFCLSID clsid);
};

typedef struct
{
	NMHDR			hdr;
	DWORD			dwMenuID;
	POINT			ptMouse;
	IUnknown*		pCmdTarget;
	IDispatch*		pContext;
} WVNCONTEXTMENU;

#define WVN_CONTEXTMENU		100
