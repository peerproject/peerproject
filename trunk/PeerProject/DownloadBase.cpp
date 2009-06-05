//
// DownloadBase.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "Downloads.h"
#include "DownloadBase.h"
#include "DownloadTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CDownloadBase construction

CDownloadBase::CDownloadBase() :
	m_bSHA1Trusted		( false )
,	m_bTigerTrusted		( false )
,	m_bED2KTrusted		( false )
,	m_bBTHTrusted		( false )
,	m_bMD5Trusted		( false )
,	m_nCookie			( 1 )
,	m_pTask				( NULL )
{
}

CDownloadBase::~CDownloadBase()
{
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase check if a task is already running

bool CDownloadBase::IsTasking() const
{
	return m_pTask != NULL;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase set a new running task

void CDownloadBase::SetTask(CDownloadTask* pTask)
{
	m_pTask = pTask;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase return currently running task

DWORD CDownloadBase::GetTaskType() const
{
	return m_pTask->GetTaskType();
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase check if a task is the same as the currently running one

bool CDownloadBase::CheckTask(CDownloadTask* pTask) const
{
	return m_pTask == pTask;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase cancel currently running task

void CDownloadBase::AbortTask()
{
	if ( !IsTasking() )
		return;

	m_pTask->Abort();

	m_pTask = NULL;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase modified

void CDownloadBase::SetModified()
{
	++m_nCookie;
}

//////////////////////////////////////////////////////////////////////
// CDownloadBase serialize

void CDownloadBase::Serialize(CArchive& ar, int nVersion)
{
	if ( ar.IsStoring() )
	{
		ar << m_sName;
		ar << m_sSearchKeyword;
		ar << m_nSize;
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
	else
	{
		ar >> m_sName;

		if ( nVersion >= 29 )
		{
			if ( nVersion >= 33 )
			{
				ar >> m_sSearchKeyword;
			}
			ar >> m_nSize;
		}
		else
		{
			DWORD nSize;
			ar >> nSize;
			m_nSize = nSize;
		}
		uint32 b;
		SerializeIn( ar, m_oSHA1, nVersion );
		ar >> b;
		m_bSHA1Trusted = b != 0;
		SerializeIn( ar, m_oTiger, nVersion );
		ar >> b;
		m_bTigerTrusted = b != 0;
		if ( nVersion >= 22 )
		{
			SerializeIn( ar, m_oMD5, nVersion );
			ar >> b;
			m_bMD5Trusted = b != 0;
		}
		if ( nVersion >= 13 )
		{
			SerializeIn( ar, m_oED2K, nVersion );
			ar >> b;
			m_bED2KTrusted = b != 0;
		}
		if ( nVersion >= 37 )
		{
			SerializeIn( ar, m_oBTH, nVersion );
			ar >> b;
			m_bBTHTrusted = b != 0;
		}
	}
}
