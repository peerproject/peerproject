//
// LibraryBuilderPlugins.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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


class CLibraryBuilderPlugins : private boost::noncopyable
{
// Construction
public:
	CLibraryBuilderPlugins();
	virtual ~CLibraryBuilderPlugins()=0;

// Attributes
private:
	typedef CMap< CString, LPCTSTR, ILibraryBuilderPlugin*, ILibraryBuilderPlugin* > CPluginMap;

	CCriticalSection	m_pSection;
	CPluginMap			m_pMap;

	ILibraryBuilderPlugin* LoadPlugin(LPCTSTR pszType);

// Operations
public:
	bool	ExtractPluginMetadata(DWORD nIndex, const CString& strPath, HANDLE hFile);
	void	CleanupPlugins();
};
