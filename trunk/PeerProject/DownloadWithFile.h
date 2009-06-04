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
	TRISTATE		m_bVerify;			// Verify status (TRI_TRUE - verified, TRI_FALSE - failed, TRI_UNKNOWN - not yet)
	DWORD			m_tReceived;
	BOOL			m_bMoving;			// Is complete file moving?
private:
	auto_ptr< CFragmentedFile >	m_pFile;// File(s)
	DWORD			m_nFileError;		// Last file/disk error

// Operations
public:
	float				GetProgress() const;
	QWORD				GetVolumeComplete() const;
	QWORD				GetVolumeRemaining() const;
	DWORD				GetTimeRemaining() const;
	CString				GetDisplayName() const;
	BOOL				IsFileOpen() const;
	BOOL				IsComplete() const;
	BOOL				PrepareFile();
	BOOL				GetFragment(CDownloadTransfer* pTransfer);
	BOOL				IsPositionEmpty(QWORD nOffset);
	BOOL				AreRangesUseful(const Fragments::List& oAvailable);
	BOOL				IsRangeUseful(QWORD nOffset, QWORD nLength);
	BOOL				IsRangeUsefulEnough(CDownloadTransfer* pTransfer, QWORD nOffset, QWORD nLength);
	BOOL				ClipUploadRange(QWORD nOffset, QWORD& nLength) const;
	BOOL				GetRandomRange(QWORD& nOffset, QWORD& nLength) const;
	BOOL				SubmitData(QWORD nOffset, LPBYTE pData, QWORD nLength);
	QWORD				EraseRange(QWORD nOffset, QWORD nLength);
	BOOL				MakeComplete();
	QWORD				InvalidateFileRange(QWORD nOffset, QWORD nLength);
	Fragments::List		GetEmptyFragmentList() const;
	Fragments::List		GetWantedFragmentList() const;
	CFragmentedFile*	GetFile();
	BOOL				FindByPath(const CString& sPath) const;
	DWORD				GetFileCount() const;
	QWORD				GetOffset(DWORD nIndex) const;
	QWORD				GetLength(DWORD nIndex) const;
	CString				GetPath(DWORD nIndex) const;
	CString				GetName(DWORD nIndex) const;
	QWORD				GetCompleted(DWORD nIndex) const;
	int					SelectFile(CSingleLock* pLock) const;
	DWORD				GetFileError() const;
	void				SetFileError(DWORD nFileError);
	void				ClearFileError();
protected:
	BOOL				OpenFile();
	void				CloseFile();
	void				AttachFile(auto_ptr< CFragmentedFile >& pFile);
	void				DeleteFile();
	DWORD				MoveFile(LPCTSTR pszDestination, LPPROGRESS_ROUTINE lpProgressRoutine = NULL, LPVOID lpData = NULL);
	BOOL				FlushFile();
	BOOL				ReadFile(QWORD nOffset, LPVOID pData, QWORD nLength, QWORD* pnRead = NULL);
	BOOL				WriteFile(QWORD nOffset, LPCVOID pData, QWORD nLength, QWORD* pnWritten = NULL);
	void				SerializeFile(CArchive& ar, int nVersion);
	void				SetVerifyStatus(TRISTATE bVerify);
	BOOL				OnVerify(LPCTSTR pszPath, BOOL bVerified);
private:
	Fragments::List		GetPossibleFragments(const Fragments::List& oAvailable, Fragments::Fragment& oLargest);

	// Not supported
	//BOOL				AppendMetadata();
	//BOOL				AppendMetadataID3v1(HANDLE hFile, CXMLElement* pXML);

// Overrides
public:
	virtual BOOL	IsMoving() const;
protected:
	virtual CString	GetAvailableRanges() const;
	virtual void	Serialize(CArchive& ar, int nVersion);
};
