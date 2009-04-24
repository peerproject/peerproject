//
// DlgSettingsManager.cpp
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
#include "Skin.h"
#include "DlgSettingsManager.h"

#include "PageSettingsRich.h"
#include "PageSettingsGeneral.h"
#include "PageSettingsLibrary.h"
#include "PageSettingsMedia.h"
#include "PageSettingsCommunity.h"
#include "PageSettingsIRC.h"
#include "PageSettingsWeb.h"
#include "PageSettingsConnection.h"
#include "PageSettingsDownloads.h"
#include "PageSettingsUploads.h"
#include "PageSettingsRemote.h"
#include "PageSettingsScheduler.h"
#include "PageSettingsNetworks.h"
#include "PageSettingsGnutella.h"
#include "PageSettingsDonkey.h"
#include "PageSettingsBitTorrent.h"
#include "PageSettingsProtocols.h"
#include "PageSettingsPlugins.h"
#include "PageSettingsSkins.h"
#include "PageSettingsTraffic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CSettingsManagerDlg, CSettingsSheet)
	//{{AFX_MSG_MAP(CSettingsManagerDlg)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDRETRY, OnApply)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSettingsManagerDlg dialog

CSettingsManagerDlg::CSettingsManagerDlg(CWnd* pParent) : CSettingsSheet( pParent, IDS_SETTINGS )
{
	m_nTopMargin = 52;
}

void CSettingsManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsManagerDlg operations

CSettingsManagerDlg* CSettingsManagerDlg::m_pThis = NULL;

BOOL CSettingsManagerDlg::Run(LPCTSTR pszWindow)
{
	if ( m_pThis ) return FALSE;
	CSettingsManagerDlg pSheet;
	m_pThis = &pSheet;
	BOOL bResult = ( pSheet.DoModal( pszWindow ) == IDOK );
	m_pThis = NULL;
	return bResult;
}

void CSettingsManagerDlg::OnSkinChange(BOOL bSet)
{
	if ( m_pThis == NULL ) return;

	if ( bSet )
	{
		m_pThis->SkinMe( _T("CSettingSheet"), IDR_MAINFRAME );
		m_pThis->Invalidate();
	}
	else
	{
		m_pThis->m_pSkin = NULL;
	}
}

INT_PTR CSettingsManagerDlg::DoModal(LPCTSTR pszWindow)
{
	BOOL bAdvanced			= Settings.General.GUIMode != GUI_BASIC;

	CRichSettingsPage		gGeneral( _T("CGeneralSettingsGroup") );
	CGeneralSettingsPage	pGeneral;
	CLibrarySettingsPage	pLibrary;
	CMediaSettingsPage		pMedia;
	CCommunitySettingsPage	pCommunity;
	CWebSettingsPage		pWeb;
	CRichSettingsPage		gInternet( _T("CInternetSettingsGroup") );
	CConnectionSettingsPage	pConnection;
	CDownloadsSettingsPage	pDownloads;
	CUploadsSettingsPage	pUploads;
	CRemoteSettingsPage		pRemote;
	CSchedulerSettingsPage	pScheduler;
	CNetworksSettingsPage	gNetworks;
	CGnutellaSettingsPage	pGnutella;
	CDonkeySettingsPage		pDonkey;
	CBitTorrentSettingsPage	pTorrent;
	CSkinsSettingsPage		pSkins;
	CPluginsSettingsPage	pPlugins;
	CAdvancedSettingsPage	pAdvanced;
	CProtocolsSettingsPage	pProtocols;
	CIRCSettingsPage		pIRC;

	AddGroup( &gGeneral );
	AddPage( &pGeneral );
	AddPage( &pLibrary );
	AddPage( &pMedia );
	AddPage( &pCommunity );
	AddPage( &pIRC );
	AddPage( &pWeb );
	AddGroup( &gInternet );
	AddPage( &pConnection );
	AddPage( &pDownloads );
	AddPage( &pUploads );
	AddPage( &pRemote );
	if ( bAdvanced )
	{
		AddPage( &pScheduler );
		AddGroup( &gNetworks );
		AddPage( &pGnutella );
#ifndef LAN_MODE
		AddPage( &pDonkey );
		AddPage( &pTorrent );
#endif //LAN_MOD
	}
	AddGroup( &pSkins );
	AddGroup( &pPlugins );
	if ( bAdvanced )
	{
		AddGroup( &pAdvanced );
		AddPage( &pProtocols ); //ToDo: Remove or Improve
	}
	if ( pszWindow != NULL )
	{
		SetActivePage( GetPage( pszWindow ) );
	}
	else
	{
		SetActivePage( GetPage( Settings.General.LastSettingsPage ) );
	}

	INT_PTR nReturn = CSettingsSheet::DoModal();

	if ( m_pFirst )
	{
		Settings.General.LastSettingsPage = m_pFirst->GetRuntimeClass()->m_lpszClassName;
	}

	return nReturn;
}

void CSettingsManagerDlg::AddPage(CSettingsPage* pPage)
{
	CString strCaption = Skin.GetDialogCaption( CString( pPage->GetRuntimeClass()->m_lpszClassName ) );
	CSettingsSheet::AddPage( pPage, strCaption.GetLength() ? (LPCTSTR)strCaption : NULL );
}

void CSettingsManagerDlg::AddGroup(CSettingsPage* pPage)
{
	if ( pPage->IsKindOf( RUNTIME_CLASS(CRichSettingsPage) ) )
	{
		CString strCaption = ((CRichSettingsPage*)pPage)->m_sCaption;
		CSettingsSheet::AddGroup( pPage, strCaption );
	}
	else
	{
		CString strName = pPage->GetRuntimeClass()->m_lpszClassName;
		CString strCaption = Skin.GetDialogCaption( strName );
		CSettingsSheet::AddGroup( pPage, strCaption.GetLength() ? (LPCTSTR)strCaption : NULL );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsManagerDlg message handlers

BOOL CSettingsManagerDlg::OnInitDialog()
{
	CSettingsSheet::OnInitDialog();

	m_bmHeader.LoadBitmap( IDB_WIZARD );

	SkinMe( _T("CSettingSheet"), IDR_MAINFRAME, TRUE );

	return TRUE;
}

void CSettingsManagerDlg::DoPaint(CDC& dc)
{
	CRect rc;
	GetClientRect( &rc );

	BITMAP pInfo;
	m_bmHeader.GetBitmap( &pInfo );

	CDC mdc;
	mdc.CreateCompatibleDC( &dc );
	CBitmap* pOldBitmap = (CBitmap*)mdc.SelectObject( &m_bmHeader );
	dc.BitBlt( 0, 0, pInfo.bmWidth, pInfo.bmHeight, &mdc, 0, 0, SRCCOPY );
	mdc.SelectObject( pOldBitmap );
	mdc.DeleteDC();

	/* //ToDo: Remove This?
	dc.FillSolidRect( 438, 0, rc.right - 438, 48, RGB( 0xBE, 0, 0 ) );

	dc.Draw3dRect( 438, 48, rc.right - 437, 2,
		RGB( 169, 0, 0 ), RGB( 110, 59, 59 ) );

	dc.Draw3dRect( 0, 50, rc.Width() + 1, 1,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
	*/

	CSettingsSheet::DoPaint( dc );
}

void CSettingsManagerDlg::OnOK()
{
	CSettingsSheet::OnOK();
	Settings.Save();
	AfxGetMainWnd()->Invalidate();
}

void CSettingsManagerDlg::OnApply()
{
	CSettingsSheet::OnApply();
	AfxGetMainWnd()->Invalidate();
}
