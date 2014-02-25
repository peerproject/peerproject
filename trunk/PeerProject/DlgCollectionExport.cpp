//
// DlgCollectionExport.cpp
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
#include "DlgCollectionExport.h"

#include "Library.h"
#include "LibraryFolders.h"
#include "AlbumFolder.h"
#include "SharedFile.h"
#include "LiveList.h"
#include "Colors.h"
#include "ImageFile.h"
#include "ThumbCache.h"
#include "Schema.h"
#include "XML.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

// Set Column Order
enum {
	COL_NAME,
	COL_AUTHOR,
	COL_VERSION,
	COL_PATH,
	COL_URL,
	COL_EMAIL,
	COL_INFO,
	COL_FILENAME,
	COL_THUMBS,
	COL_LAST	// Count
};

IMPLEMENT_DYNAMIC(CCollectionExportDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CCollectionExportDlg, CSkinDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDOK, &CCollectionExportDlg::OnOK)
	ON_BN_CLICKED(IDC_TEMPLATES_DELETE, &CCollectionExportDlg::OnTemplatesDeleteOrBack)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TEMPLATES, &CCollectionExportDlg::OnItemChangedTemplates)
END_MESSAGE_MAP()


CCollectionExportDlg::CCollectionExportDlg(const CAlbumFolder* pFolder, CWnd* pParent)
	: CSkinDialog(CCollectionExportDlg::IDD, pParent)
	, m_pFolder		( pFolder )
	, m_nSelected	( -1 )
	, m_nStep		( 0 )
	, m_bThumbnails	( FALSE )
{
}

CCollectionExportDlg::~CCollectionExportDlg()
{
}

void CCollectionExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange( pDX );

	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_STATIC_AUTHOR, m_wndLblAuthor);
	DDX_Control(pDX, IDC_STATIC_NAME, m_wndLblName);
	DDX_Control(pDX, IDC_STATIC_DESC, m_wndLblDesc);
	DDX_Control(pDX, IDC_STATIC_GROUPBOX, m_wndGroupBox);
	DDX_Control(pDX, IDC_TEMPLATES_EXPLAIN, m_wndExplain);
	DDX_Control(pDX, IDC_TEMPLATES_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_TEMPLATE_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_TEMPLATE_NAME, m_wndName);
	DDX_Control(pDX, IDC_TEMPLATE_AUTHOR, m_wndAuthor);
	DDX_Control(pDX, IDC_TEMPLATES, m_wndList);
}

BOOL CCollectionExportDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	m_gdiImageList.Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 1, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_COLOR16|ILC_MASK, 1, 1 );
	AddIcon( IDI_SKIN, m_gdiImageList );

	m_wndList.SetImageList( &m_gdiImageList, LVSIL_SMALL );

	// Show template name, author and version number columns, hide the rest (info)
	m_wndList.InsertColumn( COL_NAME,	_T("Name"), LVCFMT_LEFT, 134, 0 );
	m_wndList.InsertColumn( COL_AUTHOR,	_T("Author"), LVCFMT_LEFT, 114, 1 );
	m_wndList.InsertColumn( COL_VERSION, _T("Version"), LVCFMT_LEFT, 32, 2 );
	m_wndList.InsertColumn( COL_PATH,	_T("Path"), LVCFMT_LEFT, 0, 3 );
	m_wndList.InsertColumn( COL_URL,	_T("URL"), LVCFMT_LEFT, 0, 4 );
	m_wndList.InsertColumn( COL_EMAIL,	_T("Email"), LVCFMT_LEFT, 0, 5 );
	m_wndList.InsertColumn( COL_INFO,	_T("Description"), LVCFMT_LEFT, 0, 6 );
	m_wndList.InsertColumn( COL_FILENAME, _T("Filename"), LVCFMT_LEFT, 0, 7 );
	m_wndList.InsertColumn( COL_THUMBS,	_T("Thumbnails"), LVCFMT_LEFT, 0, 8 );

	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );

	// Translate window
	SkinMe( _T("CCollectionExportDlg"), IDI_COLLECTION );

	if ( Settings.General.LanguageRTL )
		m_wndDesc.ModifyStyleEx( WS_EX_RTLREADING|WS_EX_RIGHT|WS_EX_LEFTSCROLLBAR,
			WS_EX_LTRREADING|WS_EX_LEFT|WS_EX_RIGHTSCROLLBAR, 0 );

	m_nSelected = -1;
	m_wndName.SetWindowText( _T("") );
	m_wndAuthor.SetWindowText( _T("") );

	// Get label and button caption for the first screen, save the rest to variables for later use.
	CString str;
	m_wndOK.GetWindowText( str );
	int nPos = str.Find( '|' );
	if ( nPos > 0 )
	{
		m_sBtnNext = str.Left( nPos );
		m_sBtnExport = str.Mid( nPos + 1 );
		m_wndOK.SetWindowText( m_sBtnNext );
	}

	m_wndExplain.GetWindowText( str );
	nPos = str.Find( '|' );
	if ( nPos > 0 )
	{
		m_sLblExplain1 = str.Left( nPos );
		m_sLblExplain2 = str.Mid( nPos + 1 );
		m_wndExplain.SetWindowText( m_sLblExplain1 );
	}

	m_wndDelete.GetWindowText( str );
	nPos = str.Find( '|' );
	if ( nPos > 0 )
	{
		m_sBtnDelete = str.Left( nPos );
		m_sBtnBack = str.Mid( nPos + 1 );
		m_wndDelete.SetWindowText( m_sBtnDelete );
	}

	m_wndDelete.EnableWindow( FALSE );
	m_wndOK.EnableWindow( FALSE );
	m_nStep = 1;

	CWaitCursor pCursor;

	// Get templates info from Templates folder and fill in the list
	EnumerateTemplates();

	return TRUE;
}

void CCollectionExportDlg::EnumerateTemplates(LPCTSTR pszPath)
{
	WIN32_FIND_DATA pFind;
	HANDLE hSearch;

	CString strPath;
	strPath.Format( _T("%s\\Templates\\%s*.*"),
		(LPCTSTR)Settings.General.Path, pszPath ? pszPath : _T("") );

	hSearch = FindFirstFile( strPath, &pFind );

	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.cFileName[0] == _T('.') ) continue;

			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				strPath.Format( _T("%s%s\\"),
					pszPath ? pszPath : _T(""), pFind.cFileName );

				EnumerateTemplates( strPath );
			}
			else if ( _tcsistr( pFind.cFileName, _T(".xml") ) != NULL )
			{
				AddTemplate( pszPath, pFind.cFileName );
			}
		}
		while ( FindNextFile( hSearch, &pFind ) );

		FindClose( hSearch );
	}
}

BOOL CCollectionExportDlg::AddTemplate(LPCTSTR pszPath, LPCTSTR pszName)
{
	CString strXML = Settings.General.Path + _T("\\Templates\\");
	if ( pszPath ) strXML += pszPath;
	strXML += pszName;

	strXML = LoadFile( strXML );
	if ( strXML.IsEmpty() ) return FALSE;

	CXMLElement* pXML = NULL;

	int nManifest = strXML.Find( _T("<manifest") );

	if ( nManifest > 0 )
	{
		CString strManifest = strXML.Mid( nManifest ).SpanExcluding( _T(">") ) + '>';

		if ( CXMLElement* pManifest = CXMLElement::FromString( strManifest ) )
		{
			pXML = new CXMLElement( NULL, _T("template") );
			pXML->AddElement( pManifest );
		}
	}

	if ( pXML == NULL )
	{
		pXML = CXMLElement::FromString( strXML, TRUE );
		if ( pXML == NULL ) return FALSE;
	}

	strXML.Empty();

	const CXMLElement* pManifest = pXML->GetElementByName( _T("manifest") );

	if ( ! pXML->IsNamed( _T("template") ) || pManifest == NULL )
	{
		delete pXML;
		return FALSE;
	}

	CString strName		= pManifest->GetAttributeValue( _T("name"), pszName );
	CString strAuthor	= pManifest->GetAttributeValue( _T("author"), _T("-") );
	CString strVersion	= pManifest->GetAttributeValue( _T("version"), _T("-") );
	CString strIcon		= pManifest->GetAttributeValue( _T("icon") );
	CString strURL		= pManifest->GetAttributeValue( _T("link") );
	CString strEmail	= pManifest->GetAttributeValue( _T("email") );
	CString strDesc		= pManifest->GetAttributeValue( _T("description") );
	CString strThumbs	= pManifest->GetAttributeValue( _T("thumbnails"), _T("false") );
	CString strFilename	= pManifest->GetAttributeValue( _T("filename"), _T("index.htm") );

	CString strPath;
	strPath.Format( _T("%s%s"), pszPath ? pszPath : _T(""), pszName );

	delete pXML;

	if ( Settings.General.LanguageRTL )
	{
		strName = _T("\x202A") + strName;
		strAuthor = _T("\x202A") + strAuthor;
	}

	if ( ! strIcon.IsEmpty() )
	{
		if ( pszPath )
			strIcon = Settings.General.Path + _T("\\Templates\\") + pszPath + strIcon;
		else
			strIcon = Settings.General.Path + _T("\\Templates\\") + strIcon;
	}
	else
	{
		if ( pszPath )
			strIcon = Settings.General.Path + _T("\\Templates\\") + pszPath + strIcon + pszName;
		else
			strIcon = Settings.General.Path + _T("\\Templates\\") + strIcon + pszName;

		strIcon = strIcon.Left( strIcon.GetLength() - 3 ) + _T("ico");
	}

	if ( StartsWith( strURL, _PT("http://") ) )
		; // Do nothing
	else if ( StartsWith( strURL, _PT("www.") ) )
		strURL = _T("http://") + strURL;
	else
		strURL.Empty();

	//if ( strEmail.Find( '@' ) < 1 )
	//	strEmail.Empty();

	CLiveItem pItem( COL_LAST, 0 );

	HICON hIcon;
	if ( ExtractIconEx( strIcon, 0, NULL, &hIcon, 1 ) != NULL && hIcon != NULL )
		pItem.SetImage( AddIcon( hIcon, m_gdiImageList ) );
	else
		pItem.SetImage( 0 );

	pItem.Set( COL_NAME,	strName );
	pItem.Set( COL_AUTHOR,	strAuthor );
	pItem.Set( COL_VERSION,	strVersion );
	pItem.Set( COL_PATH,	strPath );
	pItem.Set( COL_URL, 	strURL );
	pItem.Set( COL_EMAIL,	strEmail );
	pItem.Set( COL_INFO,	strDesc );
	pItem.Set( COL_FILENAME, strFilename );
	pItem.Set( COL_THUMBS,	strThumbs );

	/*int nItem =*/ pItem.Add( &m_wndList, -1, COL_LAST );

	return TRUE;
}

BOOL CCollectionExportDlg::Step1()
{
	// First wizard screen done
	CWaitCursor pCursor;

	// Change explanation and button captions
	m_wndExplain.SetWindowText( m_sLblExplain2 );
	m_wndOK.SetWindowText( m_sBtnExport );
	m_wndDelete.SetWindowText( m_sBtnBack );

	// Hide the first screen controls
	m_wndList.ShowWindow( FALSE );
	m_wndAuthor.ShowWindow( FALSE );
	m_wndName.ShowWindow( FALSE );
	m_wndDesc.ShowWindow( FALSE );
	m_wndLblAuthor.ShowWindow( FALSE );
	m_wndLblName.ShowWindow( FALSE );
	m_wndLblDesc.ShowWindow( FALSE );
	m_wndGroupBox.ShowWindow( FALSE );
	if ( m_wndWizard.GetSize() )	// We already viewed the second screen
	{
		m_wndWizard.ShowWindow( SW_SHOW );
		if ( ! m_wndWizard.IsValid() )
			m_wndOK.EnableWindow( FALSE );
		return TRUE;
	}

	// Find position of wizard control
	CRect rcReference1, rcReference2, rcNew;
	m_wndList.GetWindowRect( &rcReference1 );
	ScreenToClient( &rcReference1 );
	m_wndGroupBox.GetWindowRect( &rcReference2 );
	ScreenToClient( &rcReference2 );
	rcNew.left	= rcReference1.left;
	rcNew.top	= rcReference1.top;
	rcNew.bottom = rcReference1.bottom;
	rcNew.right	= rcReference2.right;

	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	m_sXMLPath = Settings.General.Path + _T("\\Templates\\") + m_wndList.GetItemText( nItem, COL_PATH );
	m_sNewFilename = m_wndList.GetItemText( nItem, COL_FILENAME );
	m_bThumbnails = m_wndList.GetItemText( nItem, COL_THUMBS ).CompareNoCase( _T("true") ) == 0;

	if ( ! m_wndWizard )
	{
		CSingleLock pLock( &Library.m_pSection, TRUE );
		m_wndWizard.Create( WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP, rcNew, this, IDC_WIZARD, m_sXMLPath, m_pFolder );
	}
	else
	{
		m_wndWizard.ShowWindow( TRUE );
	}

	if ( ! m_wndWizard.IsValid() )
		m_wndOK.EnableWindow( FALSE );

	return TRUE;
}

BOOL CCollectionExportDlg::Step2()
{
	// Second wizard screen done (Processing)
	CWaitCursor pCursor;

	const CString strPath = BrowseForFolder( _T("Select folder for output:") );
	if ( strPath.IsEmpty() )
		return FALSE;	// No folder selected

	CString strTitle;
	CStringA strXMLUTF8;

	{
		CSingleLock pLock( &Library.m_pSection );
		if ( ! SafeLock( pLock ) )
			return ErrorMessage( _T("Library is currently busy.") );

		if ( ! m_pFolder || ! m_pFolder->GetFileCount() )
			return FALSE;	// Folder disappeared

		CAutoPtr< CXMLElement > pXML( m_pFolder->CreateXML() );
		if ( ! pXML )
			return FALSE;	// Out of memory

		strXMLUTF8 = UTF8Encode( pXML->ToString( TRUE, TRUE ) );
		if ( strXMLUTF8.IsEmpty() )
			return FALSE;	// Out of memory

		strTitle = m_pFolder->m_sName;	// For $title$
	}

	const CString strFile = strPath + _T("\\Collection.xml");

	CFile pFile;
	if ( ! pFile.Open( strFile, CFile::modeWrite|CFile::modeCreate ) )
		return ErrorMessage( _T("Failed to write file:"), strFile );	// File creation failed

	try
	{
		pFile.Write( (LPCSTR)strXMLUTF8, strXMLUTF8.GetLength() );
		pFile.Close();
	}
	catch ( CException* pException )
	{
		pFile.Abort();
		pException->Delete();
		return ErrorMessage( _T("Failed to write file:"), strFile );	// File write failed
	}

	for ( int nPosTpl = 0 ; nPosTpl < m_wndWizard.m_pTemplatePaths.GetSize() ; nPosTpl++ )
	{
		CString strTemplateName = m_wndWizard.m_pTemplatePaths.GetAt( nPosTpl );
		if ( strTemplateName.CompareNoCase( m_wndWizard.m_sMainFilePath ) == 0 )
			strTemplateName = m_sNewFilename;

		if ( strTemplateName.CompareNoCase( m_wndWizard.m_sEvenFilePath ) == 0 ||
			 strTemplateName.CompareNoCase( m_wndWizard.m_sOddFilePath ) == 0 )
			continue;

		const CString strNewFilePath = strPath + _T("\\") +
				( m_sNewFilename.Compare( _T("index.htm") ) != 0 ? m_sNewFilename :
				( strTemplateName.Left( strTemplateName.ReverseFind( _T('.') ) ) + _T(".htm") ) );

		CString strSource = LoadFile( DirFromPath( m_sXMLPath ) + _T("\\") +
			( strTemplateName.CompareNoCase( m_sNewFilename ) ? strTemplateName : m_wndWizard.m_sMainFilePath ) );

		// Substitute item IDs with the values from wizard edit boxes.
		// The phrase "Individual file replacement" --
		// when each file has a unique id substitution.
		for ( POSITION pos = m_wndWizard.m_pItems.GetStartPosition() ; pos != NULL ; )
		{
			CString strControlID, strMap;
			m_wndWizard.m_pItems.GetNextAssoc( pos, strControlID, strMap );

			int nPosVert = strMap.Find( _T('|') );
			const UINT nFileID = _ttoi( (LPCTSTR)strMap.Left( nPosVert ) );		// File # (starting 0)
			strMap = strMap.Mid( nPosVert + 1 );	// Remove first entry
			nPosVert = strMap.Find( _T('|') );
			const CString strReplaceID = _T("$") + strMap.Left( nPosVert ) + _T("$");	// Replacement ID from XML
			strMap = strMap.Mid( nPosVert + 1 );
			const UINT nControlID = _ttoi( (LPCTSTR)strControlID );

			const CEdit* pEdit = (CEdit*)m_wndWizard.GetDlgItem( nControlID );
			if ( ! pEdit || ! pEdit->IsKindOf( RUNTIME_CLASS( CEdit ) ) )
				return ErrorMessage( _T("Control placed badly:"), strControlID );

			CString strReplace;
			pEdit->GetWindowText( strReplace );

			if ( nFileID < (UINT)m_wndWizard.m_pFileDocs.GetSize() && strTemplateName.CompareNoCase( m_sNewFilename ) == 0 )
			{
				for ( int nPosDocs = 0 ; nPosDocs < m_wndWizard.m_pFileDocs.GetSize() ; )
				{
					CString strNewReplace = strReplace;

					// Remove path when default file changed
					if ( ! strMap.IsEmpty() && strReplace.Find( _T(':') ) != -1 )
						strNewReplace = strReplace.Mid( strReplace.ReverseFind( _T('\\') ) + 1 );

					// Ensure first char is not backslash (may be entered in XML)
					//if ( ! strMap.IsEmpty() && strNewReplace[ 1 ] == _T('\\') )
					//	strNewReplace = strNewReplace.Mid( 1 );

					// Single filepicker is replaced everywhere
					// e.g. various bullets may be identical
					if ( strMap.IsEmpty() || strMap == "s" )
					{
						ReplaceNoCase( m_wndWizard.m_pFileDocs.GetAt( nPosDocs++ ), strReplaceID, strNewReplace );
					}
					else if ( strMap == "m" )	// Individual file doc replacement; multi-file picker
					{
						strNewReplace.Replace( _T('\\'), _T('/') );
						ReplaceNoCase( m_wndWizard.m_pFileDocs.GetAt( nFileID ), strReplaceID, strNewReplace );
					}

					// Copy selected images
					if ( ! strMap.IsEmpty() )
					{
						CString strTarget, strSourceFile;

						// If default file left, add old value to target and destination, since it may contain a relative path.
						if ( strReplace.Find( _T(':') ) == -1 )
						{
							strReplace.Replace( _T('/'), _T('\\') );
							strTarget = strPath + _T('\\') + strReplace;
							strSourceFile = DirFromPath( m_sXMLPath ) + _T('\\') + strReplace;
						}
						else
						{
							strTarget = strPath + _T('\\') + strNewReplace;
							strSourceFile = strReplace;
						}
						// Check if destination file does not exists
						if ( GetFileAttributes( strTarget ) == INVALID_FILE_ATTRIBUTES )
						{
							// Create dirs recursively
							CreateDirectory( strTarget.Left( strTarget.ReverseFind( _T('\\') ) ) );
							if ( ! CopyFile( strSourceFile, strTarget, TRUE ) )
								return ErrorMessage( _T("Failed to write file to:"), strTarget );	// File disappeared
						}
					}

					if ( strMap == "m" )
						break;
				} // For each even/odd file
			}

			// Ordinary template ignores individual file replacements
			if ( ! strSource.IsEmpty() && strMap.IsEmpty() || strMap == "s" )
				ReplaceNoCase( strSource, strReplaceID, strReplace );
		} // For each wizard row

		// Combine file docs and embed in "main" template
		if ( strTemplateName.CompareNoCase( m_sNewFilename ) == 0 )
		{
			CString strResult;
			for ( int nPosFileDocs = 0 ; nPosFileDocs < m_wndWizard.m_pFileDocs.GetSize() ; nPosFileDocs++ )
			{
				if ( m_bThumbnails )
				{
					// Create thumbnails for every file
					CString strFilePath = m_wndWizard.m_pFilePaths.GetAt( nPosFileDocs );
					CImageFile pImageFile;
					if ( CThumbCache::Cache( strFilePath, &pImageFile ) )
					{
						CString strTarget;
						strTarget.Format( _T("%s\\thumbs\\%d.jpg"), strPath, nPosFileDocs + 1 );
						CreateDirectory( strTarget.Left( strTarget.ReverseFind( _T('\\') ) ) );
						if ( ! pImageFile.SaveToFile( strTarget, 85 ) )
							return ErrorMessage( _T("Failed to write file to:"), strTarget );	// File disappeared
					}
				}

				strResult += m_wndWizard.m_pFileDocs.GetAt( nPosFileDocs );
			}

			ReplaceNoCase( strSource, _T("$data$"), strResult );
			ReplaceNoCase( strSource, _T("$title$"), strTitle );	// Folder Name
		}

		const CStringA strSourceUTF8 = UTF8Encode( strSource );

		// Output to file
		CFile pNewFile;
		if ( ! pNewFile.Open( strNewFilePath, CFile::modeWrite|CFile::modeCreate ) )
			return ErrorMessage( _T("Failed to write file:"), strNewFilePath );

		try
		{
			pNewFile.Write( (LPCSTR)strSourceUTF8, strSourceUTF8.GetLength() );
			pNewFile.Close();
		}
		catch ( CException* pException )
		{
			// File write failed
			pNewFile.Abort();
			pException->Delete();
			return ErrorMessage( _T("Failed to write file:"), strNewFilePath );
		}
	} // For each template file

	// Copy all non-parsed files such as images, stylesheets etc.
	for ( int nPosImg = 0 ; nPosImg < m_wndWizard.m_pImagePaths.GetSize() ; nPosImg++ )
	{
		const CString strFileName = m_wndWizard.m_pImagePaths.GetAt( nPosImg );
		const CString strTarget = strPath + _T('\\') + strFileName;

		// Destination file does not exist
		if ( GetFileAttributes( strTarget ) == INVALID_FILE_ATTRIBUTES )
		{
			// Source file exists
			CString strSource = DirFromPath( m_sXMLPath ) + _T('\\') + strFileName;
			if ( GetFileAttributes( strSource ) != INVALID_FILE_ATTRIBUTES )
			{
				// Create dirs recursively
				CreateDirectory( strTarget.Left( strTarget.ReverseFind( _T('\\') ) ) );
				if ( ! CopyFile( strSource, strTarget, TRUE ) )
					return ErrorMessage( _T("Failed to write file to:"), strTarget );
			}
		}
	} // For Image/Resource

	CSkinDialog::OnOK();

	return TRUE;
}

BOOL CCollectionExportDlg::ErrorMessage(LPCTSTR pszError, LPCTSTR pszTarget /*NULL*/)
{
	// File unwriteable
	CString strError = pszError;
	if ( *pszTarget )
		strError += (CString)_T("  ") + pszTarget;
	theApp.Message( MSG_ERROR, _T("%s  %s"), pszError, pszTarget );
	MsgBox( strError, MB_OK | MB_ICONERROR );
	return FALSE;	// Always False (Failure)
}

void CCollectionExportDlg::OnOK()
{
	switch ( m_nStep )
	{
	case 1:
		if ( ! Step1() )
			return;
		break;

	case 2:
		if ( ! Step2() )
			return;
		break;
	}
	m_nStep++;
}

void CCollectionExportDlg::OnTemplatesDeleteOrBack()
{
	m_wndOK.EnableWindow( TRUE );	// Enable if template was invalid
	switch ( m_nStep )
	{
	case 1:	// The first screen -- button "Delete"
		{
			if ( m_nSelected < 0 ) return;

			CString strName = m_wndList.GetItemText( m_nSelected, 0 );
			CString strBase = m_wndList.GetItemText( m_nSelected, 3 );

			CString strPrompt;
			strPrompt.Format( LoadString( IDS_TEMPLATE_DELETE ), (LPCTSTR)strName );

			if ( MsgBox( strPrompt, MB_ICONQUESTION|MB_OKCANCEL|MB_DEFBUTTON2 ) != IDOK ) return;

			CString strPath;
			strPath.Format( _T("%s\\Templates\\%s"),
				(LPCTSTR)Settings.General.Path, (LPCTSTR)strBase );

			DeleteFileEx( strPath, FALSE, TRUE, TRUE );

			int nSlash = strPath.ReverseFind( _T('\\') );
			strPath = strPath.Left( nSlash ) + _T("\\*.xml");

			WIN32_FIND_DATA pFind;
			HANDLE hSearch = FindFirstFile( strPath, &pFind );

			if ( hSearch != INVALID_HANDLE_VALUE )
			{
				FindClose( hSearch );
			}
			else
			{
				strPath = strPath.Left( strPath.GetLength() - 3 ) + _T("*");
				hSearch = FindFirstFile( strPath, &pFind );

				if ( hSearch != INVALID_HANDLE_VALUE )
				{
					strPath = strPath.Left( strPath.GetLength() - 3 );

					do
					{
						if ( pFind.cFileName[0] == '.' ) continue;
						DeleteFileEx( strPath + pFind.cFileName, FALSE, TRUE, TRUE );
					}
					while ( FindNextFile( hSearch, &pFind ) );

					FindClose( hSearch );
				}

				strPath = strPath.Left( strPath.GetLength() - 1 );
				RemoveDirectory( strPath );
			}

			m_wndList.DeleteItem( m_nSelected );
			m_wndName.SetWindowText( _T("") );
			m_wndAuthor.SetWindowText( _T("") );
			m_wndDesc.SetWindowText( _T("") );
			m_wndDelete.EnableWindow( FALSE );

			m_nSelected = -1;
		}
		break;
	case 2: // The second screen -- button "Back"
		{
			// Change explanation and button captions
			m_wndDelete.SetWindowText( m_sBtnDelete );
			m_wndExplain.SetWindowText( m_sLblExplain1 );
			m_wndOK.SetWindowText( m_sBtnNext );

			// Show the first screen controls
			m_wndList.ShowWindow( TRUE );
			m_wndAuthor.ShowWindow( TRUE );
			m_wndName.ShowWindow( TRUE );
			m_wndDesc.ShowWindow( TRUE );
			m_wndLblAuthor.ShowWindow( TRUE );
			m_wndLblName.ShowWindow( TRUE );
			m_wndLblDesc.ShowWindow( TRUE );
			m_wndGroupBox.ShowWindow( TRUE );

			// Hide wizard control
			m_wndWizard.ShowWindow( SW_HIDE );

			m_nStep--;
		}
		break;
	}
}

void CCollectionExportDlg::OnItemChangedTemplates(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	*pResult = 0;

	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == m_nSelected ) return; 	// Selection is the same

	// Selection changed, destroy control
	m_wndWizard.DestroyWindow();

	m_nSelected = nItem;

	if ( nItem >= 0 )
	{
		m_wndName.SetWindowText( m_wndList.GetItemText( nItem, 0 ) );
		m_wndAuthor.SetWindowText( m_wndList.GetItemText( nItem, COL_AUTHOR ) );
		m_wndDesc.SetWindowText( m_wndList.GetItemText( nItem, COL_INFO ) );
		m_wndDelete.EnableWindow( TRUE );
		m_wndOK.EnableWindow( TRUE );
	}
	else
	{
		m_wndName.SetWindowText( _T("") );
		m_wndAuthor.SetWindowText( _T("") );
		m_wndDesc.SetWindowText( _T("") );
		m_wndDelete.EnableWindow( FALSE );
		m_wndOK.EnableWindow( FALSE );
	}
}

HBRUSH CCollectionExportDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( m_nSelected >= 0 )
	{
		if ( pWnd == &m_wndName )
		{
			if ( m_wndList.GetItemText( m_nSelected, COL_URL ).GetLength() )
			{
				pDC->SetTextColor( Colors.m_crTextLink );
				pDC->SelectObject( &theApp.m_gdiFontLine );
			}
		}
		else if ( pWnd == &m_wndAuthor )
		{
			if ( m_wndList.GetItemText( m_nSelected, COL_EMAIL ).GetLength() )
			{
				pDC->SetTextColor( Colors.m_crTextLink );
				pDC->SelectObject( &theApp.m_gdiFontLine );
			}
		}
	}

	return hbr;
}

BOOL CCollectionExportDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( m_nSelected >= 0 && m_nStep == 1 )
	{
		CPoint point;
		GetCursorPos( &point );

		CRect rc;
		m_wndName.GetWindowRect( &rc );

		if ( rc.PtInRect( point ) )
		{
			if ( m_wndList.GetItemText( m_nSelected, COL_URL ).GetLength() )
				SetCursor( theApp.LoadCursor( IDC_HAND ) );
			return TRUE;
		}

		m_wndAuthor.GetWindowRect( &rc );

		if ( rc.PtInRect( point ) )
		{
			if ( m_wndList.GetItemText( m_nSelected, COL_EMAIL ).GetLength() )
				SetCursor( theApp.LoadCursor( IDC_HAND ) );
			return TRUE;
		}
	}

	return CSkinDialog::OnSetCursor( pWnd, nHitTest, message );
}

void CCollectionExportDlg::OnLButtonUp(UINT /*nFlags*/, CPoint point)
{
	if ( m_nSelected < 0 ) return;

	CRect rc;
	m_wndName.GetWindowRect( &rc );

	ClientToScreen( &point );

	if ( rc.PtInRect( point ) )
	{
		CString strURL = m_wndList.GetItemText( m_nSelected, COL_URL );

		if ( ! strURL.IsEmpty() )
			ShellExecute( GetSafeHwnd(), _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL );

		return;
	}

	m_wndAuthor.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		CString strEmail = m_wndList.GetItemText( m_nSelected, COL_EMAIL );

		if ( ! strEmail.IsEmpty() )
			ShellExecute( GetSafeHwnd(), _T("open"), _T("mailto:") + strEmail, NULL, NULL, SW_SHOWNORMAL );

		return;
	}
}

BOOL CCollectionExportDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		if ( m_wndWizard && m_wndWizard.IsWindowVisible() && m_wndWizard.OnTab() )
			return TRUE;	// ToDo: When template is invalid tab key does not work.
	}

	return CSkinDialog::PreTranslateMessage( pMsg );
}

CString CCollectionExportDlg::DirFromPath(LPCTSTR szPath)
{
	CString strDir(szPath);
	int nIndex( strDir.ReverseFind( _T('\\') ) );

	if ( nIndex != -1 )
		strDir = strDir.Left( nIndex );

	return strDir;
}
