//
// Object.h : Declaration of the CWebHook
//
// This file is part of PeerProject (peerproject.org) © 2009-2010
// Portions Copyright Shareaza Development Team, 2009.
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
#include "WebHook_i.h"

// CWebHook

typedef IDispatchImpl< IWebHook, &IID_IWebHook, &LIBID_WebHookLib, /*wMajor =*/ 1, /*wMinor =*/ 0 > IWebHookDispatchImpl;

class ATL_NO_VTABLE CWebHook :
	public CComObjectRootEx< CComMultiThreadModel >,
	public CComCoClass< CWebHook, &CLSID_WebHook >,
	public IWebHookDispatchImpl,
	public IObjectWithSiteImpl< CWebHook >
{
public:
	CWebHook();

DECLARE_REGISTRY_RESOURCEID(IDR_OBJECT)

BEGIN_COM_MAP(CWebHook)
	COM_INTERFACE_ENTRY(IWebHook)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, IDispatch)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()
DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct();
	void FinalRelease();

protected:
	CComPtr < IUnknown >	m_pUnkMarshaler;
	DWORD					m_dwCookie;
	CString					m_sURL;

	bool IsEnabled() const;
	bool IsHooked(const CString& sExt) const;
	void Connect();
	void Disconnect();
	void AddLink(const CString& sURL);

// IDispatchImpl
	STDMETHOD(Invoke)(
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr);

// IObjectWithSite
	STDMETHOD(SetSite)(
		/* [in] */ IUnknown* pUnkSite);

// IWebHook
	STDMETHOD(AddLink)(
		/* [in] */ VARIANT oLink);
};

OBJECT_ENTRY_AUTO(__uuidof(WebHook), CWebHook)
