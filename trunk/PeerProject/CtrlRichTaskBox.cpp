//
// CtrlRichTaskBox.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "RichDocument.h"
#include "CtrlRichTaskBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CRichTaskBox, CTaskBox)

BEGIN_MESSAGE_MAP(CRichTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CRichTaskBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRichTaskBox construction

CRichTaskBox::CRichTaskBox()
	: m_pDocument	( NULL )
	, m_nWidth		( -1 )
{
}

CRichTaskBox::~CRichTaskBox()
{
	if ( m_pDocument ) delete m_pDocument;
}

/////////////////////////////////////////////////////////////////////////////
// CRichTaskBox message handlers

BOOL CRichTaskBox::Create(CTaskPanel* pPanel, LPCTSTR pszCaption, UINT nIDIcon)
{
	return CTaskBox::Create( pPanel, 0, pszCaption, nIDIcon );
}

int CRichTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	CRect rc;
	GetClientRect( &rc );
	m_wndView.Create( WS_CHILD|WS_VISIBLE, rc, this, 1 );
	m_wndView.SetOwner( GetPanel()->GetOwner() );

	return 0;
}

void CRichTaskBox::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize( nType, cx, cy );

	if ( cx != m_nWidth )
	{
		m_nWidth = cx;
		SetSize( m_wndView.FullHeightMove( 0, 0, cx ) );
	}
}

void CRichTaskBox::SetDocument(CRichDocument* pDocument)
{
	m_wndView.SetDocument( pDocument );
	Update();
}

void CRichTaskBox::Update()
{
	if ( m_wndView.IsModified() )
	{
		CRect rc;
		GetClientRect( &rc );
		m_nWidth = rc.Width();

		SetSize( m_wndView.FullHeightMove( 0, 0, m_nWidth ) );
		m_wndView.Invalidate();
	}
}