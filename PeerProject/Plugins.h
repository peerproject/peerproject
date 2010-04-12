//
// Plugins.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2006.
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

class CPlugin;
class CChildWnd;


class CPlugins
{
public:
	CPlugins();
	virtual ~CPlugins();

public:
	CList< CPlugin* >	m_pList;
	UINT				m_nCommandID;

	void		Register(); 	// Register all plugins in PeerProject installation folder

	void		Enumerate();
	void		Clear();
	BOOL		LookupCLSID(LPCTSTR pszGroup, LPCTSTR pszKey, CLSID& pCLSID) const;
	BOOL		LookupEnable(REFCLSID pCLSID, LPCTSTR pszExt = NULL) const;
	CPlugin*	Find(REFCLSID pCLSID) const;
	UINT		GetCommandID();

	// IGeneralPlugin mirroring
	void		OnSkinChanged();
	// ICommandPlugin mirroring
	void		RegisterCommands();
	void		InsertCommands();
	BOOL		OnCommand(CChildWnd* pActiveWnd, UINT nCommandID);
	BOOL		OnUpdate(CChildWnd* pActiveWnd, CCmdUI* pCmdUI);
	// IExecutePlugin mirroring
	BOOL		OnExecuteFile(LPCTSTR pszFile, BOOL bUseImageViewer = FALSE);
	BOOL		OnEnqueueFile(LPCTSTR pszFile);
	// IChatPlugin mirroring
	BOOL		OnChatMessage(LPCTSTR pszChatID, BOOL bOutgoing, LPCTSTR pszFrom, LPCTSTR pszTo, LPCTSTR pszMessage);

	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CPlugin* GetNext(POSITION& pos) const
	{
		return m_pList.GetNext( pos );
	}

	inline INT_PTR GetCount() const
	{
		return m_pList.GetCount();
	}
};


class CPlugin
{
public:
	CPlugin(REFCLSID pCLSID, LPCTSTR pszName);
	virtual ~CPlugin();

	CLSID						m_pCLSID;
	CString						m_sName;
	DWORD						m_nCapabilities;
	CComPtr< IGeneralPlugin >	m_pPlugin;
	CComPtr< ICommandPlugin >	m_pCommand;
	CComPtr< IExecutePlugin >	m_pExecute;
	CComPtr< IChatPlugin >		m_pChat;

	void		Stop();
	BOOL		Start();
	BOOL		StartIfEnabled();
	CString		GetStringCLSID() const;
	HICON		LookupIcon() const;
};

extern CPlugins Plugins;
