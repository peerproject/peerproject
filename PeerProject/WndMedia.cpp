//
// WndMedia.cpp
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
#include "ImageServices.h"
#include "Skin.h"
#include "WndMedia.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CMediaWnd, CPanelWnd, 1)

BEGIN_MESSAGE_MAP(CMediaWnd, CPanelWnd)
	//{{AFX_MSG_MAP(CMediaWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_NCLBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_SYSCOMMAND()
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_MESSAGE(0x0319, OnMediaKey)
	ON_MESSAGE(WM_DEVMODECHANGE, OnDevModeChange)
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(WM_ENQUEUEFILE, OnEnqueueFile)
	ON_MESSAGE(WM_PLAYFILE, OnPlayFile)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMediaWnd construction

CMediaWnd::CMediaWnd() : CPanelWnd( TRUE, FALSE )
{
	m_bPanelClose = TRUE;
	Create( IDR_MEDIAFRAME );
}

CMediaWnd::~CMediaWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMediaWnd operations

BOOL CMediaWnd::PlayFile(LPCTSTR pszFile)
{
	return m_wndFrame.PlayFile( pszFile );
}

BOOL CMediaWnd::EnqueueFile(LPCTSTR pszFile)
{
	return m_wndFrame.EnqueueFile( pszFile );
}

BOOL CMediaWnd::IsPlaying()
{
	return m_wndFrame.IsPlaying();
}

void CMediaWnd::OnFileDelete(LPCTSTR pszFile)
{
	m_wndFrame.OnFileDelete( pszFile );
}

/////////////////////////////////////////////////////////////////////////////
// CMediaWnd message handlers

int CMediaWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	m_wndFrame.Create( this );
	LoadState();

	ENABLE_DROP()

	return 0;
}

void CMediaWnd::OnDestroy()
{
	DISABLE_DROP()

	SaveState();
	CPanelWnd::OnDestroy();
}

void CMediaWnd::OnSkinChange()
{
	CPanelWnd::OnSkinChange();
	m_wndFrame.OnSkinChange();
}

BOOL CMediaWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndFrame.m_hWnd != NULL )
	{
		if ( m_wndFrame.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}

	return CPanelWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

LRESULT CMediaWnd::OnIdleUpdateCmdUI(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if ( m_wndFrame.m_hWnd != NULL && m_wndFrame.GetParent() != this )
	{
		m_wndFrame.OnUpdateCmdUI();
	}

	CPanelWnd::OnIdleUpdateCmdUI();

	return 0;
}

BOOL CMediaWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN )
	{
		if ( m_wndFrame.m_hWnd && m_wndFrame.PreTranslateMessage( pMsg ) ) return TRUE;
	}

	return CPanelWnd::PreTranslateMessage(pMsg);
}

void CMediaWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( ( nID & 0xFFF0 ) == SC_CLOSE )
	{
		PostMessage( WM_CLOSE );
		return;
	}

	CPanelWnd::OnSysCommand( nID, lParam );
}

void CMediaWnd::OnSize(UINT nType, int cx, int cy)
{
	CPanelWnd::OnSize( nType, cx, cy );

	if ( m_wndFrame.m_hWnd != NULL && m_wndFrame.GetParent() == this )
	{
		m_wndFrame.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
	}
}

void CMediaWnd::OnPaint()
{
	CPaintDC dc( this );

	if ( m_wndFrame.m_hWnd == NULL || m_wndFrame.GetParent() != this )
	{
		CRect rc;
		GetClientRect( &rc );
		dc.FillSolidRect( &rc, CoolInterface.m_crMediaWindowBack );
	}
}

LRESULT CMediaWnd::OnMediaKey(WPARAM wParam, LPARAM lParam)
{
	return m_wndFrame.SendMessage( 0x0319, wParam, lParam );
}

LRESULT CMediaWnd::OnDevModeChange(WPARAM wParam, LPARAM lParam)
{
	return m_wndFrame.SendMessage( WM_DEVMODECHANGE, wParam, lParam );
}

LRESULT CMediaWnd::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	return m_wndFrame.SendMessage( WM_DISPLAYCHANGE, wParam, lParam );
}

BOOL CMediaWnd::OnNcActivate(BOOL bActive)
{
	m_wndFrame.UpdateScreenSaverStatus( bActive );

	return CPanelWnd::OnNcActivate(bActive);
}

/////////////////////////////////////////////////////////////////////////////
// CMediaWnd IDropTarget implementation

IMPLEMENT_DROP(CMediaWnd,CPanelWnd)

BOOL CMediaWnd::OnDrop(IDataObject* pDataObj, DWORD /*grfKeyState*/, POINT ptScreen, DWORD* pdwEffect, BOOL bDrop)
{
	if ( ! pDataObj )
		return TRUE;

	FORMATETC fmtc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if ( SUCCEEDED ( pDataObj->QueryGetData( &fmtc ) ) )
	{
		*pdwEffect = DROPEFFECT_COPY;

		if ( ! bDrop )
			return TRUE;

		CList< CString > oFiles;
		if ( CPeerProjectDataSource::ObjectToFiles( pDataObj, oFiles ) == S_OK )
		{
			CPoint pt( ptScreen );
			m_wndFrame.ScreenToClient( &pt );
			CWnd* pDropped = m_wndFrame.ChildWindowFromPoint( pt );

			BOOL bEnqueue;
			if ( pDropped != NULL )
				bEnqueue = ( pDropped->IsKindOf( RUNTIME_CLASS(CMediaListCtrl) ) );
			else
				bEnqueue = FALSE;

			for ( POSITION pos = oFiles.GetHeadPosition() ; pos ; )
			{
				CString strFile = oFiles.GetNext( pos );
				// Async enqueuing/playing of file to prevent locking in dialogs and so on
				if ( bEnqueue )
					PostMessage( WM_ENQUEUEFILE, 0, (LPARAM) new CString( strFile ) );
				else
					PostMessage( WM_PLAYFILE, 0, (LPARAM) new CString( strFile ) );
			}
			return TRUE;
		}

		*pdwEffect = DROPEFFECT_NONE;
	}
	return FALSE;
}

LRESULT CMediaWnd::OnEnqueueFile(WPARAM /*wParam*/, LPARAM lParam)
{
	m_wndFrame.EnqueueFile( *(CString*)lParam );
	delete (CString*)lParam;
	return 0;
}

LRESULT CMediaWnd::OnPlayFile(WPARAM /*wParam*/, LPARAM lParam)
{
	m_wndFrame.PlayFile( *(CString*)lParam );
	delete (CString*)lParam;
	return 0;
}
