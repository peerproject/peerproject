//
// Class.h : Declaration of the CClass
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2007.
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
#include "ZIPBuilder.h"

class ATL_NO_VTABLE CZIPBuilder :
	public CComObjectRootEx< CComMultiThreadModel >,
	public CComCoClass< CZIPBuilder, &CLSID_ZIPBuilder >,
	public ILibraryBuilderPlugin
{
public:
	CZIPBuilder() throw()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CLASS)

BEGIN_COM_MAP(CZIPBuilder)
	COM_INTERFACE_ENTRY(ILibraryBuilderPlugin)
END_COM_MAP()

// ILibraryBuilderPlugin
public:
	STDMETHOD(Process)(/*[in]*/ BSTR sFile, /*[in]*/ ISXMLElement* pXML);
};

OBJECT_ENTRY_AUTO(__uuidof(ZIPBuilder), CZIPBuilder)
