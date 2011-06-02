//
// WebServices.cpp
//
// This file is part of PeerProject (peerproject.org) © 2011
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

// Note: Consolidated library external web service interfaces
// (Bitzi Ticket, MusicBrainz, legacy ShareMonkey)
// Moved from CtrlLibraryFileView, CFileExecutor


#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "WebServices.h"
#include "FileExecutor.h"
#include "Skin.h"
#include "XML.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "SharedFile.h"
#include "Library.h"
#include "DlgBitziDownload.h"
#include "ShareMonkeyData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


/////////////////////////////////////////////////////////////////////
// CtrlLibraryFileView Web Services Handling
//
// Note: Moved from CLibraryFileView

//void CWebServices::ClearServicePages()
//{
//	for ( POSITION pos = m_pServiceDataPages.GetHeadPosition() ; pos ; )
//	{
//		CMetaPanel* pPanelData = m_pServiceDataPages.GetNext( pos );
//		delete pPanelData;
//	}
//
//	m_pServiceDataPages.RemoveAll();
//	m_nCurrentPage = 0;
//	m_bServiceFailed = FALSE;
//
//	GetFrame()->SetPanelData( NULL );
//}


/////////////////////////////////////////////////////////////////////
// BitziTicket Services

//void CWebServices::OnUpdateLibraryBitziWeb(CCmdUI* pCmdUI)
//{
//	if ( m_bGhostFolder )
//		pCmdUI->Enable( FALSE );
//	else
//		pCmdUI->Enable( GetSelectedCount() == 1 && ! Settings.WebServices.BitziWebSubmit.IsEmpty() );
//}

//void CWebServices::OnLibraryBitziWeb()
//{
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//
//	if ( CLibraryFile* pFile = GetSelectedFile() )
//	{
//		DWORD nIndex = pFile->m_nIndex;
//		pLock.Unlock();
//		ShowBitziTicket( nIndex );
//	}
//}

//void CWebServices::OnUpdateLibraryBitziDownload(CCmdUI* pCmdUI)
//{
//	if ( m_bGhostFolder || m_bRequestingService )
//		pCmdUI->Enable( FALSE );
//	else
//		pCmdUI->Enable( GetSelectedCount() > 0 && ! Settings.WebServices.BitziXML.IsEmpty() );
//}

//void WebServices::OnLibraryBitziDownload()
//{
//	GetFrame()->SetDynamicBar( NULL );
//
//	if ( ! Settings.WebServices.BitziOkay )
//	{
//		CString strFormat;
//		Skin.LoadString( strFormat, IDS_LIBRARY_BITZI_MESSAGE );
//		if ( AfxMessageBox( strFormat, MB_ICONQUESTION|MB_YESNO ) != IDYES ) return;
//		Settings.WebServices.BitziOkay = true;
//		Settings.Save();
//	}
//
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//	CBitziDownloadDlg dlg;
//
//	POSITION posSel = StartSelectedFileLoop();
//	while ( CLibraryFile* pFile = GetNextSelectedFile( posSel ) )
//	{
//		if ( pFile->m_oSHA1 )
//			dlg.AddFile( pFile->m_nIndex );
//	}
//
//	pLock.Unlock();
//
//	dlg.DoModal();
//}


/////////////////////////////////////////////////////////////////////
// MusicBrainz Services

//void CWebServices::OnUpdateMusicBrainzLookup(CCmdUI* pCmdUI)
//{
//	if ( m_bGhostFolder || GetSelectedCount() != 1 || m_bRequestingService )
//	{
//		pCmdUI->Enable( FALSE );
//		return;
//	}
//
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//
//	CLibraryFile* pFile = GetSelectedFile();
//	if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) || pFile->m_pMetadata == NULL )
//	{
//		pCmdUI->Enable( FALSE );
//		return;
//	}
//
//	CMetaList* pMetaList = new CMetaList();
//	pMetaList->Setup( pFile->m_pSchema, FALSE );
//	pMetaList->Combine( pFile->m_pMetadata );
//
//	pCmdUI->Enable( pMetaList->IsMusicBrainz() );
//
//	delete pMetaList;
//}

//void CWebServices::OnMusicBrainzLookup()
//{
//	CLibraryFrame* pFrame = GetFrame();
//	pFrame->SetDynamicBar( L"WebServices.MusicBrainz" );
//}

// Called when the selection changes
//void CWebServices::CheckDynamicBar()
//{
//	bool bIsMusicBrainz = false;
//	ClearServicePages();
//
//	CLibraryFrame* pFrame = GetFrame();
//	if ( _tcscmp( pFrame->GetDynamicBarName(), L"WebServices.MusicBrainz" ) == 0 )
//		bIsMusicBrainz = true;
//
//	if ( GetSelectedCount() != 1 )
//	{
//		if ( bIsMusicBrainz )
//		{
//			pFrame->SetDynamicBar( NULL );
//			m_bRequestingService = FALSE; // ToDo: Abort operation
//		}
//		return;
//	}
//
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//	CLibraryFile* pFile = GetSelectedFile();
//
//	if ( pFile == NULL ) // Ghost file
//	{
//		pFrame->SetDynamicBar( NULL );
//		m_bRequestingService = FALSE;
//		return;
//	}
//
//	if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) || pFile->m_pMetadata == NULL )
//	{
//		if ( bIsMusicBrainz )
//			pFrame->SetDynamicBar( NULL );
//
//		m_bRequestingService = FALSE;	// ToDo: Abort operation
//		return;
//	}
//
//	CMetaList* pMetaList = new CMetaList();
//	pMetaList->Setup( pFile->m_pSchema, FALSE );
//	pMetaList->Combine( pFile->m_pMetadata );
//
//	if ( ! pMetaList->IsMusicBrainz() && bIsMusicBrainz )
//		pFrame->SetDynamicBar( NULL );
//	else
//		pFrame->HideDynamicBar();
//
//	m_bRequestingService = FALSE;	// ToDo: Abort operation
//	delete pMetaList;
//
//	pLock.Unlock();
//}

//void CWebServices::OnUpdateMusicBrainzMatches(CCmdUI* pCmdUI)
//{
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//
//	CLibraryFile* pFile = GetSelectedFile();
//
//	ASSERT( pFile->m_pMetadata != NULL );
//
//	CXMLAttribute* pAttribute = pFile->m_pMetadata->GetAttribute( L"mbpuid" );
//	pCmdUI->Enable( pAttribute != NULL && ! pAttribute->GetValue().IsEmpty() );
//}

//void CWebServices::OnMusicBrainzMatches()
//{
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//
//	CLibraryFile* pFile = GetSelectedFile();
//	ASSERT( pFile->m_pMetadata != NULL );
//
//	CXMLAttribute* pAttribute = pFile->m_pMetadata->GetAttribute( L"mbpuid" );
//	CString strURL = L"http://musicbrainz.org/show/puid/?matchesonly=0&amp;puid=" + pAttribute->GetValue();
//
//	ShellExecute( GetSafeHwnd(), _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL );
//}

//void CWebServices::OnUpdateMusicBrainzAlbums(CCmdUI* pCmdUI)
//{
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//	CLibraryFile* pFile = GetSelectedFile();
//
//	ASSERT( pFile->m_pMetadata != NULL );
//
//	CXMLAttribute* pAttribute = pFile->m_pMetadata->GetAttribute( L"mbartistid" );
//	pCmdUI->Enable( pAttribute != NULL && ! pAttribute->GetValue().IsEmpty() );
//}

//void CWebServices::OnMusicBrainzAlbums()
//{
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//
//	CLibraryFile* pFile = GetSelectedFile();
//	ASSERT( pFile->m_pMetadata != NULL );
//
//	CXMLAttribute* pAttribute = pFile->m_pMetadata->GetAttribute( L"mbartistid" );
//	CString strURL = L"http://musicbrainz.org/artist/" + pAttribute->GetValue();
//
//	ShellExecute( GetSafeHwnd(), _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL );
//}


/////////////////////////////////////////////////////////////////////
// ShareMonkey Services  (Obsolete site, for reuse)

//void CWebServices::OnUpdateShareMonkeyLookup(CCmdUI* pCmdUI)
//{
//	pCmdUI->Enable( GetSelectedCount() == 1 && ! m_bRequestingService );
//}

//void CWebServices::OnShareMonkeyLookup()
//{
//	GetFrame()->SetDynamicBar( L"WebServices.ShareMonkey.WithSave" );
//}

//void CWebServices::OnUpdateShareMonkeyDownload(CCmdUI* pCmdUI)
//{
//	pCmdUI->Enable( ! m_bRequestingService && m_pServiceDataPages.GetCount() == 0 );
//}

//void CWebServices::OnShareMonkeyDownload()
//{
//	if ( ! Settings.WebServices.ShareMonkeyOkay )
//	{
//		CString strFormat;
//		Skin.LoadString( strFormat, IDS_SHAREMONKEY_MESSAGE );
//		if ( AfxMessageBox( strFormat, MB_ICONQUESTION|MB_YESNO ) != IDYES ) return;
//		Settings.WebServices.ShareMonkeyOkay = true;
//		Settings.Save();
//	}
//
//	CShareMonkeyData* pPanelData = new CShareMonkeyData( m_nCurrentPage );
//
//	CString strStatus;
//	LoadString( strStatus, IDS_TIP_STATUS );
//	strStatus.TrimRight( ':' );
//	pPanelData->Add( strStatus, L"Please wait..." );
//
//	if ( m_nCurrentPage == 0 )
//		ClearServicePages();
//	GetFrame()->SetPanelData( pPanelData );
//
//	m_pServiceDataPages.AddTail( pPanelData );
//
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//	CLibraryFile* pFile = GetSelectedFile();
//
//	// Should be set to FALSE, and abort button created
//	m_bRequestingService = pFile != NULL;
//	if ( m_bRequestingService )
//	{
//		DWORD nIndex = pFile->m_nIndex;
//		pLock.Unlock();
//		pPanelData->Start( this, nIndex );
//		return;
//	}
//
//	pLock.Unlock();
//}

//void CWebServices::OnUpdateShareMonkeySave(CCmdUI* pCmdUI)
//{
//	BOOL bShow = TRUE;
//	if ( m_bServiceFailed && m_nCurrentPage == m_pServiceDataPages.GetCount() - 1 )
//		bShow = FALSE;
//	pCmdUI->Enable( bShow && ! m_bRequestingService && m_pServiceDataPages.GetCount() > 0 );
//}

//void CWebServices::OnShareMonkeySave()
//{
//	INT_PTR nCurr = 0;
//	CShareMonkeyData* pPanelData = NULL;
//
//	for ( POSITION pos = m_pServiceDataPages.GetHeadPosition() ; pos ; nCurr++ )
//	{
//		if ( m_nCurrentPage == nCurr )
//		{
//			pPanelData = static_cast< CShareMonkeyData* >( m_pServiceDataPages.GetNext( pos ) );
//			break;
//		}
//	}
//
//	if ( pPanelData == NULL ) return;
//
//	CSingleLock pLock( &Library.m_pSection, TRUE );
//
//	CLibraryFile* pFile = GetSelectedFile();
//	CSchemaPtr pSchema = pFile->m_pSchema ? pFile->m_pSchema : pPanelData->GetSchema();
//
//	if ( pSchema )
//	{
//		CXMLElement* pRoot = pSchema->Instantiate( TRUE );
//		CXMLElement* pXML = NULL;
//
//		if ( pFile->m_pMetadata )
//		{
//			pXML = pFile->m_pMetadata->Clone();
//			pRoot->AddElement( pXML );
//		}
//		else
//			pXML = pRoot->AddElement( pSchema->m_sSingular );
//
//		CXMLAttribute* pTitle = new CXMLAttribute( NULL, L"title" );
//		pXML->AddAttribute( pTitle );
//
//		CXMLAttribute* pDescription = NULL;
//		if ( pSchema->CheckURI( CSchema::uriApplication ) )
//		{
//			pDescription = new CXMLAttribute( NULL, L"fileDescription" );
//			pXML->AddAttribute( pDescription );
//		}
//		else if ( pSchema->CheckURI( CSchema::uriArchive ) )
//		{
//			// No description... There should be games
//		}
//		else
//		{
//			pDescription = new CXMLAttribute( NULL, L"description" );
//			pXML->AddAttribute( pDescription );
//		}
//
//		if ( pTitle )
//			pTitle->SetValue( pPanelData->m_sProductName );
//		if ( pDescription )
//			pDescription->SetValue( pPanelData->m_sDescription );
//
//		pFile->SetMetadata( pRoot );
//		delete pRoot;
//	}
//}

//void CWebServices::OnUpdateShareMonkeySaveOption(CCmdUI* pCmdUI)
//{
//	OnUpdateShareMonkeySave( pCmdUI );
//}

//void CWebServices::OnShareMonkeySaveOption()
//{
//	Settings.WebServices.ShareMonkeySaveThumbnail = ! Settings.WebServices.ShareMonkeySaveThumbnail;
//}

//void CWebServices::OnUpdateShareMonkeyPrevious(CCmdUI* pCmdUI)
//{
//	pCmdUI->Enable( ! m_bRequestingService && m_nCurrentPage > 0 );
//}

//void CWebServices::OnShareMonkeyPrevious()
//{
//	INT_PTR nCurr = m_nCurrentPage--;
//	CShareMonkeyData* pPanelData = NULL;
//
//	POSITION pos = m_pServiceDataPages.GetHeadPosition();
//
//	while ( pos && nCurr-- )
//	{
//		pPanelData = static_cast< CShareMonkeyData* >( m_pServiceDataPages.GetNext( pos ) );
//	}
//
//	GetFrame()->SetPanelData( pPanelData );
//}

//void CWebServices::OnUpdateShareMonkeyNext(CCmdUI* pCmdUI)
//{
//	BOOL bShow = TRUE;
//	if ( m_bServiceFailed && m_nCurrentPage == m_pServiceDataPages.GetCount() - 1 )
//		bShow = FALSE;
//	pCmdUI->Enable( bShow && ! m_bRequestingService && m_pServiceDataPages.GetCount() > 0 );
//}

//void CWebServices::OnShareMonkeyNext()
//{
//	INT_PTR nCurr = ++m_nCurrentPage;
//	nCurr++;
//
//	if ( m_nCurrentPage > m_pServiceDataPages.GetCount() - 1 )
//	{
//		OnShareMonkeyDownload();
//	}
//	else
//	{
//		CShareMonkeyData* pPanelData = NULL;
//		POSITION pos = m_pServiceDataPages.GetHeadPosition();
//
//		while ( pos && nCurr-- )
//		{
//			pPanelData = static_cast< CShareMonkeyData* >( m_pServiceDataPages.GetNext( pos ) );
//		}
//
//		GetFrame()->SetPanelData( pPanelData );
//	}
//}

//void CWebServices::OnUpdateShareMonkeyPrices(CCmdUI* pCmdUI)
//{
//	BOOL bShow = TRUE;
//	if ( m_bServiceFailed && m_pServiceDataPages.GetCount() == 1 || m_pServiceDataPages.GetCount() == 0 )
//		bShow = FALSE;
//	pCmdUI->Enable( ! m_bRequestingService && bShow );
//}

//void CWebServices::OnShareMonkeyPrices()
//{
//	POSITION pos = m_pServiceDataPages.GetHeadPosition();
//	CShareMonkeyData* pData = NULL;
//
//	// ToDo: Change m_pServiceDataPages to CMap. Now it's stupid
//	for ( INT_PTR nPage = 0 ; nPage <= m_nCurrentPage ; nPage++ )
//	{
//		pData = static_cast< CShareMonkeyData* >( m_pServiceDataPages.GetNext( pos ) );
//	}
//
//	if ( pData && ! pData->m_pChild )
//	{
//		CShareMonkeyData* pChild = new CShareMonkeyData( 0, CShareMonkeyData::stStoreMatch );
//		pData->m_pChild = pChild;
//		CString strStatus;
//		LoadString( strStatus, IDS_TIP_STATUS );
//		strStatus.TrimRight( ':' );
//		pChild->Add( strStatus, L"Please wait..." );
//		pChild->m_sSessionID = pData->m_sSessionID;
//		pChild->m_sProductID = pData->m_sProductID;
//		pChild->m_sThumbnailURL = pData->m_sThumbnailURL;
//
//		GetFrame()->SetPanelData( pChild );
//		pChild->Start( this, 0 );
//	}
//	else
//	{
//		GetFrame()->SetPanelData( pData ? pData->m_pChild : NULL );
//	}
//}

//void CWebServices::OnUpdateShareMonkeyCompare(CCmdUI* pCmdUI)
//{
//	BOOL bShow = TRUE;
//	if ( m_bServiceFailed && m_pServiceDataPages.GetCount() == 1 || m_pServiceDataPages.GetCount() == 0 )
//		bShow = FALSE;
//	pCmdUI->Enable( ! m_bRequestingService && bShow );
//}

//void CWebServices::OnShareMonkeyCompare()
//{
//	POSITION pos = m_pServiceDataPages.GetHeadPosition();
//	CShareMonkeyData* pData = NULL;
//
//	// ToDo: Change m_pServiceDataPages to CMap. Now it's stupid
//	for ( INT_PTR nPage = 0 ; nPage <= m_nCurrentPage ; nPage++ )
//	{
//		pData = static_cast< CShareMonkeyData* >( m_pServiceDataPages.GetNext( pos ) );
//	}
//
//	if ( pData && pData->m_sComparisonURL.GetLength() )
//		ShellExecute( GetSafeHwnd(), _T("open"), pData->m_sComparisonURL, NULL, NULL, SW_SHOWNORMAL );
//}

//void CWebServices::OnUpdateShareMonkeyBuy(CCmdUI* pCmdUI)
//{
//	pCmdUI->Enable( ! m_bServiceFailed && ! m_bRequestingService && m_pServiceDataPages.GetCount() > 0 );
//}

//void CWebServices::OnShareMonkeyBuy()
//{
//	POSITION pos = m_pServiceDataPages.GetHeadPosition();
//	CShareMonkeyData* pData = NULL;
//
//	// ToDo: Change m_pServiceDataPages to CMap. Now it's stupid
//	for ( INT_PTR nPage = 0 ; nPage <= m_nCurrentPage ; nPage++ )
//	{
//		pData = static_cast< CShareMonkeyData* >( m_pServiceDataPages.GetNext( pos ) );
//	}
//
//	if ( pData && pData->m_sBuyURL.GetLength() )
//		ShellExecute( GetSafeHwnd(), _T("open"), pData->m_sBuyURL, NULL, NULL, SW_SHOWNORMAL );
//}


//////////////////////////////////////////////////////////////////////
// CWebServices show Bitzi ticket	(Format URL)
//
// Note: Moved from CFileExecutor

BOOL CWebServices::ShowBitziTicket(DWORD nIndex)
{
	if ( ! Settings.WebServices.BitziOkay )
	{
		if ( AfxMessageBox( LoadString( IDS_LIBRARY_BITZI_MESSAGE ), MB_ICONQUESTION|MB_YESNO ) != IDYES )
			return FALSE;
		Settings.WebServices.BitziOkay = TRUE;
		Settings.Save();
	}

	CSingleLock pLock( &Library.m_pSection, TRUE );

	CLibraryFile* pFile = Library.LookupFile( nIndex );
	if ( pFile == NULL ) return FALSE;

	if ( ! pFile->m_oSHA1 || ! pFile->m_oTiger || ! pFile->m_oED2K )
	{
		CString strMessage;
		strMessage.Format( LoadString( IDS_LIBRARY_BITZI_HASHED ), (LPCTSTR)pFile->m_sName );
		pLock.Unlock();
		AfxMessageBox( strMessage, MB_ICONINFORMATION );
		return FALSE;
	}

	CString strURL = Settings.WebServices.BitziWebView;
	CString str;
	CFile hFile;

	if ( hFile.Open( pFile->GetPath(), CFile::modeRead|CFile::shareDenyNone ) )
	{
		strURL = Settings.WebServices.BitziWebSubmit;

		if ( hFile.GetLength() > 0 )
		{
			static LPCTSTR pszHex = _T("0123456789ABCDEF");
			BYTE nBuffer[20];
			int nPeek = hFile.Read( nBuffer, 20 );
			hFile.Close();

			for ( int nByte = 0 ; nByte < nPeek ; nByte++ )
			{
				str += pszHex[ (BYTE)nBuffer[ nByte ] >> 4 ];
				str += pszHex[ (BYTE)nBuffer[ nByte ] & 15 ];
			}

			strURL.Replace( _T("(FIRST20)"), str );
		}
		else
			strURL.Replace( _T("(FIRST20)"), _T("0") );
	}
	else
		strURL.Replace( _T("(URN)"), pFile->m_oSHA1.toString() + _T(".") + pFile->m_oTiger.toString() );

	CString strName = pFile->m_sName;
	LPCTSTR pszExt = _tcsrchr( strName, '.' );
	int nExtLen = pszExt ? static_cast< int >( _tcslen( pszExt ) - 1 ) : 0;
	const CString strExt = strName.Right( nExtLen ).Trim().MakeUpper();

	strURL.Replace( _T("(NAME)"), URLEncode( strName ) );
	strURL.Replace( _T("(SHA1)"), pFile->m_oSHA1.toString() );
	strURL.Replace( _T("(TTH)"), pFile->m_oTiger.toString() );
	strURL.Replace( _T("(ED2K)"), pFile->m_oED2K.toString() );
	strURL.Replace( _T("(AGENT)"), URLEncode( Settings.SmartAgent() ) );

	str.Format( _T("%I64i"), pFile->GetSize() );
	strURL.Replace( _T("(SIZE)"), str );

	CString strINFO = _T("&tag.tiger.tree=") + pFile->m_oTiger.toString();
	if ( pFile->m_oMD5 )
		strINFO += _T("&tag.md5.md5=") + pFile->m_oMD5.toString();
	if ( ! pFile->m_sComments.Trim().IsEmpty() )
		strINFO += _T("&tag.subjective.comment=") + URLEncode( pFile->m_sComments );

//	BOOL bAudioSchema = FALSE, bVideoSchema = FALSE, bImageSchema = FALSE, bAppSchema = FALSE;
//	if ( pFile->m_pSchema->CheckURI( CSchema::uriAudio ) )
//		bAudioSchema = TRUE;
//	else if ( pFile->m_pSchema->CheckURI( CSchema::uriVideo ) )
//		bVideoSchema = TRUE;
//	else if ( pFile->m_pSchema->CheckURI( CSchema::uriImage ) )
//		bImageSchema = TRUE;
//	else if ( pFile->m_pSchema->CheckURI( CSchema::uriApplication ) )
//		bAppSchema = TRUE;

	if ( pFile->m_pMetadata != NULL && pFile->m_pSchema != NULL )
	{
		CXMLElement* pMetadata = pFile->m_pMetadata;
		int nTemp, nAudioTag = 0, nImageTag = 0;
		CString strDescription, strAudioTag, strImageTag;

		for ( POSITION pos = pMetadata->GetAttributeIterator() ; pos ; )
		{
			CString strReplace;
			CXMLNode* pNode = pMetadata->GetNextAttribute( pos );
			str = pNode->GetName();
			strReplace = pNode->GetValue();
			str.MakeLower();

			if ( str == _T("link") )
			{
				strINFO += _T("&tag.url.url=") + URLEncode( strReplace );
			}
			else if ( str == _T("description") )
			{
				strDescription = URLEncode( strReplace.Trim() );	// _T("&tag.objective.description=") +
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriAudio ) )
			{
				if ( str == _T("title") )
					strINFO += _T("&tag.audiotrack.title=") + URLEncode( strReplace.Trim() );
				else if ( str == _T("artist") )
					strINFO += _T("&tag.audiotrack.artist=") + URLEncode( strReplace.Trim() );
				else if ( str == _T("album") )
					strINFO += _T("&tag.audiotrack.album=") + URLEncode( strReplace.Trim() );
				else if ( str == _T("track") )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.audiotrack.tracknumber=") + strReplace;
				}
				else if ( str == _T("year") )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.audiotrack.year=") + strReplace;
				}
				else if ( strExt == _T("MP3") || strExt == _T("OGG") || strExt == _T("WAV") )
				{
					// ToDo: Read WAV information in FileExecutor.cpp, bitzi submit is already ready
					if ( str == _T("bitrate") )
					{
						if ( strExt == _T("MP3") )
						{
							strAudioTag += _T("&tag.mp3.vbr=");

							if( _tcsstr( strReplace, _T("~") ) )
								strAudioTag += _T("y");
							else
								strAudioTag += _T("n");
						}

						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == _T("MP3") )
							strAudioTag += _T("&tag.mp3.bitrate=");
						else if ( strExt == _T("OGG") )
							strAudioTag += _T("&tag.vorbis.bitrate=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strAudioTag += strReplace;
							nAudioTag++;
						}
					}
					// ToDo: Read sampleSize of WAV in FileExecutor.cpp, bitzi submit is already ready
					else if ( str == _T("sampleSize") )
					{
						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == _T("WAV") )
						{
							strAudioTag += _T("&tag.wav.samplesize=") + strReplace;
							nAudioTag++;
						}
					}
					else if ( str == _T("seconds") )
					{
						nTemp = (int)( _wtof( strReplace ) * 1000 );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == _T("MP3") )
							strAudioTag += _T("&tag.mp3.duration=");
						else if ( strExt == _T("OGG") )
							strAudioTag += _T("&tag.vorbis.duration=");
						else if ( strExt == _T("WAV") )
							strAudioTag += _T("&tag.wav.duration=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strAudioTag += strReplace;
							nAudioTag++;
						}
					}
					else if ( str == _T("sampleRate") )
					{
						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == _T("MP3") )
							strAudioTag += _T("&tag.mp3.samplerate=");
						else if ( strExt == _T("OGG") )
							strAudioTag += _T("&tag.vorbis.samplerate=");
						else if ( strExt == _T("WAV") )
							strAudioTag += _T("&tag.wav.samplerate=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strAudioTag += strReplace;
							nAudioTag++;
						}
					}
					else if ( str == _T("channels") )
					{
						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == _T("OGG") )
							strAudioTag += _T("&tag.vorbis.channels=");
						else if ( strExt == _T("WAV") )
							strAudioTag += _T("&tag.wav.channels=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strAudioTag += strReplace;
							nAudioTag++;
						}
					}
					else if ( str == _T("soundType") )
					{
						if ( strExt == _T("MP3") )
						{
							if ( ! strReplace.CompareNoCase( _T("Stereo") ) || ! strReplace.CompareNoCase( _T("Joint Stereo" ) ) || ! strReplace.CompareNoCase( _T("Dual Channel") ) )
								strAudioTag += _T("&tag.mp3.stereo=y");
							else if ( ! strReplace.CompareNoCase( _T("Single Channel") ) || ! strReplace.CompareNoCase( _T("Mono" ) ) )
								strAudioTag += _T("&tag.mp3.stereo=n");
							else
								strReplace.Empty();
						}

						if ( ! strReplace.IsEmpty() )
							nAudioTag++;
					}
					else if ( str == _T("encoder") )
					{
						if ( strExt == _T("MP3") )
							strAudioTag += _T("&tag.mp3.encoder=");
						else if ( strExt == _T("OGG") )
							strAudioTag += _T("&tag.vorbis.encoder=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
							strAudioTag += URLEncode( strReplace );
					}
				}
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriImage ) )
			{
				if ( str == "width" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strImageTag += _T("&tag.image.width=") + strReplace;
					nImageTag++;
				}
				else if ( str == _T("height") )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strImageTag += _T("&tag.image.height=") + strReplace;
					nImageTag++;
				}
				else if ( str == _T("colors") )
				{
					if ( strReplace == "2" ) strReplace = "1";
					else if ( strReplace == "16" ) strReplace = "4";
					else if ( strReplace == "256" || strReplace == "Greyscale" ) strReplace = "8";
					else if ( strReplace == "64K" ) strReplace = "16";
					else if ( strReplace == "16.7M" ) strReplace = "24";
					else strReplace = "";

					if ( ! strReplace.IsEmpty() )
					{
						strImageTag += _T("&tag.image.bpp=") + strReplace;
						nImageTag++;
					}
				}
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriVideo ) )
			{
				if ( str == _T("realdescription") )
				{
					strDescription = URLEncode( strReplace.Trim() );	// _T("&tag.objective.description=") +
				}
				else if ( str == "width" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.width=") + strReplace;
				}
				else if ( str == _T("height") )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.height=") + strReplace;
				}
				else if ( str == _T("frameRate") )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.fps=") + strReplace;
				}
				else if ( str == _T("minutes") )
				{
					nTemp = (int)( _wtof( strReplace ) * 60 * 1000 );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.duration=") + strReplace;
				}
				else if ( str == _T("bitrate") )
				{
					// ToDo: Read video's bitrate in FileExecutor.cpp, bitzi submit is already ready
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.bitrate=") + strReplace;
				}
				else if ( str == _T("codec") )
				{
					strReplace.MakeUpper();
					strINFO += _T("&tag.video.codec=") + URLEncode( strReplace );
				}
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriApplication ) )
			{
				if ( str == _T("filedescription") )
					strDescription = URLEncode( strReplace.Trim() );
				else if ( str == _T("title") && strDescription.IsEmpty() )
					strDescription = URLEncode( strReplace.Trim() );
			}
		} // end for pos

		if ( nAudioTag == 4 )
			strINFO += strAudioTag;
		else if ( nImageTag == 3 )
			strINFO += strImageTag;		// Handle extensions below

		if ( ! strDescription.IsEmpty() )
			strINFO += _T("&tag.objective.description=") + strDescription;
	} // end if

	// Video Extensions		// +Images	ToDo: Handle any other types?
	if ( ! strExt.IsEmpty() )
	{
		SwitchMap( FileExt )
		{
			FileExt[ L"AVI" ]	= 'a';
			FileExt[ L"DIVX" ]	= 'v';
			FileExt[ L"XVID" ]	= 'x';
			FileExt[ L"MKV" ]	= 'k';
			FileExt[ L"WEBM" ]	= 'e';
			FileExt[ L"HDMOV" ]	= 'q';
			FileExt[ L"MOV" ]	= 'q';
			FileExt[ L"QT" ] 	= 'q';
			FileExt[ L"MPG" ]	= 'm';
			FileExt[ L"MPEG" ]	= 'm';
			FileExt[ L"MPE" ]	= 'm';
			FileExt[ L"M1V" ]	= '1';
			FileExt[ L"MP2" ]	= '2';
			FileExt[ L"M2V" ]	= '2';
			FileExt[ L"M2TS" ]	= '2';
			FileExt[ L"MPV2" ]	= '2';
			FileExt[ L"MP4" ]	= '4';
			FileExt[ L"M4V" ]	= '4';
			FileExt[ L"WMV" ]	= 'w';
			FileExt[ L"WM" ] 	= 'w';
			FileExt[ L"ASF" ]	= 'w';
			FileExt[ L"RM" ] 	= 'r';
			FileExt[ L"RMVB" ]	= 'r';
			FileExt[ L"RAM" ]	= 'r';
			FileExt[ L"OGM" ]	= 'o';
			FileExt[ L"OGV" ]	= 'o';
			FileExt[ L"VOB" ]	= 'b';
			FileExt[ L"VP6" ]	= '6';
			FileExt[ L"3GP" ]	= 'g';
			FileExt[ L"IVF" ]	= 'i';
			FileExt[ L"FLV" ]	= 'f';
			FileExt[ L"SWF" ]	= 'f';

			FileExt[ L"PNG" ]	= 'P';
			FileExt[ L"GIF" ]	= 'P';
			FileExt[ L"BMP" ]	= 'P';
			FileExt[ L"PSD" ]	= 'P';
			FileExt[ L"TIF" ]	= 'P';
			FileExt[ L"ICO" ]	= 'P';
			FileExt[ L"WEBP" ]	= 'P';
			FileExt[ L"SVG" ]	= 'P';
			FileExt[ L"JPG" ]	= 'J';
			FileExt[ L"JPEG" ]	= 'J';
			FileExt[ L"JPE" ]	= 'J';
			FileExt[ L"JFIF" ]	= 'J';

			FileExt[ L"MP3" ]	= 'X';
			FileExt[ L"AAC" ]	= 'X';
			FileExt[ L"FLAC" ]	= 'X';
			FileExt[ L"PNG" ]	= 'X';
			FileExt[ L"GIF" ]	= 'X';
			FileExt[ L"JPG" ]	= 'X';
			FileExt[ L"PDF" ]	= 'X';
			FileExt[ L"ZIP" ]	= 'X';
			FileExt[ L"RAR" ]	= 'X';
			FileExt[ L"EXE" ]	= 'X';
		}

		switch( FileExt[ strExt ] )
		{
		case 'a':		// avi
			strINFO += _T("&tag.video.format=AVI");
			break;
		case 'k':		// mkv
			strINFO += _T("&tag.video.format=Matroska");
			break;
		case 'q':		// mov hdmov qt
			strINFO += _T("&tag.video.format=QuickTime");
			break;
		case 'r':		// rm rmvb rv ram rpm?
			strINFO += _T("&tag.video.format=Real");
			break;
		case 'm':		// mpg mpeg mpe
			strINFO += _T("&tag.video.format=MPEG");
			break;
		case '1':		// m1v
			strINFO += _T("&tag.video.format=MPEG-1");
			break;
		case '2':		// mp2 m2p mpv2 m2ts
			strINFO += _T("&tag.video.format=MPEG-2");
			break;
		case '4':		// mp4 m4v
			strINFO += _T("&tag.video.format=MPEG-4");
			break;
		case 'v':		// divx (div/tix?)
			strINFO += _T("&tag.video.format=DivX");
			break;
		case 'x':		// xvid
			strINFO += _T("&tag.video.format=XviD");
			break;
		case 'w':		// wmv wm wmd? asf
			strINFO += _T("&tag.video.format=") + URLEncode( L"Windows Media");
			break;
		case 'o':		// ogm ogv
			strINFO += _T("&tag.video.format=") + URLEncode( L"Ogg Media File");
			break;
		case 'g':		// 3gp
			strINFO += _T("&tag.video.format=3GP");
			break;
		case '6':		// vp6
			strINFO += _T("&tag.video.format=VP6");
			break;
		case 'b':		// vob
			strINFO += _T("&tag.video.format=DVD");
			break;
		case 'i':		// ivf
			strINFO += _T("&tag.video.format=Indeo");
			break;
		case 'f':		// swf flv
			strINFO += _T("&tag.video.format=Flash");
			break;
		case 'e':		// webm
			strINFO += _T("&tag.video.format=WebM");
			break;

		case 'P':		// png gif bmp psd tif webp ico svg
			strINFO += _T("&tag.image.format=") + strExt;
			break;
		case 'J':		// jpg jpeg jpe jfif
			strINFO += _T("&tag.image.format=JPEG");
			break;

		case 'X':		// Skip common non-video types
			break;

		default:		// Unknown extension, try video schema
			if ( pFile->m_pSchema != NULL && pFile->m_pSchema->CheckURI( CSchema::uriVideo ) )
				strINFO += _T("&tag.video.format=") + URLEncode( strExt );
		}
	}

	pLock.Unlock();

	strURL.Replace( _T("&(INFO)"), strINFO );

	ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL );

	return TRUE;
}
