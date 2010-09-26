//
// SQLite.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2008.
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

// Example:
//	auto_ptr< CDatabase* > db( theApp.GetDatabase( true ) );	// /Data/PeerProject.db3
//	if ( db->Prepare( _T("SELECT Number FROM Table;") ) )
//	{
//		while( db->Step() || db->IsBusy() )
//		{
//			if ( db->IsBusy() )
//			{
//				Sleep( 1000 );
//				continue;
//			}
//			int n = db->GetInt32( _T("Number") );
//		}
//	}

#pragma once

struct sqlite3;
struct sqlite3_stmt;


class CDatabase
{
public:
	CDatabase(LPCWSTR szDatabase);
	~CDatabase();

	operator bool() const throw();					// Return true if database successfully opened

	bool			Exec(LPCTSTR szQuery);			// Execute multiple queries without parameters
	bool			Prepare(LPCTSTR szQuery);		// Prep single query
	bool			Step(); 						// Run one query iteration
	void			Finalize(); 					// Finalize query
	bool			IsBusy() const throw(); 		// Return true if latest SQL call failed for a locked table state
	int				GetCount() const throw();		// Return the number of values in the current row of the result set
	int				GetType(LPCWSTR pszName) const;	// Return column type in the current row of the result set
	CString			GetLastErrorMessage() const;	// Return database error

	__int32			GetInt32(LPCTSTR pszName) const;
	__int64			GetInt64(LPCTSTR pszName) const;
	double			GetDouble(LPCTSTR pszName) const;
	CString			GetString(LPCTSTR pszName) const;
	LPCVOID			GetBlob(LPCTSTR pszName, int* pnLength) const;

	// Note: First parameter has an index of 1
	bool			Bind(int nIndex, __int32 nData);
	bool			Bind(int nIndex, __int64 nData);
	bool			Bind(int nIndex, double dData);
	bool			Bind(int nIndex, LPCTSTR sData);
	bool			Bind(int nIndex, LPCVOID pData, int nLength);

protected:
	typedef CMap< CString, const CString&, int, int > CRaw;		// std::map< std::wstring, int >

	sqlite3*		m_db;							// Handle to SQL database
	sqlite3_stmt*	m_st;							// SQL statement handle
	CRaw			m_raw;							// Column name to column number map
	CString			m_sQuery;						// SQL query
	bool			m_bBusy;						// Last SQL call returned with busy error

#ifdef _DEBUG
	DWORD			m_nThread;						// Thread ID (debug only)
#endif

	bool			PrepareHelper();				// Prepare single query
	int				GetColumn(LPCTSTR pszName) const; // Return column index by column name

private:
	CDatabase(const CDatabase&);
	CDatabase& operator=(const CDatabase&);
};
