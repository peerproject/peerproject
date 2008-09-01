//
// CoolMenu.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
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

class CShellItem
{
public:
	CShellItem(LPCTSTR szFullPath, void** ppFolder = NULL, HWND hWnd = NULL) :
		m_pidl( NULL ),
		m_pLastId( NULL )
	{
		CComPtr< IShellFolder > pDesktop;
		HRESULT hr = SHGetDesktopFolder( &pDesktop );
		if ( FAILED( hr ) )
			return;

		hr = pDesktop->ParseDisplayName( hWnd, 0, CT2OLE( szFullPath ), NULL,
			&m_pidl, NULL );
		if ( FAILED( hr ) )
			return;

		m_pLastId = m_pidl;
		USHORT temp;
		for (;;)
		{
			USHORT offset = m_pLastId->mkid.cb;
			temp = *(USHORT*)( (BYTE*)m_pLastId + offset );
			if ( temp == 0 )
				break;
			m_pLastId = (LPITEMIDLIST)( (BYTE*)m_pLastId + offset );
		}
		if ( ppFolder )
		{
			temp = m_pLastId->mkid.cb;
			m_pLastId->mkid.cb = 0;
			hr = pDesktop->BindToObject( m_pidl, NULL, IID_IShellFolder, ppFolder );
			m_pLastId->mkid.cb = temp;
		}
	}

	~CShellItem()
	{
		if ( m_pidl )
			CoTaskMemFree( m_pidl );
	}

	inline operator LPITEMIDLIST() const throw()
	{
		return m_pidl;
	}

public:
	LPITEMIDLIST	m_pidl;		// Full path
	LPITEMIDLIST	m_pLastId;	// Filename only
};

class CShellList :
	public CList< CShellItem* >
{
public:
	CShellList(const CStringList& oFiles) :
		m_pID( NULL )
	{
		for ( POSITION pos = oFiles.GetHeadPosition(); pos; )
		{
			CString strPath = oFiles.GetNext( pos );
			CShellItem* pItemIDList = new CShellItem( strPath,
				( m_pFolder ? NULL : (void**)&m_pFolder ) );	// Get only one
			if ( pItemIDList->m_pidl )
				AddTail( pItemIDList );
			else
				// Bad path
				delete pItemIDList;
		}

		if ( GetCount() == 0 )
			// No files
			return;

		m_pID.reset( new LPCITEMIDLIST [ GetCount() ] );
		if ( ! m_pID.get() )
			// Out of memory
			return;

		int i = 0;
		for ( POSITION pos = GetHeadPosition(); pos; i++)
			m_pID[ i ] = GetNext( pos )->m_pLastId;
	}

	~CShellList()
	{
		for ( POSITION pos = GetHeadPosition(); pos; )
			delete GetNext( pos );
		RemoveAll();
	}

	// Creates menu from file paths list
	bool GetMenu(HWND hWnd, void** ppContextMenu)
	{
		return SUCCEEDED( m_pFolder->GetUIObjectOf( hWnd, (UINT)GetCount(),
			m_pID.get(), IID_IContextMenu, NULL, ppContextMenu ) );
	}

protected:
	CComPtr< IShellFolder >		m_pFolder;	// First file folder
	auto_array< LPCITEMIDLIST >	m_pID;		// File ItemID array
};
