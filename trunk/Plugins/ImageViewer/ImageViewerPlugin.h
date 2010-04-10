//
// ImageViewerPlugin.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Original author Michael Stokes released portions into the public domain.
// You are free to redistribute and modify this page without any restrictions.
//

#pragma once

#include "Resource.h"
#include "ImageViewer.h"

class CImageWindow;


class ATL_NO_VTABLE CImageViewerPlugin : 
	public CComObjectRootEx< CComSingleThreadModel >,
	public CComCoClass< CImageViewerPlugin, &CLSID_ImageViewerPlugin >,
	public IGeneralPlugin,
	public IExecutePlugin,
	public ICommandPlugin
{
// Construction
public:
	CImageViewerPlugin();
	virtual ~CImageViewerPlugin();

// Attributes
public:
	CComPtr<IApplication>	m_pApplication;
	CComPtr<IUserInterface>	m_pInterface;

	CImageWindow*	m_pWindow;
	HCURSOR			m_hcMove;

	UINT			m_nCmdBestFit;
	UINT			m_nCmdActualSize;
	UINT			m_nCmdRefresh;
	UINT			m_nCmdClose;

// Operations
public:
	BOOL			OpenNewWindow(LPCTSTR pszFilePath);
	void			RemoveWindow(CImageWindow* pWindow);
	
// Interfaces
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_IMAGEVIEWER)

	BEGIN_COM_MAP(CImageViewerPlugin)
		COM_INTERFACE_ENTRY(IGeneralPlugin)
		COM_INTERFACE_ENTRY(IExecutePlugin)
		COM_INTERFACE_ENTRY(ICommandPlugin)
	END_COM_MAP()

// IGeneralPlugin
protected:
	virtual HRESULT STDMETHODCALLTYPE SetApplication( 
		/* [in] */ IApplication __RPC_FAR *pApplication);
	virtual HRESULT STDMETHODCALLTYPE QueryCapabilities(
		/* [in] */ DWORD __RPC_FAR *pnCaps);
	virtual HRESULT STDMETHODCALLTYPE Configure();
	virtual HRESULT STDMETHODCALLTYPE OnSkinChanged();

// IExecutePlugin
protected:
	virtual HRESULT STDMETHODCALLTYPE OnExecute(
		/* [in] */ BSTR sFilePath );
	virtual HRESULT STDMETHODCALLTYPE OnEnqueue(
		/* [in] */ BSTR sFilePath );

// ICommandPlugin
protected:
	virtual HRESULT STDMETHODCALLTYPE RegisterCommands();
	virtual HRESULT STDMETHODCALLTYPE InsertCommands();
	virtual HRESULT STDMETHODCALLTYPE OnUpdate( 
		/* [in] */ UINT nCommandID,
		/* [out][in] */ TRISTATE __RPC_FAR *pbVisible,
		/* [out][in] */ TRISTATE __RPC_FAR *pbEnabled,
		/* [out][in] */ TRISTATE __RPC_FAR *pbChecked);
	virtual HRESULT STDMETHODCALLTYPE OnCommand( 
		/* [in] */ UINT nCommandID);

};

