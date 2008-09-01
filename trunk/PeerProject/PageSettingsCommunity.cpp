//
// PageSettingsCommunity.cpp
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
#include "PageSettingsCommunity.h"
#include "DlgProfileManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCommunitySettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CCommunitySettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CCommunitySettingsPage)
	ON_BN_CLICKED(IDC_EDIT_PROFILE, OnEditProfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCommunitySettingsPage property page

CCommunitySettingsPage::CCommunitySettingsPage() : CSettingsPage(CCommunitySettingsPage::IDD)
{
	//{{AFX_DATA_INIT(CCommunitySettingsPage)
	m_bChatEnable = FALSE;
	m_bChatAllNetworks = FALSE;
	m_bChatFilter = FALSE;
	m_bChatCensor = FALSE;
	//}}AFX_DATA_INIT
}

CCommunitySettingsPage::~CCommunitySettingsPage()
{
}

void CCommunitySettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommunitySettingsPage)
	DDX_Check(pDX, IDC_CHAT_ENABLE, m_bChatEnable);
	DDX_Check(pDX, IDC_CHAT_ALLNETWORKS, m_bChatAllNetworks);
	DDX_Check(pDX, IDC_CHAT_FILTER, m_bChatFilter);
	DDX_Check(pDX, IDC_CHAT_CENSOR, m_bChatCensor);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCommunitySettingsPage message handlers

BOOL CCommunitySettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_bChatEnable		= Settings.Community.ChatEnable;
	m_bChatAllNetworks	= Settings.Community.ChatAllNetworks;
	m_bChatFilter		= Settings.Community.ChatFilter;
	m_bChatCensor		= Settings.Community.ChatCensor;

	UpdateData( FALSE );

	return TRUE;
}

void CCommunitySettingsPage::OnEditProfile()
{
	CProfileManagerDlg dlg;
	dlg.DoModal();
}

void CCommunitySettingsPage::OnOK()
{
	UpdateData();

	Settings.Community.ChatEnable		= m_bChatEnable != FALSE;
	Settings.Community.ChatAllNetworks	= m_bChatAllNetworks != FALSE;
	Settings.Community.ChatFilter		= m_bChatFilter != FALSE;
	Settings.Community.ChatCensor		= m_bChatCensor != FALSE;

	CSettingsPage::OnOK();
}

