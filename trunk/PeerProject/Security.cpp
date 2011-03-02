//
// Security.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "Security.h"
#include "PeerProjectFile.h"
#include "QuerySearch.h"
#include "RegExp.h"
#include "Buffer.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CSecurity Security;
CAdultFilter AdultFilter;
CMessageFilter MessageFilter;


//////////////////////////////////////////////////////////////////////
// CSecurity construction

CSecurity::CSecurity()
	: m_bDenyPolicy ( FALSE )
{
}

CSecurity::~CSecurity()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CSecurity rule access

POSITION CSecurity::GetIterator() const
{
	return m_pRules.GetHeadPosition();
}

CSecureRule* CSecurity::GetNext(POSITION& pos) const
{
	return m_pRules.GetNext( pos );
}

INT_PTR CSecurity::GetCount() const
{
	return m_pRules.GetCount();
}

BOOL CSecurity::Check(CSecureRule* pRule) const
{
	CQuickLock oLock( m_pSection );

	return pRule != NULL && GetGUID( pRule->m_pGUID ) != NULL;
}

CSecureRule* CSecurity::GetGUID(const GUID& pGUID) const
{
	CQuickLock oLock( m_pSection );

	for ( POSITION pos = m_pRules.GetHeadPosition() ; pos ; )
	{
		CSecureRule* pRule = m_pRules.GetNext( pos );
		if ( pRule->m_pGUID == pGUID ) return pRule;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CSecurity rule modification

void CSecurity::Add(CSecureRule* pRule)
{
	{
		CQuickLock oLock( m_pSection );

		if ( pRule->m_nType == CSecureRule::srAddress )
		{
			pRule->MaskFix();
			if ( pRule->m_nMask[3] == 255 && pRule->m_nMask[2] == 255 && pRule->m_nMask[1] == 255 )
				m_Cache.erase( *(DWORD*)pRule->m_nIP );
			else
				m_Cache.clear();
		}

		CSecureRule* pExistingRule = GetGUID( pRule->m_pGUID );
		if ( pExistingRule == NULL )
		{
			m_pRules.AddHead( pRule );
		}
		else if ( pExistingRule != pRule )
		{
			*pExistingRule = *pRule;
			delete pRule;
		}
	}

	// Check all lists for newly denied hosts
	PostMainWndMessage( WM_SANITY_CHECK );
}

void CSecurity::Remove(CSecureRule* pRule)
{
	CQuickLock oLock( m_pSection );

	POSITION pos = m_pRules.Find( pRule );
	if ( pos ) m_pRules.RemoveAt( pos );
	delete pRule;
}

void CSecurity::MoveUp(CSecureRule* pRule)
{
	CQuickLock oLock( m_pSection );

	POSITION posMe = m_pRules.Find( pRule );
	if ( posMe == NULL ) return;

	POSITION posOther = posMe;
	m_pRules.GetPrev( posOther );

	if ( posOther )
	{
		m_pRules.InsertBefore( posOther, pRule );
		m_pRules.RemoveAt( posMe );
	}
}

void CSecurity::MoveDown(CSecureRule* pRule)
{
	CQuickLock oLock( m_pSection );

	POSITION posMe = m_pRules.Find( pRule );
	if ( posMe == NULL ) return;

	POSITION posOther = posMe;
	m_pRules.GetNext( posOther );

	if ( posOther )
	{
		m_pRules.InsertAfter( posOther, pRule );
		m_pRules.RemoveAt( posMe );
	}
}

void CSecurity::Clear()
{
	CQuickLock oLock( m_pSection );

	for ( POSITION pos = m_Complains.GetStartPosition() ; pos ; )
	{
		DWORD pAddress;
		CComplain* pComplain;
		m_Complains.GetNextAssoc( pos, pAddress, pComplain );
		delete pComplain;
	}
	m_Complains.RemoveAll();

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		delete GetNext( pos );
	}

	m_pRules.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CSecurity ban

void CSecurity::BanHelper(const IN_ADDR* pAddress, const CPeerProjectFile* pFile, int nBanLength, BOOL bMessage, LPCTSTR szComment)
{
	CQuickLock oLock( m_pSection );

	const DWORD tNow = static_cast< DWORD >( time( NULL ) );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CSecureRule* pRule = GetNext( pos );

		if ( ( pAddress && pRule->Match( pAddress ) ) ||
			( pFile && pRule->Match( pFile ) ) )				// Non-regexp name, hash, or size:ext:0000
		{
			if ( pRule->m_nAction == CSecureRule::srDeny )
			{
				if ( nBanLength == banWeek && ( pRule->m_nExpire < tNow + 604000 ) )
					pRule->m_nExpire = static_cast< DWORD >( time( NULL ) + 604800 );
				else if ( nBanLength == banCustom && ( pRule->m_nExpire < tNow + Settings.Security.DefaultBan + 3600 ) )
					pRule->m_nExpire = static_cast< DWORD >( time( NULL ) + Settings.Security.DefaultBan + 3600 );
				else if ( nBanLength == banForever && ( pRule->m_nExpire != CSecureRule::srIndefinite ) )
					pRule->m_nExpire = CSecureRule::srIndefinite;
				else if ( bMessage && pAddress )
					theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_ALREADY_BLOCKED,
						(LPCTSTR)CString( inet_ntoa( *pAddress ) ) );
				return;
			}
		}
	}

	CSecureRule* pRule	= new CSecureRule();
	pRule->m_nAction	= CSecureRule::srDeny;

	switch ( nBanLength )
	{
	case banSession:
		pRule->m_nExpire	= CSecureRule::srSession;
		pRule->m_sComment	= _T("Quick Ban");
		break;
	case ban5Mins:
		pRule->m_nExpire	= static_cast< DWORD >( time( NULL ) + 300 );
		pRule->m_sComment	= _T("Temp Ignore");
		break;
	case ban30Mins:
		pRule->m_nExpire	= static_cast< DWORD >( time( NULL ) + 1800 );
		pRule->m_sComment	= _T("Temp Ignore");
		break;
	case ban2Hours:
		pRule->m_nExpire	= static_cast< DWORD >( time( NULL ) + 7200 );
		pRule->m_sComment	= _T("Temp Ignore");
		break;
	case banWeek:
		pRule->m_nExpire	= static_cast< DWORD >( time( NULL ) + 604800 );
		pRule->m_sComment	= _T("Client Block");
		break;
	case banCustom:
		pRule->m_nExpire	= static_cast< DWORD >( time( NULL ) + Settings.Security.DefaultBan + 3600 );
		pRule->m_sComment	= _T("Ban");
		break;
	case banForever:
		pRule->m_nExpire	= CSecureRule::srIndefinite;
		pRule->m_sComment	= _T("Ban");
		break;
	default:
		pRule->m_nExpire	= CSecureRule::srSession;
		pRule->m_sComment	= _T("Session Ban");
	}

	if ( szComment )
		pRule->m_sComment = szComment;

	if ( pAddress )
		CopyMemory( pRule->m_nIP, pAddress, sizeof pRule->m_nIP );
	else if ( pFile && ( pFile->m_oSHA1 || pFile->m_oTiger || pFile->m_oBTH || pFile->m_oED2K || pFile->m_oMD5 ) )
	{
		pRule->m_nType = CSecureRule::srContentAny;
		pRule->SetContentWords(
			( pFile->m_oSHA1  ? pFile->m_oSHA1.toUrn()  + _T(" ") : CString() ) +
			( pFile->m_oED2K  ? pFile->m_oED2K.toUrn()  + _T(" ") : CString() ) +
			( pFile->m_oTiger ? pFile->m_oTiger.toUrn() + _T(" ") : CString() ) +
			( pFile->m_oMD5   ? pFile->m_oMD5.toUrn()   + _T(" ") : CString() ) +
			( pFile->m_oBTH   ? pFile->m_oBTH.toUrn()             : CString() ) );
	}

	Add( pRule );

	if ( bMessage && pAddress )
	{
		theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_BLOCKED,
			(LPCTSTR)CString( inet_ntoa( *pAddress ) ) );
	}
}


//////////////////////////////////////////////////////////////////////
// CSecurity complain

bool CSecurity::Complain(const IN_ADDR* pAddress, int nBanLength, int nExpire, int nCount)
{
	CQuickLock oLock( m_pSection );

	const DWORD nNow = static_cast< DWORD >( time( NULL ) );

	CComplain* pComplain = NULL;
	if ( m_Complains.Lookup( pAddress->s_addr, pComplain ) )
	{
		if ( pComplain->m_nExpire < nNow )
		{
			pComplain->m_nScore = 1;
		}
		else
		{
			pComplain->m_nScore ++;
			if ( pComplain->m_nScore > nCount )
			{
				m_Complains.RemoveKey( pAddress->s_addr );
				delete pComplain;
				Ban( pAddress, nBanLength );
				return true;
			}
		}
	}
	else
	{
		pComplain = new CComplain;
		pComplain->m_nScore = 1;
		m_Complains.SetAt( pAddress->s_addr, pComplain );
	}

	pComplain->m_nExpire = nNow + nExpire;

	return false;
}

//////////////////////////////////////////////////////////////////////
// CSecurity access checks

BOOL CSecurity::IsDenied(const IN_ADDR* pAddress)
{
	if ( m_Cache.count( *(DWORD*)pAddress ) )
		return m_bDenyPolicy;
		// theApp.Message( MSG_DEBUG, _T("Skipped Repeat IP Security Check  (%i Cached)"), m_Cache.size() );

	CQuickLock oLock( m_pSection );

	const DWORD nNow = static_cast< DWORD >( time( NULL ) );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posLast = pos;
		CSecureRule* pRule = GetNext( pos );

		if ( pRule->IsExpired( nNow ) )
		{
			m_pRules.RemoveAt( posLast );
			delete pRule;
		}
		else if ( pRule->Match( pAddress ) )
		{
			pRule->m_nToday ++;
			pRule->m_nEver ++;

			// Add 5 min penalty for early access
			if ( pRule->m_nExpire > CSecureRule::srSession &&
				pRule->m_nExpire < nNow + 300 )
				pRule->m_nExpire = nNow + 300;

			if ( pRule->m_nAction == CSecureRule::srDeny )		return TRUE;
			if ( pRule->m_nAction == CSecureRule::srAccept )	return FALSE;
		}
	}

	m_Cache.insert( *(DWORD*)pAddress );	// Skip future lookups

	return m_bDenyPolicy;
}

BOOL CSecurity::IsDenied(LPCTSTR pszContent)
{
	CQuickLock oLock( m_pSection );

	const DWORD nNow = static_cast< DWORD >( time( NULL ) );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posLast = pos;
		CSecureRule* pRule = GetNext( pos );

		if ( pRule->IsExpired( nNow ) )
		{
			m_pRules.RemoveAt( posLast );
			delete pRule;
		}
		else if ( pRule->Match( pszContent ) )
		{
			pRule->m_nToday ++;
			pRule->m_nEver ++;

			// Add 5 min penalty for early access
			if ( pRule->m_nExpire > CSecureRule::srSession &&
				pRule->m_nExpire < nNow + 300 )
				pRule->m_nExpire = nNow + 300;

			if ( pRule->m_nAction == CSecureRule::srDeny )		return TRUE;
			if ( pRule->m_nAction == CSecureRule::srAccept )	return FALSE;
		}
	}

	return m_bDenyPolicy;
}

BOOL CSecurity::IsDenied(const CPeerProjectFile* pFile) 	// ToDo: Never called?
{
	CQuickLock oLock( m_pSection );

	DWORD nNow = static_cast< DWORD >( time( NULL ) );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posLast = pos;
		CSecureRule* pRule = GetNext( pos );

		if ( pRule->IsExpired( nNow ) )
		{
			m_pRules.RemoveAt( posLast );
			delete pRule;
		}
		else if ( pRule->Match( pFile ) )	// Non-regexp name, hash, or size:ext:0000
		{
			pRule->m_nToday ++;
			pRule->m_nEver ++;

			// Add 5 min penalty for early access
			if ( pRule->m_nExpire > CSecureRule::srSession &&
				pRule->m_nExpire < nNow + 300 )
				pRule->m_nExpire = nNow + 300;

			if ( pRule->m_nAction == CSecureRule::srDeny )		return TRUE;
			if ( pRule->m_nAction == CSecureRule::srAccept )	return FALSE;
		}
	}

	return m_bDenyPolicy;
}

BOOL CSecurity::IsDenied(const CQuerySearch* pQuery, const CString& strContent)
{
	CQuickLock oLock( m_pSection );

	const DWORD nNow = static_cast< DWORD >( time( NULL ) );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posLast = pos;
		CSecureRule* pRule = GetNext( pos );

		if ( pRule->IsExpired( nNow ) )
		{
			m_pRules.RemoveAt( posLast );
			delete pRule;
		}
		else if ( pRule->Match( pQuery, strContent ) )
		{
			pRule->m_nToday ++;
			pRule->m_nEver ++;

			if ( pRule->m_nAction == CSecureRule::srDeny )		return TRUE;
			if ( pRule->m_nAction == CSecureRule::srAccept )	return FALSE;
		}
	}

	return m_bDenyPolicy;
}

//////////////////////////////////////////////////////////////////////
// CSecurity expire

void CSecurity::Expire()
{
	CQuickLock oLock( m_pSection );

	DWORD nNow = static_cast< DWORD >( time( NULL ) );

	for ( POSITION pos = m_Complains.GetStartPosition() ; pos ; )
	{
		DWORD pAddress;
		CComplain* pComplain;
		m_Complains.GetNextAssoc( pos, pAddress, pComplain );
		if ( pComplain->m_nExpire < nNow )
		{
			m_Complains.RemoveKey( pAddress );
			delete pComplain;
		}
	}

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posLast = pos;
		CSecureRule* pRule = GetNext( pos );

		if ( pRule->IsExpired( nNow ) )
		{
			m_pRules.RemoveAt( posLast );
			delete pRule;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CSecurity load and save

BOOL CSecurity::Load()
{
	CQuickLock oLock( m_pSection );

	CFile pFile;
	const CString strFile = Settings.General.UserPath + _T("\\Data\\Security.dat");

	if ( ! pFile.Open( strFile, CFile::modeRead ) ) return FALSE;

	try
	{
		CArchive ar( &pFile, CArchive::load, 131072 );	// 128 KB buffer
		Serialize( ar );
		ar.Close();
	}
	catch ( CException* pException )
	{
		pException->Delete();
	}

	pFile.Close();

	return TRUE;
}

BOOL CSecurity::Save()
{
	CFile pFile;
	const CString strFile = Settings.General.UserPath + _T("\\Data\\Security.dat");

	CQuickLock oLock( m_pSection );

	if ( ! pFile.Open( strFile, CFile::modeWrite|CFile::modeCreate ) )
		return FALSE;

	try
	{
		CArchive ar( &pFile, CArchive::store, 131072 );	// 128 KB buffer
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

//////////////////////////////////////////////////////////////////////
// CSecurity serialize

void CSecurity::Serialize(CArchive& ar)
{
	int nVersion = SECURITY_SER_VERSION;

	if ( ar.IsStoring() )
	{
		ar << nVersion;
		ar << m_bDenyPolicy;

		ar.WriteCount( GetCount() );

		for ( POSITION pos = GetIterator() ; pos ; )
		{
			CSecureRule* pRule = GetNext( pos );
			pRule->Serialize( ar, nVersion );
		}
	}
	else // Loading
	{
		Clear();

		ar >> nVersion;
		ar >> m_bDenyPolicy;

		DWORD nNow = static_cast< DWORD >( time( NULL ) );

		for ( DWORD_PTR nCount = ar.ReadCount() ; nCount > 0 ; nCount-- )
		{
			CSecureRule* pRule = new CSecureRule( FALSE );
			pRule->Serialize( ar, nVersion );

			if ( pRule->IsExpired( nNow, TRUE ) )
				delete pRule;
			else
				m_pRules.AddTail( pRule );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CSecurity XML

LPCTSTR CSecurity::xmlns = _T("http://schemas.peerproject.org/Security.xsd");

CXMLElement* CSecurity::ToXML(BOOL bRules)
{
	CXMLElement* pXML = new CXMLElement( NULL, _T("security") );
	pXML->AddAttribute( _T("xmlns"), CSecurity::xmlns );

	if ( bRules )
	{
		for ( POSITION pos = GetIterator() ; pos ; )
		{
			pXML->AddElement( GetNext( pos )->ToXML() );
		}
	}

	return pXML;
}

BOOL CSecurity::FromXML(CXMLElement* pXML)
{
	if ( ! pXML->IsNamed( _T("security") ) ) return FALSE;

	int nCount = 0;

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = pXML->GetNextElement( pos );

		if ( pElement->IsNamed( _T("rule") ) )
		{
			CQuickLock oLock( m_pSection );
			CSecureRule* pRule	= NULL;
			CString strGUID		= pElement->GetAttributeValue( _T("guid") );
			BOOL bExisting		= FALSE;
			GUID pGUID;

			if ( Hashes::fromGuid( strGUID, &pGUID ) )
			{
				if ( ( pRule = GetGUID( pGUID ) ) != NULL )
					bExisting = TRUE;

				if ( pRule == NULL )
				{
					pRule = new CSecureRule( FALSE );
					pRule->m_pGUID = pGUID;
				}
			}
			else
			{
				pRule = new CSecureRule();
			}

			if ( pRule->FromXML( pElement ) )
			{
				if ( ! bExisting )
					m_pRules.AddTail( pRule );

				nCount++;
			}
			else
			{
				if ( ! bExisting ) delete pRule;
			}
		}
	}

	return nCount > 0;
}

//////////////////////////////////////////////////////////////////////
// CSecurity import

BOOL CSecurity::Import(LPCTSTR pszFile)
{
	CString strText;
	CBuffer pBuffer;
	CFile pFile;

	if ( ! pFile.Open( pszFile, CFile::modeRead ) ) return FALSE;
	pBuffer.EnsureBuffer( (DWORD)pFile.GetLength() );
	pBuffer.m_nLength = (DWORD)pFile.GetLength();
	pFile.Read( pBuffer.m_pBuffer, pBuffer.m_nLength );
	pFile.Close();

	CXMLElement* pXML = CXMLElement::FromBytes( pBuffer.m_pBuffer, pBuffer.m_nLength, TRUE );
	BOOL bResult = FALSE;

	if ( pXML != NULL )
	{
		bResult = FromXML( pXML );
		delete pXML;
	}
	else
	{
		CString strLine;

		while ( pBuffer.ReadLine( strLine ) )
		{
			strLine.Trim();
			if ( strLine.IsEmpty() ) continue;
			if ( strLine.GetAt( 0 ) == ';' ) continue;

			CSecureRule* pRule = new CSecureRule();

			if ( pRule->FromGnucleusString( strLine ) )
			{
				CQuickLock oLock( m_pSection );
				m_pRules.AddTail( pRule );
				bResult = TRUE;
			}
			else
			{
				delete pRule;
			}
		}
	}

	// Check all lists for newly denied hosts
	PostMainWndMessage( WM_SANITY_CHECK );

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CSecurity.IsClientBad/IsClientBanned
//
// Check user agent for troublemakers

BOOL CSecurity::IsClientBad(const CString& sUserAgent) const
{
	// No user agent- Assume bad. (Allowed connection but no searches performed)
	if ( sUserAgent.IsEmpty() ) 								return TRUE;

	// PeerProject Fakes
	if ( LPCTSTR szVersion = _tcsistr( sUserAgent, _T("PeerProject") ) )
	{
		szVersion += 11;
		if ( _tcsistr( sUserAgent, _T(" 1.") ) )				return FALSE;
	//	if ( _tcsistr( sUserAgent, _T(" 2.") ) )				return FALSE;

		return TRUE;
	}

	// Shareaza Fakes/Obsolete
	if ( LPCTSTR szVersion = _tcsistr( sUserAgent, _T("shareaza") ) )
	{
		szVersion += 8;
		if ( _tcsistr( sUserAgent, _T(" 2.0") ) )				return TRUE;
		if ( _tcsistr( sUserAgent, _T(" 2.") ) )				return FALSE;
		if ( _tcsistr( sUserAgent, _T("Plus") ) )				return FALSE;

		return TRUE;
	}

	// Dianlei: Shareaza rip-off
	// Based on Alpha code, need verification for current 1.x status
	if ( LPCTSTR szVersion = _tcsistr( sUserAgent, _T("Dianlei") ) )
	{
		szVersion += 7;
		if ( _tcsistr( szVersion, _T(" 0.") ) )					return TRUE;

		return FALSE;
	}

	// BearShare Selectivity
	if ( LPCTSTR szVersion = _tcsistr( sUserAgent, _T("BearShare") ) )
	{
		szVersion += 9;
		if ( _tcsistr( szVersion, _T(" 4.") ) )					return FALSE;
		if ( _tcsistr( szVersion, _T(" 5.") ) )					return FALSE;

		return TRUE;
	}

	// Any iMesh
	if ( _tcsistr( sUserAgent, _T("iMesh") ) )					return TRUE;

	// Other Miscillaneous

	if ( _tcsistr( sUserAgent, _T("Trilix") ) )					return TRUE;

	if ( _tcsistr( sUserAgent, _T("Gnutella Turbo") ) )			return TRUE;

	if ( _tcsistr( sUserAgent, _T("Mastermax File Sharing") ) )	return TRUE;

	if ( _tcsistr( sUserAgent, _T("Fildelarprogram") ) )		return TRUE;

	if ( _tcsistr( sUserAgent, _T("Fastload.TV") ) )			return TRUE;

	// Other GPL Violaters, Etc.

	if ( _tcsistr( sUserAgent, _T("K-Lite") ) )					return TRUE;

	if ( _tcsistr( sUserAgent, _T("SlingerX") ) )				return TRUE;

	if ( _tcsistr( sUserAgent, _T("C -3.0.1") ) )				return TRUE;

	if ( _tcsistr( sUserAgent, _T("vagaa") ) )					return TRUE;

	if ( _tcsistr( sUserAgent, _T("mxie") ) )					return TRUE;

	if ( _tcsistr( sUserAgent, _T("WinMX") ) )					return TRUE;

	if ( _tcsistr( sUserAgent, _T("eTomi") ) )					return TRUE;

	// Unknown- Assume OK
	return FALSE;
}

// Checks the user agent to see if it's a leecher or banned client
BOOL CSecurity::IsClientBanned(const CString& sUserAgent)
{
	// No user agent- assume OK
	if ( sUserAgent.IsEmpty() )
		return FALSE;

	// Foxy (Private G2)
	if ( _tcsistr( sUserAgent, _T("Foxy") ) )					return TRUE;

	// i2hub leecher client. (Tested, does not upload)
	if ( _tcsistr( sUserAgent, _T("i2hub") ) )					return TRUE;

	// Check by content filter
	// ToDo: Implement user agent filter type
	return IsDenied( sUserAgent );
}

BOOL CSecurity::IsAgentBlocked(const CString& sUserAgent) const
{
	// The remote computer didn't send a "User-Agent", or it sent whitespace
	if ( sUserAgent.IsEmpty() ||
		CString( sUserAgent ).Trim().IsEmpty() )				return TRUE; // ?

	// Loop through the user-defined list of programs to block
	for ( string_set::const_iterator i = Settings.Uploads.BlockAgents.begin() ;
		i != Settings.Uploads.BlockAgents.end() ; i++ )
	{
		if ( _tcsistr( sUserAgent, *i ) )						return TRUE;
	}

	// Allow it
	return FALSE;
}

BOOL CSecurity::IsVendorBlocked(const CString& sVendor) const
{
	// Foxy (Private G2)
	if ( _tcsistr( sVendor, _T("foxy") ) )						return TRUE;

	// Allow it
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// CSecureRule construction

CSecureRule::CSecureRule(BOOL bCreate)
{
	m_nType		= srAddress;
	m_nAction	= BYTE( Security.m_bDenyPolicy ? srAccept : srDeny );
	m_nExpire	= srIndefinite;
	m_nToday	= 0;
	m_nEver		= 0;

	m_nIP[0]	= m_nIP[1] = m_nIP[2] = m_nIP[3] = 0;
	m_nMask[0]	= m_nMask[1] = m_nMask[2] = m_nMask[3] = 255;
	m_pContent	= NULL;
	m_nContentLength = 0;

	if ( bCreate ) CoCreateGuid( &m_pGUID );
}

CSecureRule::CSecureRule(const CSecureRule& pRule)
{
	m_pContent = NULL;
	*this = pRule;
}

CSecureRule& CSecureRule::operator=(const CSecureRule& pRule)
{
	m_nType		= pRule.m_nType;
	m_nAction	= pRule.m_nAction;
	m_sComment	= pRule.m_sComment;
	m_pGUID		= pRule.m_pGUID;
	m_nExpire	= pRule.m_nExpire;
	m_nToday	= pRule.m_nToday;
	m_nEver		= pRule.m_nEver;
	m_nIP[0]	= pRule.m_nIP[0];
	m_nIP[1]	= pRule.m_nIP[1];
	m_nIP[2]	= pRule.m_nIP[2];
	m_nIP[3]	= pRule.m_nIP[3];
	m_nMask[0]	= pRule.m_nMask[0];
	m_nMask[1]	= pRule.m_nMask[1];
	m_nMask[2]	= pRule.m_nMask[2];
	m_nMask[3]	= pRule.m_nMask[3];

	delete [] m_pContent;
	m_pContent	= pRule.m_nContentLength ? new TCHAR[ pRule.m_nContentLength ] : NULL;
	m_nContentLength = pRule.m_nContentLength;
	CopyMemory( m_pContent, pRule.m_pContent, m_nContentLength * sizeof( TCHAR ) );

	return *this;
}

CSecureRule::~CSecureRule()
{
	if ( m_pContent ) delete [] m_pContent;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule remove and reset

void CSecureRule::Remove()
{
	Security.Remove( this );
}

void CSecureRule::Reset()
{
	m_nToday = m_nEver = 0;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule expiry check

BOOL CSecureRule::IsExpired(DWORD nNow, BOOL bSession) const
{
	if ( m_nExpire == srIndefinite ) return FALSE;
	if ( m_nExpire == srSession ) return bSession;
	return m_nExpire < nNow;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule match

BOOL CSecureRule::Match(const IN_ADDR* pAddress) const
{
	if ( m_nType == srAddress && pAddress )
	{
		DWORD* pBase = (DWORD*)m_nIP;
		DWORD* pMask = (DWORD*)m_nMask;
		DWORD* pTest = (DWORD*)pAddress;

		// This only works if IP's are &ed before entered in the list
		if ( ( ( *pTest ) & ( *pMask ) ) == ( *pBase ) )
			return ! IsExpired( (DWORD)time( NULL ) );
	}
	return FALSE;
}

BOOL CSecureRule::Match(LPCTSTR pszContent) const
{
	if ( m_nType != srContentRegExp && pszContent && m_pContent )
	{
		if ( IsExpired( (DWORD)time( NULL ) ) )
			return FALSE;

		for ( LPCTSTR pszFilter = m_pContent ; *pszFilter ; )
		{
			BOOL bFound = _tcsistr( pszContent, pszFilter ) != NULL;

			if ( ! bFound && ( m_nType == srContentAll || m_nType == srSizeType ) )
				return FALSE;
			if ( bFound && ( m_nType == srContentAny || m_nType == srSizeType ) )
				return TRUE;

			pszFilter += _tcslen( pszFilter ) + 1;
		}

		if ( m_nType == srContentAll )
			return TRUE;
	}

	return FALSE;
}

BOOL CSecureRule::Match(const CPeerProjectFile* pFile) const
{
	if ( m_nType != srContentRegExp && m_nType != srAddress && pFile && m_pContent )
	{
		if ( m_nType == srSizeType )
		{
			if ( pFile->m_nSize == 0 || pFile->m_nSize == SIZE_UNKNOWN )
				return FALSE;

			LPCTSTR pszExt = PathFindExtension( (LPCTSTR)pFile->m_sName );
			if ( *pszExt != '.' )
				return FALSE;

			pszExt++;
			CString strCompare;
			strCompare.Format( _T("size:%s:%u"), pszExt, pFile->m_nSize );	// %I64i
			return Match( strCompare );
		}

		if ( Match( pFile->m_sName ) )
			return TRUE;

		if ( m_nContentLength > 30 )
			return
				( pFile->m_oSHA1  && Match( pFile->m_oSHA1.toUrn() ) ) ||
				( pFile->m_oED2K  && Match( pFile->m_oED2K.toUrn() ) ) ||
				( pFile->m_oTiger && Match( pFile->m_oTiger.toUrn() ) ) ||
				( pFile->m_oBTH   && Match( pFile->m_oBTH.toUrn() ) ) ||
				( pFile->m_oMD5   && Match( pFile->m_oMD5.toUrn() ) );
	}

	return FALSE;
}

BOOL CSecureRule::Match(const CQuerySearch* pQuery, const CString& strContent) const
{
	if ( m_nType != srContentRegExp || ! m_pContent )
		return FALSE;

	// Build a regular expression filter from the search query words
	// Returns an empty string if not applied or if the filter was invalid

	// Substitutes (Special-case tags):
	// <%>, <$>, <_>	Insert all query keywords.
	// <1>...<9>		Insert specific query keyword, 1-9.
	// <>				Insert next query keyword. (?)

	// Example Search Conversion:
	// Filter:			.*(<2><1>)|(<%>).*
	// "Word1 Word2":	.*(word2\s*word1\s*)|(word1\s*word2\s*).*

	CString strFilter;
	int nTotal = 0;
	for ( LPCTSTR pszPattern = m_pContent ; *pszPattern ; pszPattern++ )
	{
		if ( *pszPattern == '<' )
		{
			pszPattern++;
			bool bEnds = false;
			bool bNumber = false;
			bool bAll = ( *pszPattern == '%' || *pszPattern == '$' || *pszPattern == '_' || *pszPattern == '>' );
			if ( ! bAll ) bNumber = ( *pszPattern == '1' || *pszPattern == '2' || *pszPattern == '3' || *pszPattern == '4' ||
				*pszPattern == '5' || *pszPattern == '6' || *pszPattern == '7' || *pszPattern == '8' || *pszPattern == '9' );
			for ( ; *pszPattern ; pszPattern++ )
			{
				if ( *pszPattern == '>' )
				{
					bEnds = true;
					break;
				}
			}
			if ( bEnds ) // Closed '>'
			{
				if ( bAll ) // <%>,<$>,<_>,<>
				{
					// Add all keywords at the "< >" position
					for ( CQuerySearch::const_iterator i = pQuery->begin() ; i != pQuery->end() ; ++i )
					{
						strFilter.AppendFormat( L"%s\\s*",
							CString( i->first, (int)( i->second ) ) );
					}
				}
				else if ( bNumber ) // <1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>
				{
					pszPattern--; // Go back
					int nNumber = 0, nWord = 1;

					// Numbers from 1 to 9, no more
					if ( _stscanf( &pszPattern[0], L"%i", &nNumber ) != 1 )
						nNumber = ++nTotal;

					// Add specified keyword at the "< >" position
					for ( CQuerySearch::const_iterator i = pQuery->begin() ; i != pQuery->end() ; ++i, ++nWord )
					{
						if ( nWord == nNumber )
						{
							strFilter.AppendFormat( L"%s\\s*",
								CString( i->first, (int)( i->second ) ) );
							break;
						}
					}
					pszPattern++;	// Return to the last position
				}
			}
			else // No closing '>'
			{
				strFilter.Empty();
				break;
			}
		}
		else // No replacing
		{
			strFilter += *pszPattern;
		}
	}

	if ( strFilter.IsEmpty() )
	{
		theApp.Message( MSG_DEBUG, L"Invalid RegExp filter:  %s", m_pContent );
		return FALSE;
	}

	return RegExp::Match( strFilter, strContent );
}

//////////////////////////////////////////////////////////////////////
// CSecureRule content list helpers

void CSecureRule::SetContentWords(const CString& strContent)
{
	if ( m_nType == srContentRegExp )
	{
		delete [] m_pContent;
		m_nContentLength = strContent.GetLength() + 2;
		LPTSTR pszContent = new TCHAR[ m_nContentLength ];
		_tcscpy_s( pszContent, m_nContentLength, strContent );
		m_pContent = pszContent;
		pszContent += strContent.GetLength();
		*pszContent++ = 0;
		*pszContent++ = 0;
		return;
	}

	LPTSTR pszContent	= (LPTSTR)(LPCTSTR)strContent;
	int nTotalLength	= 3;
	CList< CString > pWords;

	int nStart = 0, nPos = 0;
	for ( ; *pszContent ; nPos++, pszContent++ )
	{
		if ( *pszContent == ' ' || *pszContent == '\t' )
		{
			if ( nStart < nPos )
			{
				pWords.AddTail( strContent.Mid( nStart, nPos - nStart ) );
				nTotalLength += nPos - nStart + 1;
			}
			nStart = nPos + 1;
		}
	}

	if ( nStart < nPos )
	{
		pWords.AddTail( strContent.Mid( nStart, nPos - nStart ) );
		nTotalLength += nPos - nStart + 1;
	}

	if ( m_pContent )
	{
		delete [] m_pContent;
		m_pContent = NULL;
		m_nContentLength = 0;
	}

	if ( pWords.IsEmpty() ) return;

	m_pContent	= new TCHAR[ m_nContentLength = nTotalLength ];
	pszContent	= m_pContent;

	for ( POSITION pos = pWords.GetHeadPosition() ; pos ; )
	{
		CString strWord = pWords.GetNext( pos );
		CopyMemory( pszContent, (LPCTSTR)strWord, ( strWord.GetLength() + 1 ) * sizeof(TCHAR) );
		pszContent += strWord.GetLength() + 1;
	}

	*pszContent++ = 0;
	*pszContent++ = 0;
}

CString CSecureRule::GetContentWords() const
{
	if ( m_pContent == NULL )
		return CString();

	if ( m_nType == srContentRegExp )
		return CString( m_pContent );

	ASSERT( m_nType != srAddress );

	CString strWords;
	for ( LPCTSTR pszFilter = m_pContent ; *pszFilter ; )
	{
		if ( ! strWords.IsEmpty() )
			strWords += ' ';
		strWords += pszFilter;

		pszFilter += _tcslen( pszFilter ) + 1;
	}

	return strWords;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule serialize

void CSecureRule::Serialize(CArchive& ar, int nVersion)
{
	CString strTemp;

	if ( ar.IsStoring() )
	{
		ar << (int)m_nType;
		ar << m_nAction;
		ar << m_sComment;

		ar.Write( &m_pGUID, sizeof(GUID) );

		ar << m_nExpire;
		ar << m_nEver;

		switch ( m_nType )
		{
		case srAddress:
			ar.Write( m_nIP, 4 );
			ar.Write( m_nMask, 4 );
			break;
		case srContentAny:
		case srContentAll:
		case srContentRegExp:
		case srSizeType:
			strTemp = GetContentWords();
			ar << strTemp;
			break;
		}
	}
	else // Loading
	{
		int nType;
		ar >> nType;
		ar >> m_nAction;

		//if ( nVersion > 2 )
			ar >> m_sComment;

		//if ( nVersion > 3 )
			ReadArchive( ar, &m_pGUID, sizeof(GUID) );
		//else
		//	CoCreateGuid( &m_pGUID );

		ar >> m_nExpire;
		ar >> m_nEver;

		switch ( nType )
		{
		case 0:
			m_nType = srAddress;
			break;
		case 1:
			m_nType = srContentAny;
			break;
		case 2:
			m_nType = srContentAll;
			break;
		case 3:
			m_nType = srContentRegExp;
			break;
		case 4:
			m_nType = srSizeType;
			break;
		}

		if ( m_nType == srAddress )
		{
			ReadArchive( ar, m_nIP, 4 );
			ReadArchive( ar, m_nMask, 4 );
			MaskFix();				// Make sure old rules are updated to new format (obsolete?)
		}
		else
		{
			if ( nVersion < 5 )		// Shareaza Import?
			{
				ASSERT( m_nType == srContentAny );

				BYTE foo;
				ar >> foo;
				switch ( foo )
				{
				case 1:
					m_nType = srContentAll;
					break;
				case 2:
					m_nType = srContentRegExp;
					break;
				}
			}

			//if ( nVersion < 3 )
			//{
			//	for ( DWORD_PTR nCount = ar.ReadCount() ; nCount > 0 ; nCount-- )
			//	{
			//		CString strWord;
			//		ar >> strWord;
			//
			//		strTemp += ' ';
			//		strTemp += strWord;
			//	}
			//}
			//else
				ar >> strTemp;

			SetContentWords( strTemp );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CSecureRule XML

CXMLElement* CSecureRule::ToXML()
{
	CXMLElement* pXML = new CXMLElement( NULL, _T("rule") );
	CString strValue;

	// Order has no influence on output (Indeterminate CMap)

	wchar_t szGUID[39];
	szGUID[ StringFromGUID2( *(GUID*)&m_pGUID, szGUID, 39 ) - 2 ] = 0;
	pXML->AddAttribute( _T("guid"), (CString)&szGUID[1] );

	if ( ! m_sComment.IsEmpty() )
		pXML->AddAttribute( _T("comment"), m_sComment );

	pXML->AddAttribute( _T("action"),
		( m_nAction == srDeny ? _T("deny") : m_nAction == srAccept ? _T("accept") : _T("null") ) ); 	// srNull

	if ( m_nExpire > srSession )
	{
		strValue.Format( _T("%lu"), m_nExpire );
		pXML->AddAttribute( _T("expire"), strValue );
	}
	else if ( m_nExpire == srSession )
	{
		pXML->AddAttribute( _T("expire"), _T("session") );
	}

	if ( m_nType == srAddress )
	{
		if ( *(DWORD*)m_nMask != 0xFFFFFFFF )
		{
			strValue.Format( _T("%lu.%lu.%lu.%lu"),
				m_nMask[0], m_nMask[1], m_nMask[2], m_nMask[3] );
			pXML->AddAttribute( _T("mask"), strValue );
		}

		strValue.Format( _T("%lu.%lu.%lu.%lu"),
			m_nIP[0], m_nIP[1], m_nIP[2], m_nIP[3] );
		pXML->AddAttribute( _T("address"), strValue );

		pXML->AddAttribute( _T("type"), _T("address") );
	}
	else	// srContentAny srContentAll srContentRegExp srSizeType
	{
		CString str;
		switch ( m_nType )
		{
		case srContentAny:
			str = L"any";
			break;
		case srContentAll:
			str = L"all";
			break;
		case srContentRegExp:
			str = L"regexp";
			break;
		case srSizeType:
			str = L"size";
			break;
		//case srAddress:
		default:
			str = L"null";
		}

		pXML->AddAttribute( _T("match"), str );
		pXML->AddAttribute( _T("content"), GetContentWords() );
		pXML->AddAttribute( _T("type"), _T("content") );
	}

	return pXML;
}

BOOL CSecureRule::FromXML(CXMLElement* pXML)
{
	m_sComment = pXML->GetAttributeValue( _T("comment") );

	CString strType = pXML->GetAttributeValue( _T("type") );

	if ( strType.CompareNoCase( _T("address") ) == 0 )
	{
		int x[4];

		m_nType = srAddress;

		CString strAddress = pXML->GetAttributeValue( _T("address") );
		if ( _stscanf( strAddress, _T("%lu.%lu.%lu.%lu"), &x[0], &x[1], &x[2], &x[3] ) == 4 )
		{
			m_nIP[0] = (BYTE)x[0]; m_nIP[1] = (BYTE)x[1];
			m_nIP[2] = (BYTE)x[2]; m_nIP[3] = (BYTE)x[3];
		}

		CString strMask = pXML->GetAttributeValue( _T("mask") );
		if ( _stscanf( strMask, _T("%lu.%lu.%lu.%lu"), &x[0], &x[1], &x[2], &x[3] ) == 4 )
		{
			m_nMask[0] = (BYTE)x[0]; m_nMask[1] = (BYTE)x[1];
			m_nMask[2] = (BYTE)x[2]; m_nMask[3] = (BYTE)x[3];
		}
	}
	else if ( strType.CompareNoCase( _T("content") ) == 0 )
	{
		m_nType = srContentAny;
		CString strMatch = pXML->GetAttributeValue( _T("match") );
		if ( strMatch.CompareNoCase( _T("all") ) == 0 )
			m_nType = srContentAll;
		else if ( strMatch.CompareNoCase( _T("regexp") ) == 0 )
			m_nType = srContentRegExp;
		else if ( strMatch.CompareNoCase( _T("size") ) == 0 )
			m_nType = srSizeType;
		SetContentWords( pXML->GetAttributeValue( _T("content") ) );
		if ( m_pContent == NULL ) return FALSE;
	}
	else
	{
		return FALSE;
	}

	CString strAction = pXML->GetAttributeValue( _T("action") );

	if ( strAction.CompareNoCase( _T("null") ) == 0 || strAction.CompareNoCase( _T("none") ) == 0 )
		m_nAction = srNull;
	else if ( strAction.CompareNoCase( _T("accept") ) == 0 )
		m_nAction = srAccept;
	else if ( strAction.CompareNoCase( _T("deny") ) == 0 || strAction.IsEmpty() )
		m_nAction = srDeny;
	else
		return FALSE;

	CString strExpire = pXML->GetAttributeValue( _T("expire") );
	m_nExpire = srIndefinite;

	if ( strExpire.CompareNoCase( _T("session") ) == 0 )
		m_nExpire = srSession;
	else if ( strExpire.CompareNoCase( _T("indefinite") ) != 0 )
		_stscanf( strExpire, _T("%lu"), &m_nExpire );

	MaskFix();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule Gnucelus strings

CString CSecureRule::ToGnucleusString() const
{
	CString strRule;

	if ( m_nType != srAddress ) return strRule;
	if ( m_nAction != srDeny ) return strRule;

	if ( *(DWORD*)m_nMask == 0xFFFFFFFF )
	{
		strRule.Format( _T("%lu.%lu.%lu.%lu"),
			m_nIP[0], m_nIP[1], m_nIP[2], m_nIP[3] );
	}
	else
	{
		int nFrom[4], nTo[4];

		for ( int nByte = 0 ; nByte < 4 ; nByte++ )
		{
			nFrom[ nByte ]	= m_nIP[ nByte ] & m_nMask[ nByte ];
			nTo[ nByte ]	= m_nIP[ nByte ] | ( ~m_nMask[ nByte ] );
		}

		strRule.Format( _T("%lu.%lu.%lu.%lu-%lu.%lu.%lu.%lu"),
			nFrom[0], nFrom[1], nFrom[2], nFrom[3],
			nTo[0], nTo[1], nTo[2], nTo[3] );
	}

	strRule += ':';
	strRule += m_sComment;
	strRule += ':';

	return strRule;
}

BOOL CSecureRule::FromGnucleusString(CString& str)
{
	int nPos, x[4];

	nPos = str.Find( ':' );
	if ( nPos < 1 ) return FALSE;

	CString strAddress = str.Left( nPos );
	str = str.Mid( nPos + 1 );

	if ( _stscanf( strAddress, _T("%lu.%lu.%lu.%lu"), &x[0], &x[1], &x[2], &x[3] ) != 4 )
		return FALSE;

	m_nIP[0] = (BYTE)x[0]; m_nIP[1] = (BYTE)x[1];
	m_nIP[2] = (BYTE)x[2]; m_nIP[3] = (BYTE)x[3];

	nPos = strAddress.Find( '-' );

	if ( nPos >= 0 )
	{
		strAddress = strAddress.Mid( nPos + 1 );

		if ( _stscanf( strAddress, _T("%lu.%lu.%lu.%lu"), &x[0], &x[1], &x[2], &x[3] ) != 4 )
			return FALSE;

		for ( int nByte = 0 ; nByte < 4 ; nByte++ )
		{
			BYTE nTop = (BYTE)x[ nByte ], nBase = (BYTE)x[ nByte ];

			for ( BYTE nValue = m_nIP[ nByte ] ; nValue < nTop ; nValue++ )
			{
				m_nMask[ nByte ] &= ~( nValue ^ nBase );
			}
		}
	}

	m_nType		= srAddress;
	m_nAction	= srDeny;
	m_nExpire	= srIndefinite;
	m_sComment	= str.SpanExcluding( _T(":") );

	MaskFix();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule Netmask Fix

void CSecureRule::MaskFix()
{
	DWORD nNetwork = 0 , nOldMask = 0 , nNewMask = 0;

	for ( int nByte = 0 ; nByte < 4 ; nByte++ )		// Convert the byte arrays to dwords
	{
		BYTE nMaskByte = 0;
		BYTE nNetByte = 0;
		nNetByte = m_nIP[ nByte ];
		nMaskByte = m_nMask[ nByte ];
		for ( int nBits = 0 ; nBits < 8 ; nBits++ )
		{
			nNetwork <<= 1;
			if ( nNetByte & 0x80 )
				nNetwork |= 1;

			nNetByte <<= 1;

			nOldMask <<= 1;
			if ( nMaskByte & 0x80 )
				nOldMask |= 1;

			nMaskByte <<= 1;
		}
	}

	DWORD nTempMask = nOldMask;

	for ( int nBits = 0 ; nBits < 32 ; nBits++ )	// Get upper contiguous bits from subnet mask
	{
		if ( nTempMask & 0x80000000 )				// Check the high bit
		{
			nNewMask >>= 1;							// Shift mask down
			nNewMask |= 0x80000000;					// Put the bit on
		}
		else
		{
			break;									// Found a 0 so ignore the rest
		}
		nTempMask <<= 1;
	}

	if ( nNewMask != nOldMask )						// Set rule to expire if mask is invalid
	{
		m_nExpire = srSession;
		return;
	}

	nNetwork &= nNewMask;							// Do the & now so we don't have to each time there's a match

	for ( int nByte = 0 ; nByte < 4 ; nByte++ )		// Convert the dwords back to byte arrays
	{
		BYTE nNetByte = 0;
		for ( int nBits = 0 ; nBits < 8 ; nBits++ )
		{
			nNetByte <<= 1;
			if ( nNetwork & 0x80000000 )
				nNetByte |= 1;

			nNetwork <<= 1;
		}
		m_nIP[ nByte ] = nNetByte;
	}
}


//////////////////////////////////////////////////////////////////////
// CAdultFilter construction

CAdultFilter::CAdultFilter()
	: m_pszBlockedWords	( NULL )
	, m_pszDubiousWords	( NULL )
	, m_pszChildWords	( NULL )
{
}

CAdultFilter::~CAdultFilter()
{
	if ( m_pszBlockedWords ) delete [] m_pszBlockedWords;
	m_pszBlockedWords = NULL;

	if ( m_pszDubiousWords ) delete [] m_pszDubiousWords;
	m_pszDubiousWords = NULL;

	if ( m_pszChildWords ) delete [] m_pszChildWords;
	m_pszChildWords = NULL;
}

void CAdultFilter::Load()
{
	CFile pFile;
	CString strBlockedWords, strDubiousWords, strChildWords;
	const CString strFile = Settings.General.Path + _T("\\Data\\AdultFilter.dat");

	// Delete current adult filters (if present)
	if ( m_pszBlockedWords ) delete [] m_pszBlockedWords;
	m_pszBlockedWords = NULL;

	if ( m_pszDubiousWords ) delete [] m_pszDubiousWords;
	m_pszDubiousWords = NULL;

	if ( m_pszChildWords ) delete [] m_pszChildWords;
	m_pszChildWords = NULL;

	// Load the adult filter from disk
	if ( pFile.Open( strFile, CFile::modeRead ) )
	{
		try
		{
			CBuffer pBuffer;
			const DWORD nLen = (DWORD)pFile.GetLength();
			if ( ! pBuffer.EnsureBuffer( nLen ) )
				AfxThrowUserException();

			pBuffer.m_nLength = nLen;
			pFile.Read( pBuffer.m_pBuffer, pBuffer.m_nLength );
			pFile.Close();

			pBuffer.ReadLine( strBlockedWords );	// Line 1: words that are blocked
			if ( ! strBlockedWords.IsEmpty() && strBlockedWords.GetAt( 0 ) == '#' )
				strBlockedWords.Empty();
			pBuffer.ReadLine( strDubiousWords );	// Line 2: words that may be okay
			if ( ! strDubiousWords.IsEmpty() && strDubiousWords.GetAt( 0 ) == '#' )
				strDubiousWords.Empty();
			pBuffer.ReadLine( strChildWords );		// Line 3: words for child pornography
			if ( ! strChildWords.IsEmpty() && strChildWords.GetAt( 0 ) == '#' )
				strChildWords.Empty();
		}
		catch ( CException* pException )
		{
			if ( pFile.m_hFile != CFile::hFileNull )
				pFile.Close();	// File is still open so close it
			pException->Delete();
		}
	}

	// Insert some defaults if the load failed
	if ( strBlockedWords.IsEmpty() )
		strBlockedWords = L"xxx porn fuck cock cunt vagina pussy nude naked boobs breast hentai "
						  L"lesbian whore shit rape preteen hardcore lolita playboy penthouse "
						  L"topless r-rated x-rated dildo pr0n erotic sexy orgasm nipple fetish "
						  L"upskirt beastiality bestiality pedofil necrofil tits lolicon shemale fisting";
	if ( strDubiousWords.IsEmpty() )
		strDubiousWords = L"ass sex anal gay teen thong babe bikini viagra dick cum sluts";

	if ( strChildWords.IsEmpty() )
		strChildWords = L"child preteen";

	// Load the blocked words into the Adult Filter
	if ( strBlockedWords.GetLength() > 3 )
	{
		LPCTSTR pszPtr = strBlockedWords;
		int nWordLen = 3;
		CList< CString > pWords;

		int nStart = 0, nPos = 0;
		for ( ; *pszPtr ; nPos++, pszPtr++ )
		{
			if ( *pszPtr == ' ' )
			{
				if ( nStart < nPos )
				{
					pWords.AddTail( strBlockedWords.Mid( nStart, nPos - nStart ) );
					nWordLen += ( nPos - nStart ) + 1;
				}
				nStart = nPos + 1;
			}
		}

		if ( nStart < nPos )
		{
			pWords.AddTail( strBlockedWords.Mid( nStart, nPos - nStart ) );
			nWordLen += ( nPos - nStart ) + 1;
		}

		m_pszBlockedWords = new TCHAR[ nWordLen ];
		LPTSTR pszFilter = m_pszBlockedWords;

		for ( POSITION pos = pWords.GetHeadPosition() ; pos ; )
		{
			CString strWord( pWords.GetNext( pos ) );
			ToLower( strWord );

			CopyMemory( pszFilter, (LPCTSTR)strWord, sizeof(TCHAR) * ( strWord.GetLength() + 1 ) );
			pszFilter += strWord.GetLength() + 1;
		}

		*pszFilter++ = 0;
		*pszFilter++ = 0;
	}

	// Load the possibly blocked words into the Adult Filter
	if ( strDubiousWords.GetLength() > 3 )
	{
		LPCTSTR pszPtr = strDubiousWords;
		int nWordLen = 3;
		CList< CString > pWords;

		int nStart = 0, nPos = 0;
		for ( ; *pszPtr ; nPos++, pszPtr++ )
		{
			if ( *pszPtr == ' ' )
			{
				if ( nStart < nPos )
				{
					pWords.AddTail( strDubiousWords.Mid( nStart, nPos - nStart ) );
					nWordLen += ( nPos - nStart ) + 1;
				}
				nStart = nPos + 1;
			}
		}

		if ( nStart < nPos )
		{
			pWords.AddTail( strDubiousWords.Mid( nStart, nPos - nStart ) );
			nWordLen += ( nPos - nStart ) + 1;
		}

		m_pszDubiousWords = new TCHAR[ nWordLen ];
		LPTSTR pszFilter = m_pszDubiousWords;

		for ( POSITION pos = pWords.GetHeadPosition() ; pos ; )
		{
			CString strWord( pWords.GetNext( pos ) );
			ToLower( strWord );

			CopyMemory( pszFilter, (LPCTSTR)strWord, sizeof(TCHAR) * ( strWord.GetLength() + 1 ) );
			pszFilter += strWord.GetLength() + 1;
		}

		*pszFilter++ = 0;
		*pszFilter++ = 0;
	}

	// Load child pornography words into the Adult Filter
	if ( strChildWords.GetLength() > 3 )
	{
		LPCTSTR pszPtr = strChildWords;
		int nWordLen = 3;
		CList< CString > pWords;

		int nStart = 0, nPos = 0;
		for ( ; *pszPtr ; nPos++, pszPtr++ )
		{
			if ( *pszPtr == ' ' )
			{
				if ( nStart < nPos )
				{
					pWords.AddTail( strChildWords.Mid( nStart, nPos - nStart ) );
					nWordLen += ( nPos - nStart ) + 1;
				}
				nStart = nPos + 1;
			}
		}

		if ( nStart < nPos )
		{
			pWords.AddTail( strChildWords.Mid( nStart, nPos - nStart ) );
			nWordLen += ( nPos - nStart ) + 1;
		}

		m_pszChildWords = new TCHAR[ nWordLen ];
		LPTSTR pszFilter = m_pszChildWords;

		for ( POSITION pos = pWords.GetHeadPosition() ; pos ; )
		{
			CString strWord( pWords.GetNext( pos ) );
			ToLower( strWord );

			CopyMemory( pszFilter, (LPCTSTR)strWord, sizeof(TCHAR) * ( strWord.GetLength() + 1 ) );
			pszFilter += strWord.GetLength() + 1;
		}

		*pszFilter++ = 0;
		*pszFilter++ = 0;
	}
}

BOOL CAdultFilter::IsHitAdult(LPCTSTR pszText) const
{
	if ( pszText )
		return IsFiltered( pszText );

	return FALSE;
}

BOOL CAdultFilter::IsSearchFiltered(LPCTSTR pszText) const
{
	if ( Settings.Search.AdultFilter && pszText )
		return IsFiltered( pszText );

	return FALSE;
}

BOOL CAdultFilter::IsChatFiltered(LPCTSTR pszText) const
{
	if ( Settings.Community.ChatCensor && pszText )
		return IsFiltered( pszText );

	return FALSE;
}

BOOL CAdultFilter::Censor(TCHAR* pszText) const
{
	BOOL bModified = FALSE;
	if ( ! pszText ) return FALSE;

	// Check and replace blocked words
	if ( m_pszBlockedWords )
	{
		for ( LPCTSTR pszWord = m_pszBlockedWords ; *pszWord ; )
		{
			TCHAR* pReplace = (TCHAR*)_tcsistr( pszText, pszWord );

			if ( pReplace != NULL )
			{
				TCHAR cExpletives[6] = {'#','@','$','%','&','*'};

				for ( unsigned nLoop = 0 ; nLoop < _tcslen( pszWord ) ; nLoop++ )
				{
					*pReplace = cExpletives[ ( nLoop % 6 ) ];
					pReplace++;
				}

				bModified = TRUE;
			}

			pszWord += _tcslen( pszWord ) + 1;
		}
	}

	return bModified;
}

BOOL CAdultFilter::IsChildPornography(LPCTSTR pszText) const
{
	if ( pszText )
	{
		LPCTSTR pszWord;
		bool bFound = false;

		for ( pszWord = m_pszChildWords ; *pszWord ; )
		{
			if ( _tcsistr( pszText, pszWord ) != NULL )
			{
				bFound = true;
				break;
			}
			pszWord += _tcslen( pszWord ) + 1;
		}

		return ( bFound && IsFiltered( pszText ) );
	}

	return FALSE;
}

BOOL CAdultFilter::IsFiltered(LPCTSTR pszText) const
{
	if ( pszText )
	{
		LPCTSTR pszWord;

		// Check blocked words
		if ( m_pszBlockedWords )
		{
			for ( pszWord = m_pszBlockedWords ; *pszWord ; )
			{
				if ( _tcsistr( pszText, pszWord ) != NULL ) return TRUE;
				pszWord += _tcslen( pszWord ) + 1;
			}
		}

		// Check dubious words
		if ( m_pszDubiousWords )
		{
			size_t nDubiousWords = 0, nWordsPermitted = min( _tcslen( pszText ) / 8, 4u );

			for ( pszWord = m_pszDubiousWords ; *pszWord ; )
			{
				if ( _tcsistr( pszText, pszWord ) != NULL ) nDubiousWords++;
				if ( nDubiousWords > nWordsPermitted ) return TRUE;
				pszWord += _tcslen( pszWord ) + 1;
			}
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// CMessageFilter construction

CMessageFilter::CMessageFilter()
{
	m_pszED2KSpam = NULL;
	m_pszFilteredPhrases = NULL;
}

CMessageFilter::~CMessageFilter()
{
	if ( m_pszED2KSpam ) delete [] m_pszED2KSpam;
	m_pszED2KSpam = NULL;

	if ( m_pszFilteredPhrases ) delete [] m_pszFilteredPhrases;
	m_pszFilteredPhrases = NULL;

}

void CMessageFilter::Load()
{
	CFile pFile;
	CString strFilteredPhrases, strED2KSpamPhrases;
	const CString strFile = Settings.General.Path + _T("\\Data\\MessageFilter.dat");

	// Delete current filter (if present)
	if ( m_pszFilteredPhrases ) delete [] m_pszFilteredPhrases;
	m_pszFilteredPhrases = NULL;

	// Load the message filter from disk
	if ( pFile.Open( strFile, CFile::modeRead ) )
	{
		try
		{
			CBuffer pBuffer;
			DWORD nLen = (DWORD)pFile.GetLength();
			if ( ! pBuffer.EnsureBuffer( nLen ) )
				AfxThrowUserException();

			pBuffer.m_nLength = nLen;
			pFile.Read( pBuffer.m_pBuffer, pBuffer.m_nLength );
			pFile.Close();

			pBuffer.ReadLine( strED2KSpamPhrases );
			pBuffer.ReadLine( strFilteredPhrases );
		}
		catch ( CException* pException )
		{
			if ( pFile.m_hFile != CFile::hFileNull )
				pFile.Close();		// If file is still open close it
			pException->Delete();
		}
	}

	// Insert some defaults if there was a read error

	if ( strED2KSpamPhrases.IsEmpty() )
		strED2KSpamPhrases = _T("Your client is connecting too fast|Join the L33cher Team|PeerFactor|Your client is making too many connections|ZamBoR 2|AUTOMATED MESSAGE:|eMule FX the BEST eMule ever|DI-Emule");

	if ( strFilteredPhrases.IsEmpty() )
		strFilteredPhrases = _T("");


	// Load the ED2K spam into the filter
	if ( strED2KSpamPhrases.GetLength() > 3 )
	{
		LPCTSTR pszPtr = strED2KSpamPhrases;
		int nWordLen = 3;
		CList< CString > pWords;

		int nStart = 0, nPos = 0;
		for ( ; *pszPtr ; nPos++, pszPtr++ )
		{
			if ( *pszPtr == '|' )
			{
				if ( nStart < nPos )
				{
					pWords.AddTail( strED2KSpamPhrases.Mid( nStart, nPos - nStart ) );
					nWordLen += ( nPos - nStart ) + 1;
				}
				nStart = nPos + 1;
			}
		}

		if ( nStart < nPos )
		{
			pWords.AddTail( strED2KSpamPhrases.Mid( nStart, nPos - nStart ) );
			nWordLen += ( nPos - nStart ) + 1;
		}

		m_pszED2KSpam = new TCHAR[ nWordLen ];
		LPTSTR pszFilter = m_pszED2KSpam;

		for ( POSITION pos = pWords.GetHeadPosition() ; pos ; )
		{
			CString strWord( pWords.GetNext( pos ) );
			ToLower( strWord );

			CopyMemory( pszFilter, (LPCTSTR)strWord, sizeof(TCHAR) * ( strWord.GetLength() + 1 ) );
			pszFilter += strWord.GetLength() + 1;
		}

		*pszFilter++ = 0;
		*pszFilter++ = 0;
	}

	// Load the blocked strings into the filter
	if ( strFilteredPhrases.GetLength() > 3 )
	{
		LPCTSTR pszPtr = strFilteredPhrases;
		int nWordLen = 3;
		CList< CString > pWords;

		int nStart = 0, nPos = 0;
		for ( ; *pszPtr ; nPos++, pszPtr++ )
		{
			if ( *pszPtr == '|' )
			{
				if ( nStart < nPos )
				{
					pWords.AddTail( strFilteredPhrases.Mid( nStart, nPos - nStart ) );
					nWordLen += ( nPos - nStart ) + 1;
				}
				nStart = nPos + 1;
			}
		}

		if ( nStart < nPos )
		{
			pWords.AddTail( strFilteredPhrases.Mid( nStart, nPos - nStart ) );
			nWordLen += ( nPos - nStart ) + 1;
		}

		m_pszFilteredPhrases = new TCHAR[ nWordLen ];
		LPTSTR pszFilter = m_pszFilteredPhrases;

		for ( POSITION pos = pWords.GetHeadPosition() ; pos ; )
		{
			CString strWord( pWords.GetNext( pos ) );
			ToLower( strWord );

			CopyMemory( pszFilter, (LPCTSTR)strWord, sizeof(TCHAR) * ( strWord.GetLength() + 1 ) );
			pszFilter += strWord.GetLength() + 1;
		}

		*pszFilter++ = 0;
		*pszFilter++ = 0;
	}
}

BOOL CMessageFilter::IsED2KSpam( LPCTSTR pszText )
{
	if ( ! Settings.Community.ChatFilterED2K || ! pszText )
		return FALSE;

	// Check for Ed2K spam phrases
	if ( m_pszED2KSpam )
	{
		LPCTSTR pszWord;
		for ( pszWord = m_pszED2KSpam ; *pszWord ; )
		{
			if ( _tcsistr( pszText, pszWord ) != NULL ) return TRUE;
			pszWord += _tcslen( pszWord ) + 1;
		}
	}

	return FALSE;
}

BOOL CMessageFilter::IsFiltered( LPCTSTR pszText )
{
	if ( ! Settings.Community.ChatFilter || ! pszText )
		return FALSE;

	// Check for filtered (spam) phrases
	if ( m_pszFilteredPhrases )
	{
		LPCTSTR pszWord;
		for ( pszWord = m_pszFilteredPhrases ; *pszWord ; )
		{
			if ( _tcsistr( pszText, pszWord ) != NULL ) return TRUE;
			pszWord += _tcslen( pszWord ) + 1;
		}
	}

	return FALSE;
}
