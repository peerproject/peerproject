//
// GFLReader.h : Declaration of the CGFLReader
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Nikolay Raspopov, 2005.
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

#include "resource.h"
#include "GFLImageServices.h"

class ATL_NO_VTABLE CGFLReader : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGFLReader, &CLSID_GFLReader>,
	public IImageServicePlugin
{
public:
	CGFLReader () throw()
	{
		m_pUnkMarshaler = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_GFLREADER)

BEGIN_COM_MAP(CGFLReader)
	COM_INTERFACE_ENTRY(IImageServicePlugin)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()
DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct () throw();
	void FinalRelease () throw();

	CComPtr<IUnknown> m_pUnkMarshaler;

// IImageServicePlugin
public:
	STDMETHOD(LoadFromFile)(
		/* [in] */ BSTR sFile,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [out] */ SAFEARRAY** ppImage );
	STDMETHOD(LoadFromMemory)(
		/* [in] */ BSTR sType,
		/* [in] */ SAFEARRAY* pMemory,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [out] */ SAFEARRAY** ppImage );
	STDMETHOD(SaveToFile)(
		/* [in] */ BSTR sFile,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [in] */ SAFEARRAY* pImage);
	STDMETHOD(SaveToMemory)(
		/* [in] */ BSTR sType,
		/* [out] */ SAFEARRAY** ppMemory,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [in] */ SAFEARRAY* pImage);
};

OBJECT_ENTRY_AUTO(__uuidof(GFLReader), CGFLReader)
