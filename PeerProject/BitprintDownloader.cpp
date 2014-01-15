//
// BitprintDownloader.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2014
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
#include "BitprintDownloader.h"
#include "DlgBitprintDownload.h"
#include "Network.h"
#include "Library.h"
#include "SharedFile.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CBitprintDownloader construction

CBitprintDownloader::CBitprintDownloader()
	: m_pDlg		( NULL )
	, m_hInternet	( NULL )
	, m_hSession	( NULL )
	, m_hRequest	( NULL )
	, m_bFinished	( FALSE )
	, m_nFailures	( 0 )
	, m_nDelay		( 0 )
	, m_pXML		( NULL )
{
}

CBitprintDownloader::~CBitprintDownloader()
{
	Stop();

	if ( m_pXML ) delete m_pXML;
	m_pXML = NULL;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader file list

void CBitprintDownloader::AddFile(DWORD nIndex)
{
	CSingleLock pLock( &m_pSection, TRUE );
	m_pFiles.AddTail( nIndex );
}

INT_PTR CBitprintDownloader::GetFileCount()
{
	CSingleLock pLock( &m_pSection, TRUE );
	return m_pFiles.GetCount();
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader start

BOOL CBitprintDownloader::Start(CBitprintDownloadDlg* pDlg)
{
	if ( m_hInternet ) return FALSE;

	m_hInternet = CNetwork::SafeInternetOpen();
	if ( ! m_hInternet ) return FALSE;

	m_hSession	= NULL;
	m_hRequest	= NULL;
	m_pDlg		= pDlg;
	m_bFinished	= FALSE;
	m_nDelay	= 0;
	m_nFailures	= 0;

	BeginThread( "BitprintDownloader" );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader stop

void CBitprintDownloader::Stop()
{
	if ( m_hSession ) InternetCloseHandle( m_hSession );
	m_hSession = NULL;

	if ( m_hInternet ) InternetCloseHandle( m_hInternet );
	m_hInternet = NULL;

	CloseThread();

	m_pDlg = NULL;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader working flag

BOOL CBitprintDownloader::IsWorking()
{
	return IsThreadAlive() && ! m_bFinished;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader thread run

void CBitprintDownloader::OnRun()
{
	while ( m_hInternet != NULL )
	{
		m_pSection.Lock();

		if ( m_pFiles.IsEmpty() )
		{
			m_pSection.Unlock();
			break;
		}

		m_nFileIndex = m_pFiles.RemoveHead();

		m_pSection.Unlock();

		m_pDlg->OnNextFile( m_nFileIndex );

		if ( BuildRequest() )
		{
			m_pDlg->OnRequesting( m_nFileIndex, m_sFileName );

			if ( ExecuteRequest() )
			{
				if ( DecodeResponse() )
				{
					m_pDlg->OnSuccess( m_nFileIndex );
				}
				else
				{
					if ( m_hInternet == NULL ) break;
					m_pDlg->OnFailure( m_nFileIndex, _T("Not Found") );
				}
			}
			else if ( ++m_nFailures >= 3 )
			{
				if ( m_hInternet == NULL ) break;
				m_pDlg->OnFailure( m_nFileIndex, _T("Aborting") );
				break;
			}
			else
			{
				if ( m_hInternet == NULL ) break;

				if ( m_hRequest ) InternetCloseHandle( m_hRequest );
				m_hRequest = NULL;

				m_pDlg->OnFailure( m_nFileIndex, _T("Failed") );

				Sleep( 1000 );
			}
		}

		m_pDlg->OnFinishedFile( m_nFileIndex );

		if ( m_hRequest ) InternetCloseHandle( m_hRequest );
		m_hRequest = NULL;

		m_sResponse.Empty();

		if ( m_pXML ) delete m_pXML;
		m_pXML = NULL;

		Sleep( min( m_nDelay, 500ul ) );
	}

	if ( m_hSession != NULL && ! m_bFinished ) InternetCloseHandle( m_hSession );
	m_hSession = NULL;

	m_bFinished = TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader request builder

BOOL CBitprintDownloader::BuildRequest()
{
	{
		CQuickLock oLock( Library.m_pSection );
		CLibraryFile* pFile = Library.LookupFile( m_nFileIndex );

		if ( ! pFile ) return FALSE;

		m_sFileName = pFile->m_sName;
		m_sFileSHA1 = pFile->m_oSHA1.toString();
		m_sFileTiger = pFile->m_oTiger.toString();
	}

	if ( m_sFileSHA1.IsEmpty() || m_sFileTiger.IsEmpty() ) return FALSE;

	m_sURL = Settings.WebServices.BitprintXML;
	m_sURL.Replace( _T("(SHA1)"), m_sFileSHA1 );
	m_sURL.Replace( _T("(TTH)"), m_sFileTiger );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader request executer

BOOL CBitprintDownloader::ExecuteRequest()
{
	const DWORD tTime = GetTickCount();

	theApp.Message( MSG_DEBUG | MSG_FACILITY_OUTGOING, _T("[Bitprint] Sent request: %s"), m_sURL );

	CString strHost = m_sURL;
	int nPos = strHost.Find( _T("http://") );
	if ( nPos != 0 ) return FALSE;
	strHost = strHost.Mid( 7 );
	nPos = strHost.Find( '/' );
	if ( nPos < 0 ) return FALSE;
	CString strPath = strHost.Mid( nPos );
	strHost = strHost.Left( nPos );
	nPos = strHost.Find( ':' );

	int nPort = INTERNET_DEFAULT_HTTP_PORT;

	if ( nPos > 0 )
	{
		_stscanf( strHost.Mid( nPos + 1 ), _T("%i"), &nPort );
		strHost = strHost.Left( nPos );
	}

	if ( m_hSession == NULL )
	{
		m_hSession = InternetConnect( m_hInternet, strHost, INTERNET_PORT( nPort ),
			NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
		if ( m_hSession == NULL )
			return FALSE;
	}

	m_hRequest = HttpOpenRequest( m_hSession, _T("GET"), strPath, NULL, NULL, NULL,
		INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0 );

	if ( m_hRequest == NULL )
	{
		if ( m_hSession ) InternetCloseHandle( m_hSession );

		m_hSession = InternetConnect( m_hInternet, strHost, INTERNET_PORT( nPort ),
			NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );

		if ( m_hSession == NULL ) return FALSE;

		m_hRequest = HttpOpenRequest( m_hSession, _T("GET"), strPath, NULL, NULL, NULL,
			INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0 );

		if ( m_hRequest == NULL ) return FALSE;
	}

	if ( ! HttpSendRequest( m_hRequest, NULL, 0, NULL, 0 ) )
		return FALSE;

	TCHAR szStatusCode[32];
	DWORD nStatusCode = 0, nStatusLen = 32;

	if ( ! HttpQueryInfo( m_hRequest, HTTP_QUERY_STATUS_CODE, szStatusCode,
		&nStatusLen, NULL ) )
		return FALSE;

	if ( _stscanf( szStatusCode, _T("%u"), &nStatusCode ) != 1 ||
		nStatusCode < 200 || nStatusCode > 299 )
		return FALSE;

	LPBYTE pResponse = NULL;
	DWORD nRemaining, nResponse = 0;

	while ( InternetQueryDataAvailable( m_hRequest, &nRemaining, 0, 0 ) && nRemaining > 0 )
	{
		BYTE* pNewResponse = (BYTE*)realloc( pResponse, nResponse + nRemaining );
		if ( ! pNewResponse )
		{
			free( pResponse );
			return FALSE;
		}
		pResponse = pNewResponse;
		InternetReadFile( m_hRequest, pResponse + nResponse, nRemaining, &nRemaining );
		nResponse += nRemaining;
	}

	if ( nRemaining )
	{
		free( pResponse );
		return FALSE;
	}

	m_sResponse.Empty();

	LPTSTR pszResponse = m_sResponse.GetBuffer( nResponse );
	for ( nStatusCode = 0 ; nStatusCode < nResponse ; nStatusCode++ )
		pszResponse[ nStatusCode ] = (TCHAR)pResponse[ nStatusCode ];
	m_sResponse.ReleaseBuffer( nResponse );

	free( pResponse );

	if ( m_hRequest ) InternetCloseHandle( m_hRequest );
	m_hRequest = NULL;

	m_nDelay = ( GetTickCount() - tTime ) * 2;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader request decoder

BOOL CBitprintDownloader::DecodeResponse()
{
	if ( m_pXML ) delete m_pXML;

	theApp.Message( MSG_DEBUG | MSG_FACILITY_INCOMING, _T("[Bitprint] Got response: %s"), m_sResponse );

	m_pXML = CXMLElement::FromString( m_sResponse, TRUE );
	if ( m_pXML == NULL )
		return FALSE;

	for ( POSITION pos = SchemaCache.GetIterator() ; pos ; )
	{
		CSchemaPtr pSchema = SchemaCache.GetNext( pos );

		if ( pSchema->m_sBitprintTest.GetLength() &&
			LookupValue( pSchema->m_sBitprintTest ).GetLength() )
		{
			CXMLElement* pMetadata = ImportData( pSchema );

			if ( pMetadata == NULL )
				return FALSE;

			return SubmitMetaData( pMetadata );
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader value lookup

CString CBitprintDownloader::LookupValue(LPCTSTR pszPath)
{
	CString strName, strPath( pszPath );
	CXMLElement* pXML = m_pXML;
	BOOL bFirst = TRUE;

	while ( ! strPath.IsEmpty() )
	{
		strName = strPath.SpanExcluding( _T("/") );
		strPath = strPath.Mid( strName.GetLength() );

		if ( strPath.IsEmpty() )
			return pXML->GetAttributeValue( strName, NULL );

		if ( bFirst )
		{
			bFirst = FALSE;
			if ( strName.CompareNoCase( pXML->GetName() ) )
				pXML = NULL;
		}
		else
		{
			pXML = pXML->GetElementByName( strName );
		}

		if ( ! pXML )
		{
			strName.Empty();
			return strName;
		}

		strPath = strPath.Mid( 1 );
	}

	strName.Empty();
	if ( pXML ) strName = pXML->GetValue();

	return strName;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader import data

CXMLElement* CBitprintDownloader::ImportData(CSchemaPtr pSchema)
{
	CXMLElement* pRoot	= pSchema->Instantiate( TRUE );
	CXMLElement* pXML	= pRoot->AddElement( pSchema->m_sSingular );
	int nCount = 0;

	for ( POSITION pos = pSchema->m_pBitprintMap.GetHeadPosition() ; pos ; )
	{
		CSchemaBitprint* pMap = (CSchemaBitprint*)pSchema->m_pBitprintMap.GetNext( pos );

		CString strValue = LookupValue( pMap->m_sFrom );
		if ( strValue.IsEmpty() ) continue;

		if ( pMap->m_nFactor )
		{
			double nValue;

			if ( _stscanf( strValue, _T("%lf"), &nValue ) == 1 )
			{
				nValue *= pMap->m_nFactor;

				if ( nValue == (double)( (int)nValue ) )
					strValue.Format( _T("%i"), (int)nValue );
				else
					strValue.Format( _T("%f"), nValue );
			}
		}

		pXML->AddAttribute( pMap->m_sTo, strValue );
		nCount++;
	}

	if ( nCount )
		return pRoot;

	delete pRoot;

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CBitprintDownloader submit metadata

BOOL CBitprintDownloader::SubmitMetaData(CXMLElement* pXML)
{
	CQuickLock oLock( Library.m_pSection );

	CLibraryFile* pFile = Library.LookupFile( m_nFileIndex );

	if ( pFile == NULL )
	{
		delete pXML;
		return FALSE;
	}

	BOOL bSuccess = pFile->MergeMetadata( pXML, TRUE );

	delete pXML;

	return bSuccess;
}
