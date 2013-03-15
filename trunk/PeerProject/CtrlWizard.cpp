//
// CtrlWizard.cpp
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
#include "CtrlWizard.h"

#include "Library.h"
#include "LibraryFolders.h"
#include "AlbumFolder.h"
#include "CtrlIconButton.h"
#include "Transfer.h"
#include "SharedFile.h"
#include "Colors.h"
#include "Skin.h"
#include "XML.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CWizardCtrl, CWnd)
CWizardCtrl::CWizardCtrl()
	: m_nCaptionWidth	( 160 )
	, m_nItemHeight 	( 32 )
	, m_nScroll 		( 0 )
	, m_bShowBorder		( TRUE )
{
}

CWizardCtrl::~CWizardCtrl()
{
	m_pTemplatePaths.RemoveAll();
	m_pImagePaths.RemoveAll();
}

BEGIN_MESSAGE_MAP(CWizardCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_WIZARD_CONTROL, OnBtnPress)
END_MESSAGE_MAP()

void CWizardCtrl::Clear()
{
	m_pCaptions.RemoveAll();
	m_pFileDocs.RemoveAll();
	m_pItems.RemoveAll();
	for ( INT_PTR nCount = 0 ; nCount < m_pControls.GetCount() ; nCount++ )
	{
		CWnd* pWnd = m_pControls.ElementAt( nCount );
		pWnd->DestroyWindow();
		delete pWnd;
	}
	m_pControls.RemoveAll();
}

BOOL CWizardCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPCTSTR pszXMLPath, CAlbumFolder* pFolder)
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_CLIPCHILDREN;
	m_sXMLPath = pszXMLPath;
	m_bValid = FALSE;
	m_pFolder = pFolder;
	return CWnd::Create( NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL );
}

void CWizardCtrl::OnPaint()
{
	CRect rcClient, rcItem;
	int nOffsetX, nOffsetY;
	CPaintDC dc( this );	// Device context for painting

	GetClientRect( &rcClient );
	rcItem.CopyRect( &rcClient );
	dc.FillSolidRect( &rcItem, Colors.m_crWindow );
	CFont* pOldFont = (CFont*)dc.SelectObject( &theApp.m_gdiFont );
	dc.SetBkMode( OPAQUE );

	if ( m_pControls.GetSize() > 0 && m_bValid )
	{
		rcItem.bottom = rcItem.top + m_nItemHeight + 4;
		rcItem.OffsetRect( 0, -m_nScroll );

		nOffsetY = m_nItemHeight;
		nOffsetY = nOffsetY / 2 - dc.GetTextExtent( _T("Xg") ).cy / 2 - 1;
		int nRows = 0;

		for ( INT_PTR nControl = 0 ; nControl < m_pControls.GetSize() ; nControl++ )
		{
			float nFactor = 1;
			CWnd* pControl	= m_pControls.GetAt( nControl );
			if ( ! pControl->IsKindOf( RUNTIME_CLASS( CIconButtonCtrl ) ) )
			{
				CString strUINT, strValue;
				strUINT.Format( _T("%d"), IDC_WIZARD_CONTROL + nControl );
				m_pItems.Lookup( strUINT, strValue );
				if ( strValue.Right( 1 ) == "m" )	// Make more space for multipicker rows
				{
					nFactor = 1.5;
					rcItem.bottom += (int)(m_nItemHeight * nFactor);
				}

				dc.SetBkColor( Colors.m_crSchemaRow[ nRows & 1 ] );
				// Draw row label
				dc.ExtTextOut( rcItem.left + 6, rcItem.top + nOffsetY, ETO_OPAQUE|ETO_CLIPPED,
					&rcItem, m_pCaptions.GetAt( nControl ), NULL );
				// Draw file name for multipicker row
				if ( nFactor != 1 )
				{
					CRect rcFileName;
					CString strFileName = m_pCaptions.GetAt( nControl + 1 );

					rcFileName.CopyRect( &rcItem );
					rcFileName.top += (int)(m_nItemHeight / 2 * nFactor) - 4;
					rcFileName.bottom += (int)(m_nItemHeight / 2 * nFactor) - 4;
					CSize size = dc.GetTextExtent( strFileName );
					if ( size.cx > rcFileName.Width() - 24 - 12 )
					{
						// Show only song name if possible
						int nDashPos = strFileName.ReverseFind( '-' );
						if ( nDashPos != -1 )
							strFileName = _T("\x2026") + strFileName.Right( strFileName.GetLength() - nDashPos - 1 );
					}
					DWORD dwOptions = Settings.General.LanguageRTL ? ETO_RTLREADING : 0;
					dc.ExtTextOut( rcFileName.left + 24, rcFileName.top + nOffsetY, ETO_CLIPPED|dwOptions,
						&rcFileName, strFileName, NULL );
				}

				rcItem.OffsetRect( 0, (int)(m_nItemHeight * nFactor) );
				nRows++;
			}
		}
	}
	else  // No customizations
	{
		CString str;
		if ( ! m_bValid )
			LoadString( str, IDS_COLLECTION_WIZARD_NOTVALID );
		else
			LoadString( str, IDS_COLLECTION_WIZARD_NOCUSTOM );
		nOffsetX = rcClient.Width() / 2 - dc.GetTextExtent( str ).cx / 2 - 1;
		nOffsetY = rcClient.Height() / 2 - dc.GetTextExtent( _T("Xg") ).cy / 2 - 1;
		dc.ExtTextOut( rcItem.left + nOffsetX, rcItem.top + nOffsetY, ETO_OPAQUE|ETO_CLIPPED,
					   &rcItem, str, NULL );
	}

	dc.SelectObject( pOldFont );
}

int CWizardCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;
	return 0;
}

BOOL CWizardCtrl::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void CWizardCtrl::OnNcPaint()
{
	CWnd::OnNcPaint();

	if ( m_bShowBorder )
	{
		CWindowDC dc( this );

		CRect rc;
		GetWindowRect( &rc );
		rc.OffsetRect( -rc.left, -rc.top );
		dc.Draw3dRect( &rc, Colors.m_crSysActiveCaption, Colors.m_crSysActiveCaption );
	}
}

void CWizardCtrl::ScrollBy(int nDelta)
{
	int nBefore = m_nScroll;

	m_nScroll += nDelta;
	m_nScroll = max( 0, min( GetScrollLimit( SB_VERT ), m_nScroll ) );
	nDelta = m_nScroll - nBefore;

	for ( CWnd* pWnd = GetWindow( GW_CHILD ) ; pWnd ; pWnd = pWnd->GetNextWindow() )
	{
		pWnd->ModifyStyle( WS_VISIBLE, 0 );
	}

	ScrollWindowEx( 0, -nDelta, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN|SW_INVALIDATE );
	Layout();
	UpdateWindow();
}

void CWizardCtrl::OnVScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
	SCROLLINFO pScroll = {};

	pScroll.cbSize	= sizeof( pScroll );
	pScroll.fMask	= SIF_ALL;

	GetScrollInfo( SB_VERT, &pScroll );

	switch ( nSBCode )
	{
	case SB_TOP:
		m_nScroll = 0;
		break;
	case SB_BOTTOM:
		m_nScroll = pScroll.nMax - 1;
		break;
	case SB_LINEUP:
		m_nScroll -= 8;
		break;
	case SB_LINEDOWN:
		m_nScroll += 8;
		break;
	case SB_PAGEUP:
		m_nScroll -= pScroll.nPage;
		break;
	case SB_PAGEDOWN:
		m_nScroll += pScroll.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nScroll = pScroll.nTrackPos;
		break;
	}

	int nDelta = m_nScroll - pScroll.nPos;
	m_nScroll = pScroll.nPos;

	ScrollBy( nDelta );
}

namespace
{
struct CompareFiles
{
	bool operator()(CLibraryFile* lhs, CLibraryFile* rhs) const
	{
		return lhs->m_sName < rhs->m_sName;
	}
};
}

void CWizardCtrl::OnShowWindow(BOOL bShow, UINT /*nStatus*/)
{
	if ( bShow && m_pControls.IsEmpty() )
	{
		CString strXML = LoadFile( m_sXMLPath );
		if ( strXML.IsEmpty() ) return;

		CXMLElement* pBase = NULL;
		pBase = CXMLElement::FromString( strXML, FALSE );

		CollectFiles( pBase );
		CollectImages( pBase );

		// Sort Files
		// ToDo: Make sure all CLibraryFile*s remain valid throughout
		CQuickLock oLibraryLock( Library.m_pSection );

		std::vector< CLibraryFile* > pList;
		pList.reserve( m_pFolder->GetFolderCount() );
		for ( POSITION pos = m_pFolder->GetFileIterator() ; pos ; )
		{
			if ( CLibraryFile* pFile = m_pFolder->GetNextFile( pos ) )
				pList.push_back( pFile );
		}
		std::sort( pList.begin(), pList.end(), CompareFiles() );

		MakeControls( pBase, pList );
		if ( pBase ) delete pBase;

		// When no multi-pickers in XML prepare docs separately
		if ( m_pFileDocs.GetCount() == 0 )
		{
			CString strTemplatePath = m_sXMLPath.Left( m_sXMLPath.ReverseFind( _T('\\') ) + 1 );
			CString strOddTemplate  = LoadFile( strTemplatePath + m_sOddFilePath );
			CString strEvenTemplate = LoadFile( strTemplatePath + m_sEvenFilePath );

			int nFileCount = 1;
			for ( std::size_t pos = 0 ; pos != pList.size() ; ++pos )
			{
				pList[ pos ]->PrepareDoc( ( nFileCount & 1 ) ? strOddTemplate : strEvenTemplate, m_pFileDocs );
				++nFileCount;
			}
		}
		Layout();
	}
	Invalidate();
}

// Create docs paths collection
BOOL CWizardCtrl::CollectFiles(CXMLElement* pBase)
{
	if ( pBase == NULL ) return FALSE;

	CXMLElement* pTemplate = pBase->GetElementByName( _T("files"), FALSE );
	if ( ! pTemplate ) return FALSE;

	for ( POSITION pos = pTemplate->GetElementIterator() ; pos ; )
	{
		CXMLElement* pItem = pTemplate->GetNextElement( pos );
		if ( pItem->IsNamed( _T("file") ) )
		{
			CString strType = pItem->GetAttributeValue( _T("type") );
			CString strPath = pItem->GetAttributeValue( _T("path") );

			if ( strType == "main" )
			{
				if ( ! m_bValid )	// Use only the first file of type "main"
				{
					m_sMainFilePath = strPath;
					m_bValid = TRUE;
				}
			}
			else if ( strType == "evenfile" )
				m_sEvenFilePath = strPath;
			else if ( strType == "oddfile" )
				m_sOddFilePath = strPath;
			m_pTemplatePaths.Add( strPath );
		}
	}

	if ( m_sOddFilePath.IsEmpty() )
		m_sOddFilePath = m_sEvenFilePath;

	return TRUE;
}

// Create images paths collection
BOOL CWizardCtrl::CollectImages(CXMLElement* pBase)
{
	if ( pBase == NULL ) return FALSE;

	CXMLElement* pTemplate = pBase->GetElementByName( _T("images"), FALSE );
	if ( ! pTemplate ) return FALSE;

	for ( POSITION pos = pTemplate->GetElementIterator() ; pos ; )
	{
		CXMLElement* pItem = pTemplate->GetNextElement( pos );
		if ( pItem->IsNamed( _T("image") ) )
			m_pImagePaths.Add( pItem->GetAttributeValue( _T("path") ) );
	}

	return TRUE;
}

// Here we create controls, captions and docs collections
BOOL CWizardCtrl::MakeControls(CXMLElement* pBase, std::vector< CLibraryFile* > pList)
{
	if ( pBase == NULL ) return FALSE;

	CString strTemplatePath = m_sXMLPath.Left( m_sXMLPath.ReverseFind( _T('\\') ) + 1 );
	CString strOddTemplate  = LoadFile( strTemplatePath + m_sOddFilePath );
	CString strEvenTemplate = LoadFile( strTemplatePath + m_sEvenFilePath );

	CXMLElement* pTemplate = pBase->GetElementByName( _T("wizardtext"), FALSE );
	if ( ! pTemplate ) return FALSE;

	CWnd* pControl = NULL;
	CRect rc;

	for ( POSITION posTemplate = pTemplate->GetElementIterator() ; posTemplate ; )
	{
		CXMLElement* pLangGroup = pTemplate->GetNextElement( posTemplate );
		CString strLang = pLangGroup->GetAttributeValue( _T("language") );

		// Collect only english and language specific data
		if ( pLangGroup->IsNamed( _T("text") ) &&
			( strLang == Settings.General.Language || ( strLang == "en" && m_pControls.IsEmpty() ) ) )
		{
			// If english data loaded but language specific data found later,
			// then empty controls, captions, and docs collections
			if ( strLang != "en" ) Clear();

			int nItemCount = 0;
			for ( POSITION posLangGroup = pLangGroup->GetElementIterator() ; posLangGroup ; )
			{
				CXMLElement* pItem = pLangGroup->GetNextElement( posLangGroup );
				if ( pItem->IsNamed( _T("item") ) )
				{
					CString strType = pItem->GetAttributeValue( _T("type") );
					bool bFilePickers = ( strType == "single-filepicker" || strType == "multi-filepicker" );
					bool bPickers = ( bFilePickers || strType == "colorpicker" );

					if ( strType == "textbox" || bPickers )
					{
						// Do we need to check if the current text id is actually used in EvenFile.tpl and OddFile.tpl ?
						int nFileCount = 0;

						for ( std::size_t pos = 0 ; pos != pList.size() ; ++pos )
						{
							CEdit* pEdit = new CEdit();

							DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL;
							if ( bPickers ) dwStyle |= ES_READONLY;
							pEdit->Create( dwStyle, rc, this, IDC_WIZARD_CONTROL + nItemCount );
							pEdit->ModifyStyleEx( 0, WS_EX_CLIENTEDGE );
							if ( bPickers && Settings.General.LanguageRTL )
								pEdit->ModifyStyleEx( WS_EX_RTLREADING, WS_EX_RIGHT, 0 );

							CString strText = pItem->GetAttributeValue( _T("default") );
							if ( strType == "colorpicker" ) strText.MakeUpper();
							pEdit->SetWindowText( strText );

							pControl = pEdit;
							strText = pItem->GetAttributeValue( _T("value") ) + ':';

							m_pCaptions.Add( strText );
							m_pControls.Add( pControl );

							// Each edit control UINT is mapped to "file #|item ID in XML|filepicker type"
							CString strUINT;
							strUINT.Format( _T("%d"), IDC_WIZARD_CONTROL + nItemCount );
							strText.Format( _T("%d"), nFileCount );
							strText += _T("|") + pItem->GetAttributeValue( _T("id") ) + _T("|");
							if ( strType.Find( _T("multi-") ) != -1 )
								strText += 'm';
							else if ( strType.Find( _T("single-") ) != -1 )
								strText += 's';
							m_pItems.SetAt( strUINT, strText );
							// Is it correct?
							SetWindowLongPtr( pControl->GetSafeHwnd(), GWLP_USERDATA, (LONG_PTR)pItem );
							pControl->SetFont( &theApp.m_gdiFont );
							nItemCount++;

							if ( bPickers ) // Add buttons after control
							{
								CIconButtonCtrl* pButton = new CIconButtonCtrl();
								CString strCaption;
								pButton->Create( rc, this, IDC_WIZARD_CONTROL + nItemCount );
								if ( bFilePickers )
								{
									// A kind of hack to store button type
									pButton->SetText( _T(" ") );
									pButton->SetCoolIcon( IDI_BROWSE, Settings.General.LanguageRTL );
								}
								else if ( strType == "colorpicker" )
									pButton->SetCoolIcon( IDI_COLORS, Settings.General.LanguageRTL );
								pControl = pButton;
								// Store file name which will be displayed for each multipicker row
								strCaption = pList[ pos ]->m_sName;
								m_pCaptions.Add( strCaption );
								m_pControls.Add( pControl );
								nItemCount++;
								// Is it correct?
								SetWindowLongPtr( pControl->GetSafeHwnd(), GWLP_USERDATA, (LONG_PTR)pItem );
							}
							nFileCount++;

							if ( strType != "multi-filepicker" )
								break;

							if ( nFileCount > m_pFileDocs.GetCount() )
								pList[ pos ]->PrepareDoc( ( nFileCount & 1 ) ? strOddTemplate : strEvenTemplate, m_pFileDocs );
						} // for pos
					}
				}
			} // for posLangGroup
		}
	} // for posTemplate

	return TRUE;
}

void CWizardCtrl::Layout()
{
	CRect rcClient, rcNew;

	GetClientRect( &rcClient );

	SCROLLINFO pScroll = {};
	pScroll.cbSize	= sizeof( pScroll );
	pScroll.fMask	= SIF_PAGE|SIF_POS|SIF_RANGE|SIF_DISABLENOSCROLL;
	pScroll.nPage	= rcClient.Height();
	pScroll.nPos	= m_nScroll;
	if ( ! m_bValid )
	{
		pScroll.nMax = 0;
		SetScrollInfo( SB_VERT, &pScroll );
		return;
	}

	HDWP hDWP = BeginDeferWindowPos( static_cast< int >( m_pControls.GetSize() ) );

	int nTop = -m_nScroll;
	BOOL bSameRow = FALSE;

	for ( INT_PTR nControl = 0 ; nControl < m_pControls.GetSize() ; nControl++ )
	{
		CWnd* pControl = m_pControls.GetAt( nControl );
		float nFactor = 1;
		CString strUINT, strValue;

		if ( pControl->IsKindOf( RUNTIME_CLASS( CIconButtonCtrl ) ) )
		{
			// Place on previous row
			strUINT.Format( _T("%d"), IDC_WIZARD_CONTROL + nControl - 1 );
			m_pItems.Lookup( strUINT, strValue );
			if ( strValue.Right( 1 ) == "m" ) nFactor = 2;

			rcNew.left = rcClient.right - 35;
			rcNew.right = rcClient.right - 9;
			rcNew.top = nTop - (int)(m_nItemHeight / 2 * nFactor) - 13;
			rcNew.bottom = nTop - (int)(m_nItemHeight / 2 * nFactor) + 13;
			bSameRow = TRUE;
			nFactor = 1;
		}
		else if ( m_nCaptionWidth )
		{
			strUINT.Format( _T("%d"), IDC_WIZARD_CONTROL + nControl );
			m_pItems.Lookup( strUINT, strValue );
			if ( strValue.Right( 1 ) == "m" )
			{
				strUINT.Format( _T("%d"), IDC_WIZARD_CONTROL + nControl - 2 );
				m_pItems.Lookup( strUINT, strValue );
				if ( strValue.Right( 1 ) == "m" )	// Not the first multipicker row
					nFactor = 1.5;
			}
			rcNew.left		= m_nCaptionWidth;
			rcNew.right		= rcClient.right - 40;
			rcNew.top		= nTop + m_nItemHeight / 2 - 9;
			rcNew.bottom	= nTop + m_nItemHeight / 2 + 9;
			bSameRow = FALSE;
		}

		hDWP = DeferWindowPos( hDWP, pControl->GetSafeHwnd(), NULL, rcNew.left, rcNew.top,
			rcNew.Width(), rcNew.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER );

		if ( ! bSameRow )
		{
			pScroll.nMax += (int)(m_nItemHeight * nFactor);
			nTop += (int)(m_nItemHeight * nFactor);
		}
	}

	EndDeferWindowPos( hDWP );

	pScroll.nMax--;
	SetScrollInfo( SB_VERT, &pScroll );
}

BOOL CWizardCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if ( HIWORD( wParam ) != BN_CLICKED )
		return CWnd::OnCommand( wParam, lParam );	// Is this right? (Old formatting confused)

	CIconButtonCtrl* pButton = (CIconButtonCtrl*)CWnd::FromHandle( (HWND)lParam );
	CEdit* pEdit = (CEdit*)( GetDlgItem( static_cast< int >( wParam - 1 ) ) );

	if ( ! pButton->IsKindOf( RUNTIME_CLASS(CIconButtonCtrl) ) &&
		 ! pEdit->IsKindOf( RUNTIME_CLASS(CEdit) ) )
		return TRUE;

	if ( pButton->GetWindowTextLength() )	// File picker
	{
		CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("All Files(*.*)|*.*||"), this );
		if ( dlg.DoModal() == IDOK )
			pEdit->SetWindowText( dlg.GetPathName() );
	}
	else // Color picker
	{
		// Find default color
		int r, g, b;
		CString str;
		pEdit->GetWindowText( str );
		COLORREF crColor = _stscanf( str, _T("#%2x%2x%2x"), &r, &g, &b ) != 3 ?
			RGB(0, 0, 0) : RGB(r, g, b);

		CColorDialog dlg( crColor, CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT, this );
		if ( dlg.DoModal() == IDOK )
		{
			crColor = dlg.GetColor();
			str.Format( _T("#%0.2x%0.2x%0.2x"),
					GetRValue(crColor), GetGValue(crColor), GetBValue(crColor) );
			pEdit->SetWindowText( str.MakeUpper() );	// No need to use CharUpper
		}
	}

	return CWnd::OnCommand( wParam, lParam );
}

void CWizardCtrl::OnBtnPress()
{
	GetOwner()->SendMessage( WM_COMMAND, MAKELONG( GetDlgCtrlID(), BN_CLICKED ), (LPARAM)GetSafeHwnd() );
}

/////////////////////////////////////////////////////////////////////////////
// CWizardCtrl focus movement

BOOL CWizardCtrl::OnTab()
{
	CWnd* pFocus	= GetFocus();
	CWnd* pPrevious	= NULL;

	BOOL bShift	= GetAsyncKeyState( VK_SHIFT ) & 0x8000;
	BOOL bNext	= FALSE;

	if ( pFocus == GetWindow( GW_HWNDPREV ) )
	{
		if ( bShift ) return FALSE;
		bNext = TRUE;
	}

	for ( INT_PTR nControl = 0 ; nControl < m_pControls.GetSize() ; nControl++ )
	{
		CWnd* pControl = m_pControls.GetAt( nControl );

		if ( bNext )
		{
			SetFocusTo( pControl );
			return TRUE;
		}

		if ( pControl == pFocus || pControl->GetWindow( GW_CHILD ) == pFocus )
		{
			if ( bShift )
			{
				if ( pPrevious )
				{
					SetFocusTo( pPrevious );
				}
				else
				{
					pFocus = GetWindow( GW_HWNDPREV );
					if ( pFocus ) pFocus->SetFocus();
				}

				return TRUE;
			}

			bNext = TRUE;
		}

		pPrevious = pControl;
	}

	if ( bNext )
	{
		pFocus = GetWindow( GW_HWNDNEXT );
		if ( pFocus == NULL ) GetWindow( GW_HWNDFIRST );
		if ( pFocus ) pFocus->SetFocus();
		return TRUE;
	}

	return FALSE;
}

void CWizardCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	if ( ! m_pControls.IsEmpty() )
	{
		CWnd* pWnd = m_pControls.GetAt( 0 );
		SetFocusTo( pWnd );
	}
}

void CWizardCtrl::SetFocusTo(CWnd* pControl)
{
	CRect rcClient, rcControl;

	GetClientRect( &rcClient );
	pControl->GetWindowRect( &rcControl );
	ScreenToClient( &rcControl );

	if ( rcControl.top < rcClient.top )
		ScrollBy( rcControl.top - rcClient.top - 8 );
	else if ( rcControl.bottom > rcClient.bottom )
		ScrollBy( rcControl.bottom - rcClient.bottom + 8 );

	pControl->SetFocus();
}

void CWizardCtrl::OnDestroy()
{
	Clear();
	m_nScroll = 0;
}
