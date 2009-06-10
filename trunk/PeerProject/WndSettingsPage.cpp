//
// WndSettingsPage.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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
#include "Settings.h"
#include "WndSettingsSheet.h"
#include "WndSettingsPage.h"

#include "Skin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSettingsPage, CDialog)

BEGIN_MESSAGE_MAP(CSettingsPage, CDialog)
	//{{AFX_MSG_MAP(CSettingsPage)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSettingsPage construction

CSettingsPage::CSettingsPage(UINT nIDTemplate, LPCTSTR pszCaption) : CDialog( nIDTemplate )
{
	//{{AFX_DATA_INIT(CSettingsPage)
	//}}AFX_DATA_INIT

	if ( pszCaption != NULL ) m_sCaption = pszCaption;
	else if ( m_lpszTemplateName != NULL ) LoadDefaultCaption();
}

CSettingsPage::~CSettingsPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage operations

BOOL CSettingsPage::LoadDefaultCaption()
{
	CDialogTemplate pTemplate;
	DLGTEMPLATE* pData;
	LPWORD pWord;

	if ( ! pTemplate.Load( m_lpszTemplateName ) ) return FALSE;

	pData = (DLGTEMPLATE*)GlobalLock( pTemplate.m_hTemplate );

	if ( ((DLGTEMPLATEEX*)pData)->signature == 0xFFFF )
	{
		pWord = (WORD*)( (DLGTEMPLATEEX*)pData + 1 );
	}
	else
	{
		pWord = (WORD*)( pData + 1 );
	}

	if ( *pWord == 0xFFFF )
	{
		pWord += 2;
	}
	else
	{
		while ( *pWord++ );
	}

	if ( *pWord == 0xFFFF )
	{
		pWord += 2;
	}
	else
	{
		while ( *pWord++ );
	}

	m_sCaption = (wchar_t*)pWord;

	GlobalUnlock( pTemplate.m_hTemplate );

	return m_sCaption.GetLength() > 0;
}

BOOL CSettingsPage::Create(CRect& rcPage, CWnd* pSheetWnd)
{
	ASSERT_VALID(this);
	ASSERT( m_lpszTemplateName != NULL );

	CDialogTemplate pTemplate;
	LPDLGTEMPLATE pData;

	if ( ! pTemplate.Load( m_lpszTemplateName ) ) return FALSE;
	pData = (LPDLGTEMPLATE)GlobalLock( pTemplate.m_hTemplate );

	DWORD dwExStyle = Settings.General.LanguageRTL ? WS_EX_RTLREADING|WS_EX_RIGHT|WS_EX_LEFTSCROLLBAR|WS_EX_LAYOUTRTL :
		WS_EX_LTRREADING|WS_EX_LEFT|WS_EX_RIGHTSCROLLBAR;

	if ( ((DLGTEMPLATEEX*)pData)->signature == 0xFFFF )
	{
		DLGTEMPLATEEX* pEx = (DLGTEMPLATEEX*)pData;
		pEx->style		= WS_CHILDWINDOW|WS_OVERLAPPED|DS_3DLOOK|DS_SETFONT|DS_CONTROL;
		pEx->exStyle	= dwExStyle|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;
	}
	else
	{
		pData->style			= WS_CHILDWINDOW|WS_OVERLAPPED|DS_3DLOOK|DS_SETFONT|DS_CONTROL;
		pData->dwExtendedStyle	= dwExStyle|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;
	}

	GlobalUnlock( pTemplate.m_hTemplate );
	CreateIndirect( pTemplate.m_hTemplate, pSheetWnd );
	SetFont( &theApp.m_gdiFont );

	MoveWindow( rcPage );

	return ( m_hWnd != NULL );
}

CSettingsPage* CSettingsPage::GetPage(CRuntimeClass* pClass) const
{
	return GetSheet()->GetPage( pClass );
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage message handlers

BOOL CSettingsPage::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_wndToolTip.Create( this );
	m_wndToolTip.Activate( TRUE );
	m_wndToolTip.SetMaxTipWidth( 200 );
	// Show the tooltip for 20 seconds
	m_wndToolTip.SetDelayTime( TTDT_AUTOPOP, 20 * 1000 );

	Skin.Apply( NULL, this, 0, &m_wndToolTip );

	return TRUE;
}

void CSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CSettingsPage)
	//}}AFX_DATA_MAP
}

void CSettingsPage::SetModified(BOOL bChanged)
{
	ASSERT_VALID(this);
	GetSheet()->SetModified( bChanged );
}

BOOL CSettingsPage::OnApply()
{
	ASSERT_VALID(this);
	OnOK();
	return TRUE;
}

void CSettingsPage::OnReset()
{
	ASSERT_VALID(this);
	OnCancel();
}

void CSettingsPage::OnOK()
{
	ASSERT_VALID(this);
}

void CSettingsPage::OnCancel()
{
	ASSERT_VALID(this);
}

BOOL CSettingsPage::OnSetActive()
{
	ASSERT_VALID(this);
	return TRUE;
}

BOOL CSettingsPage::OnKillActive()
{
	ASSERT_VALID(this);
	if ( ! UpdateData() ) return FALSE;
	return TRUE;
}

BOOL CSettingsPage::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( &rc );
	pDC->FillSolidRect( &rc, Skin.m_crDialog );
	return TRUE;
}

HBRUSH CSettingsPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkColor( Skin.m_crDialog );
		hbr = Skin.m_brDialog;
	}

	return hbr;
}


BOOL CSettingsPage::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST )
	{
		MSG msg;
		CopyMemory( &msg, pMsg, sizeof(MSG) );
		HWND hWndParent = ::GetParent( msg.hwnd );

		while ( hWndParent && hWndParent != m_hWnd )
		{
			msg.hwnd = hWndParent;
			hWndParent = ::GetParent( hWndParent );
		}

		if ( msg.hwnd )
		{
			m_wndToolTip.RelayEvent( &msg );
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CEditPath

IMPLEMENT_DYNCREATE(CEditPath, CEdit)

BEGIN_MESSAGE_MAP(CEditPath, CEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CEditPath::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CEdit::OnLButtonDblClk( nFlags, point );

	CString sPath;
	GetWindowText( sPath );

	sPath = CString( _T("\\\\?\\") ) + sPath;	// very long path

	if ( GetFileAttributes( sPath ) != INVALID_FILE_ATTRIBUTES )
	{
		ShellExecute( GetSafeHwnd(), NULL, sPath, NULL, NULL, SW_SHOWDEFAULT );
	}
}
