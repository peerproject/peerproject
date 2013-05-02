//
// DownloadBase.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
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
#include "Settings.h"
#include "PeerProject.h"
#include "DownloadBase.h"
#include "DownloadTask.h"
#include "Downloads.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CDownloadBase, CPeerProjectFile)

//////////////////////////////////////////////////////////////////////
// CDownloadBase construction

CDownloadBase::CDownloadBase()
	: m_bSHA1Trusted	( false )
	, m_bTigerTrusted	( false )
	, m_bED2KTrusted	( false )
	, m_bBTHTrusted		( false )
	, m_bMD5Trusted		( false )
	, m_nCookie			( 1 )
	, m_nSaveCookie		( 0 )
	, m_pTask			( NULL )
{
}

CDownloadBase::~CDownloadBase()
{
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase check if a task is already running

bool CDownloadBase::IsTasking() const
{
	return ( m_pTask != NULL );
}

bool CDownloadBase::IsMoving() const
{
	return ( GetTaskType() == dtaskCopy );
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase set a new running task

void CDownloadBase::SetTask(CDownloadTask* pTask)
{
	m_pTask = pTask;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase return currently running task

dtask CDownloadBase::GetTaskType() const
{
	return m_pTask ? m_pTask->GetTaskType() : dtaskNone;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase check if a task is the same as the currently running one

bool CDownloadBase::CheckTask(CDownloadTask* pTask) const
{
	return ( m_pTask == pTask );
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase cancel currently running task

void CDownloadBase::AbortTask()
{
	if ( ! IsTasking() )
		return;

	m_pTask->Abort();

	m_pTask = NULL;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase modified

bool CDownloadBase::IsModified() const
{
	return ( m_nCookie != m_nSaveCookie );
}

void CDownloadBase::SetModified()
{
	++m_nCookie;
}

//////////////////////////////////////////////////////////////////////
// CDownload control : safe rename (moved to DownloadWithFile)

//////////////////////////////////////////////////////////////////////
// CDownloadBase serialize

void CDownloadBase::Serialize(CArchive& ar, int nVersion)
{
	if ( ar.IsStoring() )
	{
		ar << m_sName;
		ar << m_nSize;
		ar << m_tDate;
	//	CString strSearchKeyword;
	//	ar << strSearchKeyword;	// ToDo: ShareazaPlus feature?
		SerializeOut( ar, m_oSHA1 );
		ar << (uint32)m_bSHA1Trusted;
		SerializeOut( ar, m_oTiger );
		ar << (uint32)m_bTigerTrusted;
		SerializeOut( ar, m_oMD5 );
		ar << (uint32)m_bMD5Trusted;
		SerializeOut( ar, m_oED2K );
		ar << (uint32)m_bED2KTrusted;
		SerializeOut( ar, m_oBTH );
		ar << (uint32)m_bBTHTrusted;
	}
	else // Loading
	{
		ar >> m_sName;

		if ( nVersion < 100 && nVersion > 32 )
		{
			CString strSearchKeyword;
			ar >> strSearchKeyword;		// Shareaza compatibility for ShareazaPlus
		}

		//if ( nVersion < 28 )
		//{
		//	DWORD nSize;
		//	ar >> nSize;
		//	m_nSize = nSize;
		//}
		//else
		ar >> m_nSize;

		if ( nVersion >= 1000 )
			ar >> m_tDate;
		else	// Shareaza import
			m_tDate = CTime::GetCurrentTime();

	//	CString strSearchKeyword;
	//	ar >> strSearchKeyword;			// ToDo: SharazaPlus feature?

		uint32 b;
		SerializeIn( ar, m_oSHA1, nVersion );
		ar >> b;
		m_bSHA1Trusted = b != 0;
		SerializeIn( ar, m_oTiger, nVersion );
		ar >> b;
		m_bTigerTrusted = b != 0;
		//if ( nVersion > 36 )
		//{
			SerializeIn( ar, m_oMD5, nVersion );
			ar >> b;
			m_bMD5Trusted = b != 0;

			SerializeIn( ar, m_oED2K, nVersion );
			ar >> b;
			m_bED2KTrusted = b != 0;

			SerializeIn( ar, m_oBTH, nVersion );
			ar >> b;
			m_bBTHTrusted = b != 0;
		//}
	}
}
