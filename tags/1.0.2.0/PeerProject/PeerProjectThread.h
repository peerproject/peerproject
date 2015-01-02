//
// PeerProjectThread.h
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


#ifdef _DEBUG
	#define MS_VC_EXCEPTION	0x406D1388
	#define ALMOST_INFINITE	INFINITE
#else // Release
	#define ALMOST_INFINITE	20000	// 20s Thread Timeout
#endif


class CPeerThread : public CWinThread
{
	DECLARE_DYNAMIC(CPeerThread)

public:
	CPeerThread(AFX_THREADPROC pfnThreadProc = NULL, LPVOID pParam = NULL);
	virtual ~CPeerThread();

public:
	virtual HANDLE CreateThread(LPCSTR pszName, int nPriority, DWORD dwCreateFlags, UINT nStackSize, LPSECURITY_ATTRIBUTES lpSecurityAttrs, DWORD* pnThreadID);
	virtual BOOL InitInstance();
	virtual int Run();

	static void Add(CPeerThread* pThread, LPCSTR pszName);
	static void Remove(DWORD nThreadID);
	static HANDLE GetHandle(DWORD nThreadID);
	static bool IsThreadAlive(DWORD nThreadID);
	static bool SetThreadPriority(DWORD nThreadID, int nPriority);
	static void DeleteThread(DWORD nThreadID);
	static void DetachThread(DWORD nThreadID);
	static HANDLE BeginThread(LPCSTR pszName, AFX_THREADPROC pfnThreadProc,
							  LPVOID pParam, int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,
							  DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL, DWORD* pnThreadID = NULL);
	static void CloseThread(DWORD nThreadID, DWORD dwTimeout = ALMOST_INFINITE);

protected:
	typedef struct
	{
		CPeerThread*	pThread;	// Thread object
		LPCSTR			pszName;	// Thread name
	} CThreadTag;

	typedef CMap< DWORD, DWORD, CThreadTag, const CThreadTag& > CThreadMap;

	static CCriticalSection	m_ThreadMapSection;	// Guarding of m_ThreadMap
	static CThreadMap		m_ThreadMap;		// Map of running threads
	AFX_THREADPROC			m_pfnThreadProcExt;
	LPDWORD					m_pnOwnerThreadID;

private:
	CPeerThread(const CPeerThread&);
	CPeerThread& operator=(const CPeerThread&);
};
