//
// PageSettingsProtocols.cpp
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
#include "PageSettingsProtocols.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CProtocolsSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CProtocolsSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CProtocolsSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProtocolsSettingsPage property page

CProtocolsSettingsPage::CProtocolsSettingsPage() : CSettingsPage(CProtocolsSettingsPage::IDD)
{
	//{{AFX_DATA_INIT(CProtocolsSettingsPage)
	//}}AFX_DATA_INIT
}

CProtocolsSettingsPage::~CProtocolsSettingsPage()
{
}

void CProtocolsSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProtocolsSettingsPage)
	DDX_Control(pDX, IDC_PROTOCOLS, m_wndTree);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CProtocolsSettingsPage message handlers

BOOL CProtocolsSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	HTREEITEM hNetworks = AddItem( NULL, _T("Peer-to-Peer Network Protocols") );
	
	HTREEITEM hG2 = AddItem( hNetworks, _T("Gnutella2 Network") );
	AddItem( hG2, _T("Name"), _T("Gnutella2") );
	AddItem( hG2, _T("Type"), _T("Decentralised") );
	AddItem( hG2, _T("PrimaryURN"), _T("sha1") );
	AddItem( hG2, _T("NodeClass"), _T("Hub, Leaf") );
	AddItem( hG2, _T("ProtocolVersion"), _T("1.0") );
	AddItem( hG2, _T("ComponentVersion"), _T("2.0.0.0") );
	
	HTREEITEM hG1 = AddItem( hNetworks, _T("Gnutella1 Network") );
	AddItem( hG1, _T("Name"), _T("Gnutella1") );
	AddItem( hG1, _T("Type"), _T("Decentralised") );
	AddItem( hG1, _T("PrimaryURN"), _T("sha1") );
	AddItem( hG1, _T("NodeClass"), _T("Leaf") );
	AddItem( hG1, _T("ProtocolVersion"), _T("0.6") );
	AddItem( hG1, _T("ComponentVersion"), _T("2.0.0.0") );
	
	HTREEITEM hED = AddItem( hNetworks, _T("eDonkey2000 Network") );
	AddItem( hED, _T("Name"), _T("eDonkey2000") );
	AddItem( hED, _T("Type"), _T("Server-Based") );
	AddItem( hED, _T("PrimaryURN"), _T("ed2k (compound md4)") );
	AddItem( hED, _T("NodeClass"), _T("Client") );
	AddItem( hED, _T("ProtocolVersion"), _T("1.1") );
	AddItem( hED, _T("ComponentVersion"), _T("2.3.0.0") );
	
	HTREEITEM hTransfers = AddItem( NULL, _T("File Transfer Protocols") );
	
	HTREEITEM hHTTP = AddItem( hTransfers, _T("Hypertext Transfer Protocol (HTTP)") );
	AddItem( hHTTP, _T("Name"), _T("Hypertext Transfer Protocol (HTTP)") );
	AddItem( hHTTP, _T("Prefix"), _T("http://") );
	AddItem( hHTTP, _T("TransferMode"), _T("Stream") );
	AddItem( hHTTP, _T("Directions"), _T("Download, Upload") );
	AddItem( hHTTP, _T("Capabilities"), _T("THEX, PFS, Metadata, HUGE, Browse") );
	AddItem( hHTTP, _T("Encodings"), _T("Deflate, Backwards") );
	AddItem( hHTTP, _T("ProtocolVersion"), _T("1.1") );
	AddItem( hHTTP, _T("ComponentVersion"), _T("2.0.0.0") );

	HTREEITEM hFTP = AddItem( hTransfers, _T("File Transfer Protocol (FTP)") );
	AddItem( hFTP, _T("Name"), _T("File Transfer Protocol (FTP)") );
	AddItem( hFTP, _T("Prefix"), _T("ftp://") );
	AddItem( hFTP, _T("TransferMode"), _T("Stream") );
	AddItem( hFTP, _T("Directions"), _T("Download") );
	AddItem( hFTP, _T("Capabilities"), _T("Passive") );
	AddItem( hFTP, _T("ProtocolVersion"), _T("1.0") );
	AddItem( hFTP, _T("ComponentVersion"), _T("2.2.0.0") );
	
	HTREEITEM hEFTP = AddItem( hTransfers, _T("eDonkey2000 Client Link (EFTP)") );
	AddItem( hEFTP, _T("Name"), _T("eDonkey2000 Client Link FTP (EFTP)") );
	AddItem( hEFTP, _T("Prefix"), _T("ed2kftp://") );
	AddItem( hEFTP, _T("TransferMode"), _T("Block") );
	AddItem( hEFTP, _T("Directions"), _T("Download, Upload") );
	AddItem( hEFTP, _T("Capabilities"), _T("Hashset, SourceExchange, Deflate") );
	AddItem( hEFTP, _T("ProtocolVersion"), _T("1.1") );
	AddItem( hEFTP, _T("ComponentVersion"), _T("2.3.0.0") );
	
	HTREEITEM hBT = AddItem( hTransfers, _T("BitTorrent Coupling (BT)") );
	AddItem( hBT, _T("Name"), _T("BitTorrent Coupling (BT)") );
	AddItem( hBT, _T("Prefix"), _T("btc://") );
	AddItem( hBT, _T("TransferMode"), _T("Block") );
	AddItem( hBT, _T("Directions"), _T("Download, Upload") );
	// AddItem( hBT, _T("Capabilities"), _T("Hashset, SourceExchange, Deflate") );
	AddItem( hBT, _T("ProtocolVersion"), _T("1.0") );
	AddItem( hBT, _T("ComponentVersion"), _T("2.2.0.0") );
	
	return TRUE;
}

HTREEITEM CProtocolsSettingsPage::AddItem(HTREEITEM hParent, LPCTSTR pszText, LPCTSTR pszValue)
{
	if ( Settings.General.LanguageRTL ) m_wndTree.ModifyStyleEx( 0, TVS_RTLREADING, 0 );
	if ( pszValue != NULL )
	{
		CString str;
		str.Format( _T("%s = %s"), pszText, pszValue );
		if ( Settings.General.LanguageRTL ) str = _T("\x202A") + str;
		return m_wndTree.InsertItem( TVIF_TEXT|TVIF_STATE,
			str, 0, 0, 0, 0, 0, hParent, TVI_LAST );
	}
	else
	{
		CString str(pszText);
		if ( Settings.General.LanguageRTL ) str = _T("\x202A") + str;
		return m_wndTree.InsertItem( TVIF_TEXT|TVIF_STATE,
			str, 0, 0, TVIS_EXPANDED|TVIS_BOLD,
			TVIS_EXPANDED|TVIS_BOLD, 0, hParent, TVI_LAST );
	}
}
