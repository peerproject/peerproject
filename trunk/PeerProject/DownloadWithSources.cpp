//
// DownloadWithSources.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "Downloads.h"
#include "Download.h"
#include "DownloadWithSources.h"
#include "DownloadTransfer.h"
#include "DownloadSource.h"
#include "Network.h"
#include "Neighbours.h"
#include "Transfer.h"
#include "QueryHit.h"
#include "PeerProjectURL.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "Library.h"
#include "SharedFile.h"
#include "XML.h"
#include "SHA.h"
#include "MD4.h"
#include "TigerTree.h"
#include "QueryHashMaster.h"
#include "VendorCache.h"
#include "Transfers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CDownloadWithSources construction

CDownloadWithSources::CDownloadWithSources() :
	m_pSourceFirst		( NULL )
,	m_pSourceLast		( NULL )

,	m_nSourceCount		( 0 )
,	m_pXML				( NULL )

,	m_nBTSourceCount	( 0 )
,	m_nG1SourceCount	( 0 )
,	m_nG2SourceCount	( 0 )
,	m_nEdSourceCount	( 0 )
,	m_nHTTPSourceCount	( 0 )
,	m_nFTPSourceCount	( 0 )
{
}

CDownloadWithSources::~CDownloadWithSources()
{
	ClearSources();
	if ( m_pXML != NULL ) delete m_pXML;
	
	for ( POSITION pos = m_pFailedSources.GetHeadPosition() ; pos ; )
		delete m_pFailedSources.GetNext( pos );

	m_pFailedSources.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources list access

DWORD CDownloadWithSources::GetSourceCount(BOOL bNoPush, BOOL bSane) const
{
	CQuickLock pLock( Transfers.m_pSection );

	if ( ! bNoPush && ! bSane ) return m_nSourceCount;
	
	DWORD tNow = GetTickCount();
	DWORD nCount = 0;
	
	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; pSource = pSource->m_pNext )
	{
		if ( ! bNoPush || ! pSource->m_bPushOnly )
		{
			if ( ! bSane || (
			   ( pSource->m_tAttempt < tNow ||
				 pSource->m_tAttempt - tNow <= 900000 ) &&
				 ! pSource->m_bKeep ) )
			{
				nCount++;
			}
		}
	}
	
	return nCount;
}

DWORD CDownloadWithSources::GetEffectiveSourceCount() const
{
	DWORD nResult = 0;
	if ( Settings.IsG1Allowed() )
		nResult += m_nG1SourceCount;
	if ( Settings.IsG2Allowed() )
		nResult += m_nG2SourceCount;
	if ( Settings.IsEdAllowed() )
		nResult += m_nEdSourceCount;
	if ( Settings.IsG1Allowed() || Settings.IsG2Allowed() )
		nResult += m_nHTTPSourceCount;
	return nResult + m_nBTSourceCount + m_nFTPSourceCount;
}

DWORD CDownloadWithSources::GetBTSourceCount(BOOL bNoPush) const
{
	CQuickLock pLock( Transfers.m_pSection );

	DWORD tNow = GetTickCount();
	DWORD nCount = 0;
	
	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; pSource = pSource->m_pNext )
	{
		if ( ( pSource->m_nProtocol == PROTOCOL_BT ) &&									// Only counting BT sources
			 ( pSource->m_tAttempt < tNow || pSource->m_tAttempt - tNow <= 900000 ) &&	// Don't count dead sources
			 ( ! pSource->m_bPushOnly || ! bNoPush ) )									// Push sources might not be counted
		{
			nCount++;
		}
	}

	return nCount;
}

DWORD CDownloadWithSources::GetED2KCompleteSourceCount() const
{
	CQuickLock pLock( Transfers.m_pSection );

	DWORD tNow = GetTickCount();
	DWORD nCount = 0;
	
	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; pSource = pSource->m_pNext )
	{
		if ( ( ! pSource->m_bPushOnly ) &&						// Push sources shouldn't be counted since you often cannot reach them
			 ( pSource->m_tAttempt < tNow || pSource->m_tAttempt - tNow <= 900000 ) &&	// Only count sources that are probably active
			 ( pSource->m_nProtocol == PROTOCOL_ED2K ) &&		// Only count ed2k sources
             ( pSource->m_oAvailable.empty() && pSource->IsOnline() ) )	// Only count complete sources
			
		{
			nCount++;
		}
	}

	return nCount;
}

BOOL CDownloadWithSources::CheckSource(CDownloadSource* pCheck) const
{
	CQuickLock pLock( Transfers.m_pSection );

	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; pSource = pSource->m_pNext )
	{
		if ( pSource == pCheck ) return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources clear

void CDownloadWithSources::ClearSources()
{
	CQuickLock pLock( Transfers.m_pSection );

	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; )
	{
		CDownloadSource* pNext = pSource->m_pNext;
		delete pSource;
		pSource = pNext;
	}

	m_pSourceFirst = m_pSourceLast = NULL;
	m_nSourceCount = m_nEdSourceCount = m_nG1SourceCount = m_nFTPSourceCount = 0;
	m_nG2SourceCount = m_nHTTPSourceCount = m_nBTSourceCount = 0;
	
	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources add a query-hit source

BOOL CDownloadWithSources::AddSourceHit(CQueryHit* pHit, BOOL bForce)
{
	CQuickLock pLock( Transfers.m_pSection );

	BOOL bHash = FALSE;
	BOOL bUpdated = FALSE;
	
	if ( ! bForce )
	{
		if ( m_oSHA1 && pHit->m_oSHA1 )
		{
			if ( m_oSHA1 != pHit->m_oSHA1 ) return FALSE;
			bHash = TRUE;
		}
		// We should check Tiger as well as others. This is because
		// there exist some hash combinations, even for PeerProject 2.2.0.0 
		// installer file, i.e. with the same SHA1 but different Tiger (CyberBob).

		if ( m_oTiger && pHit->m_oTiger )
		{
			if ( m_oTiger != pHit->m_oTiger ) return FALSE;
			bHash = TRUE;
		}
        if ( m_oED2K && pHit->m_oED2K )
		{
			if ( m_oED2K != pHit->m_oED2K ) return FALSE;
			bHash = TRUE;
		}
		if ( m_oMD5 && pHit->m_oMD5 )
		{
			if ( m_oMD5 != pHit->m_oMD5 ) return FALSE;
			bHash = TRUE;
		}
		// BTH check is a last chance
		if ( ! bHash && m_oBTH && pHit->m_oBTH )
		{
			if ( m_oBTH != pHit->m_oBTH ) return FALSE;
			bHash = TRUE;
		}
	}
	
	if ( ! bHash && ! bForce )
	{
		if ( Settings.General.HashIntegrity ) return FALSE;
		
		if ( m_sName.IsEmpty() || pHit->m_sName.IsEmpty() ) return FALSE;
		if ( m_nSize == SIZE_UNKNOWN || ! pHit->m_bSize ) return FALSE;
		
		if ( m_nSize != pHit->m_nSize ) return FALSE;
		if ( m_sName.CompareNoCase( pHit->m_sName ) ) return FALSE;
	}
	
	if ( !m_oSHA1 && pHit->m_oSHA1 )
	{
		m_oSHA1 = pHit->m_oSHA1;
		bUpdated = TRUE;
	}
    if ( !m_oTiger && pHit->m_oTiger )
	{
		m_oTiger = pHit->m_oTiger;
		bUpdated = TRUE;
	}
    if ( !m_oED2K && pHit->m_oED2K )
	{
		m_oED2K = pHit->m_oED2K;
		bUpdated = TRUE;
	}
	if ( !m_oBTH && pHit->m_oBTH )
	{
		m_oBTH = pHit->m_oBTH;
		bUpdated = TRUE;
	}
	if ( !m_oMD5 && pHit->m_oMD5 )
	{
		m_oMD5 = pHit->m_oMD5;
		bUpdated = TRUE;
	}
	
	if ( m_nSize == SIZE_UNKNOWN && pHit->m_bSize )
	{
		m_nSize = pHit->m_nSize;
	}
	else if ( m_nSize != SIZE_UNKNOWN && pHit->m_bSize && m_nSize != pHit->m_nSize )
	{
		return FALSE;
	}

	if ( m_sName.IsEmpty() && pHit->m_sName.GetLength() )
	{
		m_sName = pHit->m_sName;
	}
	
	if ( Settings.Downloads.Metadata && m_pXML == NULL )
	{
		if ( pHit->m_pXML != NULL && pHit->m_sSchemaPlural.GetLength() )
		{
			m_pXML = new CXMLElement( NULL, pHit->m_sSchemaPlural );
			m_pXML->AddAttribute( _T("xmlns:xsi"), CXMLAttribute::xmlnsInstance );
			m_pXML->AddAttribute( CXMLAttribute::schemaName, pHit->m_sSchemaURI );
			m_pXML->AddElement( pHit->m_pXML->Clone() );
			
			if ( CSchema* pSchema = SchemaCache.Get( pHit->m_sSchemaURI ) )
			{
				pSchema->Validate( m_pXML, TRUE );
			}
		}
	}

	/*
	if ( pHit->m_nProtocol == PROTOCOL_ED2K )
	{
		Neighbours.FindDonkeySources( pHit->m_oED2K,
			(IN_ADDR*)pHit->m_oClientID.begin(), (WORD)pHit->m_oClientID.begin()[1] );
	}
	*/

	// No URL, stop now with success
	if ( ! pHit->m_sURL.IsEmpty() )
	{	
		if ( ! AddSourceInternal( new CDownloadSource( (CDownload*)this, pHit ) ) )
		{
			return FALSE;
		}
	}

	if ( bUpdated )	QueryHashMaster.Invalidate();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources add miscellaneous sources

BOOL CDownloadWithSources::AddSourceED2K(DWORD nClientID, WORD nClientPort, DWORD nServerIP, WORD nServerPort, const Hashes::Guid& oGUID)
{
	return AddSourceInternal( new CDownloadSource( (CDownload*)this, nClientID, nClientPort, nServerIP, nServerPort, oGUID ) );
}

BOOL CDownloadWithSources::AddSourceBT(const Hashes::BtGuid& oGUID, IN_ADDR* pAddress, WORD nPort)
{
	// Unreachable (Push) BT sources should never be added.
	if ( Network.IsFirewalledAddress( pAddress, Settings.Connection.IgnoreOwnIP ) )
		return FALSE;
	
	// Check for own IP, in case IgnoreLocalIP is not set
	if ( ( Settings.Connection.IgnoreOwnIP ) && Network.IsSelfIP( *pAddress ) ) 
		return FALSE;

	return AddSourceInternal( new CDownloadSource( (CDownload*)this, oGUID, pAddress, nPort ) );
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources add a single URL source

BOOL CDownloadWithSources::AddSourceURL(LPCTSTR pszURL, BOOL bURN, FILETIME* pLastSeen, int nRedirectionCount, BOOL bFailed)
{
	if ( pszURL == NULL ) return FALSE;
	if ( *pszURL == 0 ) return FALSE;
	if ( nRedirectionCount > 5 ) return FALSE; // No more than 5 redirections
	
	BOOL bHashAuth = FALSE;
	BOOL bValidated = FALSE;
	CPeerProjectURL pURL;
	
	if ( *pszURL == '@' )
	{
		bHashAuth = TRUE;
		pszURL++;
	}
	
	if ( ! pURL.Parse( pszURL ) ) return FALSE;
	
	if ( bURN )
	{
		if ( Network.IsSelfIP( pURL.m_pAddress ) ||
			 Network.IsFirewalledAddress( &pURL.m_pAddress, TRUE ) || 
			 Network.IsReserved( &pURL.m_pAddress ) ) return FALSE;
	}

	CQuickLock pLock( Transfers.m_pSection );

	CFailedSource* pBadSource = LookupFailedSource( pszURL );
	if ( pBadSource )
	{
		// Add a positive vote, add to the downloads if the negative votes compose
		// less than 2/3 of total.
		int nTotal = pBadSource->m_nPositiveVotes + pBadSource->m_nNegativeVotes + 1;
		if ( bFailed )
			pBadSource->m_nNegativeVotes++;
		else
			pBadSource->m_nPositiveVotes++;

		if ( nTotal > 30 && pBadSource->m_nNegativeVotes / nTotal > 2 / 3 )
			return FALSE;
	}
	else if ( bFailed )
	{
		AddFailedSource( pszURL, false );
		VoteSource( pszURL, false );
		return TRUE;
	}

	// Validate SHA1
	if ( pURL.m_oSHA1 && m_oSHA1 )
	{
		if ( m_oSHA1 != pURL.m_oSHA1 ) return FALSE;
		bValidated = TRUE;
	}
	// Validate Tiger
	if ( pURL.m_oTiger && m_oTiger )
	{
		if ( m_oTiger != pURL.m_oTiger ) return FALSE;
		bValidated = TRUE;
	}
	// Validate ED2K
	if ( pURL.m_oED2K && m_oED2K )
	{
		if ( m_oED2K != pURL.m_oED2K ) return FALSE;
		bValidated = TRUE;
	}
	// Validate MD5
	if ( pURL.m_oMD5 && m_oMD5 )
	{
		if ( m_oMD5 != pURL.m_oMD5 ) return FALSE;
		bValidated = TRUE;
	}
	// Validate BTH
	if ( pURL.m_oBTH && m_oBTH && ! bValidated )
	{
		if ( m_oBTH != pURL.m_oBTH ) return FALSE;
		bValidated = TRUE;
	}
	// Validate size
	if ( m_nSize != SIZE_UNKNOWN && pURL.m_bSize && pURL.m_nSize != SIZE_UNKNOWN )
	{
		if ( m_nSize != pURL.m_nSize ) return FALSE;
	}

	// Get SHA1
	if ( pURL.m_oSHA1 && ! m_oSHA1 )
	{
		m_oSHA1 = pURL.m_oSHA1;
	}
	// Get Tiger
	if ( pURL.m_oTiger && ! m_oTiger )
	{
		m_oTiger = pURL.m_oTiger;
	}
	// Get ED2K
	if ( pURL.m_oED2K && ! m_oED2K )
	{
		m_oED2K = pURL.m_oED2K;
	}
	// Get MD5
	if ( pURL.m_oMD5 && ! m_oMD5 )
	{
		m_oMD5 = pURL.m_oMD5;
	}
	// Get BTH
	if ( pURL.m_oBTH && ! m_oBTH )
	{
		m_oBTH = pURL.m_oBTH;
	}
	// Get size
	if ( m_nSize == SIZE_UNKNOWN &&
		pURL.m_bSize && pURL.m_nSize && pURL.m_nSize != SIZE_UNKNOWN )
	{
		m_nSize = pURL.m_nSize;
	}
	// Get name
	if ( m_sName.IsEmpty() && pURL.m_sName.GetLength() )
	{
		m_sName = pURL.m_sName;
	}

	return AddSourceInternal( new CDownloadSource( static_cast< const CDownload* >( this ),
		pszURL, bURN, bHashAuth, pLastSeen, nRedirectionCount ) );
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources add several URL sources

int CDownloadWithSources::AddSourceURLs(LPCTSTR pszURLs, BOOL bURN, BOOL bFailed)
{
	if ( IsCompleted() || IsMoving() )
	{
		ClearSources();
		return 0;
	}
	else if ( IsPaused() )
		return 0;

	int nCount = 0;

	CMapStringToFILETIME oUrls;
	SplitStringToURLs( pszURLs, oUrls );

	for ( POSITION pos = oUrls.GetStartPosition(); pos; )
	{
		CString strURL;
		FILETIME tSeen = {};
		oUrls.GetNextAssoc( pos, strURL, tSeen );
		
		if ( AddSourceURL( strURL, bURN,
			( tSeen.dwLowDateTime | tSeen.dwHighDateTime ) ? &tSeen : NULL, 0, bFailed ) )
		{
			if ( bFailed )
			{
				theApp.Message( MSG_DEBUG, L"Adding X-NAlt: %s", (LPCTSTR)strURL );
			}
			else
			{
				theApp.Message( MSG_DEBUG, L"Adding X-Alt: %s", (LPCTSTR)strURL );
			}
			nCount++;
		}
	}

	return nCount;
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources internal source adder

BOOL CDownloadWithSources::AddSourceInternal(CDownloadSource* pSource)
{
	//Check/Reject if source is invalid
	if ( ! pSource->m_bPushOnly )
	{
		//Reject invalid IPs (Sometimes ed2k sends invalid 0.x.x.x sources)
		if ( pSource->m_pAddress.S_un.S_un_b.s_b1 == 0 || pSource->m_nPort == 0 )
		{
			delete pSource;
			return FALSE;
		}

		//Reject if source is the local IP/port
		if ( Network.IsSelfIP( pSource->m_pAddress ) )
		{
			if ( ( ( pSource->m_nServerPort == 0 ) && (Settings.Connection.InPort == pSource->m_nPort ) )
				|| ( Settings.Connection.IgnoreOwnIP ) )
			{	
				delete pSource;
				return FALSE;
			}
		}
	}
	else if ( pSource->m_nProtocol == PROTOCOL_ED2K )
	{
		//Reject invalid server IPs (Sometimes ed2k sends invalid 0.x.x.x sources)
		if ( pSource->m_pServerAddress.S_un.S_un_b.s_b1 == 0 )
		{
			delete pSource;
			return FALSE;
		}
	}
	
	bool bG2Exists = false;
	bool bExistingIsRaza = false;
	bool bDeleteSource = false;
	CDownloadSource* pCopy = NULL;
	CQuickLock pLock( Transfers.m_pSection );

	if ( pSource->m_nRedirectionCount == 0 ) // Don't check for existing sources if source is a redirection
	{
		for ( CDownloadSource* pExisting = m_pSourceFirst ; pExisting ; )
		{	
			if ( pExisting->Equals( pSource ) ) // IPs and ports are equal
			{	
				if ( !bExistingIsRaza )
					bExistingIsRaza = VendorCache.IsExtended( pExisting->m_sServer );

				if ( !bG2Exists )
					bG2Exists = ( pExisting->m_nProtocol == PROTOCOL_HTTP || pExisting->m_nProtocol == PROTOCOL_G2 );

				// Point to non-HTTP source which is PeerProject
				if ( bExistingIsRaza )
					pCopy = bG2Exists ? NULL : pExisting;
				
				if ( pExisting->m_nProtocol == pSource->m_nProtocol )
					bDeleteSource = true;

				if ( pExisting->m_pTransfer != NULL ) // We already downloading
				{
					// Remove new source which is not HTTP and return
					if ( ( pExisting->m_nProtocol == PROTOCOL_HTTP || pExisting->m_nProtocol == PROTOCOL_G2 ) && 
						 ( pSource->m_nProtocol != PROTOCOL_HTTP && pSource->m_nProtocol != PROTOCOL_G2 ) )
						bDeleteSource = true;
				}
				else // We are not downloading
				{
					// Replace non-HTTP source with a new one (we will add it later)
					if ( ( pExisting->m_nProtocol != PROTOCOL_HTTP && pExisting->m_nProtocol != PROTOCOL_G2 ) && 
						 ( pSource->m_nProtocol == PROTOCOL_HTTP || pSource->m_nProtocol == PROTOCOL_G2 ) )
					{
						// Set connection delay the same as for the old source
						pSource->m_tAttempt = pExisting->m_tAttempt;
						pCopy = NULL;	// We are adding HTTP source, thus no need to make G2

						if ( pExisting->m_pNext )
						{
							pExisting = pExisting->m_pNext;
							pExisting->m_pPrev->Remove( TRUE, FALSE );
						}
						else
						{
							pExisting->Remove( TRUE, FALSE );
							pExisting = NULL;
						}
						// We are already pointing to the next existing source, continue loop
						continue;
					}
				}
			}
			pExisting = pExisting->m_pNext;
		}
	}

	// We don't need to make G2 source
	if ( pCopy && ( pCopy->m_bPushOnly || bExistingIsRaza && bG2Exists ) )
		pCopy = NULL;

	// Make G2 source from the existing non-HTTP PeerProject source
	if ( pCopy && Settings.Gnutella2.EnableToday )
	{
		CString strURL = GetURL( pCopy->m_pAddress, pCopy->m_nPort );
		if ( strURL.GetLength() )
		{
			CDownloadSource* pG2Source  = new CDownloadSource( (CDownload*)this, strURL );
			pG2Source->m_sServer = pCopy->m_sServer;	// Copy user-agent
			pG2Source->m_tAttempt = pCopy->m_tAttempt;	// Set the same connection delay
			pG2Source->m_nProtocol = PROTOCOL_HTTP;

			m_nSourceCount++;
			m_nHTTPSourceCount++;

			pG2Source->m_pPrev = m_pSourceLast;
			pG2Source->m_pNext = NULL;
				
			if ( m_pSourceLast != NULL )
			{
				m_pSourceLast->m_pNext = pG2Source;
				m_pSourceLast = pG2Source;
			}
			else
			{
				m_pSourceFirst = m_pSourceLast = pG2Source;
			}

			if ( pG2Source->m_pTransfer == NULL )
			{
				if ( CDownloadTransfer* pTransfer = pG2Source->CreateTransfer() )
					pTransfer->Initiate();
			}
		}
	}
	
	if ( bDeleteSource )
	{
		delete pSource;
		SetModified();
		return FALSE;
	}

	m_nSourceCount++;

	if ( pSource->m_nProtocol == PROTOCOL_G1 )
		m_nG1SourceCount++;
	else if ( pSource->m_nProtocol == PROTOCOL_G2 )
		m_nG2SourceCount++;
	else if ( pSource->m_nProtocol == PROTOCOL_ED2K )
		m_nEdSourceCount++;
	else if ( pSource->m_nProtocol == PROTOCOL_HTTP )
		m_nHTTPSourceCount++;
	else if ( pSource->m_nProtocol == PROTOCOL_BT )
		m_nBTSourceCount++;
	else if ( pSource->m_nProtocol == PROTOCOL_FTP )
		m_nFTPSourceCount++;

	pSource->m_pPrev = m_pSourceLast;
	pSource->m_pNext = NULL;
		
	if ( m_pSourceLast != NULL )
	{
		m_pSourceLast->m_pNext = pSource;
		m_pSourceLast = pSource;
	}
	else
	{
		m_pSourceFirst = m_pSourceLast = pSource;
	}
	
	SetModified();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources query for URLs

CString CDownloadWithSources::GetSourceURLs(CList< CString >* pState, int nMaximum, PROTOCOLID nProtocol, CDownloadSource* pExcept)
{
	CQuickLock pLock( Transfers.m_pSection );

	CString strSources;
	
	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; pSource = pSource->m_pNext )
	{
		if ( pSource != pExcept && pSource->m_bPushOnly == FALSE &&
			 pSource->m_nFailures == 0 && pSource->m_bReadContent &&
			 ( pSource->m_bSHA1 || pSource->m_bED2K || pSource->m_bBTH  || pSource->m_bMD5 ) &&
			 ( pState == NULL || pState->Find( pSource->m_sURL ) == NULL ) )
		{
			// Only return appropriate sources
			if ( ( nProtocol == PROTOCOL_HTTP ) && ( pSource->m_nProtocol != PROTOCOL_HTTP ) ) continue;
			if ( ( nProtocol == PROTOCOL_G1 ) && ( pSource->m_nGnutella != 1 ) ) continue;
			//if ( bHTTP && pSource->m_nProtocol != PROTOCOL_HTTP ) continue;

			if ( pState != NULL ) pState->AddTail( pSource->m_sURL );

			if ( nProtocol == PROTOCOL_G1 )
			{
				if ( strSources.GetLength() ) 
					strSources += ',';
				strSources += CString( inet_ntoa( pSource->m_pAddress ) );
				if ( pSource->m_nPort != GNUTELLA_DEFAULT_PORT )
				{
					CString strURL;
					strURL.Format( _T("%hu"), pSource->m_nPort );
					strSources += ':' + strURL;
				}
			}
			else if ( pSource->m_sURL.Find( _T("Zhttp://") ) >= 0 ||
				pSource->m_sURL.Find( _T("Z%2C http://") ) >= 0 )
			{
				// Ignore buggy URLs
				TRACE( _T("CDownloadWithSources::GetSourceURLs() Bad URL: %s\n"), pSource->m_sURL );
			}
			else
			{
				CString strURL = pSource->m_sURL;
				strURL.Replace( _T(","), _T("%2C") );

				if ( strSources.GetLength() ) strSources += _T(", ");
				strSources += strURL;
				strSources += ' ';
				strSources += TimeToString( &pSource->m_tLastSeen );
			}
			
			if ( nMaximum == 1 ) break;
			else if ( nMaximum > 1 ) nMaximum --;
		}
	}
	
	return strSources;
}

// Returns a string containing the most recent failed sources
CString	CDownloadWithSources::GetTopFailedSources(int nMaximum, PROTOCOLID nProtocol)
{
	// Currently we return only the string for G1, in X-NAlt format
	if ( nProtocol != PROTOCOL_G1 ) return CString();

	CString strSources, str;
	CFailedSource* pResult = NULL;
	CQuickLock pLock( Transfers.m_pSection );

	for ( POSITION pos = m_pFailedSources.GetHeadPosition() ; pos ; )
	{
		pResult = m_pFailedSources.GetNext( pos );
		// Only return sources which we detected as failed
		if ( pResult && pResult->m_bLocal )
		{
			if ( _tcsistr( pResult->m_sURL, _T("http://") ) != NULL )
			{
				int nPos = pResult->m_sURL.Find( ':', 8 );
				if ( nPos < 0 ) continue;
				str = pResult->m_sURL.Mid( 7, nPos - 7 );
				int nPosSlash = pResult->m_sURL.Find( '/', nPos );
				if ( nPosSlash < 0 ) continue;

				if ( strSources.GetLength() ) 
					strSources += ',';

				strSources += str;
				str = pResult->m_sURL.Mid( nPos + 1, nPosSlash - nPos - 1 );
				if ( str != _T("6346") )
				{
					strSources += ':';
					strSources += str;
				}

				if ( nMaximum == 1 ) break;
				else if ( nMaximum > 1 ) nMaximum--;
			}
		}
	}
	return strSources;
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources query hit handler

BOOL CDownloadWithSources::OnQueryHits(CQueryHit* pHits)
{
	for ( ; pHits ; pHits = pHits->m_pNext )
	{
		if ( pHits->m_sURL.GetLength() ) AddSourceHit( pHits );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources remove overlapping sources

void CDownloadWithSources::RemoveOverlappingSources(QWORD nOffset, QWORD nLength)
{
	CQuickLock pLock( Transfers.m_pSection );

	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; )
	{
		CDownloadSource* pNext = pSource->m_pNext;
		
		if ( pSource->TouchedRange( nOffset, nLength ) )
		{
			theApp.Message( MSG_ERROR, IDS_DOWNLOAD_VERIFY_DROP,
				(LPCTSTR)CString( inet_ntoa( pSource->m_pAddress ) ),
				(LPCTSTR)pSource->m_sServer, (LPCTSTR)m_sName,
				nOffset, nOffset + nLength - 1 );
			pSource->Remove( TRUE, FALSE );
		}
		
		pSource = pNext;
	}
}

// The function takes an URL and finds a failed source in the list;
// If bReliable is true, it checks only localy checked failed sources
// and those which have more than 30 votes from other users and negative
// votes compose 2/3 of the total number of votes.
CFailedSource* CDownloadWithSources::LookupFailedSource(LPCTSTR pszUrl, bool bReliable)
{
	CQuickLock pLock( Transfers.m_pSection );
	CFailedSource* pResult = NULL;

	for ( POSITION pos = m_pFailedSources.GetHeadPosition() ; pos ; )
	{
		pResult = m_pFailedSources.GetNext( pos );
		if ( pResult && pResult->m_sURL.Compare( pszUrl ) == 0 )
		{
#ifndef NDEBUG
			theApp.Message( MSG_DEBUG, _T("Votes for file %s: negative - %i, positive - %i; offline status: %i"), 
				pszUrl, pResult->m_nNegativeVotes, 
				pResult->m_nPositiveVotes, 
				pResult->m_bOffline );
#endif
			if ( pResult->m_bLocal )
				break;
			
			if ( bReliable ) // Not used at the moment anywhere, we check that explicitly
			{
				INT_PTR nTotalVotes = pResult->m_nNegativeVotes + pResult->m_nPositiveVotes;
				if ( nTotalVotes > 30 && pResult->m_nNegativeVotes / nTotalVotes > 2 / 3 )
					break;
			}
			break; // temp solution to have the same source not added more than once
				   // we should check IPs which add these sources though, since voting takes place
		}
		else
			pResult = NULL;
	}
	return pResult;
}

void CDownloadWithSources::AddFailedSource(CDownloadSource* pSource, bool bLocal, bool bOffline)
{
	CString strURL;
	if ( pSource->m_nProtocol == PROTOCOL_BT && pSource->m_oGUID )
	{
		strURL.Format( _T("btc://%s/%s/"),
            (LPCTSTR)pSource->m_oGUID.toString(),
			(LPCTSTR)m_oBTH.toString() );
	}
	else
		strURL = pSource->m_sURL;

	AddFailedSource( (LPCTSTR)strURL, bLocal, bOffline );
}

void CDownloadWithSources::AddFailedSource(LPCTSTR pszUrl, bool bLocal, bool bOffline)
{
	CQuickLock pLock( Transfers.m_pSection );

	if ( LookupFailedSource( pszUrl ) == NULL )
	{
		CFailedSource* pBadSource = new CFailedSource( pszUrl, bLocal, bOffline );
		m_pFailedSources.AddTail( pBadSource );
		theApp.Message( MSG_DEBUG, L"Bad sources count for \"%s\": %i", m_sName, m_pFailedSources.GetCount() );
	}
}

void CDownloadWithSources::VoteSource(LPCTSTR pszUrl, bool bPositively)
{
	if ( CFailedSource* pBadSource = LookupFailedSource( pszUrl ) )
	{
		if ( bPositively )
			pBadSource->m_nPositiveVotes++;
		else
			pBadSource->m_nNegativeVotes++;
	}
}

void CDownloadWithSources::ExpireFailedSources()
{
	CQuickLock pLock( Transfers.m_pSection );

	DWORD tNow = GetTickCount();
	for ( POSITION pos = m_pFailedSources.GetHeadPosition() ; pos ; )
	{
		POSITION posThis = pos;
		CFailedSource* pBadSource = m_pFailedSources.GetNext( pos );
		if ( m_pFailedSources.GetAt( posThis ) == pBadSource )
		{
			// Expire bad sources added more than 2 hours ago
			if ( tNow - pBadSource->m_nTimeAdded > 2 * 3600 * 1000 )
			{
				delete pBadSource;
				m_pFailedSources.RemoveAt( posThis );
			}
			else break; // We appended to tail, so we do not need to move further
		}
	}
}

void CDownloadWithSources::ClearFailedSources()
{
	CQuickLock pLock( Transfers.m_pSection );

	for ( POSITION pos = m_pFailedSources.GetHeadPosition() ; pos ; )
	{
		POSITION posThis = pos;
		CFailedSource* pBadSource = m_pFailedSources.GetNext( pos );
		if ( m_pFailedSources.GetAt( posThis ) == pBadSource )
		{
			delete pBadSource;
			m_pFailedSources.RemoveAt( posThis );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources remove a source

void CDownloadWithSources::RemoveSource(CDownloadSource* pSource, BOOL bBan)
{
	CQuickLock pLock( Transfers.m_pSection );

	if ( bBan && pSource->m_sURL.GetLength() )
	{
		AddFailedSource( pSource );
	}
	
	ASSERT( m_nSourceCount > 0 );
	m_nSourceCount --;

	if ( pSource->m_nProtocol == PROTOCOL_G1 )
		m_nG1SourceCount--;
	else if ( pSource->m_nProtocol == PROTOCOL_G2 )
		m_nG2SourceCount--;
	else if ( pSource->m_nProtocol == PROTOCOL_ED2K )
		m_nEdSourceCount--;
	else if ( pSource->m_nProtocol == PROTOCOL_HTTP )
		m_nHTTPSourceCount--;
	else if ( pSource->m_nProtocol == PROTOCOL_BT )
		m_nBTSourceCount--;
	else if ( pSource->m_nProtocol == PROTOCOL_FTP )
		m_nFTPSourceCount--;

	if ( pSource->m_pPrev != NULL )
		pSource->m_pPrev->m_pNext = pSource->m_pNext;
	else
		m_pSourceFirst = pSource->m_pNext;
	
	if ( pSource->m_pNext != NULL )
		pSource->m_pNext->m_pPrev = pSource->m_pPrev;
	else
		m_pSourceLast = pSource->m_pPrev;
	
	delete pSource;
	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources sort a source

void CDownloadWithSources::SortSource(CDownloadSource* pSource, BOOL bTop)
{
	CQuickLock pLock( Transfers.m_pSection );

	ASSERT( m_nSourceCount > 0 );
	
	if ( pSource->m_pPrev != NULL )
		pSource->m_pPrev->m_pNext = pSource->m_pNext;
	else
		m_pSourceFirst = pSource->m_pNext;
	
	if ( pSource->m_pNext != NULL )
		pSource->m_pNext->m_pPrev = pSource->m_pPrev;
	else
		m_pSourceLast = pSource->m_pPrev;
	
	if ( ! bTop )
	{
		pSource->m_pPrev = m_pSourceLast;
		pSource->m_pNext = NULL;
		
		if ( m_pSourceLast != NULL )
		{
			m_pSourceLast->m_pNext = pSource;
			m_pSourceLast = pSource;
		}
		else
		{
			m_pSourceFirst = m_pSourceLast = pSource;
		}
	}
	else
	{
		pSource->m_pPrev = NULL;
		pSource->m_pNext = m_pSourceFirst;
		
		if ( m_pSourceFirst != NULL )
		{
			m_pSourceFirst->m_pPrev = pSource;
			m_pSourceFirst = pSource;
		}
		else
		{
			m_pSourceFirst = m_pSourceLast = pSource;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources sort a source by state (Downloading, etc...)

void CDownloadWithSources::SortSource(CDownloadSource* pSource)
{
	CQuickLock pLock( Transfers.m_pSection );

	ASSERT( m_nSourceCount > 0 );

	//Remove source from current position. (It's unsorted, and would interfere with sort)
	if ( pSource->m_pPrev != NULL )
		pSource->m_pPrev->m_pNext = pSource->m_pNext;
	else
		m_pSourceFirst = pSource->m_pNext;
	
	if ( pSource->m_pNext != NULL )
		pSource->m_pNext->m_pPrev = pSource->m_pPrev;
	else
		m_pSourceLast = pSource->m_pPrev;
	


	if ( ( m_pSourceFirst == NULL ) || ( m_pSourceLast == NULL ) )
	{	//Only one source
		m_pSourceFirst = m_pSourceLast = pSource;
		pSource->m_pNext = pSource->m_pPrev = NULL;
	}
	else
	{	//Sort sources
		CDownloadSource* pCompare = m_pSourceFirst;

		while ( ( pCompare != NULL ) && (pCompare->m_nSortOrder < pSource->m_nSortOrder) )
			pCompare = pCompare->m_pNext; //Run through the sources to the correct position

		if ( pCompare == NULL )
		{	//Source is last on list
			m_pSourceLast->m_pNext = pSource;
			pSource->m_pPrev = m_pSourceLast;
			pSource->m_pNext = NULL;
			m_pSourceLast = pSource;
		}
		else
		{	//Insert source in front of current compare source
			if ( pCompare->m_pPrev == NULL )
				m_pSourceFirst = pSource;
			else
				pCompare->m_pPrev->m_pNext = pSource;

			pSource->m_pNext = pCompare;
			pSource->m_pPrev = pCompare->m_pPrev;
			pCompare->m_pPrev= pSource;
		}

	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources source color selector

#define SRC_COLORS 6u

int CDownloadWithSources::GetSourceColor()
{
	CQuickLock pLock( Transfers.m_pSection );

	BOOL bTaken[SRC_COLORS] = {};
	unsigned int nFree = SRC_COLORS;
	
	for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; pSource = pSource->m_pNext )
	{
		if ( pSource->m_nColor >= 0 )
		{
			if ( bTaken[ pSource->m_nColor ] == FALSE )
			{
				bTaken[ pSource->m_nColor ] = TRUE;
				nFree--;
			}
		}
	}
	
	if ( nFree == 0 ) return GetRandomNum( 0u, SRC_COLORS - 1 );
	
	nFree = GetRandomNum( 0u, nFree - 1 );
	
	for ( int nColor = 0 ; nColor < SRC_COLORS ; nColor++ )
	{
		if ( bTaken[ nColor ] == FALSE )
		{
			if ( nFree-- == 0 ) return nColor;
		}
	}
	
	return GetRandomNum( 0u, SRC_COLORS - 1 );
}

//////////////////////////////////////////////////////////////////////
// CDownloadWithSources serialize

void CDownloadWithSources::Serialize(CArchive& ar, int nVersion)
{
	CDownloadBase::Serialize( ar, nVersion );
	
	CQuickLock pLock( Transfers.m_pSection );
	
	if ( ar.IsStoring() )
	{
		ar.WriteCount( (DWORD)m_nSourceCount );
		
		for ( CDownloadSource* pSource = m_pSourceFirst ; pSource ; pSource = pSource->m_pNext )
		{
			pSource->Serialize( ar, nVersion );
		}
		
		ar.WriteCount( m_pXML != NULL ? 1 : 0 );
		if ( m_pXML ) m_pXML->Serialize( ar );
	}
	else
	{
		for ( DWORD_PTR nSources = ar.ReadCount() ; nSources ; nSources-- )
		{
			// Create new source
			CDownloadSource* pSource = new CDownloadSource( (CDownload*)this );
			
			// Add to the list
			m_nSourceCount++;

			pSource->m_pPrev = m_pSourceLast;
			pSource->m_pNext = NULL;
			
			if ( m_pSourceLast != NULL )
			{
				m_pSourceLast->m_pNext = pSource;
				m_pSourceLast = pSource;
			}
			else
			{
				m_pSourceFirst = m_pSourceLast = pSource;
			}

			// Load details from disk
			pSource->Serialize( ar, nVersion );

			if ( pSource->m_nProtocol == PROTOCOL_G1 )
				m_nG1SourceCount++;
			else if ( pSource->m_nProtocol == PROTOCOL_G2 )
				m_nG2SourceCount++;
			else if ( pSource->m_nProtocol == PROTOCOL_ED2K )
				m_nEdSourceCount++;
			else if ( pSource->m_nProtocol == PROTOCOL_HTTP )
				m_nHTTPSourceCount++;
			else if ( pSource->m_nProtocol == PROTOCOL_BT )
				m_nBTSourceCount++;
			else if ( pSource->m_nProtocol == PROTOCOL_FTP )
				m_nFTPSourceCount++;

			// Extract ed2k client ID from url (m_pAddress) because it wasn't saved
			if ( ( !pSource->m_nPort ) && ( _tcsnicmp( pSource->m_sURL, _T("ed2kftp://"), 10 ) == 0 )  )
			{
				CString strURL = pSource->m_sURL.Mid(10);
				if ( strURL.GetLength())
					_stscanf( strURL, _T("%lu"), &pSource->m_pAddress.S_un.S_addr );
			}
		}
		
		if ( ar.ReadCount() )
		{
			m_pXML = new CXMLElement();
			m_pXML->Serialize( ar );
		}
	}
}

void CDownloadWithSources::MergeMetadata(const CXMLElement* pXML)
{
	CQuickLock pLock( Transfers.m_pSection );

	if ( m_pXML )
	{
		CXMLAttribute* pAttr1 = m_pXML->GetAttribute( CXMLAttribute::schemaName );
		CXMLAttribute* pAttr2 = pXML->GetAttribute( CXMLAttribute::schemaName );
		if ( pAttr1 && pAttr2 && ! pAttr1->GetValue().CompareNoCase( pAttr1->GetValue() ) )
		{
			CXMLElement* pElement1 = m_pXML->GetFirstElement();
			CXMLElement* pElement2 = pXML->GetFirstElement();
			if ( pElement1 && pElement2 )
			{
				pElement1->Merge( pElement2 );
			}
		}
	}
	else
	{
		m_pXML = pXML->Clone();
	}
}
