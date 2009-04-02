//
// DownloadWithFile.h
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

#pragma once

#include "DownloadWithTransfers.h"
#include "FragmentedFile.h"

class CDownloadWithFile : public CDownloadWithTransfers
{
// Construction
protected:
	CDownloadWithFile();
	virtual ~CDownloadWithFile();

// Attributes
public:
	CFragmentedFile*	m_pFile;
	BOOL				m_bDiskFull;
	DWORD				m_tReceived;

// Operations
public:
	float			GetProgress() const;
	QWORD			GetVolumeComplete() const;
	QWORD			GetVolumeRemaining() const;
	DWORD			GetTimeRemaining() const;
	CString			GetDisplayName() const;
	BOOL			GetFragment(CDownloadTransfer* pTransfer);
	BOOL			IsPositionEmpty(QWORD nOffset);
	BOOL			AreRangesUseful(const Fragments::List& oAvailable);
	BOOL			IsRangeUseful(QWORD nOffset, QWORD nLength);
	BOOL			IsRangeUsefulEnough(CDownloadTransfer* pTransfer, QWORD nOffset, QWORD nLength);
	BOOL			ClipUploadRange(QWORD nOffset, QWORD& nLength) const;
	BOOL			PrepareFile();
	BOOL			GetRandomRange(QWORD& nOffset, QWORD& nLength) const;
	BOOL			SubmitData(QWORD nOffset, LPBYTE pData, QWORD nLength);
	QWORD			EraseRange(QWORD nOffset, QWORD nLength);
	BOOL			MakeComplete();
	QWORD			InvalidateFileRange(QWORD nOffset, QWORD nLength);

	inline Fragments::List GetEmptyFragmentList() const
	{
		return m_pFile ? m_pFile->GetEmptyFragmentList() : Fragments::List( 0 );
	}

	inline BOOL FindByPath(const CString& sPath) const
	{
		return m_pFile && m_pFile->FindByPath( sPath );
	}

protected:
	virtual CString	GetAvailableRanges() const;
	BOOL			OpenFile();
	void			CloseFile();
	void			DeleteFile(bool bForce = false);
	BOOL			RunFile(DWORD tNow);
	BOOL			AppendMetadata();
	virtual void	Serialize(CArchive& ar, int nVersion);

private:
	Fragments::List	GetPossibleFragments(const Fragments::List& oAvailable, Fragments::Fragment& oLargest);
	BOOL			AppendMetadataID3v1(HANDLE hFile, CXMLElement* pXML);

};
