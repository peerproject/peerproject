//
// DlgDecodeMetadata.cpp
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
#include "Library.h"
#include "SharedFile.h"
#include "XML.h"
#include "Schema.h"
#include "DlgDecodeMetadata.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CDecodeMetadataDlg, CSkinDialog)
	ON_BN_CLICKED(IDC_METHOD1, OnClickedMethod1)
	ON_BN_CLICKED(IDC_METHOD2, OnClickedMethod2)
	ON_CBN_CLOSEUP(IDC_CODEPAGES, OnCloseupCodepages)
	ON_CBN_SELCHANGE(IDC_CODEPAGES, OnSelchangeCodepages)
END_MESSAGE_MAP()

const unsigned CDecodeMetadataDlg::codePages[] =
{
	// arabic, baltic, centr. european, chinese simplified, chinese traditional
	1256, 1257, 1250, 936,  950,
	// cyrillic, greek, hebrew, japanese, korean, thai, turkish, vietnamese
	1251, 1253, 1255,  932,  949, 874, 1254, 1258
};

/////////////////////////////////////////////////////////////////////////////
// CDecodeMetadataDlg dialog

CDecodeMetadataDlg::CDecodeMetadataDlg(CWnd* pParent) : CSkinDialog(CDecodeMetadataDlg::IDD, pParent)
, m_nMethod( 0 )
, m_sPreview1( L"" )
, m_sPreview2( L"" )
{
}

void CDecodeMetadataDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODEPAGES, m_wndCodepages);
	DDX_Radio(pDX, IDC_METHOD1, m_nMethod);
	DDX_Text(pDX, IDC_PREVIEW1, m_sPreview1);
	DDX_Text(pDX, IDC_PREVIEW2, m_sPreview2);
}

/////////////////////////////////////////////////////////////////////////////
// CDecodeMetadataDlg message handlers

BOOL CDecodeMetadataDlg::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	
	SkinMe( _T("CDecodeMetadataDlg"), IDI_WORLD );

	if ( !m_pFiles.IsEmpty() ) 
	{
		CQuickLock oLock( Library.m_pSection );
		CLibraryFile* pFile = Library.LookupFile( m_pFiles.GetHead() );
		if ( !pFile || !pFile->m_pMetadata || !pFile->m_pSchema ) return TRUE;

		CXMLElement* pXML = pFile->m_pMetadata;
		m_sOriginalWords = pFile->m_pSchema->GetVisibleWords( pXML );
		
		m_sPreview1 = m_sOriginalWords;
		GetEncodedText( m_sPreview1, 0 );
		m_sPreview2 = m_sOriginalWords;
		GetEncodedText( m_sPreview2, 1 );
	}

	UpdateData( FALSE );

	return TRUE;
}

void CDecodeMetadataDlg::AddFile(CLibraryFile* pFile)
{
	m_pFiles.AddTail( pFile->m_nIndex );
}

void CDecodeMetadataDlg::OnOK() 
{
	UpdateData();

	unsigned nCodePage = m_wndCodepages.GetCurSel();
	nCodePage = nCodePage < 13 ? codePages[ nCodePage ] : 1252; // english

	// close dialog and perform decoding in background
	CSkinDialog::OnOK();

	for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; )
	{
		DWORD nIndex = m_pFiles.GetNext( pos );
		
		CXMLElement* pXML;
		CLibraryFile* pFile;

		{
			CQuickLock oLock( Library.m_pSection );

			if ( m_pFiles.IsEmpty() ) break;

			pFile = Library.LookupFile( nIndex );
			if ( !pFile || !pFile->m_pMetadata || !pFile->m_pSchema ) continue;

			pXML = pFile->m_pMetadata->Clone();
		}

		for ( POSITION pos = pXML->GetAttributeIterator() ; pos ; )
		{
			CXMLAttribute* pAttribute = pXML->GetNextAttribute( pos );

			CString strAttribute = pAttribute->GetValue();
			GetEncodedText( strAttribute, m_nMethod );
			pAttribute->SetValue( strAttribute );
		}

		CQuickLock oLock( Library.m_pSection );
		// make a clean copy of schema with namespace included
		CXMLElement* pContainer	= pFile->m_pSchema->Instantiate( TRUE );
		if ( pContainer )
		{
			// append modified metadata
			/*CXMLElement* pMetadata	=*/ pContainer->AddElement( pXML );
			// save metadata by creating XML file
			pFile->SetMetadata( pContainer );
			delete pContainer;
		}
	}
}

void CDecodeMetadataDlg::GetEncodedText(CString& strText, int nMethod) const
{
	int nLength = strText.GetLength();
	LPTSTR pszSource = strText.GetBuffer( nLength );
	LPTSTR pszOutput = NULL;
	int nWide = 0;

	unsigned nCodePage = m_wndCodepages.GetCurSel();
	nCodePage = nCodePage < 13 ? codePages[ nCodePage ] : 1252; // english

	if ( nMethod == 0 )
	{
		auto_array< CHAR > pszDest( new CHAR[ nLength + 1 ] );

		{
			const TCHAR* source = pszSource;
			CHAR* dest = pszDest.get();
			while ( *dest++ = static_cast< CHAR >( *source ), *source++ );
		}

		nWide = MultiByteToWideChar( nCodePage, 0, pszDest.get(), nLength, NULL, 0 );
		pszOutput = strText.GetBuffer( nWide + 1 );
		MultiByteToWideChar( nCodePage, 0, pszDest.get(), nLength, pszOutput, nWide );
	}
	else
	{
		CStringA source = pszSource;

		nWide = MultiByteToWideChar( nCodePage, 0, source, nLength, NULL, 0 );
		pszOutput = strText.GetBuffer( nWide + 1 );
		MultiByteToWideChar( nCodePage, 0, source, nLength, pszOutput, nWide );
	}

	pszOutput[ nWide ] = 0;
	strText.ReleaseBuffer();
}

void CDecodeMetadataDlg::OnClickedMethod1()
{
	UpdateData( TRUE );
}

void CDecodeMetadataDlg::OnClickedMethod2()
{
	UpdateData( TRUE );
}

void CDecodeMetadataDlg::OnCloseupCodepages()
{
	m_sPreview1 = m_sOriginalWords;
	GetEncodedText( m_sPreview1, 0 );
	m_sPreview2 = m_sOriginalWords;
	GetEncodedText( m_sPreview2, 1 );

	UpdateData( FALSE );
}

void CDecodeMetadataDlg::OnSelchangeCodepages()
{
	OnCloseupCodepages();
}
