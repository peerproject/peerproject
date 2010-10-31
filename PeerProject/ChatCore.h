//
// ChatCore.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

	void			OnRun();

	friend class CChatSession;
};

extern CChatCore ChatCore;
