//
// PeerProjectThread.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2008.
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


#ifdef _DEBUG
	#define MS_VC_EXCEPTION	0x406D1388
	#define ALMOST_INFINITE	INFINITE
#else // Release
	#define ALMOST_INFINITE	15000	// 15s Thread Timeout
#endif


class CAppThread : public CWinThread
{
	DECLARE_DYNAMIC(CAppThread)

public:
	CAppThread(AFX_THREADPROC pfnThreadProc = NULL, LPVOID pParam = NULL);
	virtual ~CAppThread();

	virtual HANDLE CreateThread(LPCSTR pszName, int nPriority = THREAD_PRIORITY_NORMAL,
		DWORD dwCreateFlags = 0, UINT nStackSize = 0,
		LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
	virtual BOOL InitInstance();
	virtual int Run();

	static void Add(CAppThread* pThread, LPCSTR pszName);
	static void Remove(HANDLE hThread);
	static void Terminate(HANDLE hThread);

protected:
	typedef struct
	{
		CAppThread* 	pThread;	// Thread object
		LPCSTR			pszName;	// Thread name
	} CThreadTag;

	typedef CMap< HANDLE, HANDLE, CThreadTag, const CThreadTag& > CThreadMap;

	static CCriticalSection	m_ThreadMapSection;	// Guarding of m_ThreadMap
	static CThreadMap		m_ThreadMap;		// Map of running threads
	AFX_THREADPROC			m_pfnThreadProcExt;

private:
	CAppThread(const CAppThread&);
	CAppThread& operator=(const CAppThread&);
};

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName);

HANDLE BeginThread(LPCSTR pszName, AFX_THREADPROC pfnThreadProc,
				   LPVOID pParam, int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,
				   DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

void CloseThread(HANDLE* phThread, DWORD dwTimeout = ALMOST_INFINITE);
