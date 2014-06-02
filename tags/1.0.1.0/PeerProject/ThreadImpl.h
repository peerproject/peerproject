//
// ThreadImpl.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2014
// Portions copyright Shareaza Development Team, 2008.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "PeerProject.h"
#include "PeerProjectThread.h"


class CThreadImpl
{
public:
	CThreadImpl()
		: m_pCancel		( FALSE, TRUE )
		, m_hThread 	( NULL )
		, m_nThreadID	( 0 )
		, m_bCancelled	( FALSE )
	//	, m_bCompleted	( false )
	{
	}

	virtual ~CThreadImpl()
	{
		CloseThread();
	}

private:
	volatile HANDLE m_hThread;		// Thread handle
	DWORD			m_nThreadID;	// Thread ID
	CEvent			m_pWakeup;		// Thread wakeup event (optional)
	CEvent			m_pCancel;		// Thread cancel event (signaled if abort requested)
	volatile LONG	m_bCancelled;	// Thread is canceling
//	volatile bool	m_bCompleted;	// TRUE - thread runs at least once

	static UINT ThreadStart(LPVOID pParam)
	{
		CThreadImpl* pThis = reinterpret_cast< CThreadImpl* >( pParam );
		pThis->OnRun();
	//	pThis->m_bCompleted = true;	// Set complete status
		pThis->m_hThread = NULL;
		return 0;
	}

	CThreadImpl(const CThreadImpl&);
	CThreadImpl& operator=(const CThreadImpl&);

protected:
	virtual void OnRun() = 0;

public:
	inline bool BeginThread(LPCSTR szName = NULL, int nPriority = THREAD_PRIORITY_NORMAL) throw()
	{
		if ( ! IsThreadAlive() )
		{
		//	m_bCompleted = false;		// Reset complete status
			m_pCancel.ResetEvent();		// Enable thread run
			m_hThread = ::BeginThread( szName, ThreadStart, this, nPriority, 0, 0, NULL, &m_nThreadID );
		}
		return ( m_hThread != NULL );
	}

	inline void CloseThread(DWORD dwTimeout = ALMOST_INFINITE) throw()
	{
		m_pCancel.SetEvent();	// Ask thread for exit
		m_pWakeup.SetEvent();	// Wakeup thread if any
		if ( ! InterlockedCompareExchange( &m_bCancelled, TRUE, FALSE ) )
		{
			if ( m_nThreadID != GetCurrentThreadId() )
			{
				::CloseThread( m_hThread, dwTimeout );
				m_hThread = NULL;
			}
			InterlockedExchange( &m_bCancelled, FALSE );
		}
	}

	inline void Wait() throw()
	{
		if ( ! InterlockedCompareExchange( &m_bCancelled, TRUE, FALSE ) )
		{
			if ( m_nThreadID != GetCurrentThreadId() )
			{
				::CloseThread( m_hThread, INFINITE );
			}
			InterlockedExchange( &m_bCancelled, FALSE );
		}
	}

	inline bool Wakeup() throw()
	{
		return ( m_pWakeup.SetEvent() != FALSE );
	}

	inline void Doze(DWORD dwTimeout) throw()	// = INFINITE
	{
		SwitchToThread();
		do
		{
			SafeMessageLoop();
		}
		while ( MsgWaitForMultipleObjects( 1, &m_pWakeup.m_hObject, FALSE, dwTimeout, QS_ALLINPUT | QS_ALLPOSTMESSAGE ) == WAIT_OBJECT_0 + 1 );
	}

	inline HANDLE GetWakeupEvent() const throw()
	{
		return m_pWakeup;
	}

	inline bool IsThreadEnabled( DWORD dwTimeout = 0 ) const throw()
	{
		return ( WaitForSingleObject( m_pCancel, dwTimeout ) == WAIT_TIMEOUT );
	}

	inline bool IsThreadAlive() const throw()
	{
		return m_hThread && ( WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT );
	}

	inline void Exit() throw()
	{
		m_pCancel.SetEvent();
	}

	inline bool SetThreadPriority(int nPriority) throw()
	{
		return m_hThread && ( ::SetThreadPriority( m_hThread, nPriority ) != FALSE );
	}
};
