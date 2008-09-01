//
// Class.h : Declaration of the CClass
//
// This file is part of PeerProject (peerproject.org) © 2008
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
#include "SkinScan.h"

class ATL_NO_VTABLE CSkinScan : 
	public CComObjectRootEx< CComMultiThreadModel >,
	public CComCoClass< CSkinScan, &CLSID_SkinScan >,
	public ILibraryBuilderPlugin
{
public:
	CSkinScan() throw()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CLASS)

BEGIN_COM_MAP(CSkinScan)
	COM_INTERFACE_ENTRY(ILibraryBuilderPlugin)
END_COM_MAP()

// ILibraryBuilderPlugin
public:
	STDMETHOD(Process)(
		/* [in] */ HANDLE hFile,
		/* [in] */ BSTR sFile,
		/* [in] */ ISXMLElement* pXML);

protected:
	BOOL	ScanFile(LPCSTR pszXML, ISXMLElement* pOutput);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinScan), CSkinScan)
