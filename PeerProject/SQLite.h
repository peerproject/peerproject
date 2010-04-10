//
// SQLite.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
//
//	SQLite::CDatabase db( _T("C:\\Database.db3") );
//	SQLite::CStatement st( db, _T("SELECT Number FROM Table;") );
//	while( st.Step() || st.IsBusy() )
//	{
//		if ( st.IsBusy() )
//		{
//			Sleep( 1000 );
//			continue;
//		}
//		int n = st.GetInt( _T("Number") );
//	}

#pragma once

namespace SQLite {

#include "../Services/SQLite/sqlite3.h"

class CStatement;
class CDatabase;


class CDatabase : boost::noncopyable	// tr1 fix: private boost
{
public:
	CDatabase(LPCWSTR szDatabasePath = NULL);
	virtual ~CDatabase();

	operator bool() const throw();

	class CSQLitePtr : boost::noncopyable	// tr1 fix: private boost
	{
	public:
		CSQLitePtr(LPCWSTR szDatabasePath);
		virtual ~CSQLitePtr();

		sqlite3*	m_db;
	};
	typedef std::tr1::shared_ptr< CSQLitePtr > CSQLiteSharedPtr;	// tr1 fix:  was boost::

	CSQLiteSharedPtr	GetHandle() const throw();
	LPCWSTR				GetLastErrorMessage() const;
	bool				Open(LPCWSTR szDatabasePath);
	bool				Exec(LPCWSTR szQuery);

protected:
	CSQLiteSharedPtr	m_db;
};


class CStatement : boost::noncopyable	// tr1 fix: private boost
{
public:
	CStatement(const CDatabase& db, LPCWSTR szQuery);
	virtual ~CStatement();

	operator bool() const throw();

	bool			Step();
	bool			Prepare();
	void			Finalize();
	void			Reset();

	bool			IsPending() const throw();
	bool			IsBusy() const throw(); 		// Return true if latest SQL call failed because of a locked table state
	int				GetCount() const throw();		// Return the number of values in the current row of the result set
	int				GetType(LPCWSTR pszName) const;	// Return column type in the current row of the result set


	int				GetInt(LPCWSTR pszName) const;
	__int64			GetInt64(LPCWSTR pszName) const;
	double			GetDouble(LPCWSTR pszName) const;
	LPCWSTR			GetString(LPCWSTR pszName) const;
	LPCVOID			GetBlob(LPCWSTR pszName, int* pnLength) const;

	bool			Bind(int nNumber, int nData);
	bool			Bind(int nNumber, __int64 nData);
	bool			Bind(int nNumber, double dData);
	bool			Bind(int nNumber, LPCWSTR sData);
	bool			Bind(int nNumber, LPCVOID pData, int nLength);

protected:
	typedef std::map< std::wstring, int > CRaw;

	CDatabase::CSQLiteSharedPtr  m_db;	// Handle to database
	std::wstring	m_query;			// SQL query (UTF16)
	sqlite3_stmt*	m_st;				// SQL statement handle
	bool			m_prepared;			// Prepare was called successfully
	bool			m_busy;				// Last SQL call returned with busy error
	CRaw			m_raw;				// Column name to column number map

	int				GetColumn(LPCWSTR pszName) const;	// Return column index by column name
	bool			IsValidIndex(int nIndex) const; 	// Check column index for validity
};

}	// namespace SQLite
