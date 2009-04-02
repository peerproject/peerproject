//
// DownloadWithTiger.h
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

#if !defined(AFX_DOWNLOADWITHTIGER_H__8F105434_164D_4F58_BAA4_8DB2B29CA259__INCLUDED_)
#define AFX_DOWNLOADWITHTIGER_H__8F105434_164D_4F58_BAA4_8DB2B29CA259__INCLUDED_

#pragma once

#include "DownloadWithTorrent.h"

class CDownloadWithTiger : public CDownloadWithTorrent
{
// Construction
protected:
	CDownloadWithTiger();
	virtual ~CDownloadWithTiger();

// Attributes
private:
	CTigerTree	m_pTigerTree;
	BYTE*		m_pTigerBlock;
	DWORD		m_nTigerBlock;
	DWORD		m_nTigerSize;
	DWORD		m_nTigerSuccess;
private:
	CED2K		m_pHashset;
	BYTE*		m_pHashsetBlock;
	DWORD		m_nHashsetBlock;
	DWORD		m_nHashsetSuccess;
private:
	DWORD		m_nVerifyCookie;
	int			m_nVerifyHash;
	DWORD		m_nVerifyBlock;
	QWORD		m_nVerifyOffset;
	QWORD		m_nVerifyLength;
	DWORD		m_tVerifyLast;

	mutable CCriticalSection	m_pTigerSection;

// Operations
public:
	BOOL		GetNextVerifyRange(QWORD& nOffset, QWORD& nLength, BOOL& bSuccess, int nHash = HASH_NULL) const;
	BOOL		IsFullyVerified();
	BOOL		NeedTigerTree() const;
	BOOL		SetTigerTree(BYTE* pTiger, DWORD nTiger);
	CTigerTree*	GetTigerTree();
	BOOL		NeedHashset() const;
	BOOL		SetHashset(BYTE* pSource, DWORD nSource);
	CED2K*		GetHashset();
	virtual CString	GetAvailableRanges() const;
	void		ResetVerification();
	void		ClearVerification();
protected:
	QWORD		GetVerifyLength(int nHash = HASH_NULL) const;
	BOOL		ValidationCanFinish() const;
	void		RunValidation();
	virtual void	Serialize(CArchive& ar, int nVersion);
private:
	DWORD		GetValidationCookie() const;
	BOOL		FindNewValidationBlock(int nHash);
	void		ContinueValidation();
	void		FinishValidation();
	void		SubtractHelper(Fragments::List& ppCorrupted, BYTE* pBlock, QWORD nBlock, QWORD nSize);
	
	friend class CEDClient; // AddSourceED2K && m_nHashsetBlock && m_pHashsetBlock
	friend class CDownloadTipCtrl;
};

#endif // !defined(AFX_DOWNLOADWITHTIGER_H__8F105434_164D_4F58_BAA4_8DB2B29CA259__INCLUDED_)
