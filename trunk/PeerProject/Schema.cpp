//
// Schema.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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
	CString strFilters = _T("|");
	for ( POSITION pos = m_pTypeFilters.GetStartPosition() ; pos ; )
	{
		CString strType;
		BOOL bResult;
		m_pTypeFilters.GetNextAssoc( pos, strType, bResult );
		if ( bResult )
		{
			strFilters += strType;
			strFilters += _T('|');
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

	CString str( _T("title") );
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

	for ( POSITION pos = m_pBitziMap.GetHeadPosition() ; pos ; )
	{
		delete m_pBitziMap.GetNext( pos );
	}

	m_pMembers.RemoveAll();
	m_pContains.RemoveAll();
	m_pBitziMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CSchema Load

BOOL CSchema::Load(LPCTSTR pszFile)
{
	CString strFile( pszFile );
	PathRemoveExtension( strFile.GetBuffer() );
	strFile.ReleaseBuffer();

	if ( ! LoadSchema( strFile + _T(".xsd") ) ) return FALSE;

	m_sIcon = strFile + _T(".ico");

	LoadDescriptor( strFile + _T(".xml") );

	// LoadIcon() causes bad registry reads (?)
	// CCoolInterface::IsNewWindows() caused several reapeat ones.(?)
	if ( theApp.m_bIsWin2000 || ! LoadIcon() )
	{
		m_sIcon = m_sIcon.Left( m_sIcon.GetLength() - 4 );
		m_sIcon += _T(".Safe.ico");
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
	CString strXML;

	CXMLElement* pRoot = CXMLElement::FromFile( pszFile );
	if ( pRoot == NULL ) return FALSE;

	BOOL bResult = FALSE;

	m_sURI = pRoot->GetAttributeValue( _T("targetNamespace"), _T("") );

	CXMLElement* pPlural = pRoot->GetElementByName( _T("element") );

	if ( pPlural && ! m_sURI.IsEmpty() )
	{
		m_sPlural = pPlural->GetAttributeValue( _T("name") );

		CXMLElement* pComplexType = pPlural->GetFirstElement();

		if ( pComplexType && pComplexType->IsNamed( _T("complexType") ) && ! m_sPlural.IsEmpty() )
		{
			CXMLElement* pElement = pComplexType->GetFirstElement();

			if ( pElement && pElement->IsNamed( _T("element") ) )
			{
				m_sSingular = pElement->GetAttributeValue( _T("name") );

				if ( pElement->GetElementCount() )
				{
					bResult = LoadPrimary( pRoot, pElement->GetFirstElement() );
				}
				else
				{
					CString strType = pElement->GetAttributeValue( _T("type") );
					bResult = LoadPrimary( pRoot, GetType( pRoot, strType ) );
				}

				if ( m_sSingular.IsEmpty() )
					bResult = FALSE;
			}
		}
	}

	if ( CXMLElement* pMapping = pRoot->GetElementByName( _T("mapping") ) )
	{
		for ( POSITION pos = pMapping->GetElementIterator() ; pos ; )
		{
			CXMLElement* pNetwork = pMapping->GetNextElement( pos );
			if ( pNetwork )
			{
				BOOL bFound = pNetwork->IsNamed( _T("network") );

				CString strName = pNetwork->GetAttributeValue( _T("name") );
				if ( ! bFound || strName != _T("ed2k") )
					continue;

				m_sDonkeyType = pNetwork->GetAttributeValue( _T("value") );
				break;
			}
		}
	}

	// ToDo: External documents
	//CString strImported;
	//if ( CXMLElement* pImported = pRoot->GetElementByName( _T("import") ) )
	//	strImported = pImported->GetAttributeValue( _T("schemaLocation") );
	//else if ( CXMLElement* pIncluded = pRoot->GetElementByName( _T("include") ) )
	//	strImported = pIncluded->GetAttributeValue( _T("schemaDescriptor") );
	//
	//if ( strImported.GetLength() > 5 && strImported.Find( _T(".xsd") ) > 1 )
	//{
	//	CString strFile( pszFile );
	//	strFile = strFile.Left( strFile.ReverseFind( '\\' ) + 1 ) + strImported;
	//	LoadSchema( (LPCTSTR)strFile );
	//}

	delete pRoot;

	return bResult;
}

BOOL CSchema::LoadPrimary(CXMLElement* pRoot, CXMLElement* pType)
{
	if ( ! pRoot || ! pType ) return FALSE;

	if ( ! pType->IsNamed( _T("complexType") ) &&
		 ! pType->IsNamed( _T("all") ) ) return FALSE;

	for ( POSITION pos = pType->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement	= pType->GetNextElement( pos );
		CString strElement		= pElement->GetName();

		if ( strElement.CompareNoCase( _T("attribute") ) == 0 ||
			 strElement.CompareNoCase( _T("element") ) == 0 )
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
		else if ( strElement.CompareNoCase( _T("all") ) == 0 )
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

		if ( strElement.CompareNoCase( _T("simpleType") ) == 0 ||
			 strElement.CompareNoCase( _T("complexType") ) == 0 )
		{
			if ( pElement->GetAttributeValue( _T("name"), _T("?") ).CompareNoCase( pszName ) == 0 )
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

	if ( ! CheckURI( pRoot->GetAttributeValue( _T("location") ) ) ||
		 ! pRoot->IsNamed( _T("schemaDescriptor") ) )
	{
		delete pRoot;
		return FALSE;
	}

	for ( POSITION pos = pRoot->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = pRoot->GetNextElement( pos );

		if ( pElement->IsNamed( _T("object") ) )
		{
			CString strType = pElement->GetAttributeValue( _T("type") );
			ToLower( strType );

			if ( strType == _T("file") )
				m_nType = stFile;
			else if ( strType == _T("folder") || strType == _T("album") )
				m_nType = stFolder;

			strType = pElement->GetAttributeValue( _T("availability") );
			ToLower( strType );

			if ( strType == _T("system") )
				m_nAvailability = saSystem;
			else if ( strType == _T("advanced") )
				m_nAvailability = saAdvanced;
			else // "default"
				m_nAvailability = saDefault;

			if ( pElement->GetAttribute( _T("private") ) )
				m_bPrivate = TRUE;
		}
		else if ( pElement->IsNamed( _T("titles") ) )
		{
			LoadDescriptorTitles( pElement );
		}
		else if ( pElement->IsNamed( _T("members") ) )
		{
			LoadDescriptorMembers( pElement );
		}
		else if ( pElement->IsNamed( _T("extends") ) )
		{
			LoadDescriptorExtends( pElement );
		}
		else if ( pElement->IsNamed( _T("contains") ) )
		{
			LoadDescriptorContains( pElement );
		}
		else if ( pElement->IsNamed( _T("headerContent") ) )
		{
			LoadDescriptorHeaderContent( pElement );
		}
		else if ( pElement->IsNamed( _T("viewContent") ) )
		{
			LoadDescriptorViewContent( pElement );
		}
		else if ( pElement->IsNamed( _T("typeFilter") ) )
		{
			LoadDescriptorTypeFilter( pElement );
		}
		else if ( pElement->IsNamed( _T("bitziImport") ) )
		{
			LoadDescriptorBitziImport( pElement );
		}
		else if ( pElement->IsNamed( _T("images") ) )
		{
			LoadDescriptorIcons( pElement );
		}
		// ToDo: Add this to schemas for ed2k
		//else if ( pElement->IsNamed( _T("donkeyType") ) )
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

		if ( pTitle->IsNamed( _T("title") ) )
		{
			if ( pTitle->GetAttributeValue( _T("language") ).
					CompareNoCase( Settings.General.Language ) == 0 )
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

		if ( pIcon->IsNamed( _T("icon") ) )
		{
			int nSlash = m_sIcon.ReverseFind( '\\' );
			if ( nSlash >= 0 ) m_sIcon = m_sIcon.Left( nSlash + 1 );
			m_sIcon += pIcon->GetAttributeValue( _T("path") );
		}
	}
}

void CSchema::LoadDescriptorMembers(CXMLElement* pElement)
{
	BOOL bPrompt = FALSE;

	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pDisplay = pElement->GetNextElement( pos );

		if ( pDisplay->IsNamed( _T("member") ) )
		{
			CString strMember = pDisplay->GetAttributeValue( _T("name") );

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

		if ( pExtend->IsNamed( _T("schema") ) )
		{
			CString strURI = pExtend->GetAttributeValue( _T("location") );
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

		if ( pExtend->IsNamed( _T("object") ) )
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

		if ( pType->GetName().CompareNoCase( _T("type") ) == 0 )
		{
			CString strType = pType->GetAttributeValue( _T("extension"), _T("") );
			if ( strType.IsEmpty() )
				strType = pType->GetAttributeValue( _T("keyword"), _T("") );

			BOOL bResult = TRUE;
			m_pTypeFilters.SetAt( strType.MakeLower(), bResult );
		}
	}
}

void CSchema::LoadDescriptorBitziImport(CXMLElement* pElement)
{
	m_sBitziTest = pElement->GetAttributeValue( _T("testExists"), NULL );

	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pBitzi = pElement->GetNextElement( pos );

		if ( pBitzi->GetName().CompareNoCase( _T("mapping") ) == 0 )
		{
			CSchemaBitzi* pMap = new CSchemaBitzi();
			pMap->Load( pBitzi );
			m_pBitziMap.AddTail( pMap );
		}
	}
}

void CSchema::LoadDescriptorHeaderContent(CXMLElement* pElement)
{
	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pElement->GetNextElement( pos );

		BOOL bLanguage = pXML->GetAttributeValue( _T("language") ).
			CompareNoCase( Settings.General.Language ) == 0;

		if ( pXML->IsNamed( _T("title") ) )
		{
			if ( bLanguage || m_sHeaderTitle.IsEmpty() )
				m_sHeaderTitle = pXML->GetValue().Trim();
		}
		else if ( pXML->IsNamed( _T("subtitle") ) )
		{
			if ( bLanguage || m_sHeaderSubtitle.IsEmpty() )
				m_sHeaderSubtitle = pXML->GetValue().Trim();
		}
	}
}

void CSchema::LoadDescriptorViewContent(CXMLElement* pElement)
{
	m_sLibraryView = pElement->GetAttributeValue( _T("preferredView") );

	for ( POSITION pos = pElement->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pElement->GetNextElement( pos );

		BOOL bLanguage = pXML->GetAttributeValue( _T("language") ).
			CompareNoCase( Settings.General.Language ) == 0;

		if ( pXML->IsNamed( _T("tileLine1") ) )
		{
			if ( bLanguage || m_sTileLine1.IsEmpty() )
				m_sTileLine1 = pXML->GetValue().Trim();
		}
		else if ( pXML->IsNamed( _T("tileLine2") ) )
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
	if ( bNamespace ) pElement->AddAttribute( _T("xmlns:xsi"), CXMLAttribute::xmlnsInstance );
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
					str += _T(' ') + strMember;
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
		int nOpen = str.Find( '{' );
		if ( nOpen < 0 ) break;
		int nClose = str.Find( '}' );
		if ( nClose <= nOpen ) break;

		CString strMember = str.Mid( nOpen + 1, nClose - nOpen - 1 ).Trim();

		CString strValue;
		if ( CSchemaMember* pMember = GetMember( strMember ) )
			strValue = pMember->GetValueFrom( pXML, NULL, TRUE );

		str = str.Left( nOpen ) + strValue + str.Mid( nClose + 1 );
	}
}

//////////////////////////////////////////////////////////////////////
// CSchemaBitzi Bitzi Map

BOOL CSchemaBitzi::Load(CXMLElement* pXML)
{
	m_sFrom	= pXML->GetAttributeValue( _T("from"), NULL );
	m_sTo	= pXML->GetAttributeValue( _T("to"), NULL );

	CString strFactor = pXML->GetAttributeValue( _T("factor"), NULL );

	if ( strFactor.IsEmpty() || _stscanf( strFactor, _T("%lf"), &m_nFactor ) != 1 )
		m_nFactor = 0;

	return ! m_sFrom.IsEmpty() && ! m_sTo.IsEmpty();
}

//////////////////////////////////////////////////////////////////////
// CSchema Common Schema URIs

// ToDo: Safely convert any unshared to http://schemas.peerproject.org/ namespace ?

LPCTSTR CSchema::uriApplication 			= _T("http://www.shareaza.com/schemas/application.xsd");
LPCTSTR CSchema::uriAudio					= _T("http://www.limewire.com/schemas/audio.xsd");
LPCTSTR CSchema::uriArchive					= _T("http://www.shareaza.com/schemas/archive.xsd");
LPCTSTR CSchema::uriBook					= _T("http://www.limewire.com/schemas/book.xsd");
LPCTSTR CSchema::uriImage					= _T("http://www.shareaza.com/schemas/image.xsd");
LPCTSTR CSchema::uriVideo					= _T("http://www.limewire.com/schemas/video.xsd");
LPCTSTR CSchema::uriROM 					= _T("http://www.shareaza.com/schemas/rom.xsd");
LPCTSTR CSchema::uriDocument				= _T("http://www.shareaza.com/schemas/wordProcessing.xsd");
LPCTSTR CSchema::uriSpreadsheet				= _T("http://www.shareaza.com/schemas/spreadsheet.xsd");
LPCTSTR CSchema::uriPresentation			= _T("http://www.shareaza.com/schemas/presentation.xsd");
LPCTSTR CSchema::uriCollection				= _T("http://www.shareaza.com/schemas/collection.xsd");

LPCTSTR CSchema::uriLibrary					= _T("http://www.shareaza.com/schemas/libraryRoot.xsd");

LPCTSTR CSchema::uriFolder					= _T("http://www.shareaza.com/schemas/folder.xsd");
LPCTSTR CSchema::uriCollectionsFolder		= _T("http://www.shareaza.com/schemas/collectionsFolder.xsd");
LPCTSTR CSchema::uriFavouritesFolder		= _T("http://www.shareaza.com/schemas/favouritesFolder.xsd");
LPCTSTR CSchema::uriSearchFolder			= _T("http://www.shareaza.com/schemas/searchFolder.xsd");
LPCTSTR CSchema::uriAllFiles				= _T("http://www.shareaza.com/schemas/allFiles.xsd");

LPCTSTR CSchema::uriApplicationRoot			= _T("http://www.shareaza.com/schemas/applicationRoot.xsd");
LPCTSTR CSchema::uriApplicationAll			= _T("http://www.shareaza.com/schemas/applicationAll.xsd");

LPCTSTR CSchema::uriArchiveRoot 			= _T("http://www.shareaza.com/schemas/archiveRoot.xsd");
LPCTSTR CSchema::uriArchiveAll				= _T("http://www.shareaza.com/schemas/archiveAll.xsd");

LPCTSTR CSchema::uriBookRoot				= _T("http://www.shareaza.com/schemas/bookRoot.xsd");
LPCTSTR CSchema::uriBookAll					= _T("http://www.shareaza.com/schemas/bookAll.xsd");

LPCTSTR CSchema::uriImageRoot				= _T("http://www.shareaza.com/schemas/imageRoot.xsd");
LPCTSTR CSchema::uriImageAll				= _T("http://www.shareaza.com/schemas/imageAll.xsd");
LPCTSTR CSchema::uriImageAlbum				= _T("http://www.shareaza.com/schemas/imageAlbum.xsd");

LPCTSTR CSchema::uriMusicRoot				= _T("http://www.shareaza.com/schemas/musicRoot.xsd");
LPCTSTR CSchema::uriMusicAll				= _T("http://www.shareaza.com/schemas/musicAll.xsd");
LPCTSTR CSchema::uriMusicAlbum				= _T("http://www.shareaza.com/schemas/musicAlbum.xsd");
LPCTSTR CSchema::uriMusicArtist				= _T("http://www.shareaza.com/schemas/musicArtist.xsd");
LPCTSTR CSchema::uriMusicGenre				= _T("http://www.shareaza.com/schemas/musicGenre.xsd");
LPCTSTR CSchema::uriMusicAlbumCollection	= _T("http://www.shareaza.com/schemas/musicAlbumCollection.xsd");
LPCTSTR CSchema::uriMusicArtistCollection	= _T("http://www.shareaza.com/schemas/musicArtistCollection.xsd");
LPCTSTR CSchema::uriMusicGenreCollection	= _T("http://www.shareaza.com/schemas/musicGenreCollection.xsd");

LPCTSTR CSchema::uriVideoRoot				= _T("http://www.shareaza.com/schemas/videoRoot.xsd");
LPCTSTR CSchema::uriVideoAll				= _T("http://www.shareaza.com/schemas/videoAll.xsd");
LPCTSTR CSchema::uriVideoFilm				= _T("http://www.shareaza.com/schemas/videoFilm.xsd");
LPCTSTR CSchema::uriVideoSeries				= _T("http://www.shareaza.com/schemas/videoSeries.xsd");
LPCTSTR CSchema::uriVideoFilmCollection		= _T("http://www.shareaza.com/schemas/videoFilmCollection.xsd");
LPCTSTR CSchema::uriVideoSeriesCollection	= _T("http://www.shareaza.com/schemas/videoSeriesCollection.xsd");
LPCTSTR CSchema::uriVideoMusicCollection	= _T("http://www.shareaza.com/schemas/videoMusicCollection.xsd");

LPCTSTR CSchema::uriDocumentRoot			= _T("http://www.shareaza.com/schemas/documentRoot.xsd");
LPCTSTR CSchema::uriDocumentAll				= _T("http://www.shareaza.com/schemas/documentAll.xsd");

LPCTSTR CSchema::uriGhostFolder				= _T("http://schemas.peerproject.org/GhostFolder.xsd");	// http://www.shareaza.com/schemas/ghostFolder.xsd ?

LPCTSTR CSchema::uriBitTorrent				= _T("http://www.shareaza.com/schemas/bittorrent.xsd");

//LPCTSTR CSchema::uriComments				= _T("http://www.shareaza.com/schemas/comments.xsd");	// http://schemas.peerproject.org/Comments.xsd ?

//LPCTSTR CSchema::uriSkin					= _T("http://schemas.peerproject.org/Skin.xsd");
//LPCTSTR CSchema::uriPackage				= _T("http://schemas.peerproject.org/Package.xsd");
