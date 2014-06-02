//
// PreviewPlugin.h : Declaration of the CPreviewPlugin
//
// This file is part of PeerProject (peerproject.org) © 2009-2010
// Portions previously copyright Nikolay Raspopov, 2009.
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

#include "Resource.h"
#include "Preview.h"


// CPreviewPlugin

class ATL_NO_VTABLE CPreviewPlugin :
	public CComObjectRootEx< CComMultiThreadModel >,
	public CComCoClass< CPreviewPlugin, &CLSID_PreviewPlugin >,
	public IGeneralPlugin,
	public IDownloadPreviewPlugin2
{
public:
	CPreviewPlugin();

DECLARE_REGISTRY_RESOURCEID(IDR_PREVIEWPLUGIN)

BEGIN_COM_MAP(CPreviewPlugin)
	COM_INTERFACE_ENTRY(IGeneralPlugin)
	COM_INTERFACE_ENTRY(IDownloadPreviewPlugin2)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()
DECLARE_GET_CONTROLLING_UNKNOWN()

public:
	HRESULT FinalConstruct();
	void FinalRelease();

protected:
	CComPtr<IUnknown>				m_pUnkMarshaler;
	CComPtr<IDownloadPreviewSite>	m_pSite;
	bool							m_bCancel;		// Got cancel request
	CHandle							m_hProcess;		// External process handler

	bool Execute(LPCTSTR szCommand);

public:
// IGeneralPlugin
	STDMETHOD(SetApplication)(/* [in] */ IApplication* pApplication);
	STDMETHOD(QueryCapabilities)(/* [out] */ DWORD* pnCaps);
	STDMETHOD(Configure)(void);
	STDMETHOD(OnSkinChanged)(void);

// IDownloadPreviewPlugin
	STDMETHOD(SetSite)(/* [in] */ IDownloadPreviewSite* pSite);
	STDMETHOD(Preview)(/* [in] */ HANDLE hFile, /* [in */ BSTR sTarget);
	STDMETHOD(Cancel)();

// IDownloadPreviewPlugin2
	STDMETHOD(Preview2)(/* [in] */ BSTR sSource, /* [in] */ BSTR sTarget);
};

OBJECT_ENTRY_AUTO(__uuidof(PreviewPlugin), CPreviewPlugin)
