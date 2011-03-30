//
// DownloadWithFile.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
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
	TRISTATE		m_bVerify;				// Verify status (TRI_TRUE verified, TRI_FALSE failed, TRI_UNKNOWN not yet)
	DWORD			m_tReceived;
private:
	DWORD			m_nFileError;			// Last file/disk error
	auto_ptr< CFragmentedFile >	m_pFile;	// File(s)

// Operations
public:
	float			GetProgress() const;
	QWORD			GetVolumeComplete() const;
	QWORD			GetVolumeRemaining() const;
	DWORD			GetTimeRemaining() const;
	CString			GetDisplayName() const;
	BOOL			IsValid() const;
	BOOL			IsFileOpen() const;
	BOOL			IsComplete() const;
	BOOL			PrepareFile();
	BOOL			IsPositionEmpty(QWORD nOffset);
//	BOOL			GetFragment(CDownloadTransfer* pTransfer);
//	BOOL			AreRangesUseful(const Fragments::List& oAvailable);
//	BOOL			IsRangeUseful(QWORD nOffset, QWORD nLength);
//	BOOL			IsRangeUsefulEnough(CDownloadTransfer* pTransfer, QWORD nOffset, QWORD nLength);
	BOOL			ClipUploadRange(QWORD nOffset, QWORD& nLength) const;
	BOOL			GetRandomRange(QWORD& nOffset, QWORD& nLength) const;
	bool			GetAvailableRanges( CString& strRanges ) const;
	QWORD			EraseRange(QWORD nOffset, QWORD nLength);
	QWORD			InvalidateFileRange(QWORD nOffset, QWORD nLength);
	BOOL			SubmitData(QWORD nOffset, LPBYTE pData, QWORD nLength);
	BOOL			MakeComplete();
	Fragments::List	GetFullFragmentList() const;	// All fragments which must be downloaded
	Fragments::List	GetEmptyFragmentList() const;	// All empty fragments
//	Fragments::List	GetWantedFragmentList() const;
	CFragmentedFile* GetFile();
	BOOL			FindByPath(const CString& sPath) const;
	DWORD			GetFileCount() const;
	QWORD			GetOffset(DWORD nIndex) const;
	QWORD			GetLength(DWORD nIndex = 0) const;
	CString			GetPath(DWORD nIndex = 0) const;
	CString			GetName(DWORD nIndex = 0) const;
	QWORD			GetCompleted(DWORD nIndex) const;
	int				SelectFile(CSingleLock* pLock) const;
	DWORD			GetFileError() const;
	void			SetFileError(DWORD nFileError);
	void			ClearFileError();
	DWORD			MoveFile(LPCTSTR pszDestination, LPPROGRESS_ROUTINE lpProgressRoutine = NULL, LPVOID lpData = NULL);
protected:
	BOOL			OpenFile();
	void			CloseFile();
	void			AttachFile(auto_ptr< CFragmentedFile >& pFile);
	void			DeleteFile();
	BOOL			FlushFile();
	BOOL			ReadFile(QWORD nOffset, LPVOID pData, QWORD nLength, QWORD* pnRead = NULL);
	BOOL			WriteFile(QWORD nOffset, LPCVOID pData, QWORD nLength, QWORD* pnWritten = NULL);
	void			SerializeFile(CArchive& ar, int nVersion);
	BOOL			OnVerify(LPCTSTR pszPath, BOOL bVerified);

//private:
//	Fragments::List	GetPossibleFragments(const Fragments::List& oAvailable, Fragments::Fragment& oLargest);
	// Unsupported:
	//BOOL			AppendMetadata();
	//BOOL			AppendMetadataID3v1(HANDLE hFile, CXMLElement* pXML);

// Overrides
protected:
	virtual void	Serialize(CArchive& ar, int nVersion);
};
