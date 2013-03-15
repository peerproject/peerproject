//
// DlgCollectionExport.cpp
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
#include "Settings.h"
#include "PeerProject.h"
#include "DlgCollectionExport.h"

#include "Library.h"
#include "LibraryFolders.h"
#include "AlbumFolder.h"
#include "SharedFile.h"
#include "LiveList.h"
#include "Colors.h"
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


CCollectionExportDlg::CCollectionExportDlg(CAlbumFolder* pFolder, CWnd* pParent) : CSkinDialog(CCollectionExportDlg::IDD, pParent)
{
	m_pFolder = pFolder;
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

	// Show template name, author and version number columns, hide the rest info
	m_wndList.InsertColumn( COL_NAME,	_T("Name"), LVCFMT_LEFT, 134, 0 );
	m_wndList.InsertColumn( COL_AUTHOR,	_T("Author"), LVCFMT_LEFT, 114, 1 );
	m_wndList.InsertColumn( COL_VERSION, _T("Version"), LVCFMT_LEFT, 32, 2 );
	m_wndList.InsertColumn( COL_PATH,	_T("Path"), LVCFMT_LEFT, 0, 3 );
	m_wndList.InsertColumn( COL_URL,	_T("URL"), LVCFMT_LEFT, 0, 4 );
	m_wndList.InsertColumn( COL_EMAIL,	_T("Email"), LVCFMT_LEFT, 0, 5 );
	m_wndList.InsertColumn( COL_INFO,	_T("Description"), LVCFMT_LEFT, 0, 6 );

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
			if ( pFind.cFileName[0] == '.' ) continue;

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

	CXMLElement* pManifest = pXML->GetElementByName( _T("manifest") );

	if ( ! pXML->IsNamed( _T("template") ) || pManifest == NULL )
	{
		delete pXML;
		return FALSE;
	}

	CString strIcon		= pManifest->GetAttributeValue( _T("icon") );
	CString strName		= pManifest->GetAttributeValue( _T("name"), pszName );
	CString strAuthor	= pManifest->GetAttributeValue( _T("author"), _T("-") );
	CString strVersion	= pManifest->GetAttributeValue( _T("version"), _T("-") );
	CString strURL		= pManifest->GetAttributeValue( _T("link") );
	CString strEmail	= pManifest->GetAttributeValue( _T("email") );
	CString strDesc		= pManifest->GetAttributeValue( _T("description") );

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

	if ( strEmail.Find( '@' ) < 1 )
		strEmail.Empty();

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

	/*int nItem =*/ pItem.Add( &m_wndList, -1, COL_LAST );

	return TRUE;
}

void CCollectionExportDlg::OnOK()
{
	switch ( m_nStep )
	{
	case 1:	// The first wizard screen
		{
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
			if ( m_wndWizard.m_pControls.GetSize() )	// We already viewed the second screen
			{
				m_wndWizard.ShowWindow( SW_SHOW );
				if ( ! m_wndWizard.m_bValid )
					m_wndOK.EnableWindow( FALSE );
				break;
			}

			// Find position of wizard control
			CRect rcReference1, rcReference2, rcNew;
			m_wndList.GetWindowRect( &rcReference1 );
			ScreenToClient( &rcReference1 );
			m_wndGroupBox.GetWindowRect( &rcReference2 );
			ScreenToClient( &rcReference2 );
			rcNew.left = rcReference1.left;
			rcNew.top = rcReference1.top;
			rcNew.bottom = rcReference1.bottom;
			rcNew.right = rcReference2.right;
			int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
			CString strXML = Settings.General.Path + _T("\\Templates\\") + m_wndList.GetItemText( nItem, 3 );
			if ( ! m_wndWizard )
			{
				CWaitCursor pCursor;
				m_wndWizard.Create( WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,
					rcNew, this, IDC_WIZARD, strXML, m_pFolder );
				pCursor.Restore();
			}
			else
			{
				m_wndWizard.ShowWindow( TRUE );
			}

			if ( ! m_wndWizard.m_bValid )
				m_wndOK.EnableWindow( FALSE );
		}
		break;

	case 2:	// The second wizard screen
		CString strPath = BrowseForFolder( _T("Select folder for output:") );
		if ( strPath.IsEmpty() )
		{
			m_nStep--;	// Do not increment at the end of case
			break;
		}

		CSingleLock pLock( &Library.m_pSection, TRUE );

		if ( m_pFolder && m_pFolder->GetFileCount() )
		{
			CXMLElement* pXML = m_pFolder->CreateXML();
			CString strXML = pXML->ToString( TRUE, TRUE );
			delete pXML;

			CString strFile = strPath + _T("\\Collection.xml");
			CFile pFile;

			if ( pFile.Open( strFile, CFile::modeWrite|CFile::modeCreate ) )
			{
				CStringA strXMLUTF8 = UTF8Encode( strXML );

				pFile.Write( (LPCSTR)strXMLUTF8, strXMLUTF8.GetLength() );
				pFile.Close();

				int nPosTpl = 0;
				while ( nPosTpl < m_wndWizard.m_pTemplatePaths.GetSize() )
				{
					CString strTemplateName, strSource, strNewFilePath;

					strTemplateName = m_wndWizard.m_pTemplatePaths.GetAt( nPosTpl++ );
					if ( strTemplateName == m_wndWizard.m_sMainFilePath )
						strTemplateName = _T("index.htm");

					if ( strTemplateName != m_wndWizard.m_sEvenFilePath &&
							strTemplateName != m_wndWizard.m_sOddFilePath )
					{
						strNewFilePath = strPath + _T("\\") +
							strTemplateName.Left( strTemplateName.ReverseFind( '.' ) ) + _T(".htm");

						CString strTemplatePath;
						if ( strTemplateName != "index.htm" )
							strTemplatePath = DirFromPath( m_wndWizard.m_sXMLPath ) + _T("\\") + strTemplateName;
						else
							strTemplatePath = DirFromPath( m_wndWizard.m_sXMLPath ) + _T("\\") + m_wndWizard.m_sMainFilePath;

						strSource = LoadFile( strTemplatePath );
					}
					else
						continue;

					// Substitute item IDs with the values from wizard edit boxes.
					// The phrase "Individual file replacement" --
					// when each file has a unique id substitution.
					POSITION pos = m_wndWizard.m_pItems.GetStartPosition();
					while ( pos != NULL )
					{
						CString strControlID, strMap, str, strReplaceID;
						UINT nControlID, nFileID;

						m_wndWizard.m_pItems.GetNextAssoc( pos, strControlID, strMap );
						int nPosVert = strMap.Find('|');
						str = strMap.Left( nPosVert );
						nFileID = _ttoi( (LPCTSTR)str );		// File # starting 0
						strMap = strMap.Mid( nPosVert + 1 );	// Remove first entry
						nPosVert = strMap.Find('|');
						strReplaceID = strMap.Left( nPosVert );	// Replacement ID from XML
						str.Format( _T("$%s$"), strReplaceID );
						strMap = strMap.Mid( nPosVert + 1 );
						nControlID = _ttoi( (LPCTSTR)strControlID );

						CEdit* pEdit = (CEdit*)m_wndWizard.GetDlgItem( nControlID );
						CString strReplace;
						if ( pEdit->IsKindOf( RUNTIME_CLASS( CEdit ) ) )
						{
							pEdit->GetWindowText( strReplace );
						}
						else // Something wrong
						{
							MsgBox( _T("BUG: Controls placed badly.") );
							break;
						}

						if ( nFileID < (UINT)m_wndWizard.m_pFileDocs.GetSize() &&
							strTemplateName == "index.htm" )
						{
							int nPosDocs = 0;
							while ( nPosDocs < m_wndWizard.m_pFileDocs.GetSize() )
							{
								CString strNew, strNewReplace;

								// Ensure that first char is not backslash (may be entered in XML)
								//if ( ! strMap.IsEmpty() && strReplace.Left( 1 ) == '\\' )
								//	strReplace = strReplace.Mid( 1 );

								// Remove path when default file changed
								if ( ! strMap.IsEmpty() && strReplace.Find(':') != -1 )
									strNewReplace = strReplace.Mid( strReplace.ReverseFind('\\') + 1 );
								else strNewReplace = strReplace;

								// Single filepicker is replaced everywhere
								// e.g. various bullets may be identical
								if ( strMap.IsEmpty() || strMap == "s" )
								{
									ReplaceNoCase( m_wndWizard.m_pFileDocs.GetAt( nPosDocs++ ),
										str, strNewReplace );
								}
								else if ( strMap == "m" )	// Individual file doc replacement; multi-file picker
								{
									strNewReplace.Replace( '\\', '/' );
									ReplaceNoCase( m_wndWizard.m_pFileDocs.GetAt( nFileID ),
										str, strNewReplace );
								}
								// Copy selected images
								if ( ! strMap.IsEmpty() )
								{
									CString strTarget, strSourceFile;

									// If default file left, add old value to target and destination, since it may contain a relative path.
									if ( strReplace.Find(':') == -1 )
									{
										strReplace.Replace( '/', '\\' );
										strTarget = strPath + _T('\\') + strReplace;
										strSourceFile = DirFromPath( m_wndWizard.m_sXMLPath ) + _T('\\') + strReplace;
									}
									else
									{
										strTarget = strPath + _T('\\') + strNewReplace;
										strSourceFile = strReplace;
									}
									// Check if destination file does not exists
									if ( GetFileAttributes( strTarget ) == 0xFFFFFFFF )
									{
										// Create dirs recursively
										CreateDirectory( strTarget.Left( strTarget.ReverseFind( _T('\\') ) ) );
										if ( ! CopyFile( strSourceFile, strTarget, TRUE ) )
											MsgBox( _T("ToDo: File disappeared: \n") + strReplace );
									}
								}
								if ( strMap == "m" ) break;
							} // While each even/odd file
						}

						// Ordinary template ignores individual file replacements
						if ( ! strSource.IsEmpty() && strMap.IsEmpty() || strMap == "s" )
							ReplaceNoCase( strSource, str, strReplace );
					} // While each wizard row

					// Combine file docs and embed in "main" template
					if ( strTemplateName == "index.htm" )
					{
						CString strResult;
						int nPosDocs2 = 0;
						while ( nPosDocs2 < m_wndWizard.m_pFileDocs.GetSize() )
							strResult += m_wndWizard.m_pFileDocs.GetAt( nPosDocs2++ );
						ReplaceNoCase( strSource, _T("$data$"), strResult );
						strResult.Empty();
						strResult.ReleaseBuffer();
					}

					// Output to file
					CFile pNewFile;
					if ( pNewFile.Open( strNewFilePath, CFile::modeWrite|CFile::modeCreate ) )
					{
						CStringA strSourceUTF8 = UTF8Encode( strSource );

						pNewFile.Write( (LPCSTR)strSourceUTF8, strSourceUTF8.GetLength() );
						pNewFile.Close();

						// Clean-up;
						strSource.Empty();
						strSource.ReleaseBuffer();
					}
				} // While each template file

				// Copy all non-parsed files such as images, stylesheets etc.
				int nPosImg = 0;
				while ( nPosImg < m_wndWizard.m_pImagePaths.GetSize() )
				{
					CString strTarget, strFileName;
					strFileName = m_wndWizard.m_pImagePaths.GetAt( nPosImg++ );
					strTarget = strPath + _T('\\') + strFileName;

					// Destination file does not exists
					if ( GetFileAttributes( strTarget ) == 0xFFFFFFFF )
					{
						CString strSource;
						strSource = DirFromPath( m_wndWizard.m_sXMLPath ) +
								_T('\\') + strFileName;
						// Source file exists
						if ( GetFileAttributes( strSource ) != 0xFFFFFFFF )
						{
							// Create dirs recursively
							CreateDirectory( strTarget.Left( strTarget.ReverseFind( _T('\\') ) ) );
							if ( ! CopyFile( strSource, strTarget, TRUE ) )
								MsgBox( _T("ToDo: Can't write to ") + strFile );
						}
					}
				} // While Image

				CSkinDialog::OnOK();
			}
			else
			{
				pLock.Unlock();
				MsgBox( _T("ToDo: Can't write to ") + strFile );
				m_nStep--;
			}
		}
		else
		{
			pLock.Unlock();
			MsgBox( _T("ToDo: Folder disappeared.") );
			m_nStep--;
		}
		break;	// case 2:
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

			int nSlash = strPath.ReverseFind( '\\' );
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

	ClientToScreen( &point );

	CRect rc;
	m_wndName.GetWindowRect( &rc );

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
	if ( m_wndWizard && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		if ( m_wndWizard.IsWindowVisible() && m_wndWizard.OnTab() )
			return TRUE;	// ToDo: When template is invalid tab key does not work.
	}

	return CSkinDialog::PreTranslateMessage( pMsg );
}

CString CCollectionExportDlg::DirFromPath(LPCTSTR szPath)
{
	CString strDir(szPath);
	int nIndex( strDir.ReverseFind( '\\' ) );

	if ( nIndex != -1 )
		strDir = strDir.Left( nIndex );

	return strDir;
}
