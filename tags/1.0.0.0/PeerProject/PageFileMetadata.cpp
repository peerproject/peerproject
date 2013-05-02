//
// PageFileMetadata.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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
#include "PeerProject.h"
#include "PageFileMetadata.h"

#include "Library.h"
#include "SharedFile.h"
#include "SchemaCache.h"
#include "Schema.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CFileMetadataPage, CFilePropertiesPage)

BEGIN_MESSAGE_MAP(CFileMetadataPage, CFilePropertiesPage)
	ON_CBN_SELCHANGE(IDC_SCHEMAS, OnSelChangeSchemas)
	ON_CBN_CLOSEUP(IDC_SCHEMAS, OnCloseUpSchemas)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileMetadataPage property page

CFileMetadataPage::CFileMetadataPage()
	: CFilePropertiesPage	( CFileMetadataPage::IDD )
	, m_pSchemaContainer	( NULL )
	, m_pXML				( NULL )
{
}

CFileMetadataPage::~CFileMetadataPage()
{
	if ( m_pSchemaContainer )
		delete m_pSchemaContainer;
}

void CFileMetadataPage::DoDataExchange(CDataExchange* pDX)
{
	CFilePropertiesPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCHEMAS, m_wndSchemas);
}

/////////////////////////////////////////////////////////////////////////////
// CFileMetadataPage message handlers

BOOL CFileMetadataPage::OnInitDialog()
{
	CFilePropertiesPage::OnInitDialog();

	CLibraryListPtr pFiles( GetList() );

	CRect rcClient, rcCombo;
	CString strText;
	GetClientRect( &rcClient );

	m_wndSchemas.GetWindowRect( &rcCombo );
	ScreenToClient( &rcCombo );
	rcCombo.top = rcCombo.bottom + 8;
	rcCombo.bottom = rcClient.bottom - 8;

	m_wndData.Create( WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP, rcCombo, this, IDC_METADATA );
	LoadString( strText, IDS_SEARCH_NO_METADATA );
	m_wndSchemas.m_sNoSchemaText = strText;

	BOOL bCollection = FALSE;
	CSchemaPtr pSchema = NULL;

	{
		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos = pFiles->GetHeadPosition() ; pos ; )
		{
			if ( CLibraryFile* pFile = pFiles->GetNextFile( pos ) )
			{
				CSchemaPtr pThisSchema = pFile->m_pSchema;

				if ( pThisSchema != NULL && pThisSchema->m_nType == CSchema::stFolder )
					bCollection = TRUE;

				if ( pSchema == NULL )
				{
					pSchema = pThisSchema;
				}
				else if ( pSchema != pThisSchema )
				{
					pSchema = NULL;
					break;
				}
			}
		}
	}

	m_wndSchemas.Load( pSchema != NULL ? pSchema->GetURI() : _T(""), bCollection ? -1 : 0 );
	OnSelChangeSchemas();

	if ( pSchema != NULL )
	{
		m_pSchemaContainer = pSchema->Instantiate( TRUE );
		m_pXML = m_pSchemaContainer->AddElement( pSchema->m_sSingular );

		{
			CQuickLock oLock( Library.m_pSection );

			for ( POSITION pos1 = pFiles->GetHeadPosition() ; pos1 ; )
			{
				if ( CLibraryFile* pFile = pFiles->GetNextFile( pos1 ) )
				{
					if ( pFile->m_pMetadata != NULL && pSchema->Equals( pFile->m_pSchema ) )
					{
						for ( POSITION pos2 = pSchema->GetMemberIterator() ; pos2 ; )
						{
							CSchemaMember* pMember = pSchema->GetNextMember( pos2 );
							CString strOld = pMember->GetValueFrom( m_pXML, NO_VALUE, FALSE, TRUE );
							if ( strOld != MULTI_VALUE )
							{
								CString strNew = pMember->GetValueFrom( pFile->m_pMetadata, NO_VALUE );
								if ( strOld == NO_VALUE && strNew != NO_VALUE )
									m_pXML->AddAttribute( pMember->m_sName, strNew );
								else if ( strOld != strNew )
									m_pXML->AddAttribute( pMember->m_sName, MULTI_VALUE );
							}
						}
					}
				}
			}
		}

		m_wndData.UpdateData( m_pXML, FALSE );
	}

	return TRUE;
}

void CFileMetadataPage::OnSelChangeSchemas()
{
	CSchemaPtr pSchema = m_wndSchemas.GetSelected();
	CString strSelectedURI = m_wndData.GetSchemaURI();

	if ( pSchema && ! pSchema->CheckURI( strSelectedURI ) )
	{
		if ( strSelectedURI.IsEmpty() )
		{
			m_wndData.SetSchema( pSchema );
			return;
		}

		CString strBody( ::LoadHTML( GetModuleHandle( NULL ), IDR_XML_SCHEMA_MAPS ) );		// SchemaMappings.xml.gz

		if ( CXMLElement* pXML = CXMLElement::FromString( strBody, TRUE ) )
		{
			if ( pXML->IsNamed( L"schemaMappings" ) )
			{
				for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
				{
					CXMLElement* pMapping = pXML->GetNextElement( pos );
					if ( pMapping && pMapping->IsNamed( L"schemaMapping" ) )
					{
						CXMLAttribute* pSourceURI = pMapping->GetAttribute( L"sourceURI" );
						if ( pSourceURI && pSourceURI->GetValue() == m_wndData.GetSchemaURI() )
						{
							// Add attributes which correspond to other schema
							// Don't need to delete old ones- after submitting new data, they will be ignored.
							// Also allows to save the old ones if we switch schema back.
							AddCrossAttributes( pMapping, pSchema->GetURI() );
							break;
						}
					}
				}
			}
			delete pXML;
		}

		m_wndData.SetSchema( pSchema );
		if ( m_pXML )
		{
			// Change schema of data
			m_pXML->SetName( pSchema->m_sSingular );
			m_wndData.UpdateData( m_pXML, FALSE );
		}
	}
	else
		m_wndData.SetSchema( pSchema );
}

void CFileMetadataPage::AddCrossAttributes(CXMLElement* pXML, LPCTSTR pszTargetURI)
{
	if ( pXML == NULL ) return;
	CXMLElement* pTargetURI = NULL;

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		pTargetURI = pXML->GetNextElement( pos );

		if ( pTargetURI && pTargetURI->IsNamed( L"target" ) )
		{
			CXMLAttribute* pURI = pTargetURI->GetAttribute( L"uri" );
			if ( pURI && _tcscmp( pURI->GetValue(), pszTargetURI ) == 0 )
				break;
		}

		pTargetURI = NULL;
	}

	if ( pTargetURI == NULL ) return;

	for ( POSITION pos = pTargetURI->GetElementIterator() ; pos ; )
	{
		CXMLElement* pAttribute = pTargetURI->GetNextElement( pos );
		{
			if ( pAttribute && pAttribute->IsNamed( L"attribute" ) )
			{
				CXMLAttribute* pFrom = pAttribute->GetAttribute( L"from" );
				CXMLAttribute* pTo = pAttribute->GetAttribute( L"to" );
				if ( pFrom && pTo )
				{
					CString strFrom = pFrom->GetValue();
					CString strTo = pTo->GetValue();
					if ( strFrom.IsEmpty() || strTo.IsEmpty() ) continue;

					CString strValue = m_pXML->GetAttributeValue( strFrom );
					if ( strValue.GetLength() && strValue != MULTI_VALUE )
						m_pXML->AddAttribute( strTo, strValue );
				}
			}
		}
	}
}

void CFileMetadataPage::OnCloseUpSchemas()
{
	if ( CSchemaPtr pSchema = m_wndSchemas.GetSelected() )
		PostMessage( WM_KEYDOWN, VK_TAB );
}

void CFileMetadataPage::OnOK()
{
	CLibraryListPtr pFiles( GetList() );

	if ( ! pFiles ) return;

	if ( pFiles->GetCount() >= 10 )
	{
		CString strFormat, strMessage;
		LoadString( strFormat, IDS_LIBRARY_METADATA_MANY );
		strMessage.Format( strFormat, pFiles->GetCount() );
		if ( MsgBox( strMessage, MB_YESNO|MB_ICONQUESTION ) != IDYES ) return;
	}

	if ( CSchemaPtr pSchema = m_wndSchemas.GetSelected() )
	{
		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos1 = pFiles->GetHeadPosition() ; pos1 ; )
		{
			if ( CLibraryFile* pFile = pFiles->GetNextFile( pos1 ) )
			{
				CXMLElement* pContainer = pSchema->Instantiate( TRUE );
				if ( pContainer )
				{
					CXMLElement* pXML = NULL;

					if ( pFile->m_pMetadata != NULL )
					{
						pXML = pContainer->AddElement( pFile->m_pMetadata->Clone() );
						// Change schema
						pXML->SetName( pSchema->m_sSingular );
					}
					else
						pXML = pContainer->AddElement( pSchema->m_sSingular );

					// Save changed data to pXML
					m_wndData.UpdateData( pXML, TRUE );
					pFile->SetMetadata( pContainer );
					delete pContainer;
				}
			}
		}
	}
	else
	{
		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos1 = pFiles->GetHeadPosition() ; pos1 ; )
		{
			if ( CLibraryFile* pFile = pFiles->GetNextFile( pos1 ) )
				pFile->ClearMetadata();
		}
	}

	Library.Update();

	CFilePropertiesPage::OnOK();
}
