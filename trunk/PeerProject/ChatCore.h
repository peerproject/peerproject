//
// ChatCore.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "EDClient.h"

class CConnection;
class CChatSession;
class CEDClient;
class CPacket;


class CChatCore : public CThreadImpl
{
// Construction
public:
	CChatCore();
	virtual ~CChatCore();

// Attributes
public:
	CMutex			m_pSection;

protected:
	CList< CChatSession* > m_pSessions;
	template< typename T > CChatSession* FindSession(const T* pClient, BOOL bCreate);

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
	void			OnDropped(CEDClient* pClient);
	CChatSession*	FindSession(CEDClient* pClient, BOOL bCreate = TRUE);

	template< typename T > void OnMessage(const T* pClient, CPacket* pPacket = NULL)
	{
		if ( ! Settings.Community.ChatEnable ||
			 ! Settings.Community.ChatAllNetworks )
			return;		// Chat disabled

		CSingleLock pLock( &m_pSection );
		if ( pLock.Lock( 250 ) )
		{
			if ( CChatSession* pSession = FindSession< T >( pClient, TRUE ) )
			{
				pSession->OnMessage( pPacket );
			}
		}
	}

//	template< typename T > void OnDropped(const T* pClient)
//	{
//		CSingleLock pLock( &m_pSection );
//		if ( pLock.Lock( 250 ) )
//		{
//			if ( CChatSession* pSession = FindSession< T >( pClient, FALSE ) )
//			{
//				pSession->OnDropped();
//			}
//		}
//	}
//
//	template< typename T > void OnAddUser(const T* pClient, CChatUser* pUser)
//	{
//		CSingleLock pLock( &m_pSection );
//		if ( pLock.Lock( 250 ) )
//		{
//			if ( CChatSession* pSession = FindSession< T >( pClient, FALSE ) )
//			{
//				pSession->AddUser( pUser );
//				return;
//			}
//		}
//		delete pUser;
//	}
//
//	template< typename T > void OnDeleteUser(const T* pClient, CString* pUser)
//	{
//		CSingleLock pLock( &m_pSection );
//		if ( pLock.Lock( 250 ) )
//		{
//			if ( CChatSession* pSession = FindSession< T >( pClient, FALSE ) )
//			{
//				pSession->DeleteUser( pUser );
//				return;
//			}
//		}
//		delete pUser;
//	}

protected:
	void			Add(CChatSession* pSession);
	void			Remove(CChatSession* pSession);

	void			StartThread();
	void			OnRun();

	friend class CChatSession;
};

extern CChatCore ChatCore;
