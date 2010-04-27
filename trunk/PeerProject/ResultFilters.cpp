//
// ResultFilters.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2008.
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

///////////////////////////////////////////////////
// ResultFilters
// Save the filters used for results

#include "stdafx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "ResultFilters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CResultFilters::CResultFilters(void)
	: m_pFilters	( NULL )
	, m_nFilters	( 0 )
	, m_nDefault	( NONE )
{
}

CResultFilters::~CResultFilters(void)
{
	if ( m_pFilters )
	{
		for ( DWORD i = 0; i < m_nFilters; i++ )
		{
			delete m_pFilters[i];
		}
	}

	delete [] ( m_pFilters );
}

#define RESULTFILTERS_SER_VERSION	1000	// 2
// nVersion History:
//  1 - Legacy
//  2 - Cumulative Additions (Shareaza 2.3)
//  1000 - (PeerProject 1.0) (2)

void CResultFilters::Serialize(CArchive & ar)
{
	int nVersion = RESULTFILTERS_SER_VERSION;

	if ( ar.IsStoring() )
	{
		ar << nVersion;

		ar.WriteCount( m_nFilters );

		for ( DWORD i = 0; i < m_nFilters; i++ )
		{
			CFilterOptions* pFilter = m_pFilters[ i ];
			pFilter->Serialize( ar, nVersion );
		}

		ar << m_nDefault;
	}
	else // Loading
	{
		ar >> nVersion;

		m_nFilters = static_cast< DWORD >( ar.ReadCount() );

		m_pFilters = new CFilterOptions *[ m_nFilters ];
		ZeroMemory( m_pFilters, sizeof(CFilterOptions*) * m_nFilters );

		for ( DWORD i = 0; i < m_nFilters; i++ )
		{
			CFilterOptions* pFilter = new CFilterOptions();
			m_pFilters[ i ] = pFilter;
			pFilter->Serialize( ar, nVersion );
		}

		ar >> m_nDefault;
	}
}

void CResultFilters::Add(CFilterOptions *pOptions)
{
	CFilterOptions **pFilters = new CFilterOptions * [m_nFilters + 1];

	CopyMemory(pFilters, m_pFilters, sizeof(CFilterOptions *) * m_nFilters);

	pFilters[m_nFilters++] = pOptions;

	delete [] m_pFilters;

	m_pFilters = pFilters;
}

// Search for (first) filter with name strName, return index if found, -1 (NONE) otherwise
int CResultFilters::Search(const CString& strName)
{
	for ( DWORD index = 0; index < m_nFilters; index++ )
	{
		if ( strName.Compare( m_pFilters[index]->m_sName ) == 0 )
			return index;
	}
	return NONE;
 }

void CResultFilters::Remove(DWORD index)
{
	if ( index < m_nFilters )
	{
		delete m_pFilters[index];
		CopyMemory( &m_pFilters[index], &m_pFilters[index + 1], sizeof(CFilterOptions *) * (m_nFilters - index) );
		m_nFilters--;

		if ( index == m_nDefault )
			m_nDefault = NONE;
		else if ( m_nDefault != NONE && index < m_nDefault )
			m_nDefault--;

		if ( m_nFilters == 0 )
			m_nDefault = NONE;
	}
}

void CResultFilters::Load()
{
	// Delete old content first
	if ( m_pFilters )
	{
		for ( DWORD i = 0; i < m_nFilters; i++ )
		{
			delete m_pFilters[i];
		}
	}
	delete [] ( m_pFilters );

	CString strFile = Settings.General.UserPath + _T("\\Data\\Filters.dat");

	CFile f;
	if ( f.Open( strFile, CFile::modeRead ) )
	{
		try
		{
			CArchive ar( &f, CArchive::load );	// 4 KB buffer
			Serialize( ar );
		}
		catch ( CException* pException )
		{
			pException->Delete();
		}
	}
}

BOOL CResultFilters::Save()
{
	CString strFile = Settings.General.UserPath + _T("\\Data\\Filters.dat");

	CFile pFile;
	if ( ! pFile.Open( strFile, CFile::modeCreate | CFile::modeWrite ) )
		return FALSE;

	try
	{
		CArchive ar( &pFile, CArchive::store );	// 4 KB buffer
		try
		{
			Serialize( ar );
			ar.Close();
		}
		catch ( CException* pException )
		{
			ar.Abort();
			pFile.Abort();
			pException->Delete();
			return FALSE;
		}
		pFile.Close();
	}
	catch ( CException* pException )
	{
		pFile.Abort();
		pException->Delete();
		return FALSE;
	}

	return TRUE;
}


////////////////////////////////////////////////////
// FilterOptions	( The filter settings )

CFilterOptions::CFilterOptions()
{
	// Mask 0x280 = 01010000000  (Reverse Order)
	m_bRegExp			= ( Settings.Search.FilterMask & ( 1 << 0 ) ) > 0;	// 0
	m_bFilterPush		= ( Settings.Search.FilterMask & ( 1 << 1 ) ) > 0;	// 0
	m_bFilterUnstable	= ( Settings.Search.FilterMask & ( 1 << 2 ) ) > 0;	// 0
	m_bFilterBusy		= ( Settings.Search.FilterMask & ( 1 << 3 ) ) > 0;	// 0
	m_bFilterReject		= ( Settings.Search.FilterMask & ( 1 << 4 ) ) > 0;	// 0
	m_bFilterLocal		= ( Settings.Search.FilterMask & ( 1 << 5 ) ) > 0;	// 0
	m_bFilterBogus		= ( Settings.Search.FilterMask & ( 1 << 6 ) ) > 0;	// 0
	m_bFilterDRM		= ( Settings.Search.FilterMask & ( 1 << 7 ) ) > 0;	// 1
	m_bFilterRestricted	= ( Settings.Search.FilterMask & ( 1 << 8 ) ) > 0;	// 0
	m_bFilterSuspicious	= ( Settings.Search.FilterMask & ( 1 << 9 ) ) > 0;	// 1
	m_bFilterAdult		= ( Settings.Search.FilterMask & ( 1 << 10 ) ) > 0;	// 0
	//m_nFilterMinSize	= ( Settings.Search.FilterMask & ( 1 << 10 ) ) > 0;	// 1;
	//m_nFilterMaxSize	= ( Settings.Search.FilterMask & ( 1 << 11 ) ) > 0;	// 0;
	//m_nFilterSources	= ( Settings.Search.FilterMask & ( 1 << 12 ) ) > 0;	// 1;
}

void CFilterOptions::Serialize(CArchive & ar, int /*nVersion*/)	// RESULTFILTERS_SER_VERSION
{
	if ( ar.IsStoring() ) // Saving
	{
		ar << m_sName;
		ar << m_sFilter;
		ar << m_bFilterPush;
		ar << m_bFilterUnstable;
		ar << m_bFilterBusy;
		ar << m_bFilterLocal;
		ar << m_bFilterReject;
		ar << m_bFilterBogus;
		ar << m_bFilterDRM;
		ar << m_bFilterRestricted;
		ar << m_bFilterSuspicious;
		ar << m_bFilterAdult;
		ar << m_nFilterMinSize;
		ar << m_nFilterMaxSize;
		ar << m_nFilterSources;
		ar << m_bRegExp;
	}
	else // Loading
	{
		ar >> m_sName;
		ar >> m_sFilter;
		ar >> m_bFilterPush;
		ar >> m_bFilterUnstable;
		ar >> m_bFilterBusy;
		ar >> m_bFilterLocal;
		ar >> m_bFilterReject;
		ar >> m_bFilterBogus;
		ar >> m_bFilterDRM;
		//if ( nVersion >= 1000 )
			ar >> m_bFilterRestricted;
		ar >> m_bFilterSuspicious;
		ar >> m_bFilterAdult;
		ar >> m_nFilterMinSize;
		ar >> m_nFilterMaxSize;
		ar >> m_nFilterSources;
		ar >> m_bRegExp;
		if ( m_sFilter.IsEmpty() )
			m_bRegExp = FALSE;
	}
}
