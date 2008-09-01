//
// ChatCore.h
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

class CConnection;
class CChatSession;


class CChatCore :
	public CThreadImpl
{
// Construction
public:
	CChatCore();
	virtual ~CChatCore();

// Attributes
public:
	CMutex		m_pSection;
protected:
	CList< CChatSession* > m_pSessions;

// Operations
public:
	POSITION		GetIterator() const;
	CChatSession*	GetNext(POSITION& pos) const;
	INT_PTR			GetCount() const { return m_pSessions.GetCount(); }
	BOOL			Check(CChatSession* pSession) const;
	void			Close();
	void			OnAccept(CConnection* pConnection, PROTOCOLID nProtocol = PROTOCOL_NULL);
	BOOL			OnPush(const Hashes::Guid& oGUID, CConnection* pConnection);
	void			OnED2KMessage(CEDClient* pClient, CEDPacket* pPacket);
	CChatSession*	FindSession(CEDClient* pClient);
	void			StopThread();
protected:
	void			Add(CChatSession* pSession);
	void			Remove(CChatSession* pSession);
	void			StartThread();
protected:
	void			OnRun();

	friend class CChatSession;
};

extern CChatCore ChatCore;
