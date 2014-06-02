//
// Plugin.h : Declaration of the CPlugin
//
// This file is part of PeerProject (peerproject.org) © 2009-2010
// Portions Copyright Nikolay Raspopov, 2009.
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

#include "VirusTotal.h"

// CPlugin

class ATL_NO_VTABLE CPlugin :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CPlugin, &CLSID_Plugin>,
	public IGeneralPlugin,
	public ICommandPlugin
{
public:
	CPlugin() :
		m_nCmdCheck( 0 )
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PLUGIN)

BEGIN_COM_MAP(CPlugin)
	COM_INTERFACE_ENTRY(IGeneralPlugin)
	COM_INTERFACE_ENTRY(ICommandPlugin)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		m_pApplication.Release();
		m_pUserInterface.Release();
	}

protected:
	CComPtr< IApplication >		m_pApplication;		// PeerProject application
	CComPtr< IUserInterface >	m_pUserInterface;	// PeerProject GUI
	UINT						m_nCmdCheck;		// "VersionTotal Check" command ID

	// Insert menu item if no item present only
	static void InsertCommand(ISMenu* pWebMenu, int nPos, UINT nID, LPCWSTR szItem);

	// Run InternetExplorer
	HRESULT Request(LPCWSTR szHash);

// IGeneralPlugin
public:
	STDMETHOD(SetApplication)(
		/* [in] */ IApplication __RPC_FAR *pApplication);
	STDMETHOD(QueryCapabilities)(
		/* [in] */ DWORD __RPC_FAR *pnCaps);
	STDMETHOD(Configure)();
	STDMETHOD(OnSkinChanged)();

// ICommandPlugin
public:
	STDMETHOD(RegisterCommands)();
	STDMETHOD(InsertCommands)();
	STDMETHOD(OnUpdate)(
		/* [in] */ UINT nCommandID,
		/* [out][in] */ TRISTATE __RPC_FAR *pbVisible,
		/* [out][in] */ TRISTATE __RPC_FAR *pbEnabled,
		/* [out][in] */ TRISTATE __RPC_FAR *pbChecked);
	STDMETHOD(OnCommand)(
		/* [in] */ UINT nCommandID);
};

OBJECT_ENTRY_AUTO(__uuidof(Plugin), CPlugin)
