//
// PageSettingsProtocols.cpp
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
#include "PageSettingsProtocols.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CProtocolsSettingsPage, CSettingsPage)

//BEGIN_MESSAGE_MAP(CProtocolsSettingsPage, CSettingsPage)
//END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProtocolsSettingsPage property page

CProtocolsSettingsPage::CProtocolsSettingsPage() : CSettingsPage(CProtocolsSettingsPage::IDD)
{
}

CProtocolsSettingsPage::~CProtocolsSettingsPage()
{
}

void CProtocolsSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROTOCOLS, m_wndTree);
}

/////////////////////////////////////////////////////////////////////////////
// CProtocolsSettingsPage message handlers

BOOL CProtocolsSettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	HTREEITEM hNetworks = AddItem( NULL, _T("Peer-to-Peer Network Protocols") );

	HTREEITEM hG2 = AddItem( hNetworks, _T("Gnutella2 Network") );
	AddItem( hG2, _T("Name"), _T("Gnutella2  (\"G2\")") );
	AddItem( hG2, _T("Type"), _T("Decentralized") );
	AddItem( hG2, _T("NodeClass"), _T("Hub, Leaf") );
	AddItem( hG2, _T("PrimaryURN"), _T("sha1") );
	AddItem( hG2, _T("ProtocolVersion"), _T("1.0") );
	AddItem( hG2, _T("ComponentVersion"), _T("1.0.0.0") );

	HTREEITEM hG1 = AddItem( hNetworks, _T("Gnutella Network") );
	AddItem( hG1, _T("Name"), _T("Gnutella") );
	AddItem( hG1, _T("Type"), _T("Decentralized") );
	AddItem( hG1, _T("NodeClass"), _T("Leaf, Ultrapeer*") );
	AddItem( hG1, _T("PrimaryURN"), _T("sha1") );
	AddItem( hG1, _T("ProtocolVersion"), _T("0.6") );
	AddItem( hG1, _T("ComponentVersion"), _T("1.0.0.0") );

	HTREEITEM hED = AddItem( hNetworks, _T("eDonkey2000 Network") );
	AddItem( hED, _T("Name"), _T("eDonkey2000") );
	AddItem( hED, _T("Type"), _T("Server-Based") );
	AddItem( hED, _T("NodeClass"), _T("Client") );
	AddItem( hED, _T("PrimaryURN"), _T("ed2k (compound md4)") );
	AddItem( hED, _T("ProtocolVersion"), _T("1.1") );
	AddItem( hED, _T("ComponentVersion"), _T("1.0.0.0") );

	HTREEITEM hDC = AddItem( hNetworks, _T("ADC/NMDC Network") );
	AddItem( hDC, _T("Name"), _T("Direct Connect  (\"DC++\")") );
	AddItem( hDC, _T("Type"), _T("Hub-Based") );
	AddItem( hDC, _T("NodeClass"), _T("Client") );
	AddItem( hDC, _T("PrimaryURN"), _T("ttr") );
	AddItem( hDC, _T("ProtocolVersion"), _T("1.0") );
	AddItem( hDC, _T("ComponentVersion"), _T("1.0.0.0") );

	//HTREEITEM hED = AddItem( hNetworks, _T("KAD Network") );
	//AddItem( hED, _T("Name"), _T("KAD (Inactive)") );
	//AddItem( hED, _T("ComponentVersion"), _T("0.0.0.0") );

	HTREEITEM hTransfers = AddItem( NULL, _T("File Transfer Protocols") );

	HTREEITEM hHTTP = AddItem( hTransfers, _T("Hypertext Transfer Protocol (HTTP)") );
	AddItem( hHTTP, _T("Name"), _T("Hypertext Transfer Protocol (HTTP)") );
	AddItem( hHTTP, _T("Prefix"), _T("http://") );
	AddItem( hHTTP, _T("TransferMode"), _T("Stream") );
	AddItem( hHTTP, _T("Directions"), _T("Download, Upload") );
	AddItem( hHTTP, _T("Encodings"), _T("Deflate, Backwards") );
	AddItem( hHTTP, _T("Capabilities"), _T("THEX, PFS, Metadata, HUGE, Browse") );
	AddItem( hHTTP, _T("ProtocolVersion"), _T("1.1") );
	AddItem( hHTTP, _T("ComponentVersion"), _T("1.0.0.0") );

	HTREEITEM hFTP = AddItem( hTransfers, _T("File Transfer Protocol (FTP)") );
	AddItem( hFTP, _T("Name"), _T("File Transfer Protocol (FTP)") );
	AddItem( hFTP, _T("Prefix"), _T("ftp://") );
	AddItem( hFTP, _T("TransferMode"), _T("Stream") );
	AddItem( hFTP, _T("Directions"), _T("Download") );
	AddItem( hFTP, _T("Capabilities"), _T("Passive") );
	AddItem( hFTP, _T("ProtocolVersion"), _T("1.0") );
	AddItem( hFTP, _T("ComponentVersion"), _T("1.0.0.0") );

	HTREEITEM hEFTP = AddItem( hTransfers, _T("eDonkey2000 Client Link (EFTP)") );
	AddItem( hEFTP, _T("Name"), _T("eDonkey2000 Client Link FTP (EFTP)") );
	AddItem( hEFTP, _T("Prefix"), _T("ed2kftp://") );
	AddItem( hEFTP, _T("TransferMode"), _T("Block") );
	AddItem( hEFTP, _T("Directions"), _T("Download, Upload") );
	AddItem( hEFTP, _T("Capabilities"), _T("Hashset, SourceExchange, Deflate") );
	AddItem( hEFTP, _T("ProtocolVersion"), _T("1.1") );
	AddItem( hEFTP, _T("ComponentVersion"), _T("1.0.0.0") );

	HTREEITEM hBT = AddItem( hTransfers, _T("BitTorrent Coupling (BT)") );
	AddItem( hBT, _T("Name"), _T("BitTorrent Coupling (BT)") );
	AddItem( hBT, _T("Prefix"), _T("btc://") );
	AddItem( hBT, _T("TransferMode"), _T("Block") );
	AddItem( hBT, _T("Directions"), _T("Download, Upload") );
	AddItem( hBT, _T("Capabilities"), _T("PeerExchange, (etc.)") );
	AddItem( hBT, _T("ProtocolVersion"), _T("1.0") );
	AddItem( hBT, _T("ComponentVersion"), _T("1.0.0.0") );

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
		return m_wndTree.InsertItem( TVIF_TEXT|TVIF_STATE, str,
			0, 0, 0, 0, 0, hParent, TVI_LAST );
	}
	else
	{
		CString str( pszText );
		if ( Settings.General.LanguageRTL ) str = _T("\x202A") + str;
		return m_wndTree.InsertItem( TVIF_TEXT|TVIF_STATE, str,
			0, 0, TVIS_EXPANDED|TVIS_BOLD, TVIS_EXPANDED|TVIS_BOLD, 0, hParent, TVI_LAST );
	}
}
