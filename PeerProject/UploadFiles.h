//
// UploadFiles.h
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

#if !defined(AFX_UPLOADFILES_H__0F70ABF7_91B7_436B_A235_C3D01DB0D7F6__INCLUDED_)
#define AFX_UPLOADFILES_H__0F70ABF7_91B7_436B_A235_C3D01DB0D7F6__INCLUDED_

#pragma once

class CUploadFile;
class CUploadTransfer;


class CUploadFiles  
{
// Construction
public:
	CUploadFiles();
	virtual ~CUploadFiles();
	
// Attributes
protected:
	CList< CUploadFile* >	m_pList;

// Operations
public:
	void			Clear();
	CUploadFile*	GetFile(CUploadTransfer* pUpload, const Hashes::Sha1Hash& oSHA1, LPCTSTR pszName, LPCTSTR pszPath, QWORD nSize);
	void			Remove(CUploadTransfer* pTransfer);
	void			MoveToHead(CUploadTransfer* pTransfer);
	void			MoveToTail(CUploadTransfer* pTransfer);
	
// List Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}
	
	inline CUploadFile* GetNext(POSITION& pos) const
	{
		return m_pList.GetNext( pos );
	}
	
	inline INT_PTR GetCount() const
	{
		return m_pList.GetCount();
	}

	inline BOOL Check(CUploadFile* pFile) const
	{
		return m_pList.Find( pFile ) != NULL;
	}

};

extern CUploadFiles UploadFiles;

#endif // !defined(AFX_UPLOADFILES_H__0F70ABF7_91B7_436B_A235_C3D01DB0D7F6__INCLUDED_)
