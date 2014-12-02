//
// PeerProjectThread.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "PeerProjectThread.h"


inline void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
#ifdef _DEBUG
	struct
	{
		DWORD dwType;		// Must be 0x1000
		LPCSTR szName;		// Pointer to name (in user addr space)
		DWORD dwThreadID;	// Thread ID (-1=caller thread)
		DWORD dwFlags;		// Must be zero (reserved for future use)
	} info =
	{
		0x1000,
		szThreadName,
		dwThreadID,
		0
	};
	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0,
			sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR*)&info );
	}
	__except( EXCEPTION_CONTINUE_EXECUTION )
	{
	}
#else
	UNUSED(dwThreadID);
	UNUSED(szThreadName);
#endif
}


IMPLEMENT_DYNAMIC(CPeerThread, CWinThread)

CCriticalSection		CPeerThread::m_ThreadMapSection;
CPeerThread::CThreadMap	CPeerThread::m_ThreadMap;

CPeerThread::CPeerThread(AFX_THREADPROC pfnThreadProc /*= NULL*/, LPVOID pParam /*= NULL*/)
	: CWinThread( NULL, pParam )
	, m_pfnThreadProcExt( pfnThreadProc )
	, m_pnOwnerThreadID	( NULL )
{
}

CPeerThread::~CPeerThread()
{
	Remove( m_nThreadID );
}

HANDLE CPeerThread::CreateThread(LPCSTR pszName, int nPriority, DWORD dwCreateFlags, UINT nStackSize, LPSECURITY_ATTRIBUTES lpSecurityAttrs, DWORD* pnThreadID)
{
	if ( CWinThread::CreateThread( dwCreateFlags | CREATE_SUSPENDED, nStackSize, lpSecurityAttrs ) )
	{
		if ( pnThreadID )
		{
			m_pnOwnerThreadID = pnThreadID;
			*pnThreadID = m_nThreadID;
		}

		Add( this, pszName );

		VERIFY( ::SetThreadPriority( m_hThread, nPriority ) );

		if ( ! ( dwCreateFlags & CREATE_SUSPENDED ) )
			VERIFY( ResumeThread() != (DWORD)-1 );

		return m_hThread;
	}

	if  ( pnThreadID )
		*pnThreadID = 0;

	Delete();

	return NULL;
}

BOOL CPeerThread::InitInstance()
{
	CWinThread::InitInstance();

	return TRUE;
}

int CPeerThread::Run()
{
	BOOL bCOM = SUCCEEDED( OleInitialize( NULL ) );

	int ret;
	if ( m_pfnThreadProcExt )
		ret = ( *m_pfnThreadProcExt )( m_pThreadParams );
	else
		ret = CWinThread::Run();

	if ( bCOM )
	{
		__try
		{
			OleUninitialize();
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	if ( m_pnOwnerThreadID )
	{
		*m_pnOwnerThreadID = 0;
		m_pnOwnerThreadID = NULL;
	}

	return ret;
}

void CPeerThread::Add(CPeerThread* pThread, LPCSTR pszName)
{
	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	ASSERT( pThread->m_nThreadID );
	ASSERT( ! IsThreadAlive( pThread->m_nThreadID ) );

#ifdef _DEBUG
	if ( pszName )
		SetThreadName( pThread->m_nThreadID, pszName );
#endif

	CThreadTag tag = { pThread, pszName };
	m_ThreadMap.SetAt( pThread->m_nThreadID, tag );

	TRACE( L"Creating '%hs' thread (0x%x). Count: %d\n",
		( pszName ? pszName : "unnamed" ), pThread->m_nThreadID, m_ThreadMap.GetCount() );
}

void CPeerThread::Remove(DWORD nThreadID)
{
	if ( ! nThreadID )
		return;

	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	CThreadTag tag;
	if ( m_ThreadMap.Lookup( nThreadID, tag ) )
	{
		m_ThreadMap.RemoveKey( nThreadID );

		TRACE( L"Removing '%hs' thread (0x%x). Count: %d\n",
			( tag.pszName ? tag.pszName : "unnamed" ), nThreadID, m_ThreadMap.GetCount() );
	}
}

bool CPeerThread::IsThreadAlive(DWORD nThreadID)
{
	if ( ! nThreadID )
		return false;

	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	CThreadTag tag;
	return ( m_ThreadMap.Lookup( nThreadID, tag ) != FALSE );
}

bool CPeerThread::SetThreadPriority(DWORD nThreadID, int nPriority)
{
	if ( ! nThreadID )
		return false;

	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	CThreadTag tag;
	return ( m_ThreadMap.Lookup( nThreadID, tag ) && ( ::SetThreadPriority( tag.pThread->m_hThread, nPriority ) != FALSE ) );
}

HANDLE CPeerThread::GetHandle(DWORD nThreadID)
{
	if ( ! nThreadID )
		return NULL;

	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	CThreadTag tag;
	return ( m_ThreadMap.Lookup( nThreadID, tag ) ? tag.pThread->m_hThread : NULL );
}

void CPeerThread::DeleteThread(DWORD nThreadID)
{
	if ( ! nThreadID )
		return;

	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	CThreadTag tag;
	if ( m_ThreadMap.Lookup( nThreadID, tag ) )
		tag.pThread->Delete();
}

void CPeerThread::DetachThread(DWORD nThreadID)
{
	if ( ! nThreadID )
		return;

	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	CThreadTag tag;
	if ( m_ThreadMap.Lookup( nThreadID, tag ) )
		tag.pThread->m_pnOwnerThreadID = NULL;
}

HANDLE CPeerThread::BeginThread(LPCSTR pszName, AFX_THREADPROC pfnThreadProc, LPVOID pParam, int nPriority, UINT nStackSize, DWORD dwCreateFlags, LPSECURITY_ATTRIBUTES lpSecurityAttrs, DWORD* pnThreadID)
{
	if ( CPeerThread* pThread = new CPeerThread( pfnThreadProc, pParam ) )
	{
		return pThread->CreateThread( pszName, nPriority, dwCreateFlags, nStackSize, lpSecurityAttrs, pnThreadID );
	}
	return NULL;
}

void CPeerThread::CloseThread(DWORD nThreadID, DWORD dwTimeout)
{
	__try
	{
		if ( HANDLE hThread = GetHandle( nThreadID ) )
		{
			DWORD dwExitCode;
			while ( GetExitCodeThread( hThread, &dwExitCode ) && dwExitCode == STILL_ACTIVE )
			{
				if ( ! IsThreadAlive( nThreadID ) )
					return;

				::SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );

				SafeMessageLoop();

				DWORD res = MsgWaitForMultipleObjects( 1, &hThread, FALSE, dwTimeout, QS_ALLINPUT | QS_ALLPOSTMESSAGE );
				if ( res == WAIT_OBJECT_0 + 1 )
					continue;	// Handle messages
				if ( res != WAIT_TIMEOUT )
					break;		// Handle signaled state or errors

				// Timeout

				// Very dangerous function produces 100% unrecoverable TLS leaks/deadlocks
				if ( TerminateThread( hThread, 0 ) )
				{
					theApp.Message( MSG_DEBUG, L"WARNING: Terminating thread (0x%x).", nThreadID );
					TRACE( L"WARNING: Terminating thread (0x%x).\n", nThreadID );

					DeleteThread( nThreadID );
				}
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// Thread already ended
	}

	CPeerThread::Remove( nThreadID );
}
