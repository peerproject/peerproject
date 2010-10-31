//
// LibraryBuilderPlugins.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "LibraryBuilder.h"
#include "LibraryBuilderPlugins.h"
#include "Plugins.h"
#include "XML.h"
#include "XMLCOM.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

//////////////////////////////////////////////////////////////////////
// CLibraryBuilderPlugins construction

CLibraryBuilderPlugins::CLibraryBuilderPlugins()
{
}

CLibraryBuilderPlugins::~CLibraryBuilderPlugins()
{
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilderPlugins extract

bool CLibraryBuilderPlugins::ExtractPluginMetadata(DWORD nIndex, const CString& strPath, HANDLE hFile)
{
	CString strType;

	int nExtPos = strPath.ReverseFind( '.' );
	if ( nExtPos != -1 )
		strType = strPath.Mid( nExtPos );

	ToLower( strType );

	CComQIPtr< ILibraryBuilderPlugin > pPlugin( LoadPlugin( strType ) );
	if ( !pPlugin )
		return false;

	CXMLElement* pXML	= new CXMLElement();
	ISXMLElement* ppXML	= (ISXMLElement*)CXMLCOM::Wrap( pXML, IID_ISXMLElement );

	BSTR bsFile = strPath.AllocSysString();

	HRESULT hResult = pPlugin->Process( hFile, bsFile, ppXML );

	SysFreeString( bsFile );

	ppXML->Release();

	bool bSuccess = false;

	if ( SUCCEEDED( hResult ) )
	{
		if ( CXMLElement* pOuter = pXML->GetFirstElement() )
		{
			CXMLElement* pInner		= pOuter->GetFirstElement();
			CString strSchemaURI	= pOuter->GetAttributeValue( CXMLAttribute::schemaName );

			if ( pInner && strSchemaURI.GetLength() )
			{
				pInner = pInner->Detach();
				bSuccess = LibraryBuilder.SubmitMetadata( nIndex, strSchemaURI, pInner ) != 0;
			}
		}
	}
	else if ( hResult == E_UNEXPECTED )
	{
		bSuccess = LibraryBuilder.SubmitCorrupted( nIndex );
	}

	delete pXML;

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilderPlugins cleanup

void CLibraryBuilderPlugins::CleanupPlugins()
{
	CQuickLock oLock( m_pSection );

	for ( POSITION pos = m_pMap.GetStartPosition() ; pos ; )
	{
		ILibraryBuilderPlugin* pPlugin = NULL;
		CString strType;
		m_pMap.GetNextAssoc( pos, strType, pPlugin );
		if ( pPlugin )
			pPlugin->Release();
	}

	m_pMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilderPlugins load plugin

ILibraryBuilderPlugin* CLibraryBuilderPlugins::LoadPlugin(LPCTSTR pszType)
{
	CQuickLock oLock( m_pSection );

	ILibraryBuilderPlugin* pPlugin = NULL;
	if ( m_pMap.Lookup( pszType, pPlugin ) )
		return pPlugin;

	CLSID pCLSID;
	if ( !Plugins.LookupCLSID( _T("LibraryBuilder"), pszType, pCLSID ) )
	{
		m_pMap.SetAt( pszType, NULL );
		return NULL;
	}

	HRESULT hr = CoCreateInstance( pCLSID, NULL, CLSCTX_ALL,
		IID_ILibraryBuilderPlugin, (void**)&pPlugin );

	if ( SUCCEEDED( hr ) )
		m_pMap.SetAt( pszType, pPlugin );

	return pPlugin;
}
