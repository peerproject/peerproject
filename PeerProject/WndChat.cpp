//
// WndChat.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "PeerProject.h"
#include "Settings.h"
#include "WndChat.h"
#include "CtrlChatFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CChatWnd, CChildWnd)	//CPanelWnd

BEGIN_MESSAGE_MAP(CChatWnd, CChildWnd)	//CPanelWnd
	//{{AFX_MSG_MAP(CChatWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChatWnd construction

CChatWnd::CChatWnd(CChatFrame* pFrame)
{
	m_pFrame = pFrame;
	ASSERT_VALID(m_pFrame);
	Create( IDR_CHATFRAME, FALSE );
}

CChatWnd::~CChatWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd message handlers

int CChatWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	ASSERT_VALID( m_pFrame );

	m_pFrame->SetParent( this );

	LoadState( _T("CChatWnd"), FALSE );

	CRect rc;
	GetClientRect( &rc );
	m_pFrame->MoveWindow( &rc, TRUE );

	SetAlert();

	return 0;
}

void CChatWnd::OnDestroy()
{
	SaveState( _T("CChatWnd") );

	if ( m_pFrame != NULL )
	{
		m_pFrame->DestroyWindow();
		delete m_pFrame;
	}

	CChildWnd::OnDestroy();
}

void CChatWnd::OnSkinChange()
{
	CChildWnd::OnSkinChange();
	if ( m_pFrame != NULL ) m_pFrame->OnSkinChange();
}

BOOL CChatWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_pFrame != NULL )
	{
		if ( m_pFrame->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}

	return CChildWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CChatWnd::OnSize(UINT nType, int cx, int cy)
{
	CChildWnd::OnSize( nType, cx, cy );

	if ( m_pFrame != NULL )
	{
		m_pFrame->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER|SWP_SHOWWINDOW );
	}
}

void CChatWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 && m_pFrame != NULL && IsActive( TRUE ) )
	{
		m_pFrame->SetFocus();
	}
	else if ( nIDEvent == 2 && m_pFrame != NULL )
	{
		CString str;
		m_pFrame->GetWindowText( str );
		//if ( str.Find( _T("Chat : ") ) == 0 )
		//{
		//	CString strTranslation;
		//	LoadString( strTranslation, IDR_CHATFRAME );
		//	str = strTranslation + str.Mid( 4 );
		//}
		SetWindowText( str );
	}

	CChildWnd::OnTimer( nIDEvent );
}
