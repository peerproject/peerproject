//
// DlgMessage.cpp
//
// This file is part of PeerProject (peerproject.org) � 2009-2010
// Portions copyright Shareaza Development Team, 2009.
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
#include "Settings.h"
#include "DlgSkinDialog.h"
#include "DlgMessage.h"
#include "Skin.h"


// CMessageDlg dialog

IMPLEMENT_DYNAMIC(CMessageDlg, CSkinDialog)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialog	( CMessageDlg::IDD, pParent )
	, m_nType		( MB_OK )
	, m_bRemember	( FALSE )
	, m_pnDefault	( NULL )
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO_ICON, m_Icon);
	DDX_Control(pDX, IDC_INFO_TEXT, m_pText );
	DDX_Control(pDX, IDC_INFO_SPLIT, m_pSplit );
	DDX_Control(pDX, IDC_INFO_BUTTON1, m_pButton1 );
	DDX_Control(pDX, IDC_INFO_BUTTON2, m_pButton2 );
	DDX_Control(pDX, IDC_INFO_BUTTON3, m_pButton3 );
	DDX_Control(pDX, IDC_INFO_REMEMBER, m_pDefault );
	DDX_Check(pDX, IDC_INFO_REMEMBER, m_bRemember);
	DDX_Text(pDX, IDC_INFO_TEXT, m_sText);
}

BOOL CMessageDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CMessageDlg"), IDR_MAINFRAME );

	CRect rc, rcWindow;
	GetWindowRect( &rcWindow );

	// Adjust dialog height according text real height
	m_pText.GetWindowRect( &rc );
	ScreenToClient( &rc );
	CRect rcCalc( rc );
	CClientDC dc( &m_pText );
	CFont* pOldFont = dc.SelectObject( m_pText.GetFont() );
	dc.DrawText( m_sText, &rcCalc, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX );
	dc.SelectObject( pOldFont );
	rcCalc.bottom += 16;
	if ( rcCalc.Height() < 64 ) rcCalc.bottom = rc.top + 64;
	int delta_height = rcCalc.Height() - rc.Height();
	m_pText.MoveWindow( &rcCalc );

	m_pButton1.GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.MoveToY( rc.top + delta_height );
	m_pButton1.MoveWindow( &rc );

	m_pButton2.GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.MoveToY( rc.top + delta_height );
	m_pButton2.MoveWindow( &rc );

	m_pButton3.GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.MoveToY( rc.top + delta_height );
	m_pButton3.MoveWindow( &rc );

	m_pSplit.GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.MoveToY( rc.top + delta_height );
	m_pSplit.MoveWindow( &rc );

	m_pDefault.GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.MoveToY( rc.top + delta_height );
	m_pDefault.MoveWindow( &rc );

	if ( m_pnDefault == NULL )
	{
		// Resize window to hide splitter and check box
		m_pButton1.GetWindowRect( &rc );
		int border = Settings.General.LanguageRTL ?
			( rc.left - rcWindow.left ) : ( rcWindow.right - rc.right );
		delta_height = ( rc.bottom + border ) - rcWindow.bottom;
	}

	SetWindowPos( NULL, 0, 0, rcWindow.Width(), rcWindow.Height() + delta_height,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );

	switch ( m_nType & MB_ICONMASK )
	{
	case MB_ICONHAND:
		m_Icon.SetIcon( ::LoadIcon( NULL, IDI_HAND ) );
		break;
	case MB_ICONQUESTION:
		m_Icon.SetIcon( ::LoadIcon( NULL, IDI_QUESTION ) );
		break;
	case MB_ICONEXCLAMATION:
		m_Icon.SetIcon( ::LoadIcon( NULL, IDI_EXCLAMATION ) );
		break;
	case MB_ICONASTERISK:
		m_Icon.SetIcon( ::LoadIcon( NULL, IDI_ASTERISK ) );
		break;
	}

	int nButtons = 1;
	switch ( m_nType & MB_TYPEMASK )
	{
	case MB_OK:
		m_pButton1.SetWindowText( LoadString( IDS_GENERAL_OK ) );
		m_pButton1.ShowWindow( SW_NORMAL );
		m_pButton2.ShowWindow( SW_HIDE );
		m_pButton3.ShowWindow( SW_HIDE );
		break;
	case MB_OKCANCEL:
		m_pButton1.SetWindowText( LoadString( IDS_GENERAL_CANCEL ) );
		m_pButton1.ShowWindow( SW_NORMAL );
		m_pButton2.SetWindowText( LoadString( IDS_GENERAL_OK ) );
		m_pButton2.ShowWindow( SW_NORMAL );
		m_pButton3.ShowWindow( SW_HIDE );
		nButtons = 2;
		break;
	case MB_ABORTRETRYIGNORE:
		m_pButton1.SetWindowText( LoadString( IDS_GENERAL_IGNORE ) );
		m_pButton1.ShowWindow( SW_NORMAL );
		m_pButton2.SetWindowText( LoadString( IDS_GENERAL_RETRY ) );
		m_pButton2.ShowWindow( SW_NORMAL );
		m_pButton3.SetWindowText( LoadString( IDS_GENERAL_ABORT ) );
		m_pButton3.ShowWindow( SW_NORMAL );
		nButtons = 3;
		break;
	case MB_YESNOCANCEL:
		m_pButton1.SetWindowText( LoadString( IDS_GENERAL_CANCEL ) );
		m_pButton1.ShowWindow( SW_NORMAL );
		m_pButton2.SetWindowText( LoadString( IDS_GENERAL_NO ) );
		m_pButton2.ShowWindow( SW_NORMAL );
		m_pButton3.SetWindowText( LoadString( IDS_GENERAL_YES ) );
		m_pButton3.ShowWindow( SW_NORMAL );
		nButtons = 3;
		break;
	case MB_YESNO:
		m_pButton1.SetWindowText( LoadString( IDS_GENERAL_NO ) );
		m_pButton1.ShowWindow( SW_NORMAL );
		m_pButton2.SetWindowText( LoadString( IDS_GENERAL_YES ) );
		m_pButton2.ShowWindow( SW_NORMAL );
		m_pButton3.ShowWindow( SW_HIDE );
		nButtons = 2;
		break;
	case MB_RETRYCANCEL:
		m_pButton1.SetWindowText( LoadString( IDS_GENERAL_CANCEL ) );
		m_pButton1.ShowWindow( SW_NORMAL );
		m_pButton2.SetWindowText( LoadString( IDS_GENERAL_RETRY ) );
		m_pButton2.ShowWindow( SW_NORMAL );
		m_pButton3.ShowWindow( SW_HIDE );
		nButtons = 2;
		break;
	case MB_CANCELTRYCONTINUE:
		m_pButton1.SetWindowText( LoadString( IDS_GENERAL_CONTINUE ) );
		m_pButton1.ShowWindow( SW_NORMAL );
		m_pButton2.SetWindowText( LoadString( IDS_GENERAL_TRYAGAIN ) );
		m_pButton2.ShowWindow( SW_NORMAL );
		m_pButton3.SetWindowText( LoadString( IDS_GENERAL_CANCEL ) );
		m_pButton3.ShowWindow( SW_NORMAL );
		nButtons = 3;
		break;
	}

	switch ( m_nType & MB_DEFMASK )
	{
	case MB_DEFBUTTON1:
		switch ( nButtons )
		{
		case 1:
			m_pButton1.SetButtonStyle( m_pButton1.GetButtonStyle() | BS_DEFPUSHBUTTON );
			m_pButton1.SetFocus();
			break;
		case 2:
			m_pButton2.SetButtonStyle( m_pButton2.GetButtonStyle() | BS_DEFPUSHBUTTON );
			m_pButton2.SetFocus();
			break;
		case 3:
			m_pButton3.SetButtonStyle( m_pButton3.GetButtonStyle() | BS_DEFPUSHBUTTON );
			m_pButton3.SetFocus();
			break;
		}
		break;
	case MB_DEFBUTTON2:
		switch ( nButtons )
		{
		case 1:
		case 2:
			m_pButton1.SetButtonStyle( m_pButton1.GetButtonStyle() | BS_DEFPUSHBUTTON );
			m_pButton1.SetFocus();
			break;
		case 3:
			m_pButton2.SetButtonStyle( m_pButton2.GetButtonStyle() | BS_DEFPUSHBUTTON );
			m_pButton2.SetFocus();
			break;
		}
		break;
	case MB_DEFBUTTON3:
		m_pButton1.SetButtonStyle( m_pButton1.GetButtonStyle() | BS_DEFPUSHBUTTON );
		m_pButton1.SetFocus();
		break;
	}

	m_pDefault.SetWindowText( LoadString( IDS_INFO_REMEMBER ) );

	// Hide unused splitter and check box
	m_pSplit.ShowWindow( m_pnDefault ? SW_NORMAL : SW_HIDE );
	m_pDefault.ShowWindow( m_pnDefault ? SW_NORMAL : SW_HIDE );

	return FALSE;
}

void CMessageDlg::OnOK()
{
}

void CMessageDlg::OnCancel()
{
	switch ( m_nType & MB_TYPEMASK )
	{
	case MB_OK:
		EndDialog( IDOK );
		break;
	case MB_OKCANCEL:
	case MB_YESNOCANCEL:
	case MB_RETRYCANCEL:
	case MB_CANCELTRYCONTINUE:
		EndDialog( IDCANCEL );
		break;
	case MB_ABORTRETRYIGNORE:
		EndDialog( IDIGNORE );
		break;
	case MB_YESNO:
		EndDialog( IDNO );
		break;
	}
}

BEGIN_MESSAGE_MAP(CMessageDlg, CSkinDialog)
	ON_BN_CLICKED(IDC_INFO_BUTTON1, &CMessageDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_INFO_BUTTON2, &CMessageDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_INFO_BUTTON3, &CMessageDlg::OnBnClickedButton3)
END_MESSAGE_MAP()

// CMessageDlg message handlers

void CMessageDlg::OnBnClickedButton1()
{
	UpdateData();

	switch ( m_nType & MB_TYPEMASK )
	{
	case MB_OK:
		EndDialog( IDOK );
		break;
	case MB_OKCANCEL:
	case MB_YESNOCANCEL:
	case MB_RETRYCANCEL:
		EndDialog( IDCANCEL );
		break;
	case MB_ABORTRETRYIGNORE:
		EndDialog( IDIGNORE );
		break;
	case MB_YESNO:
		EndDialog( IDNO );
		break;
	case MB_CANCELTRYCONTINUE:
		EndDialog( IDCONTINUE );
		break;
	}
}

void CMessageDlg::OnBnClickedButton2()
{
	UpdateData();

	switch ( m_nType & MB_TYPEMASK )
	{
	case MB_OKCANCEL:
		EndDialog( IDOK );
		break;
	case MB_ABORTRETRYIGNORE:
	case MB_RETRYCANCEL:
		EndDialog( IDRETRY );
		break;
	case MB_YESNOCANCEL:
		EndDialog( IDNO );
		break;
	case MB_YESNO:
		EndDialog( IDYES );
		break;
	case MB_CANCELTRYCONTINUE:
		EndDialog( IDTRYAGAIN );
		break;
	}
}

void CMessageDlg::OnBnClickedButton3()
{
	UpdateData();

	switch ( m_nType & MB_TYPEMASK )
	{
	case MB_ABORTRETRYIGNORE:
		EndDialog( IDABORT );
		break;
	case MB_YESNOCANCEL:
		EndDialog( IDYES );
		break;
	case MB_CANCELTRYCONTINUE:
		EndDialog( IDCANCEL );
		break;
	}
}

INT_PTR CMessageDlg::DoModal()
{
	if ( m_pnDefault && *m_pnDefault != 0 )
	{
		switch ( m_nType & MB_TYPEMASK )
		{
		case MB_OK:
			// 0 - ask, 1 - ok
			if ( *m_pnDefault == 1 )
				return IDOK;
			break;
		case MB_OKCANCEL:
			// 0 - ask, 1 - ok, 2 - cancel
			if ( *m_pnDefault == 1 )
				return IDOK;
			else if ( *m_pnDefault == 2 )
				return IDCANCEL;
			break;
		case MB_ABORTRETRYIGNORE:
			// 0 - ask, 1 - abort, 2 - retry, 3 - ignore
			if ( *m_pnDefault == 1 )
				return IDABORT;
			else if ( *m_pnDefault == 2 )
				return IDRETRY;
			else if ( *m_pnDefault == 3 )
				return IDIGNORE;
			break;
		case MB_YESNOCANCEL:
			// 0 - ask, 1 - no, 2 - yes, 3 - cancel
			if ( *m_pnDefault == 1 )
				return IDNO;
			else if ( *m_pnDefault == 2 )
				return IDYES;
			else if ( *m_pnDefault == 3 )
				return IDCANCEL;
			break;
		case MB_YESNO:
			// 0 - ask, 1 - no, 2 - yes
			if ( *m_pnDefault == 1 )
				return IDNO;
			else if ( *m_pnDefault == 2 )
				return IDYES;
			break;
		case MB_RETRYCANCEL:
			// 0 - ask, 1 - retry, 2 - cancel
			if ( *m_pnDefault == 1 )
				return IDRETRY;
			else if ( *m_pnDefault == 2 )
				return IDCANCEL;
			break;
		case MB_CANCELTRYCONTINUE:
			// 0 - ask, 1 - cancel, 2 - try again, 3 - continue
			if ( *m_pnDefault == 1 )
				return IDCANCEL;
			else if ( *m_pnDefault == 2 )
				return IDTRYAGAIN;
			else if ( *m_pnDefault == 3 )
				return IDCONTINUE;
			break;
		}
	}

	if ( m_pnDefault )
		*m_pnDefault = 0;

	INT_PTR nResult = CSkinDialog::DoModal();

	if ( m_pnDefault && m_bRemember )
	{
		switch ( m_nType & MB_TYPEMASK )
		{
		case MB_OK:
			if ( nResult == IDOK )
				*m_pnDefault = 1;
			break;
		case MB_OKCANCEL:
			if ( nResult == IDOK )
				*m_pnDefault = 1;
			else if ( nResult == IDCANCEL )
				*m_pnDefault = 2;
			break;
		case MB_ABORTRETRYIGNORE:
			if ( nResult == IDABORT )
				*m_pnDefault = 1;
			else if ( nResult == IDRETRY )
				*m_pnDefault = 2;
			else if ( nResult == IDIGNORE )
				*m_pnDefault = 3;
			break;
		case MB_YESNOCANCEL:
			if ( nResult == IDNO )
				*m_pnDefault = 1;
			else if ( nResult == IDYES )
				*m_pnDefault = 2;
			else if ( nResult == IDCANCEL )
				*m_pnDefault = 3;
			break;
		case MB_YESNO:
			if ( nResult == IDNO )
				*m_pnDefault = 1;
			else if ( nResult == IDYES )
				*m_pnDefault = 2;
			break;
		case MB_RETRYCANCEL:
			if ( nResult == IDRETRY )
				*m_pnDefault = 1;
			else if ( nResult == IDCANCEL )
				*m_pnDefault = 2;
			break;
		case MB_CANCELTRYCONTINUE:
			if ( nResult == IDCANCEL )
				*m_pnDefault = 1;
			else if ( nResult == IDTRYAGAIN )
				*m_pnDefault = 2;
			else if ( nResult == IDCONTINUE )
				*m_pnDefault = 3;
			break;
		}
	}

	return nResult;
}

INT_PTR MsgBox(LPCTSTR lpszText, UINT nType, UINT /*nIDHelp*/, DWORD* pnDefault)
{
	CMessageDlg dlg;
	dlg.m_nType = nType;
	dlg.m_sText = lpszText;
	dlg.m_pnDefault = pnDefault;
	return dlg.DoModal();
}

INT_PTR MsgBox(UINT nIDPrompt, UINT nType, UINT nIDHelp, DWORD* pnDefault)
{
	CString strText;
	Skin.LoadString( strText, nIDPrompt );
	return MsgBox( (LPCTSTR)strText, nType, nIDHelp, pnDefault );
}
