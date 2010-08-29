//
// DownloadWithTiger.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "DownloadWithTorrent.h"

class CDownloadTransfer;


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

	CED2K		m_pHashset;
	BYTE*		m_pHashsetBlock;
	DWORD		m_nHashsetBlock;
	DWORD		m_nHashsetSuccess;

	DWORD		m_nVerifyCookie;
	int			m_nVerifyHash;
	DWORD		m_nVerifyBlock;
	QWORD		m_nVerifyOffset;
	QWORD		m_nVerifyLength;
	DWORD		m_tVerifyLast;

	mutable CCriticalSection	m_pTigerSection;

	mutable Fragments::List		m_oWFLCache;			// Wanted fragment list cache
	mutable QWORD				m_nWFLCookie;			// Wanted fragment list cookie

// Operations
public:
	CTigerTree*	GetTigerTree();
	BOOL		SetTigerTree(BYTE* pTiger, DWORD nTiger);
	BOOL		NeedTigerTree() const;
	BOOL		NeedHashset() const;
	BOOL		SetHashset(BYTE* pSource, DWORD nSource);
	CED2K*		GetHashset();
	void		ResetVerification();
	void		ClearVerification();
	void		RunValidation();
	BOOL		GetNextVerifyRange(QWORD& nOffset, QWORD& nLength, BOOL& bSuccess, int nHash = HASH_NULL) const;
	DWORD		GetVerifyLength(PROTOCOLID nProtocol = PROTOCOL_ANY, int nHash = HASH_NULL) const;

	// Get list of possible download fragments / empty fragments we really want
	Fragments::List GetPossibleFragments(const Fragments::List& oAvailable, Fragments::Fragment& oLargest);
	Fragments::List GetWantedFragmentList() const;

	BOOL		GetFragment(CDownloadTransfer* pTransfer);	// Select a fragment for a transfer
	BOOL		AreRangesUseful(const Fragments::List& oAvailable) const;
	BOOL		IsRangeUseful(QWORD nOffset, QWORD nLength) const;
	BOOL		IsRangeUsefulEnough(CDownloadTransfer* pTransfer, QWORD nOffset, QWORD nLength) const;
	// Check range against fragments list got from GetWantedFragmentList() call
	// UsefulEnough() is like IsRangeUseful() but takes the amount of useful ranges,
	// relative to the amount of garbage and source speed into account

protected:
	bool		IsFullyVerified() const;
	DWORD		GetValidationCookie() const;
	BOOL		FindNewValidationBlock(int nHash);
	void		ContinueValidation();
	void		FinishValidation();
	void		SubtractHelper(Fragments::List& ppCorrupted, BYTE* pBlock, QWORD nBlock, QWORD nSize);

	// Get list of all fragments which must be downloaded
	// but rounded to nearest smallest hash block (torrent, tiger or ed2k)
	Fragments::List GetHashableFragmentList() const;

	virtual void	Serialize(CArchive& ar, int nVersion);

	friend class CEDClient; 	// AddSourceED2K && m_nHashsetBlock && m_pHashsetBlock
	friend class CDownloadTipCtrl;
};
