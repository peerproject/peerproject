//
// UploadFile.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008
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
#include "UploadFile.h"
#include "UploadTransfer.h"
#include "FragmentedFile.h"
#include "Statistics.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CUploadFile construction

CUploadFile::CUploadFile(CUploadTransfer* pUpload, const Hashes::Sha1Hash& oSHA1, LPCTSTR pszName, LPCTSTR pszPath, QWORD nSize) :
	m_pAddress( pUpload->m_pHost.sin_addr ),
	m_nRequests( 0 ),
	m_oFragments( nSize ),
	m_bSelected( FALSE )
{
	m_sName		= pszName;
	m_sPath		= pszPath;
	m_nSize		= nSize;
    m_oSHA1		= oSHA1;
	m_pTransfers.AddTail( pUpload );
}

CUploadFile::~CUploadFile()
{
}

//////////////////////////////////////////////////////////////////////
// CUploadFile transfer operations

void CUploadFile::Add(CUploadTransfer* pUpload)
{
	if ( m_pTransfers.Find( pUpload ) == NULL ) m_pTransfers.AddTail( pUpload );
}

BOOL CUploadFile::Remove(CUploadTransfer* pUpload)
{
	POSITION pos = m_pTransfers.Find( pUpload );
	if ( pos == NULL ) return FALSE;

	m_pTransfers.RemoveAt( pos );

	return IsEmpty();
}

CUploadTransfer* CUploadFile::GetActive() const
{
	if ( IsEmpty() ) return NULL;

	for ( POSITION pos = m_pTransfers.GetHeadPosition() ; pos ; )
	{
		CUploadTransfer* pUpload = m_pTransfers.GetNext( pos );
		if ( pUpload->m_nState != upsNull )
			return pUpload;
	}

	return m_pTransfers.GetTail();
}

void CUploadFile::Remove()
{
	for ( POSITION pos = m_pTransfers.GetHeadPosition() ; pos ; )
	{
		CUploadTransfer* pUpload = m_pTransfers.GetNext( pos );
		pUpload->Remove();
	}
}

//////////////////////////////////////////////////////////////////////
// CUploadFile fragments

void CUploadFile::AddFragment(QWORD nOffset, QWORD nLength)
{
	if ( m_oFragments.empty() )
		Statistics.Current.Uploads.Files++;
	
	m_oFragments.insert( Fragments::Fragment( nOffset, nOffset + nLength ) );
}

