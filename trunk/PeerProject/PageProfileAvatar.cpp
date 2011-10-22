//
// PageProfileAvatar.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
// Portions copyright Shareaza Development Team, 2002-2007.
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
#include "Settings.h"
#include "PeerProject.h"
#include "PageProfileAvatar.h"
#include "ImageServices.h"
#include "ImageFile.h"
#include "GProfile.h"
#include "Colors.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CAvatarProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CAvatarProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CAvatarProfilePage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_AVATAR_BROWSE, OnAvatarBrowse)
	ON_BN_CLICKED(IDC_AVATAR_REMOVE, OnAvatarRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAvatarProfilePage property page

CAvatarProfilePage::CAvatarProfilePage() : CSettingsPage( CAvatarProfilePage::IDD )
{
	//{{AFX_DATA_INIT(CAvatarProfilePage)
	//}}AFX_DATA_INIT
}

CAvatarProfilePage::~CAvatarProfilePage()
{
}

void CAvatarProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvatarProfilePage)
	DDX_Control(pDX, IDC_AVATAR_REMOVE, m_wndRemove);
	DDX_Control(pDX, IDC_PREVIEW, m_wndPreview);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAvatarProfilePage message handlers

BOOL CAvatarProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	if ( CXMLElement* pAvatar = MyProfile.GetXML( _T("avatar") ) )
		m_sAvatar = pAvatar->GetAttributeValue( _T("path") );
	else
		m_sAvatar = Settings.General.Path + _T("\\Data\\DefaultAvatar.png");

	PrepareImage();

	return TRUE;
}

void CAvatarProfilePage::OnOK()
{
	if ( CXMLElement* pAvatar = MyProfile.GetXML( _T("avatar"), TRUE ) )
		pAvatar->AddAttribute( _T("path"), m_sAvatar );

	CSettingsPage::OnOK();
}

void CAvatarProfilePage::OnPaint()
{
	CPaintDC dc( this );

	CRect rc;
	m_wndPreview.GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.right = rc.left + 128;
	rc.bottom = rc.top + 128;

	if ( m_bmAvatar.m_hObject != NULL )
	{
		CDC dcMem;
		dcMem.CreateCompatibleDC( &dc );
		CBitmap* pOld = (CBitmap*)dcMem.SelectObject( &m_bmAvatar );
		dc.BitBlt( rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY );
		dcMem.SelectObject( pOld );
	}
	else
	{
		dc.Draw3dRect( &rc, Colors.m_crWindow, Colors.m_crWindow );
		rc.InflateRect( 1, 1 );
		dc.Draw3dRect( &rc, Colors.m_crMidtone, Colors.m_crMidtone );
	}
}

void CAvatarProfilePage::OnAvatarBrowse()
{
	CFileDialog dlg( TRUE, _T("png"), m_sAvatar, OFN_HIDEREADONLY,
		_T("Image Files|*.jpg;*.jpeg;*.png;*.bmp|") + LoadString( IDS_FILES_ALL ) + _T("|*.*||"), this );

	if ( dlg.DoModal() == IDOK )
	{
		m_sAvatar = dlg.GetPathName();
		PrepareImage();
		Invalidate();
	}
}

void CAvatarProfilePage::OnAvatarRemove()
{
	m_sAvatar.Empty();
	if ( m_bmAvatar.m_hObject != NULL ) m_bmAvatar.DeleteObject();
	Invalidate();
}

void CAvatarProfilePage::PrepareImage()
{
	if ( m_bmAvatar.m_hObject != NULL ) m_bmAvatar.DeleteObject();
	if ( m_sAvatar.IsEmpty() ) return;

	CImageFile pFile;

	CClientDC dc( this );
	SendMessage( WM_CTLCOLORSTATIC, (WPARAM)dc.GetSafeHdc(), (LPARAM)m_wndPreview.GetSafeHwnd() );

	if ( pFile.LoadFromFile( m_sAvatar ) && pFile.EnsureRGB( Colors.m_crDialog ) )	//dc.GetBkColor()
	{
		pFile.Resample( 128, 128 );
		m_bmAvatar.Attach( pFile.CreateBitmap() );
	}
}
