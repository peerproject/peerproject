//
// FragmentedFile.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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

#if !defined(AFX_FRAGMENTEDFILE_H__03EF9CB6_10EC_43A5_B6E0_D74B54044B51__INCLUDED_)
#define AFX_FRAGMENTEDFILE_H__03EF9CB6_10EC_43A5_B6E0_D74B54044B51__INCLUDED_

#pragma once

#include "FileFragments.hpp"

class CTransferFile;
class CEDPartImporter;

class CFragmentedFile
{
// Construction
public:
	CFragmentedFile();
	virtual ~CFragmentedFile();

// Attributes
protected:
	CTransferFile*	m_pFile;
	QWORD			m_nUnflushed;

private:
	Fragments::List	m_oFList;
	
// Operations
public:
	BOOL			Create(LPCTSTR pszFile, QWORD nLength);
	BOOL			Open(LPCTSTR pszFile);
	BOOL			Flush();
	void			Close();
	void			Clear();
	BOOL			MakeComplete();
	void			Serialize(CArchive& ar, int nVersion);
public:
	BOOL			IsPositionRemaining(QWORD nOffset) const;
	BOOL			DoesRangeOverlap(QWORD nOffset, QWORD nLength) const;
	QWORD			GetRangeOverlap(QWORD nOffset, QWORD nLength) const;
	BOOL			WriteRange(QWORD nOffset, LPCVOID pData, QWORD nLength);
	BOOL			ReadRange(QWORD nOffset, LPVOID pData, QWORD nLength);
	QWORD			InvalidateRange(QWORD nOffset, QWORD nLength);
	
// Operations
public:
	inline BOOL IsValid() const
	{
		return m_oFList.limit() > 0; // && this != NULL
	}
	
	inline BOOL IsOpen() const
	{
		return m_pFile != NULL;// && this != NULL
	}

//	inline QWORD GetTotal() const
//	{
//		return m_oFList.limit();
//	}
	
	inline QWORD GetRemaining() const
	{
		return ( m_oFList.limit() == SIZE_UNKNOWN && m_oFList.length_sum() ) ?
			SIZE_UNKNOWN : m_oFList.length_sum();
	}
	
	inline QWORD GetCompleted() const
	{
		return m_oFList.missing();
	}
	
	const Fragments::List& GetEmptyFragmentList() const
	{
		return m_oFList;
	}
	
//	inline QWORD GetEmptyFragmentCount() const
//	{
//		return m_oFList.size();
//	}
	
//	inline BOOL IsFlushNeeded() const
//	{
//		return ( m_pFile != NULL ) && ( m_nUnflushed > 0 );
//	}

	friend class CEDPartImporter;
};


#endif // !defined(AFX_FRAGMENTEDFILE_H__03EF9CB6_10EC_43A5_B6E0_D74B54044B51__INCLUDED_)
