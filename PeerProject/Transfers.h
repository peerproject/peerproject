//
// Transfers.h
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

#pragma once

#include "ThreadImpl.h"

class CTransfer;
class CTransfers;


class CTransfers :
	public CThreadImpl
{
public:
	CTransfers();
	virtual ~CTransfers();

	mutable CMutex	m_pSection;

	BOOL		IsConnectedTo(const IN_ADDR* pAddress) const;
	BOOL		StartThread();
	void		StopThread();
	void		Add(CTransfer* pTransfer);
	void		Remove(CTransfer* pTransfer);

	static INT_PTR	GetActiveCount();

private:
	CList< CTransfer* >	m_pList;
	DWORD				m_nRunCookie;

	void		OnRun();
	void		OnRunTransfers();
	void		OnCheckExit();
};

extern CTransfers Transfers;
