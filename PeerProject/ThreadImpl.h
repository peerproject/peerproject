//
// ThreadImpl.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
		: m_bCompleted	( false )
		, m_bThread 	( false )
		, m_hThread 	( NULL )
//		, m_pCancel		( FALSE, TRUE )
	{
	}
	virtual ~CThreadImpl()
	{
		CloseThread();
	}

private:
	volatile bool	m_bCompleted;	// TRUE - thread runs at least once
	volatile bool	m_bThread;		// TRUE - enable thread; FALSE - terminate thread.
	volatile HANDLE m_hThread;		// Thread handle
//	CEvent			m_pCancel;		// Thread cancel event (signaled if abort requested)
	CEvent			m_pWakeup;		// Thread wakeup event (optional)

	static UINT ThreadStart(LPVOID pParam)
	{
		CThreadImpl* pThis = reinterpret_cast< CThreadImpl* >( pParam );
		pThis->OnRun();
		pThis->m_bCompleted = true;	// Set complete status
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
			m_bCompleted = false;		// Reset complete status
//			m_pCancel.ResetEvent();		// Enable thread run
			m_bThread = true;			// Enable thread run
			m_hThread = ::BeginThread( szName, ThreadStart, this, nPriority );
		}
		return ( m_hThread != NULL );
	}

	inline void CloseThread(DWORD dwTimeout = ALMOST_INFINITE) throw()
	{
		Exit();		// Ask thread for exit
		Wakeup();	// Wakeup thread if any
		::CloseThread( (HANDLE*)&m_hThread, dwTimeout );
	}

	inline void Wait() throw()
	{
		::CloseThread( (HANDLE*)&m_hThread, INFINITE );
	}

	inline bool Wakeup() throw()
	{
		return ( m_pWakeup.SetEvent() != FALSE );
	}

	inline void Doze(DWORD dwTimeout = INFINITE) throw()
	{
		SwitchToThread();
		do
		{
			SafeMessageLoop();
		}
		while( MsgWaitForMultipleObjects( 1, &m_pWakeup.m_hObject, FALSE, dwTimeout, QS_ALLINPUT | QS_ALLPOSTMESSAGE ) == WAIT_OBJECT_0 + 1 );
	}

	inline HANDLE GetWakeupEvent() const throw()
	{
		return m_pWakeup;
	}

	inline bool IsThreadCompleted() const throw()
	{
		return m_bCompleted;
	}

	inline bool IsThreadEnabled() const throw()		// DWORD dwTimeout = 0
	{
		return m_bThread;
//		return ( WaitForSingleObject( m_pCancel, dwTimeout ) == WAIT_TIMEOUT );
	}

	inline bool IsThreadAlive() const throw()
	{
		return m_hThread && ( WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT );
	}

	inline void Exit() throw()
	{
		m_bThread = false;
//		m_pCancel.SetEvent();
	}

	inline bool SetThreadPriority(int nPriority) throw()
	{
		return m_hThread && ( ::SetThreadPriority( m_hThread, nPriority ) != FALSE );
	}
};
