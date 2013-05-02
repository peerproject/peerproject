//
// DlgURLExport.cpp
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
#include "DlgURLExport.h"
#include "Library.h"
#include "SharedFile.h"
#include "SharedFolder.h"
#include "Transfer.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CURLExportDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CURLExportDlg, CSkinDialog)
	ON_CBN_CLOSEUP(IDC_URL_TOKEN, OnCloseUpUrlToken)
	ON_CBN_SELCHANGE(IDC_URL_PRESET, OnSelChangeUrlPreset)
	ON_CBN_KILLFOCUS(IDC_URL_PRESET, OnKillFocusUrlPreset)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CURLExportDlg dialog

CURLExportDlg::CURLExportDlg(CWnd* pParent) : CSkinDialog(CURLExportDlg::IDD, pParent)
{
}

void CURLExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SAVE, m_wndSave);
	DDX_Control(pDX, IDC_COPY, m_wndCopy);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_URL_TOKEN, m_wndToken);
	DDX_Control(pDX, IDC_URL_PRESET, m_wndPreset);
	DDX_Control(pDX, IDC_URL_FORMAT, m_wndFormat);
	DDX_Control(pDX, IDC_MESSAGE, m_wndMessage);
	DDX_Text(pDX, IDC_URL_FORMAT, m_sFormat);
}

/////////////////////////////////////////////////////////////////////////////
// CURLExportDlg message handlers

BOOL CURLExportDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CURLExportDlg"), IDI_WEB_URL );

	if ( Settings.General.LanguageRTL ) m_wndProgress.ModifyStyleEx( WS_EX_LAYOUTRTL, 0, 0 );
	CString strFormat, strMessage;
	m_wndMessage.GetWindowText( strFormat );
	strMessage.Format( strFormat, m_pFiles.GetCount() );
	m_wndMessage.SetWindowText( strMessage );

	m_sFormat = Settings.Library.URLExportFormat;

	if ( m_sFormat.IsEmpty() )
		m_sFormat = _T("<a href=\"magnet:?xt=urn:bitprint:[SHA1].[TIGER]&amp;xt=urn:ed2khash:[ED2K]&amp;xt=urn:md5:[MD5]&amp;xl=[ByteSize]&amp;dn=[NameURI]\">[Name]</a><br>");

	UpdateData( FALSE );

	return TRUE;
}

void CURLExportDlg::Add(const CPeerProjectFile* pFile)
{
	ASSERT( pFile != NULL );
	m_pFiles.AddTail( pFile );
}

void CURLExportDlg::OnKillFocusUrlPreset()
{
	m_wndPreset.SetCurSel( -1 );
}

void CURLExportDlg::OnCloseUpUrlToken()
{
	int nToken = m_wndToken.GetCurSel();
	m_wndToken.SetCurSel( -1 );
	if ( nToken < 0 || nToken > 15 ) return;

	LPCTSTR pszTokens[] =
	{
		_T("[TIGER]"), _T("[SHA1]"), _T("[MD5]"), _T("[ED2K]"), _T("[BTH]"),
		_T("[Name]"), _T("[NameURI]"), _T("[FileBase]"), _T("[FileExt]"),
		_T("[Size]"), _T("[ByteSize]"), _T("[Path]"), _T("[LocalHost]"),
		_T("[LocalPort]"), _T("[Link]"), _T("[LinkURI]")
	};

	m_wndFormat.ReplaceSel( pszTokens[ nToken ] );
	m_wndFormat.SetFocus();
}

void CURLExportDlg::OnSelChangeUrlPreset()
{
	int nPreset = m_wndPreset.GetCurSel();
	if ( nPreset < 0 || nPreset > 3 ) return;

	LPCTSTR pszPresets[] =
	{
		_T("magnet:?xt=urn:bitprint:[SHA1].[TIGER]&xt=urn:ed2khash:[ED2K]&xt=urn:md5:[MD5]&xl=[ByteSize]&dn=[NameURI]"),
		_T("ed2k://|file|[NameURI]|[ByteSize]|[ED2K]|/"),
		_T("<a href=\"magnet:?xt=urn:bitprint:[SHA1].[TIGER]&amp;xt=urn:ed2khash:[ED2K]&amp;xt=urn:md5:[MD5]&amp;xl=[ByteSize]&amp;dn=[NameURI]\">[Name]</a><br>"),
		_T("<a href=\"ed2k://|file|[NameURI]|[ByteSize]|[ED2K]|/\">[Name]</a><br>"),
	};

	m_wndFormat.SetWindowText( pszPresets[ nPreset ] );
}

void CURLExportDlg::OnSave()
{
	UpdateData();

	if ( m_sFormat.IsEmpty() ) return;

	theApp.WriteProfileString( _T("Library"), _T("URLExportFormat"), m_sFormat );

	LPCTSTR pszExt = ( m_sFormat.Find( '<' ) >= 0 ) ? _T("htm") : _T("txt");
	LPCTSTR pszFilter = ( m_sFormat.Find( '<' ) >= 0 ) ?
		_T("HTML Files|*.htm;*.html|Text Files|*.txt|") + LoadString( IDS_FILES_ALL ) + _T("|*.*||") :
		_T("Text Files|*.txt|HTML Files|*.htm;*.html|") + LoadString( IDS_FILES_ALL ) + _T("|*.*||");

	CFileDialog dlg( FALSE, pszExt, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		pszFilter, this );

	if ( dlg.DoModal() != IDOK ) return;

	CFile pOutput;

	if ( ! pOutput.Open( dlg.GetPathName(), CFile::modeWrite|CFile::modeCreate ) )
		return;

	CWaitCursor pCursor;

	m_wndProgress.SetRange( 0, short( m_pFiles.GetCount() ) );
	m_wndCopy.EnableWindow( FALSE );
	m_wndSave.EnableWindow( FALSE );

	for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; )
	{
		m_wndProgress.OffsetPos( 1 );

		CString strLine = m_sFormat;

		MakeURL( m_pFiles.GetNext( pos ), strLine );

		int nBytes = WideCharToMultiByte( CP_ACP, 0, strLine, strLine.GetLength(), NULL, 0, NULL, NULL );
		LPSTR pBytes = new CHAR[nBytes];
		WideCharToMultiByte( CP_ACP, 0, strLine, strLine.GetLength(), pBytes, nBytes, NULL, NULL );
		pOutput.Write( pBytes, nBytes );
		delete [] pBytes;
	}

	pOutput.Close();

	EndDialog( IDOK );
}

void CURLExportDlg::OnCopy()
{
	UpdateData();

	if ( m_sFormat.IsEmpty() ) return;

	Settings.Library.URLExportFormat = m_sFormat;

	CWaitCursor pCursor;
	CString strOutput;

	m_wndProgress.SetRange( 0, short( m_pFiles.GetCount() ) );
	m_wndCopy.EnableWindow( FALSE );
	m_wndSave.EnableWindow( FALSE );

	for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; )
	{
		m_wndProgress.OffsetPos( 1 );

		CString strLine = m_sFormat;

		MakeURL( m_pFiles.GetNext( pos ), strLine );

		strOutput += strLine;
	}

	theApp.SetClipboard( strOutput );

	EndDialog( IDOK );
}

void CURLExportDlg::MakeURL(const CPeerProjectFile* pFile, CString& strLine)
{
	CString strItem;

	strLine.Replace( _T("[Name]"), pFile->m_sName );
	strLine.Replace( _T("[NameURI]"), URLEncode( pFile->m_sName ) );

	if ( ! pFile->m_sURL.IsEmpty() )
	{
		strLine.Replace( _T("[Link]"), pFile->m_sURL );
		strLine.Replace( _T("[LinkURI]"), URLEncode( pFile->m_sURL ) );
	}
	else
	{
		strLine.Replace( _T("[Link]"), _T("") );
		strLine.Replace( _T("[LinkURI]"), _T("") );
	}

	if ( ! pFile->m_sPath.IsEmpty() )
	{
		strLine.Replace( _T("[Path]"), pFile->m_sPath );
	}
	else
	{
		strLine.Replace( _T("[Path]"), _T("") );
	}

	if ( pFile->m_nSize != 0 && pFile->m_nSize != SIZE_UNKNOWN )
	{
		strLine.Replace( _T("[Size]"), Settings.SmartVolume( pFile->m_nSize ) );
		strItem.Format( _T("%I64u"), pFile->m_nSize );
		strLine.Replace( _T("[ByteSize]"), strItem );
	}
	else
	{
		strLine.Replace( _T("[Size]"), _T("") );
		strLine.Replace( _T("[ByteSize]"), _T("") );
	}

	strItem = pFile->m_oTiger.toString();
	strLine.Replace( _T("[TIGER]"), strItem );
	strItem = pFile->m_oSHA1.toString();
	strLine.Replace( _T("[SHA1]"), strItem );
	strItem = pFile->m_oMD5.toString();
	strLine.Replace( _T("[MD5]"), strItem );
	strItem = pFile->m_oED2K.toString();
	strLine.Replace( _T("[ED2K]"), strItem );
	strItem = pFile->m_oBTH.toString();
	strLine.Replace( _T("[BTH]"), strItem );

	const int nDot = pFile->m_sName.ReverseFind( '.' );

	if ( nDot > 0 )
	{
		strLine.Replace( _T("[FileBase]"), pFile->m_sName.Left( nDot ) );
		strLine.Replace( _T("[FileExt]"), pFile->m_sName.Mid( nDot + 1 ) );
	}
	else
	{
		strLine.Replace( _T("[FileBase]"), pFile->m_sName );
		strLine.Replace( _T("[FileExt]"), _T("") );
	}

	if ( Network.IsListening() )
	{
		strItem = inet_ntoa( Network.m_pHost.sin_addr );
		strLine.Replace( _T("[LocalHost]"), strItem );
		strItem.Format( _T("%lu"), htons( Network.m_pHost.sin_port ) );
		strLine.Replace( _T("[LocalPort]"), strItem );
	}

	strLine += _T("\r\n");
}
