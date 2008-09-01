//
// UploadFile.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#if !defined(AFX_UPLOADFILE_H__D2D194F0_8528_482F_A4DA_DE1D9C496335__INCLUDED_)
#define AFX_UPLOADFILE_H__D2D194F0_8528_482F_A4DA_DE1D9C496335__INCLUDED_

#pragma once

#include "FileFragments.hpp"
#include "PeerProjectFile.h"

class CUploadTransfer;

class CUploadFile : public CPeerProjectFile
{
public:
    CUploadFile(CUploadTransfer* pUpload, const Hashes::Sha1Hash& oSHA1, LPCTSTR pszName, LPCTSTR pszPath, QWORD nSize);
	virtual ~CUploadFile();

	void				Add(CUploadTransfer* pUpload);
	BOOL				Remove(CUploadTransfer* pUpload);
	CUploadTransfer*	GetActive() const;
	void				AddFragment(QWORD nOffset, QWORD nLength);
	void				Remove();

	inline BOOL IsEmpty() const
	{
		return m_pTransfers.IsEmpty();
	}

public:
	IN_ADDR						m_pAddress;
	DWORD						m_nRequests;
	Fragments::List				m_oFragments;
	BOOL						m_bSelected;

protected:
	CList< CUploadTransfer* >	m_pTransfers;
};

#endif // !defined(AFX_UPLOADFILE_H__D2D194F0_8528_482F_A4DA_DE1D9C496335__INCLUDED_)
