//
// DlgSecureRule.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "DlgSecureRule.h"
#include "Security.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

BEGIN_MESSAGE_MAP(CSecureRuleDlg, CSkinDialog)
	ON_CBN_SELCHANGE(IDC_RULE_EXPIRE, OnSelChangeRuleExpire)
	ON_CBN_SELCHANGE(IDC_RULE_TYPE, OnSelChangeRuleType)
	ON_COMMAND(IDC_BROWSE_FILES, OnBrowse)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSecureRuleDlg dialog

CSecureRuleDlg::CSecureRuleDlg(CWnd* pParent, CSecureRule* pRule)
	: CSkinDialog(CSecureRuleDlg::IDD, pParent)
	, m_sComment	( _T("") )
	, m_sContent	( _T("") )
	, m_nExpireD	( 0 )
	, m_nExpireH	( 0 )
	, m_nExpireM	( 0 )
	, m_nExpire 	( 0 )
	, m_nAction 	( -1 )
	, m_nType		( -1 )
	, m_nMatch		( -1 )
	, m_bNew		( FALSE )
{
	m_pRule	= pRule;
}

CSecureRuleDlg::~CSecureRuleDlg()
{
	if ( m_pRule && m_bNew )
		delete m_pRule;
}

void CSecureRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecureRuleDlg)
	DDX_Control(pDX, IDC_GROUP_NETWORK, m_wndGroupNetwork);
	DDX_Control(pDX, IDC_GROUP_CONTENT, m_wndGroupContent);
	DDX_Control(pDX, IDC_GROUP_EXTERNAL, m_wndGroupExternal);
	DDX_Control(pDX, IDC_RULE_CONTENT, m_wndContent);
	DDX_Control(pDX, IDC_FILE_PATH, m_wndPath);
	DDX_Control(pDX, IDC_IP_1, m_wndIP1);
	DDX_Control(pDX, IDC_IP_2, m_wndIP2);
	DDX_Control(pDX, IDC_IP_3, m_wndIP3);
	DDX_Control(pDX, IDC_IP_4, m_wndIP4);
	DDX_Control(pDX, IDC_MASK_1, m_wndMask1);
	DDX_Control(pDX, IDC_MASK_2, m_wndMask2);
	DDX_Control(pDX, IDC_MASK_3, m_wndMask3);
	DDX_Control(pDX, IDC_MASK_4, m_wndMask4);
	DDX_Control(pDX, IDC_EXPIRE_D, m_wndExpireD);
	DDX_Control(pDX, IDC_EXPIRE_H, m_wndExpireH);
	DDX_Control(pDX, IDC_EXPIRE_M, m_wndExpireM);
	DDX_Text(pDX, IDC_EXPIRE_D, m_nExpireD);
	DDX_Text(pDX, IDC_EXPIRE_H, m_nExpireH);
	DDX_Text(pDX, IDC_EXPIRE_M, m_nExpireM);
	DDX_Text(pDX, IDC_RULE_CONTENT, m_sContent);
	DDX_Text(pDX, IDC_RULE_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_FILE_PATH, m_sPath);
	DDX_CBIndex(pDX, IDC_RULE_TYPE, m_nType);
	DDX_CBIndex(pDX, IDC_RULE_ACTION, m_nAction);
	DDX_CBIndex(pDX, IDC_RULE_EXPIRE, m_nExpire);
	DDX_Radio(pDX, IDC_RULE_MATCH_ANY, m_nMatch);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSecureRuleDlg message handlers

BOOL CSecureRuleDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CSecureRuleDlg"), IDR_SECURITYFRAME );

	if ( m_ToolTip.Create(this) )
	{
		m_ToolTip.AddTool( &m_wndMask1, IDS_SECURITY_NETMASK );
		m_ToolTip.AddTool( &m_wndMask2, IDS_SECURITY_NETMASK );
		m_ToolTip.AddTool( &m_wndMask3, IDS_SECURITY_NETMASK );
		m_ToolTip.AddTool( &m_wndMask4, IDS_SECURITY_NETMASK );

		m_ToolTip.Activate( TRUE );
	}

	if ( ! m_pRule )
	{
		m_bNew = TRUE;
		m_pRule = new CSecureRule();
	}

	CEdit* pwIP[4]   = { &m_wndIP1, &m_wndIP2, &m_wndIP3, &m_wndIP4 };
	CEdit* pwMask[4] = { &m_wndMask1, &m_wndMask2, &m_wndMask3, &m_wndMask4 };

	switch ( m_pRule->m_nType )
	{
	case CSecureRule::srAddress:
		m_nType  = 0;
		m_nMatch = 0;	//(int)CSecureRule::srAddress;
		for ( int nByte = 0 ; nByte < 4 ; nByte++ )
		{
			CString strItem;
			strItem.Format( _T("%lu"), m_pRule->m_nMask[ nByte ] );
			pwMask[ nByte ]->SetWindowText( strItem );

			if ( m_pRule->m_nMask[ nByte ] == 0 )
			{
				pwIP[ nByte ]->SetWindowText( _T("*") );
			}
			else
			{
				strItem.Format( _T("%lu"), m_pRule->m_nIP[ nByte ] );
				pwIP[ nByte ]->SetWindowText( strItem );
			}
		}
		break;
	case CSecureRule::srContentAny:
		m_nType  = 1;	// Set dropdown index
		m_nMatch = 0;	// Set radio check
		m_sContent = m_pRule->GetContentWords();
		break;
	case CSecureRule::srContentAll:
		m_nType  = 1;
		m_nMatch = 1;
		m_sContent = m_pRule->GetContentWords();
		break;
	case CSecureRule::srContentRegExp:
		m_nType  = 1;
		m_nMatch = 2;
		m_sContent = m_pRule->GetContentWords();
		break;
	case CSecureRule::srExternal:
		m_nType  = 2;
		m_nMatch = 0;
		m_sPath = m_pRule->GetContentWords();
		break;
	case CSecureRule::srContentHash:
	case CSecureRule::srSizeType:
	default:
		m_nType  = 1;
		m_nMatch = 0;
		m_sContent = m_pRule->GetContentWords();
	}

	m_sComment = m_pRule->m_sComment;
	m_nAction  = m_pRule->m_nAction;
	m_nExpire  = min( m_pRule->m_nExpire, 2ul );

	if ( m_pRule->m_nType == CSecureRule::srExternal && m_sComment.GetLength() > 2 )
	{
		// Strip load count
		const int nToken = m_sComment.ReverseFind( _T('•') );
		if ( nToken == 0 )
			m_sComment.Empty();
		else if ( nToken > 3 )
			m_sComment = m_sComment.Left( nToken - 2 );
	}

	if ( m_nExpire == 2 )
	{
		const DWORD nTime = m_pRule->m_nExpire - static_cast< DWORD >( time( NULL ) );
		m_nExpireD = nTime / 86400;
		m_nExpireH = ( nTime % 86400 ) / 3600;
		m_nExpireM = ( nTime % 3600 ) / 60;
	}

	UpdateData( FALSE );

	OnSelChangeRuleExpire();
	OnSelChangeRuleType();

	return FALSE;
}

void CSecureRuleDlg::ShowGroup(CWnd* pWnd, BOOL bShow)
{
	while ( pWnd )
	{
		pWnd->ShowWindow( bShow ? SW_SHOW : SW_HIDE );
		pWnd = pWnd->GetNextWindow();

		if ( pWnd->GetStyle() & WS_GROUP )
		{
			if ( pWnd->GetDlgCtrlID() != IDC_RULE_MATCH_ANY )
				break;
		}
	}
}

void CSecureRuleDlg::OnSelChangeRuleType()
{
	UpdateData();

	ShowGroup( &m_wndGroupNetwork, m_nType == 0 );
	ShowGroup( &m_wndGroupContent, m_nType == 1 );
	ShowGroup( &m_wndGroupExternal, m_nType == 2 );

	switch ( m_nType )
	{
	case 0:		// IP
		m_wndIP1.SetFocus();
		m_wndIP1.SetSel( 0, -1 );
		break;
	case 1:		// Content
		m_wndContent.SetFocus();
		break;
	case 2:		// Extenal
		m_wndPath.SetFocus();
		break;
	}
}

void CSecureRuleDlg::OnSelChangeRuleExpire()
{
	UpdateData();
	m_wndExpireD.EnableWindow( m_nExpire == 2 );
	m_wndExpireH.EnableWindow( m_nExpire == 2 );
	m_wndExpireM.EnableWindow( m_nExpire == 2 );
}

void CSecureRuleDlg::OnBrowse()
{
	CFileDialog dlg( TRUE, NULL, Settings.General.UserPath + _T("\\Data\\"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		_T("Text Files|*.txt;*.dat;*.p2p|") + LoadString( IDS_FILES_ALL ) + _T("|*.*||"), this );

	if ( dlg.DoModal() != IDOK ) return;

	UpdateData( TRUE );
	m_sPath = dlg.GetPathName();
	UpdateData( FALSE );
}

BOOL CSecureRuleDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_CHAR )
	{
		CEdit* pwIP[4]		= { &m_wndIP1, &m_wndIP2, &m_wndIP3, &m_wndIP4 };
		CEdit* pwMask[4]	= { &m_wndMask1, &m_wndMask2, &m_wndMask3, &m_wndMask4 };
		CWnd* pFocus		= GetFocus();

		for ( int nByte = 0 ; nByte < 4 && pFocus ; nByte++ )
		{
			if ( pFocus == pwIP[ nByte ] || pFocus == pwMask[ nByte ] )
			{
				if ( pMsg->wParam == '*' )
				{
					if ( pFocus != pwIP[ nByte ] ) return TRUE;
					pwIP[ nByte ]->SetWindowText( _T("*") );
					pwMask[ nByte ]->SetWindowText( _T("0") );
					pwIP[ nByte ]->SetSel( 0, 1 );
					pwMask[ nByte ]->SetSel( 0, 1 );
					return TRUE;
				}

				CString str;
				pFocus->GetWindowText( str );
				if ( _tstoi( str ) > 255 )
				{
					pFocus->SetWindowText( _T("255") );
					if ( pMsg->wParam != '.' )
						return TRUE;
				}

				if ( pMsg->wParam == '.' )
				{
					if ( nByte == 3 ) return TRUE;

					if ( pFocus == pwIP[ nByte ] )
					{
						pwIP[ nByte + 1 ]->SetFocus();
						pwIP[ nByte + 1 ]->SetSel( 0, -1 );
					}
					else
					{
						pwMask[ nByte + 1 ]->SetFocus();
						pwMask[ nByte + 1 ]->SetSel( 0, -1 );
					}

					return TRUE;
				}

				if ( pMsg->wParam >= 32 && ! _istdigit( (TCHAR)pMsg->wParam ) )		// Redundant ES_NUMBER check
					return TRUE;

				break;
			}
		}
	}
	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == 'V' && ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) )
	{
		if ( GetClipboardAddress() )
			return TRUE;
	}

	m_ToolTip.RelayEvent(pMsg);

	return CSkinDialog::PreTranslateMessage(pMsg);
}

BOOL CSecureRuleDlg::GetClipboardAddress()
{
	CWnd* pFocus = GetFocus();

	BOOL bMask;
	if ( pFocus == &m_wndIP1 || pFocus == &m_wndIP2 || pFocus == &m_wndIP3 || pFocus == &m_wndIP4 )
		bMask = FALSE;
	else if ( pFocus == &m_wndMask1 || pFocus == &m_wndMask2 || pFocus == &m_wndMask3 || pFocus == &m_wndMask4 )
		bMask = TRUE;
	else
		return FALSE;

	CString str;
	pFocus->GetWindowText( str );

	if ( OpenClipboard() )
	{
		if ( HGLOBAL hData = GetClipboardData( CF_UNICODETEXT ) )
		{
			size_t nData = GlobalSize( hData );
			LPVOID pData = GlobalLock( hData );

			LPTSTR pszData = str.GetBuffer( (int)( nData + 1 ) / 2 + 1 );
			CopyMemory( pszData, pData, nData );
			pszData[ ( nData + 1 ) / 2 ] = 0;
			str.ReleaseBuffer();
			GlobalUnlock( hData );
		}

		CloseClipboard();
		str.Trim( _T(" \t\r\n") );

		if ( str.GetLength() > 16 && str.Find( _T("/255.") ) < 6 )
			return TRUE;	// Assume bad string, but should handle ip+mask too
	}

	// Handle copy/pasted IP
	if ( str.GetLength() > 8 && str.Find( _T('.') ) > 1 )
	{
		CString strIP[4] = { _T(""), _T(""), _T(""), _T("") };
		for ( int i = 0, c = 0 ; c < 4 && i < str.GetLength() ; i++ )
		{
			TCHAR Ch = str.GetAt( i );
			if ( _istdigit( Ch ) )
			{
				strIP[c].AppendChar( Ch );
				if ( strIP[c].GetLength() > 3 || _tstoi( strIP[c] ) > 255 )
					break;
				continue;
			}
			if ( Ch == _T('.') && ! strIP[c].IsEmpty() )
			{
				c++;
				continue;
			}

			return TRUE;
		}

		if ( ! strIP[2].IsEmpty() )
		{
			if ( ! bMask )
			{
				m_wndIP1.SetWindowText( strIP[0] );
				m_wndIP2.SetWindowText( strIP[1] );
				m_wndIP3.SetWindowText( strIP[2] );
				m_wndIP4.SetWindowText( strIP[3] );
			}
			else
			{
				m_wndMask1.SetWindowText( _T("255") );
				m_wndMask2.SetWindowText( strIP[1] );
				m_wndMask3.SetWindowText( strIP[2] );
				m_wndMask4.SetWindowText( strIP[3] );

				if (  strIP[0] != _T("255") || strIP[1] != _T("255") || ( ! strIP[3].IsEmpty() &&
					( strIP[2] != _T("255") && strIP[3] != _T("0") ) ||
					( strIP[3] != _T("255") && strIP[3] != _T("254") && strIP[3] != _T("252") && strIP[3] != _T("248") && strIP[3] != _T("240") && strIP[3] != _T("224") && strIP[3] != _T("192") && strIP[3] != _T("128") && strIP[3] != _T("0") ) ||
					( strIP[2] != _T("254") && strIP[2] != _T("252") && strIP[2] != _T("248") && strIP[2] != _T("240") && strIP[2] != _T("224") && strIP[2] != _T("192") && strIP[2] != _T("128") && strIP[2] != _T("0") ) ) )
					MsgBox( IDS_SECURITY_NETMASK );
			}
			return TRUE;
		}
	}

	if ( str.GetLength() > 3 )
		str = str.Left( 3 );
	for ( int i = 0 ; i < str.GetLength() ; i++ )
	{
		if ( ! _istdigit( str.GetAt( i ) ) )
			return TRUE;
	}

	if ( str.GetLength() == 3 && _tstoi( str ) > 255 )
		str = _T("255");
	pFocus->SetWindowText( str );

	return TRUE;
}

void CSecureRuleDlg::OnOK()
{
	UpdateData( TRUE );

	if ( m_nType == 0 )		// srAddress (by dropdown index)
	{
		BOOL bWarning = FALSE;
		m_pRule->m_nType = CSecureRule::srAddress;
		CEdit* pwIP[4]		= { &m_wndIP1, &m_wndIP2, &m_wndIP3, &m_wndIP4 };
		CEdit* pwMask[4]	= { &m_wndMask1, &m_wndMask2, &m_wndMask3, &m_wndMask4 };
		for ( int nByte = 0 ; nByte < 4 ; nByte++ )
		{
			CString strItem;
			DWORD nValue = 0;

			pwIP[ nByte ]->GetWindowText( strItem );
			if ( _stscanf( strItem, _T("%lu"), &nValue ) != 1 ) nValue = 0;
			m_pRule->m_nIP[ nByte ] = (BYTE)min( 255ul, nValue );

			pwMask[ nByte ]->GetWindowText( strItem );
			if ( _stscanf( strItem, _T("%lu"), &nValue ) != 1 ) nValue = 0;
			switch ( nValue )
			{
			case 255:
			case 254:
			case 252:
			case 248:
			case 240:
			case 224:
			case 192:
			case 128:
			case 0:
				m_pRule->m_nMask[ nByte ] = (BYTE)nValue;
				break;
			default:
				m_pRule->m_nMask[ nByte ] = 255;
				pwMask[ nByte ]->SetWindowText( _T("255") );
				bWarning = TRUE;
			}
		}
		if ( bWarning )
			MsgBox( IDS_SECURITY_NETMASK );
	}
	else if ( m_nType == 1 && ! m_sContent.IsEmpty() )	// sr... (by dropdown index)
	{
		if ( StartsWith( m_sContent, _T("size:"), 5 ) || StartsWith( m_sContent, _T("type:"), 5 ) )
			m_pRule->m_nType = CSecureRule::srSizeType;
		else if ( StartsWith( m_sContent, _T("urn:"), 4 ) && m_sContent.GetLength() > 20 )
			m_pRule->m_nType = CSecureRule::srContentHash;
		else if ( m_sContent.FindOneOf( _T("/:<>|\"") ) >= 0 )
			MsgBox( IDS_BT_ENCODING );	// ToDo: Better response (return)
		else
			m_pRule->m_nType = (CSecureRule::RuleType)(m_nMatch + 1);	// Note: Change if enum does not match radio button order

		m_pRule->SetContentWords( m_sContent );
	}
	else if ( m_nType == 2 )	// srExternal (by dropdown index)
	{
		if ( m_sPath.GetLength() < 8 )
			return;

		if ( m_sPath.ReverseFind( _T(':') ) != 1 &&
			! PathFileExists( Settings.General.UserPath + _T("\\Data\\") + m_sPath ) )
		{
			MsgBox( IDS_GENERAL_TRYAGAIN );
			return;
		}

		for ( POSITION pos = Security.GetIterator() ; pos ; )
		{
			CSecureRule* pRule = Security.GetNext( pos );
			if ( pRule->m_nType == CSecureRule::srExternal && (LPCTSTR)pRule->m_pContent == (LPCTSTR)m_sPath )
			{
				MsgBox( IDS_TIP_EXISTS_LIBRARY );
				return;
			}
		}

		m_pRule->m_nType = CSecureRule::srExternal;
		m_pRule->SetContentWords( m_sPath );
	}

	m_pRule->m_sComment	= m_sComment;
	m_pRule->m_nAction	= BYTE( m_nAction );
	m_pRule->m_nExpire	= m_nExpire;

	if ( m_nExpire == 2 )
	{
		m_pRule->m_nExpire = static_cast< DWORD >( time( NULL ) ) +
			m_nExpireD * 86400 + m_nExpireH * 3600 + m_nExpireM * 60;
	}

	Security.Add( m_pRule );
	m_pRule = NULL;

	CSkinDialog::OnOK();
}
