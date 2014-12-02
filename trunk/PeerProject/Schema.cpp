//
// Schema.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "Schema.h"
#include "SchemaMember.h"
#include "SchemaChild.h"
#include "ShellIcons.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CSchema construction

CSchema::CSchema()
	: m_nType		( stFile )
	, m_nAvailability ( saDefault )
	, m_bPrivate	( FALSE )
	, m_nIcon16 	( -1 )
	, m_nIcon32 	( -1 )
	, m_nIcon48 	( -1 )
{
	m_pTypeFilters.InitHashTable( 127 );
}

CSchema::~CSchema()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CSchema member access

BOOL CSchema::FilterType(LPCTSTR pszFile) const
{
	if ( m_pTypeFilters.IsEmpty() )
		return FALSE;

	LPCTSTR pszExt = PathFindExtension( pszFile );
	if ( ! *pszExt )
		return FALSE;

	const CStringBoolMap::CPair* pPair = m_pTypeFilters.PLookup( CString( pszExt + 1 ).MakeLower() );

	return pPair && pPair->value;
}

CString CSchema::GetFilterSet() const
{
	CString strFilters = L"|";
	for ( POSITION pos = m_pTypeFilters.GetStartPosition() ; pos ; )
	{
		CString strType;
		BOOL bResult;
		m_pTypeFilters.GetNextAssoc( pos, strType, bResult );
		if ( bResult )
		{
			strFilters += strType;
			strFilters += L'|';
		}
	}
	return strFilters;
}

POSITION CSchema::GetFilterIterator() const
{
	return m_pTypeFilters.GetStartPosition();
}

void CSchema::GetNextFilter(POSITION& pos, CString& sType, BOOL& bResult) const
{
	m_pTypeFilters.GetNextAssoc( pos, sType, bResult );
}

POSITION CSchema::GetMemberIterator() const
{
	return m_pMembers.GetHeadPosition();
}

CSchemaMember* CSchema::GetNextMember(POSITION& pos) const
{
	return m_pMembers.GetNext( pos );
}

CSchemaMember* CSchema::GetMember(LPCTSTR pszName) const
{
	if ( ! pszName || ! *pszName ) return NULL;

	for ( POSITION pos = GetMemberIterator() ; pos ; )
	{
		CSchemaMember* pMember = GetNextMember( pos );
		if ( pMember->m_sName.CompareNoCase( pszName ) == 0 )
			return pMember;
	}
	return NULL;
}

INT_PTR CSchema::GetMemberCount() const
{
	return m_pMembers.GetCount();
}

CString CSchema::GetFirstMemberName() const
{
	if ( m_pMembers.GetCount() )
	{
		CSchemaMember* pMember = m_pMembers.GetHead();
		return pMember->m_sName;
	}

	CString str( L"title" );
	return str;
}

//////////////////////////////////////////////////////////////////////
// CSchema clear

void CSchema::Clear()
{
	for ( POSITION pos = GetMemberIterator() ; pos ; )
	{
		delete GetNextMember( pos );
	}

	for ( POSITION pos = m_pContains.GetHeadPosition() ; pos ; )
	{
		delete m_pContains.GetNext( pos );
	}

	for ( POSITION pos = m_pBitprintsMap.GetHeadPosition() ; pos ; )
	{
		delete m_pBitprintsMap.GetNext( pos );
	}

	m_pMembers.RemoveAll();
	m_pContains.RemoveAll();
	m_pBitprintsMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CSchema Load

BOOL CSchema::Load(LPCTSTR pszFile)
{
	CString strFile( pszFile );
	PathRemoveExtension( strFile.GetBuffer() );
	strFile.ReleaseBuffer();

	if ( ! LoadSchema( strFile + L".xsd" ) ) return FALSE;

	m_sIcon = strFile + L".ico";

	LoadDescriptor( strFile + L".xml" );

	// LoadIcon() causes bad registry reads (?)
	// CCoolInterface::IsNewWindows() caused several reapeat ones.(?)
	if ( theApp.m_bIsWin2000 || ! LoadIcon() )
	{
		m_sIcon = m_sIcon.Left( m_sIcon.GetLength() - 4 );
		m_sIcon += L".Safe.ico";
		LoadIcon();
	}

	if ( m_sTitle.IsEmpty() )
	{
		m_sTitle = m_sSingular;
		m_sTitle.SetAt( 0, TCHAR( toupper( m_sTitle.GetAt( 0 ) ) ) );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSchema Load Schema

BOOL CSchema::LoadSchema(LPCTSTR pszFile)
{
	CXMLElement* pRoot = CXMLElement::FromFile( pszFile );
	if ( pRoot == NULL ) return FALSE;

	BOOL bResult = FALSE;

	m_sURI = pRoot->GetAttributeValue( L"targetNamespace", L"" );

	CXMLElement* pPlural = pRoot->GetElementByName( L"element" );

	if ( pPlural && ! m_sURI.IsEmpty() )
	{
		m_sPlural = pPlural->GetAttributeValue( L"name" );

		CXMLElement* pComplexType = pPlural->GetFirstElement();

		if ( pComplexType && pComplexType->IsNamed( L"complexType" ) && ! m_sPlural.IsEmpty() )
		{
			CXMLElement* pElement = pComplexType->GetFirstElement();

			if ( pElement && pElement->IsNamed( L"element" ) )
			{
				m_sSingular = pElement->GetAttributeValue( L"name" );

				if ( pElement->GetElementCount() )
				{
					bResult = LoadPrimary( pRoot, pElement->GetFirstElement() );
				}
				else
				{
					CString strType = pElement->GetAttributeValue( L"type" );
					bResult = LoadPrimary( pRoot, GetType( pRoot, strType ) );
				}

				if ( m_sSingular.IsEmpty() )
					bResult = FALSE;
			}
		}
	}

	if ( CXMLElement* pMapping = pRoot->GetElementByName( L"mapping" ) )
	{
		for ( POSITION pos = pMapping->GetElementIterator() ; pos ; )
		{
			CXMLElement* pNetwork = pMapping->GetNextElement( pos );
			if ( pNetwork )
			{
				BOOL bFound = pNetwork->IsNamed( L"network" );

				CString strName = pNetwork->GetAttributeValue( L"name" );
				if ( ! bFound || strName != L"ed2k" )
					continue;

				m_sDonkeyType = pNetwork->GetAttributeValue( L"value" );
				break;
			}
		}
	}

	// ToDo: External documents
	//CString strImported;
	//if ( CXMLElement* pImported = pRoot->GetElementByName( L"import" ) )
	//	strImported = pImported->GetAttributeValue( L"schemaLocation" );
	//else if ( CXMLElement* pIncluded = pRoot->GetElementByName( L"include" ) )
	//	strImported = pIncluded->GetAttributeValue( L"schemaDescriptor" );
	//
	//if ( strImported.GetLength() > 5 && strImported.Find( L".xsd" ) > 1 )
	//{
	//	CString strFile( pszFile );
	//	strFile = strFile.Left( strFile.ReverseFind( L'\\' ) + 1 ) + strImported;
	//	LoadSchema( (LPCTSTR)strFile );
	//}

	delete pRoot;

	return bResult;
}

BOOL CSchema::LoadPrimary(CXMLElement* pRoot, CXMLElement* pType)
{
	if ( ! pRoot || ! pType ) return FALSE;

	if ( ! pType->IsNamed( L"complexType" ) &&
		 ! pType->IsNamed( L"all" ) ) return FALSE;

	for ( POSITION pos = pType->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement	= pType->GetNextElement( pos );
		CString strElement		= pElement->GetName();

		if ( strElement.CompareNoCase( L"attribute" ) == 0 ||
			 strElement.CompareNoCase( L"element" ) == 0 )
		{
			CSchemaMember* pMember = new CSchemaMember( this );

			if ( pMember->LoadSchema( pRoot, pElement ) )
			{
				m_pMembers.AddTail( pMember );
			}
			else
			{
				delete pMember;
				return FALSE;
			}
		}
		else if ( strElement.CompareNoCase( L"all" ) == 0 )
		{
			if ( ! LoadPrimary( pRoot, pElement ) ) return FALSE;
		}
	}

	return TRUE;
}

CXMLElement* CSchema::GetType(CXMLElement* pRoot, LPCTSTR pszName) const
{
	if ( ! pszName || ! *pszName ) return NULL;

	for ( POSITION pos = pRoot->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = pRoot->GetNextElement( pos );

		CString strElement = pElement->GetName();

		if ( strElement.CompareNoCase( L"simpleType" ) == 0 ||
			 strElement.CompareNoCase( L"complexType" ) == 0 )
		{
			if ( pElement->GetAttributeValue( L"name", L"?" ).CompareNoCase( pszName ) == 0 )
				return pElement;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CSchema Load Descriptor

BOOL CSchema::LoadDescriptor(LPCTSTR pszFile)
{
	CXMLElement* pRoot = CXMLElement::FromFile( pszFile );
	if ( NULL == pRoot ) return FALSE;

	if ( ! CheckURI( pRoot->GetAttributeValue( L"location" ) ) ||
		 ! pRoot->IsNamed( L"schemaDescriptor" ) )
	{
		delete pRoot;
		return FALSE;
	}

	for ( POSITION pos = pRoot->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = pRoot->GetNextElement( pos );

		if ( pElement->IsNamed( L"object" ) )
		{
			CString strType = pElement->GetAttributeValue( L"type" );
			ToLower( strType );

			if ( strType == L"file" )
				m_nType = stFile;
			else if ( strType == L"folder" || strType == L"album" )
				m_nType = stFolder;

			strType = pElement->GetAttributeValue( L"availability" );
			ToLower( strType );

			if ( strType == L"system" )
				m_nAvailability = saSystem;
			else if ( strType == L"advanced" )
				m_nAvailability = saAdvanced;
			else // "default"
				m_nAvailability = saDefault;

			if ( pElement->GetAttribute( L"private" ) )
				m_bPrivate = TRUE;
		}
		else if ( pElement->IsNamed( L"titles" ) )
		{
			LoadDescriptorTitles( pElement );
		}
		else if ( pElement->IsNamed( L"members" ) )
		{
			LoadDescriptorMembers( pElement );
		}
		else if ( pElement->IsNamed( L"extends" ) )
		{
			LoadDescriptorExtends( pElement );
		}
		else if ( pElement->IsNamed( L"contains" ) )
		{
			LoadDescriptorContains( pElement );
		}
		else if ( pElement->IsNamed( L"headerContent" ) )
		{
			LoadDescriptorHeaderContent( pElement );
		}
		else if ( pElement->IsNamed( L"viewContent" ) )
		{
			LoadDescriptorViewContent( pElement );
		}
		else if ( pElement->IsNamed( L"typeFilter" ) )
		{
			LoadDescriptorTypeFilter( pElement );
		}
		else if ( pElement->IsNamed( L"BitprintsImport" ) )
		{
			LoadDescriptorBitprintsImport( pElement );
		}
		else if ( pElement->IsNamed( L"images" ) )
		{
			LoadDescriptorIcons( pElement );
		}
		// ToDo: Add this to schemas for ed2k?
		//else if ( pElement->IsNamed( L"donkeyType" ) )
		//{
		//	LoadDescriptorDonkeyType( pElement );
		//}
	}

	delete pRoot;

	return TRUE;
}

void CSchema::LoadDescriptorTitles(CXMLElement* pElement)
{
	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pTitle = pElement->GetNextElement( pos );

		if ( pTitle->IsNamed( L"title" ) )
		{
			if ( pTitle->GetAttributeValue( L"language" ).CompareNoCase( Settings.General.Language ) == 0 )
			{
				m_sTitle = pTitle->GetValue();
				break;
			}
			else if ( m_sTitle.IsEmpty() )
			{
				m_sTitle = pTitle->GetValue();
			}
		}
	}
}

void CSchema::LoadDescriptorIcons(CXMLElement* pElement)
{
	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pIcon = pElement->GetNextElement( pos );

		if ( pIcon->IsNamed( L"icon" ) )
		{
			int nSlash = m_sIcon.ReverseFind( L'\\' );
			if ( nSlash >= 0 ) m_sIcon = m_sIcon.Left( nSlash + 1 );
			m_sIcon += pIcon->GetAttributeValue( L"path" );
		}
	}
}

void CSchema::LoadDescriptorMembers(CXMLElement* pElement)
{
	BOOL bPrompt = FALSE;

	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pDisplay = pElement->GetNextElement( pos );

		if ( pDisplay->IsNamed( L"member" ) )
		{
			CString strMember = pDisplay->GetAttributeValue( L"name" );

			if ( CSchemaMember* pMember = GetMember( strMember ) )
			{
				pMember->LoadDescriptor( pDisplay );
				bPrompt |= pMember->m_bPrompt;
			}
		}
	}

	if ( bPrompt ) return;

	for ( POSITION pos = GetMemberIterator() ; pos ; )
	{
		GetNextMember( pos )->m_bPrompt = TRUE;
	}
}

void CSchema::LoadDescriptorExtends(CXMLElement* pElement)
{
	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pExtend = pElement->GetNextElement( pos );

		if ( pExtend->IsNamed( L"schema" ) )
		{
			CString strURI = pExtend->GetAttributeValue( L"location" );
			if ( ! strURI.IsEmpty() )
				m_pExtends.AddTail( strURI );
		}
	}
}

void CSchema::LoadDescriptorContains(CXMLElement* pElement)
{
	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pExtend = pElement->GetNextElement( pos );

		if ( pExtend->IsNamed( L"object" ) )
		{
			CSchemaChild* pChild = new CSchemaChild( this );

			if ( pChild->Load( pExtend ) )
				m_pContains.AddTail( pChild );
			else
				delete pChild;
		}
	}
}

void CSchema::LoadDescriptorTypeFilter(CXMLElement* pElement)
{
	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pType = pElement->GetNextElement( pos );

		if ( pType->GetName().CompareNoCase( L"type" ) == 0 )
		{
			CString strType = pType->GetAttributeValue( L"extension", L"" );
			if ( strType.IsEmpty() )
				strType = pType->GetAttributeValue( L"keyword", L"" );

			BOOL bResult = TRUE;
			m_pTypeFilters.SetAt( strType.MakeLower(), bResult );
		}
	}
}

void CSchema::LoadDescriptorBitprintsImport(CXMLElement* pElement)
{
	m_sBitprintsTest = pElement->GetAttributeValue( L"testExists", NULL );

	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pBitprints = pElement->GetNextElement( pos );

		if ( pBitprints->GetName().CompareNoCase( L"mapping" ) == 0 )
		{
			CSchemaBitprints* pMap = new CSchemaBitprints();
			pMap->Load( pBitprints );
			m_pBitprintsMap.AddTail( pMap );
		}
	}
}

void CSchema::LoadDescriptorHeaderContent(CXMLElement* pElement)
{
	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pElement->GetNextElement( pos );

		BOOL bLanguage = pXML->GetAttributeValue( L"language" ).
			CompareNoCase( Settings.General.Language ) == 0;

		if ( pXML->IsNamed( L"title" ) )
		{
			if ( bLanguage || m_sHeaderTitle.IsEmpty() )
				m_sHeaderTitle = pXML->GetValue().Trim();
		}
		else if ( pXML->IsNamed( L"subtitle" ) )
		{
			if ( bLanguage || m_sHeaderSubtitle.IsEmpty() )
				m_sHeaderSubtitle = pXML->GetValue().Trim();
		}
	}
}

void CSchema::LoadDescriptorViewContent(CXMLElement* pElement)
{
	m_sLibraryView = pElement->GetAttributeValue( L"preferredView" );

	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pElement->GetNextElement( pos );

		BOOL bLanguage = pXML->GetAttributeValue( L"language" ).
			CompareNoCase( Settings.General.Language ) == 0;

		if ( pXML->IsNamed( L"tileLine1" ) )
		{
			if ( bLanguage || m_sTileLine1.IsEmpty() )
				m_sTileLine1 = pXML->GetValue().Trim();
		}
		else if ( pXML->IsNamed( L"tileLine2" ) )
		{
			if ( bLanguage || m_sTileLine2.IsEmpty() )
				m_sTileLine2 = pXML->GetValue().Trim();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CSchema load icon

BOOL CSchema::LoadIcon()
{
	HICON hIcon16 = NULL, hIcon32 = NULL, hIcon48 = NULL;

	::LoadIcon( m_sIcon, &hIcon16, &hIcon32, &hIcon48 );

	if ( hIcon16 )
	{
		if ( Settings.General.LanguageRTL ) hIcon16 = CreateMirroredIcon( hIcon16 );
		m_nIcon16 = ShellIcons.Add( hIcon16, 16 );
		DestroyIcon( hIcon16 );
	}
	if ( hIcon32 )
	{
		if ( Settings.General.LanguageRTL ) hIcon32 = CreateMirroredIcon( hIcon32 );
		m_nIcon32 = ShellIcons.Add( hIcon32, 32 );
		DestroyIcon( hIcon32 );
	}
	if ( hIcon48 )
	{
		if ( Settings.General.LanguageRTL ) hIcon48 = CreateMirroredIcon( hIcon48 );
		m_nIcon48 = ShellIcons.Add( hIcon48, 48 );
		DestroyIcon( hIcon48 );
	}

	return hIcon16 || hIcon32 || hIcon48;
}

//////////////////////////////////////////////////////////////////////
// CSchema contained object helpers

CSchemaChild* CSchema::GetContained(LPCTSTR pszURI) const
{
	for ( POSITION pos = m_pContains.GetHeadPosition() ; pos ; )
	{
		CSchemaChild* pChild = m_pContains.GetNext( pos );
		if ( pChild->m_sURI.CompareNoCase( pszURI ) == 0 ) return pChild;
	}
	return NULL;
}

CString CSchema::GetContainedURI(int nType) const
{
	for ( POSITION pos = m_pContains.GetHeadPosition() ; pos ; )
	{
		CSchemaChild* pChild = m_pContains.GetNext( pos );

		if ( pChild->m_nType == nType ) return pChild->m_sURI;
	}

	CString strURI;
	return strURI;
}

//////////////////////////////////////////////////////////////////////
// CSchema instantiate

CXMLElement* CSchema::Instantiate(BOOL bNamespace) const
{
	CXMLElement* pElement = new CXMLElement( NULL, m_sPlural );
	pElement->AddAttribute( CXMLAttribute::schemaName, m_sURI );
	if ( bNamespace ) pElement->AddAttribute( L"xmlns:xsi", CXMLAttribute::xmlnsInstance );
	return pElement;
}

//////////////////////////////////////////////////////////////////////
// CSchema validate instance

BOOL CSchema::Validate(CXMLElement* pXML, BOOL bFix) const
{
	if ( pXML == NULL ) return FALSE;

	if ( ! pXML->IsNamed( m_sPlural ) ) return FALSE;
	if ( ! CheckURI( pXML->GetAttributeValue( CXMLAttribute::schemaName ) ) ) return FALSE;

	CXMLElement* pBody = pXML->GetFirstElement();
	if ( pBody == NULL ) return FALSE;
	if ( ! pBody->IsNamed( m_sSingular ) ) return FALSE;

	for ( POSITION pos = GetMemberIterator() ; pos ; )
	{
		CSchemaMember* pMember = GetNextMember( pos );

		CString str = pMember->GetValueFrom( pBody, L"(~np~)", FALSE );
		if ( str == L"(~np~)" ) continue;

		if ( pMember->m_bNumeric )
		{
			float nNumber = 0.0f;
			bool bValid = true;

			if ( str.GetLength() && _stscanf( str, L"%f", &nNumber ) != 1 )
				bValid = false;
			if ( nNumber < pMember->m_nMinOccurs || nNumber > pMember->m_nMaxOccurs )
				bValid = false;
			if ( ! bValid )
			{
				if ( ! bFix ) return FALSE;
				pMember->SetValueTo( pBody, L"" );
			}
		}
		else if ( pMember->m_bYear )
		{
			int nYear = 0;
			if ( _stscanf( str, L"%i", &nYear ) != 1 || nYear < 1000 || nYear > 9999 )
			{
				if ( ! bFix ) return FALSE;
				pMember->SetValueTo( pBody, L"" );
			}
		}
		else if ( pMember->m_bGUID )
		{
			Hashes::Guid tmp;
			if ( !(Hashes::fromGuid( str, &tmp[ 0 ] ) && tmp.validate() ) )
			{
				if ( ! bFix ) return FALSE;
				pMember->SetValueTo( pBody, L"" );
			}
		}
		else if ( pMember->m_nMaxLength > 0 )
		{
			if ( str.GetLength() > pMember->m_nMaxLength )
			{
				if ( ! bFix ) return FALSE;

				str = str.Left( pMember->m_nMaxLength );
				pMember->SetValueTo( pBody, str );
			}
		}
		else if ( pMember->m_bBoolean )
		{
			if ( str == L"1" || str.CompareNoCase( L"true" ) == 0 )
				str = L"true";
			else if ( str == L"0" || str.CompareNoCase( L"false" ) == 0 )
				str = L"false";
			else if ( ! bFix ) return FALSE;
			pMember->SetValueTo( pBody, L"" );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSchema indexed words

CString CSchema::GetIndexedWords(CXMLElement* pXML) const
{
	CString str;

	if ( pXML == NULL ) return str;

	for ( POSITION pos = GetMemberIterator() ; pos ; )
	{
		CSchemaMember* pMember = GetNextMember( pos );

		if ( pMember->m_bIndexed )
		{
			CString strMember = pMember->GetValueFrom( pXML, NULL, FALSE );

			if ( ! strMember.IsEmpty() )
			{
				if ( str.IsEmpty() )
					str += strMember;
				else
					str += L' ' + strMember;
			}
		}
	}
	ToLower( str );
	return str;
}

CString CSchema::GetVisibleWords(CXMLElement* pXML) const
{
	CString str;

	if ( pXML == NULL ) return str;

	for ( POSITION pos = GetMemberIterator() ; pos ; )
	{
		CSchemaMember* pMember = GetNextMember( pos );

		if ( ! pMember->m_bHidden )
		{
			CString strMember = pMember->GetValueFrom( pXML, NULL, FALSE );

			if ( ! strMember.IsEmpty() )
			{
				if ( ! str.IsEmpty() ) str += ' ';
				str += strMember;
			}
		}
	}
	return str;
}

//////////////////////////////////////////////////////////////////////
// CSchema Member Resolution

void CSchema::ResolveTokens(CString& str, CXMLElement* pXML) const
{
	for ( ;; )
	{
		int nOpen = str.Find( L'{' );
		if ( nOpen < 0 ) break;
		int nClose = str.Find( L'}' );
		if ( nClose <= nOpen ) break;

		CString strMember = str.Mid( nOpen + 1, nClose - nOpen - 1 ).Trim();

		CString strValue;
		if ( CSchemaMember* pMember = GetMember( strMember ) )
			strValue = pMember->GetValueFrom( pXML, NULL, TRUE );

		str = str.Left( nOpen ) + strValue + str.Mid( nClose + 1 );
	}
}

//////////////////////////////////////////////////////////////////////
// CSchemaBitprints Bitprints Map

BOOL CSchemaBitprints::Load(CXMLElement* pXML)
{
	m_sFrom	= pXML->GetAttributeValue( L"from", NULL );
	m_sTo	= pXML->GetAttributeValue( L"to", NULL );

	CString strFactor = pXML->GetAttributeValue( L"factor", NULL );

	if ( strFactor.IsEmpty() || _stscanf( strFactor, L"%lf", &m_nFactor ) != 1 )
		m_nFactor = 0;

	return ! m_sFrom.IsEmpty() && ! m_sTo.IsEmpty();
}

//////////////////////////////////////////////////////////////////////
// CSchema Common Schema URIs

LPCTSTR CSchema::uriApplication 			= L"http://schemas.peerproject.org/Application.xsd";				// http://www.shareaza.com/schemas/application.xsd
LPCTSTR CSchema::uriArchive					= L"http://schemas.peerproject.org/Archive.xsd";					// http://www.shareaza.com/schemas/archive.xsd
LPCTSTR CSchema::uriAudio					= L"http://schemas.peerproject.org/Audio.xsd";					// http://www.limewire.com/schemas/audio.xsd
LPCTSTR CSchema::uriBook					= L"http://schemas.peerproject.org/Book.xsd";					// http://www.limewire.com/schemas/book.xsd
LPCTSTR CSchema::uriImage					= L"http://schemas.peerproject.org/Image.xsd";					// http://www.shareaza.com/schemas/image.xsd
LPCTSTR CSchema::uriVideo					= L"http://schemas.peerproject.org/Video.xsd";					// http://www.limewire.com/schemas/video.xsd
LPCTSTR CSchema::uriROM 					= L"http://schemas.peerproject.org/ROM.xsd";						// http://www.shareaza.com/schemas/rom.xsd
LPCTSTR CSchema::uriDocument				= L"http://schemas.peerproject.org/Document.xsd";				// http://www.shareaza.com/schemas/wordProcessing.xsd
LPCTSTR CSchema::uriSpreadsheet				= L"http://schemas.peerproject.org/Spreadsheet.xsd";				// http://www.shareaza.com/schemas/spreadsheet.xsd
LPCTSTR CSchema::uriPresentation			= L"http://schemas.peerproject.org/Presentation.xsd";			// http://www.shareaza.com/schemas/presentation.xsd
LPCTSTR CSchema::uriCollection				= L"http://schemas.peerproject.org/Collection.xsd";				// http://www.shareaza.com/schemas/collection.xsd

LPCTSTR CSchema::uriLibrary					= L"http://schemas.peerproject.org/LibraryRoot.xsd";				// http://www.shareaza.com/schemas/libraryRoot.xsd

LPCTSTR CSchema::uriFolder					= L"http://schemas.peerproject.org/Folder.xsd";					// http://www.shareaza.com/schemas/folder.xsd
LPCTSTR CSchema::uriCollectionsFolder		= L"http://schemas.peerproject.org/CollectionsFolder.xsd";		// http://www.shareaza.com/schemas/collectionsFolder.xsd
LPCTSTR CSchema::uriFavoritesFolder			= L"http://schemas.peerproject.org/FavoritesFolder.xsd";			// http://www.shareaza.com/schemas/favouritesFolder.xsd
LPCTSTR CSchema::uriSearchFolder			= L"http://schemas.peerproject.org/SearchFolder.xsd";			// http://www.shareaza.com/schemas/searchFolder.xsd

LPCTSTR CSchema::uriAllFiles				= L"http://schemas.peerproject.org/AllFiles.xsd";				// http://www.shareaza.com/schemas/allFiles.xsd

LPCTSTR CSchema::uriApplicationRoot			= L"http://schemas.peerproject.org/ApplicationRoot.xsd";			// http://www.shareaza.com/schemas/applicationRoot.xsd
LPCTSTR CSchema::uriApplicationAll			= L"http://schemas.peerproject.org/ApplicationAll.xsd";			// http://www.shareaza.com/schemas/applicationAll.xsd

LPCTSTR CSchema::uriArchiveRoot 			= L"http://schemas.peerproject.org/ArchiveRoot.xsd";				// http://www.shareaza.com/schemas/archiveRoot.xsd
LPCTSTR CSchema::uriArchiveAll				= L"http://schemas.peerproject.org/ArchiveAll.xsd";				// http://www.shareaza.com/schemas/archiveAll.xsd

LPCTSTR CSchema::uriBookRoot				= L"http://schemas.peerproject.org/BookRoot.xsd";				// http://www.shareaza.com/schemas/bookRoot.xsd
LPCTSTR CSchema::uriBookAll					= L"http://schemas.peerproject.org/BookAll.xsd";					// http://www.shareaza.com/schemas/bookAll.xsd

LPCTSTR CSchema::uriImageRoot				= L"http://schemas.peerproject.org/ImageRoot.xsd";				// http://www.shareaza.com/schemas/imageRoot.xsd
LPCTSTR CSchema::uriImageAll				= L"http://schemas.peerproject.org/ImageAll.xsd";				// http://www.shareaza.com/schemas/imageAll.xsd
LPCTSTR CSchema::uriImageAlbum				= L"http://schemas.peerproject.org/ImageAlbum.xsd";				// http://www.shareaza.com/schemas/imageAlbum.xsd

LPCTSTR CSchema::uriMusicRoot				= L"http://schemas.peerproject.org/MusicRoot.xsd";				// http://www.shareaza.com/schemas/musicRoot.xsd
LPCTSTR CSchema::uriMusicAll				= L"http://schemas.peerproject.org/MusicAll.xsd";				// http://www.shareaza.com/schemas/musicAll.xsd
LPCTSTR CSchema::uriMusicAlbum				= L"http://schemas.peerproject.org/MusicAlbum.xsd";				// http://www.shareaza.com/schemas/musicAlbum.xsd
LPCTSTR CSchema::uriMusicArtist				= L"http://schemas.peerproject.org/MusicArtist.xsd";				// http://www.shareaza.com/schemas/musicArtist.xsd
LPCTSTR CSchema::uriMusicGenre				= L"http://schemas.peerproject.org/MusicGenre.xsd";				// http://www.shareaza.com/schemas/musicGenre.xsd
LPCTSTR CSchema::uriMusicAlbumCollection	= L"http://schemas.peerproject.org/MusicAlbumCollection.xsd";	// http://www.shareaza.com/schemas/musicAlbumCollection.xsd
LPCTSTR CSchema::uriMusicArtistCollection	= L"http://schemas.peerproject.org/MusicArtistCollection.xsd";	// http://www.shareaza.com/schemas/musicArtistCollection.xsd
LPCTSTR CSchema::uriMusicGenreCollection	= L"http://schemas.peerproject.org/MusicGenreCollection.xsd";	// http://www.shareaza.com/schemas/musicGenreCollection.xsd

LPCTSTR CSchema::uriVideoRoot				= L"http://schemas.peerproject.org/VideoRoot.xsd";				// http://www.shareaza.com/schemas/videoRoot.xsd
LPCTSTR CSchema::uriVideoAll				= L"http://schemas.peerproject.org/VideoAll.xsd";				// http://www.shareaza.com/schemas/videoAll.xsd
LPCTSTR CSchema::uriVideoFilm				= L"http://schemas.peerproject.org/VideoFilm.xsd";				// http://www.shareaza.com/schemas/videoFilm.xsd
LPCTSTR CSchema::uriVideoSeries				= L"http://schemas.peerproject.org/VideoSeries.xsd";				// http://www.shareaza.com/schemas/videoSeries.xsd
LPCTSTR CSchema::uriVideoFilmCollection		= L"http://schemas.peerproject.org/VideoFilmCollection.xsd";		// http://www.shareaza.com/schemas/videoFilmCollection.xsd
LPCTSTR CSchema::uriVideoSeriesCollection	= L"http://schemas.peerproject.org/VideoSeriesCollection.xsd";	// http://www.shareaza.com/schemas/videoSeriesCollection.xsd
LPCTSTR CSchema::uriVideoMusicCollection	= L"http://schemas.peerproject.org/VideoMusicCollection.xsd";	// http://www.shareaza.com/schemas/videoMusicCollection.xsd

LPCTSTR CSchema::uriDocumentRoot			= L"http://schemas.peerproject.org/DocumentRoot.xsd";			// http://www.shareaza.com/schemas/documentRoot.xsd
LPCTSTR CSchema::uriDocumentAll				= L"http://schemas.peerproject.org/DocumentAll.xsd";				// http://www.shareaza.com/schemas/documentAll.xsd

LPCTSTR CSchema::uriUnknown					= L"http://schemas.peerproject.org/Unknown.xsd";
LPCTSTR CSchema::uriUnknownFolder			= L"http://schemas.peerproject.org/UnknownFolder.xsd";

LPCTSTR CSchema::uriGhostFolder				= L"http://schemas.peerproject.org/GhostFolder.xsd";				// http://www.shareaza.com/schemas/ghostFolder.xsd

LPCTSTR CSchema::uriBitTorrent				= L"http://schemas.peerproject.org/BitTorrent.xsd";				// http://www.shareaza.com/schemas/bittorrent.xsd
LPCTSTR CSchema::uriBitTorrentFolder		= L"http://schemas.peerproject.org/BitTorrentFolder.xsd";

//LPCTSTR CSchema::uriComments				= L"http://schemas.peerproject.org/Comments.xsd";				// http://www.shareaza.com/schemas/comments.xsd

//LPCTSTR CSchema::uriSkin					= L"http://schemas.peerproject.org/Skin.xsd";
//LPCTSTR CSchema::uriPackage				= L"http://schemas.peerproject.org/Package.xsd";
