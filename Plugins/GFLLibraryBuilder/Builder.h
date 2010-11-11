//
// Builder.h : Declaration of the CBuilder
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Copyright (c) Nikolay Raspopov, 2005.
//
// GFL Library, GFL SDK and XnView
// Copyright (c) 1991-2004 Pierre-E Gougelet
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
#include "GFLLibraryBuilder.h"

class ATL_NO_VTABLE CBuilder : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CBuilder, &CLSID_Builder>,
	public ILibraryBuilderPlugin
{
public:
	CBuilder () throw()
	{
		m_pUnkMarshaler = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BUILDER)

BEGIN_COM_MAP(CBuilder)
	COM_INTERFACE_ENTRY(ILibraryBuilderPlugin)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()
DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct () throw();
	void FinalRelease () throw();

	CComPtr<IUnknown> m_pUnkMarshaler;

// ILibraryBuilderPlugin
public:
	STDMETHOD(Process)(/*[in]*/ BSTR sFile, /*[in]*/ ISXMLElement* pXML);
};

OBJECT_ENTRY_AUTO(__uuidof(Builder), CBuilder)
