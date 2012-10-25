//
// PeerProjectThread.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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


IMPLEMENT_DYNAMIC(CAppThread, CWinThread)

CCriticalSection		CAppThread::m_ThreadMapSection;
CAppThread::CThreadMap	CAppThread::m_ThreadMap;

CAppThread::CAppThread(AFX_THREADPROC pfnThreadProc /*= NULL*/, LPVOID pParam /*= NULL*/)
	: CWinThread( NULL, pParam )
	, m_pfnThreadProcExt( pfnThreadProc )
{
}

CAppThread::~CAppThread()
{
	Remove( m_hThread );
}

HANDLE CAppThread::CreateThread(LPCSTR pszName, int nPriority /*= THREAD_PRIORITY_NORMAL*/,
	DWORD dwCreateFlags /*= 0*/, UINT nStackSize /*= 0*/,
	LPSECURITY_ATTRIBUTES lpSecurityAttrs /*= NULL*/)
{
	if ( CWinThread::CreateThread( dwCreateFlags | CREATE_SUSPENDED, nStackSize,
		lpSecurityAttrs ) )
	{
		Add( this, pszName );

		VERIFY( SetThreadPriority( nPriority ) );

		if ( ! ( dwCreateFlags & CREATE_SUSPENDED ) )
			VERIFY( ResumeThread() != (DWORD)-1 );

		return m_hThread;
	}

	Delete();

	return NULL;
}

BOOL CAppThread::InitInstance()
{
	CWinThread::InitInstance();

	return TRUE;
}

int CAppThread::Run()
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

	return ret;
}

void CAppThread::Add(CAppThread* pThread, LPCSTR pszName)
{
	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	if ( pszName )
		SetThreadName( pThread->m_nThreadID, pszName );

	CThreadTag tag = { pThread, pszName };
	m_ThreadMap.SetAt( pThread->m_hThread, tag );

	TRACE( _T("Creating '%hs' thread (0x%08x). Count: %d\n"),
		( pszName ? pszName : "unnamed" ), pThread->m_hThread, m_ThreadMap.GetCount() );
}

void CAppThread::Remove(HANDLE hThread)
{
	CSingleLock oLock( &m_ThreadMapSection, TRUE );

	CThreadTag tag = { 0 };
	if ( m_ThreadMap.Lookup( hThread, tag ) )
	{
		m_ThreadMap.RemoveKey( hThread );

		TRACE( _T("Removing '%hs' thread (0x%08x). Count: %d\n"),
			( tag.pszName ? tag.pszName : "unnamed" ),
			hThread, m_ThreadMap.GetCount() );
	}
}

void CAppThread::Terminate(HANDLE hThread)
{
	// Very dangerous function produces 100% urecoverable TLS leaks/deadlocks
	if ( TerminateThread( hThread, 0 ) )
	{
		CSingleLock oLock( &m_ThreadMapSection, TRUE );

		CThreadTag tag = { 0 };
		if ( m_ThreadMap.Lookup( hThread, tag ) )
		{
			ASSERT( hThread == tag.pThread->m_hThread );
			ASSERT_VALID( tag.pThread );
			ASSERT( static_cast<CWinThread*>( tag.pThread ) != AfxGetApp() );
			tag.pThread->Delete();
		}
		else
			CloseHandle( hThread );

		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating '%hs' thread (0x%08x)."),
			( tag.pszName ? tag.pszName : "unnamed" ), hThread );
		TRACE( _T("WARNING: Terminating '%hs' thread (0x%08x).\n"),
			( tag.pszName ? tag.pszName : "unnamed" ), hThread );
	}
	else
	{
		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating thread (0x%08x) failed."), hThread );
		TRACE( _T("WARNING: Terminating thread (0x%08x) failed.\n"), hThread );
	}
}

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
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
#endif
	UNUSED(dwThreadID);
	UNUSED(szThreadName);
}

HANDLE BeginThread(LPCSTR pszName, AFX_THREADPROC pfnThreadProc,
	LPVOID pParam, int nPriority, UINT nStackSize, DWORD dwCreateFlags,
	LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
	CAppThread* pThread = new CAppThread( pfnThreadProc, pParam );
	ASSERT_VALID( pThread );
	if ( pThread )
		return pThread->CreateThread( pszName, nPriority, dwCreateFlags, nStackSize, lpSecurityAttrs );
	return NULL;
}

void CloseThread(HANDLE* phThread, DWORD dwTimeout)
{
	__try
	{
		if ( *phThread )
		{
			__try
			{
				::SetThreadPriority( *phThread, THREAD_PRIORITY_NORMAL );

				while( *phThread )
				{
					SafeMessageLoop();

					DWORD res = MsgWaitForMultipleObjects( 1, phThread,
						FALSE, dwTimeout, QS_ALLINPUT | QS_ALLPOSTMESSAGE );
					if ( res == WAIT_OBJECT_0 + 1 )
						continue;		// Handle messages

					// Handle signaled state or errors...
					if ( res == WAIT_TIMEOUT )
						CAppThread::Terminate( *phThread );

					break;
				}
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				// Thread already ended
			}

			CAppThread::Remove( *phThread );

			*phThread = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// Deleted thread handler
	}
}
