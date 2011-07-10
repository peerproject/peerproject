//
// CtrlMatchTip.cpp
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
#include "PeerProject.h"
#include "Settings.h"
#include "CtrlCoolTip.h"
#include "CtrlMatchTip.h"
#include "Colors.h"
#include "CoolInterface.h"
#include "ShellIcons.h"
#include "Library.h"
#include "SharedFile.h"
#include "MatchObjects.h"
#include "QueryHit.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "VendorCache.h"
#include "Flags.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

BEGIN_MESSAGE_MAP(CMatchTipCtrl, CWnd)
	//{{AFX_MSG_MAP(CMatchTipCtrl)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define TIP_TEXTHEIGHT	14
#define TIP_ICONHEIGHT	16
// In CtrlCoolTip.h:
//#define TIP_OFFSET_X	0
//#define TIP_OFFSET_Y	24
//#define TIP_MARGIN	6

LPCTSTR		CMatchTipCtrl::m_hClass = NULL;
CBrush		CMatchTipCtrl::m_brBack;
//COLORREF	CMatchTipCtrl::m_crBack;
//COLORREF	CMatchTipCtrl::m_crText;
//COLORREF	CMatchTipCtrl::m_crBorder;
//COLORREF	CMatchTipCtrl::m_crWarnings;


/////////////////////////////////////////////////////////////////////////////
// CMatchTipCtrl construction

CMatchTipCtrl::CMatchTipCtrl()
	: m_pOwner		( NULL )
	, m_bVisible	( FALSE )
	, m_pFile		( NULL )
	, m_pHit		( NULL )
	, m_tOpen		( 0 )
	, m_nIcon		( 0 )
{
	if ( ! m_hClass )
		m_hClass = AfxRegisterWndClass( CS_SAVEBITS |
			( ! Settings.Interface.TipShadow || theApp.m_bIsWin2000 ? 0 : CS_DROPSHADOW ) );
}

CMatchTipCtrl::~CMatchTipCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMatchTipCtrl operations

BOOL CMatchTipCtrl::Create(CWnd* pParentWnd)
{
	CRect rc( 0, 0, 0, 0 );
	m_pOwner = pParentWnd;
	DWORD dwStylesEx = WS_EX_TOPMOST | ( Settings.General.LanguageRTL ? WS_EX_LAYOUTRTL : 0 );
	return CWnd::CreateEx( dwStylesEx, m_hClass, NULL, WS_POPUP|WS_DISABLED, rc, pParentWnd, 0, NULL );
}

void CMatchTipCtrl::Show(CMatchFile* pFile, CQueryHit* pHit)
{
	if ( AfxGetMainWnd() != GetForegroundWindow() ) return;
	if ( ! Settings.Interface.TipSearch ) return;

//	m_crBack	= Colors.m_crTipBack;
//	m_crText	= Colors.m_crTipText;
//	m_crBorder	= Colors.m_crTipBorder;
//	m_crWarnings = Colors.m_crTipWarnings;	// Set color of warning messages

	if ( m_brBack.m_hObject ) m_brBack.DeleteObject();
	m_brBack.CreateSolidBrush( Colors.m_crTipBack );

	CPoint point;
	GetCursorPos( &point );

	if ( m_bVisible )
	{
		if ( pFile == m_pFile && pHit == m_pHit ) return;

		Hide();

		m_pFile	= pFile;
		m_pHit	= pHit;

		ShowInternal();
	}
	else if ( point != m_pOpen )
	{
		m_pFile	= pFile;
		m_pHit	= pHit;
		m_pOpen	= point;
		m_tOpen	= GetTickCount() + Settings.Interface.TipDelay;
	}
}

void CMatchTipCtrl::Hide()
{
	m_pFile	= NULL;
	m_pHit	= NULL;
	m_pMetadata.Clear();
	m_tOpen	= 0;

	if ( m_bVisible )
	{
		ShowWindow( SW_HIDE );
		ModifyStyleEx( WS_EX_LAYERED, 0 );
		m_bVisible = FALSE;
		GetCursorPos( &m_pOpen );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMatchTipCtrl system message handlers

int CMatchTipCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	SetTimer( 1, 250, NULL );

	return 0;
}

void CMatchTipCtrl::OnDestroy()
{
	KillTimer( 1 );
	CWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CMatchTipCtrl show logic message handlers

void CMatchTipCtrl::OnTimer(UINT_PTR /*nIDEvent*/)
{
	CPoint point;
	if ( ! GetCursorPos( &point ) || WindowFromPoint( point ) != m_pOwner )
	{
		if ( m_bVisible )
			Hide();
		return;
	}
	else
	{
		CWnd* pWnd = GetForegroundWindow();

		if ( pWnd != this && pWnd != AfxGetMainWnd() )
		{
			if ( m_bVisible )
				Hide();
			return;
		}
	}

	if ( ! m_bVisible && m_tOpen && GetTickCount() >= m_tOpen )
	{
		m_tOpen = 0;
		if ( point == m_pOpen )
			ShowInternal();
	}
}

void CMatchTipCtrl::ShowInternal()
{
	if ( m_bVisible )
		return;

	if ( m_pHit != NULL )
		LoadFromHit();
	else if ( m_pFile != NULL )
		LoadFromFile();
	else
		return;

	if ( m_sName.GetLength() > 128 )
		m_sName = m_sName.Left( 128 );

	m_bVisible = TRUE;

	CSize sz = ComputeSize();

	CRect rc( m_pOpen.x + TIP_OFFSET_X, m_pOpen.y + TIP_OFFSET_Y, 0, 0 );
	rc.right = rc.left + sz.cx;
	rc.bottom = rc.top + sz.cy;

	HMONITOR hMonitor = MonitorFromPoint( m_pOpen, MONITOR_DEFAULTTONEAREST );

	MONITORINFO oMonitor = {0};
	oMonitor.cbSize = sizeof( MONITORINFO );
	GetMonitorInfo( hMonitor, &oMonitor );

	if ( rc.right >= oMonitor.rcWork.right)
		rc.OffsetRect( oMonitor.rcWork.right - rc.right - 4, 0 );

	if ( rc.bottom >= oMonitor.rcWork.bottom )
		rc.OffsetRect( 0, -sz.cy - TIP_OFFSET_Y - 4 );

	if ( Settings.Interface.TipAlpha == 255 )
	{
		ModifyStyleEx( WS_EX_LAYERED, 0 );
	}
	else
	{
		ModifyStyleEx( 0, WS_EX_LAYERED );
		SetLayeredWindowAttributes( 0, (BYTE)Settings.Interface.TipAlpha, LWA_ALPHA );
	}

	SetWindowPos( &wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(),
		SWP_SHOWWINDOW|SWP_NOACTIVATE );
	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CMatchTipCtrl load from content

void CMatchTipCtrl::LoadFromFile()
{
	m_sName = m_pFile->m_sName;
	if ( m_pFile->GetTotalHitsCount() == 1 )
	{
		m_sCountryCode = m_pFile->GetBestCountry();
		m_sCountry = theApp.GetCountryName( m_pFile->GetBestAddress() );
	}
	else
	{
		m_sCountryCode = _T("");
		m_sCountry = _T("");
	}
	m_sSize = m_pFile->m_sSize;
	LoadTypeInfo();

	if ( Settings.General.GUIMode == GUI_BASIC )
	{
		m_sSHA1.Empty();
		m_sTiger.Empty();
		m_sED2K.Empty();
		m_sBTH.Empty();
		m_sMD5.Empty();
	}
	else
	{
		m_sSHA1 = m_pFile->m_oSHA1.toShortUrn();
		m_sTiger = m_pFile->m_oTiger.toShortUrn();
		m_sED2K = m_pFile->m_oED2K.toShortUrn();
		m_sBTH = m_pFile->m_oBTH.toShortUrn();
		m_sMD5 = m_pFile->m_oMD5.toShortUrn();
	}

	m_pFile->GetPartialTip( m_sPartial );
	m_pFile->GetQueueTip( m_sQueue );

	m_pSchema = m_pFile->AddHitsToMetadata( m_pMetadata );

	if ( m_pSchema != NULL )
	{
		m_pMetadata.Vote();
		m_pMetadata.Clean( 72 );
	}

	m_nRating = m_pFile->m_nRated ? m_pFile->m_nRating / m_pFile->m_nRated : 0;

	m_pFile->GetStatusTip( m_sStatus, m_crStatus );
	m_pFile->GetUser( m_sUser );

	if ( m_pFile->m_bBusy == 2 )
		LoadString( m_sBusy, IDS_TIP_FILE_BUSY );
	else
		m_sBusy.Empty();

	if ( m_pFile->m_bPush == 2 )
		LoadString( m_sPush, IDS_TIP_FILE_FIREWALLED );
	else
		m_sPush.Empty();

	if ( m_pFile->m_bStable == 1 )
		LoadString( m_sUnstable, IDS_TIP_FILE_UNSTABLE );
	else
		m_sUnstable.Empty();
}

void CMatchTipCtrl::LoadFromHit()
{
	m_sName = m_pHit->m_sName;
	m_sSize = Settings.SmartVolume( m_pHit->m_nSize );
	LoadTypeInfo();

	if ( Settings.General.GUIMode == GUI_BASIC )
	{
		m_sSHA1.Empty();
		m_sTiger.Empty();
		m_sED2K.Empty();
		m_sBTH.Empty();
		m_sMD5.Empty();
	}
	else
	{
		m_sSHA1 = m_pHit->m_oSHA1.toShortUrn();
		m_sTiger = m_pHit->m_oTiger.toShortUrn();
		m_sED2K = m_pHit->m_oED2K.toShortUrn();
		m_sBTH = m_pHit->m_oBTH.toShortUrn();
		m_sMD5 = m_pHit->m_oMD5.toShortUrn();
	}

	if ( m_pHit->m_nPartial )
	{
		CString strFormat;
		LoadString( strFormat, IDS_TIP_PARTIAL );
		m_sPartial.Format( strFormat, 100.0f * (float)m_pHit->m_nPartial / (float)m_pHit->m_nSize );
	}
	else
	{
		m_sPartial.Empty();
	}

	if ( m_pHit->m_nUpSlots )
	{
		CString strFormat;
		LoadString( strFormat, IDS_TIP_QUEUE );
		m_sQueue.Format( strFormat, m_pHit->m_nUpSlots,
			max( 0, m_pHit->m_nUpQueue - m_pHit->m_nUpSlots ) );
	}
	else
	{
		m_sQueue.Empty();
	}

	m_pSchema = m_pHit->m_pSchema;

	m_pMetadata.Setup( m_pSchema );

	if ( m_pSchema != NULL )
	{
		if ( m_pHit->m_pXML && m_pSchema->Equals( m_pHit->m_pSchema ) )
			m_pMetadata.Combine( m_pHit->m_pXML );
		m_pMetadata.Clean( 72 );
	}

	m_nRating = m_pHit->m_nRating;

	m_sStatus.Empty();

	if ( m_pFile->GetLibraryStatus() == TRI_FALSE )
	{
		LoadString( m_sStatus, IDS_TIP_EXISTS_LIBRARY );
		m_crStatus = Colors.m_crTextStatus ;
	}
	else if ( m_pFile->m_bDownload || m_pHit->m_bDownload )
	{
		LoadString( m_sStatus, IDS_TIP_EXISTS_DOWNLOAD );
		m_crStatus = Colors.m_crTextStatus ;
	}
	else if ( m_pHit->m_bBogus )
	{
		LoadString( m_sStatus, IDS_TIP_BOGUS );
		m_crStatus = Colors.m_crTextAlert ;
	}
	else if ( ! m_pHit->m_sComments.IsEmpty() )
	{
		if ( m_pHit->m_nRating == 1 )
			LoadString( m_sStatus, IDS_TIP_EXISTS_BLACKLISTED );
		m_sStatus += m_pHit->m_sComments;
		m_crStatus = Colors.m_crTextAlert ;
	}
	else if ( m_pFile->GetLibraryStatus() == TRI_TRUE )  // Ghost rated
	{
		LoadString( m_sStatus, IDS_TIP_EXISTS_DELETED );
		m_crStatus = Colors.m_crTextAlert ;
	}

	// Is this a firewalled eDonkey client
	if ( m_pHit->m_nProtocol == PROTOCOL_ED2K && m_pHit->m_bPush == TRI_TRUE )
	{
		m_sUser.Format( _T("%lu@%s - %s"),
			m_pHit->m_oClientID.begin()[2],
			(LPCTSTR)CString( inet_ntoa( (IN_ADDR&)*m_pHit->m_oClientID.begin() ) ),
			(LPCTSTR)m_pHit->m_pVendor->m_sName );
	}
	else
	{
		m_sUser.Format( _T("%s:%u - %s"),
			(LPCTSTR)CString( inet_ntoa( m_pHit->m_pAddress ) ),
			m_pHit->m_nPort,
			(LPCTSTR)m_pHit->m_pVendor->m_sName );
	}

	// Add the Nickname if there is one and they are being shown
	if ( Settings.Search.ShowNames && ! m_pHit->m_sNick.IsEmpty() )
		m_sUser = m_pHit->m_sNick + _T(" (") + m_sUser + _T(")");

	m_sCountryCode = m_pHit->m_sCountry;
	m_sCountry = theApp.GetCountryName( m_pHit->m_pAddress );

	if ( m_pHit->m_bBusy == 2 )
		LoadString( m_sBusy, IDS_TIP_SOURCE_BUSY );
	else
		m_sBusy.Empty();

	if ( m_pHit->m_bPush == 2 )
		LoadString( m_sPush, IDS_TIP_SOURCE_FIREWALLED );
	else
		m_sPush.Empty();

	if ( m_pHit->m_bStable == 1 )
		LoadString( m_sUnstable, IDS_TIP_SOURCE_UNSTABLE );
	else
		m_sUnstable.Empty();
}

BOOL CMatchTipCtrl::LoadTypeInfo()
{
	int nPeriod = m_sName.ReverseFind( '.' );

	m_sType.Empty();
	m_nIcon = 0;

	if ( nPeriod > 0 )
	{
		CString strType = m_sName.Mid( nPeriod );
		CString strName, strMime;

		ShellIcons.Lookup( strType, NULL, NULL, &strName, &strMime );
		m_nIcon = ShellIcons.Get( strType, 32 );

		if ( ! strName.IsEmpty() )
		{
			m_sType = strName;
			if ( strMime.GetLength() )
				m_sType += _T(" (") + strMime + _T(")");
		}
		else
		{
			m_sType = strType.Mid( 1 );
		}
	}

	if ( m_sType.IsEmpty() )
		m_sType = _T("Unknown");

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMatchTipCtrl layout

CSize CMatchTipCtrl::ComputeSize()
{
	CClientDC dc( this );
	CSize sz( 0, 0 );

	CFont* pOldFont = (CFont*)dc.SelectObject( &CoolInterface.m_fntBold );

	ExpandSize( dc, sz, m_sName );
	sz.cy += TIP_TEXTHEIGHT;

	dc.SelectObject( &CoolInterface.m_fntNormal );

	if ( ! m_sUser.IsEmpty() )
	{
		ExpandSize( dc, sz, m_sUser );
		sz.cy += TIP_TEXTHEIGHT;
	}

	if ( ! m_sCountry.IsEmpty() )
	{
		ExpandSize( dc, sz, m_sCountry, 18 + 2 );
		sz.cy += TIP_TEXTHEIGHT;
	}

	sz.cy += 5 + 6;

	if ( ! m_sStatus.IsEmpty() )
	{
		dc.SelectObject( &CoolInterface.m_fntBold );
		ExpandSize( dc, sz, m_sStatus );
		dc.SelectObject( &CoolInterface.m_fntNormal );
		sz.cy += TIP_TEXTHEIGHT;
		sz.cy += 5 + 6;
	}

	sz.cy += 32;
	CString strTest;
	LoadString( strTest, IDS_TIP_SIZE );
	strTest.Append( _T(": ") );
	ExpandSize( dc, sz, strTest + m_sSize, 40 );
	LoadString( strTest, IDS_TIP_TYPE );
	strTest.Append( _T(": ") );
	ExpandSize( dc, sz, strTest + m_sType, 40 );

	if ( ! m_sSHA1.IsEmpty() || ! m_sTiger.IsEmpty() || ! m_sED2K.IsEmpty() ||
		! m_sBTH.IsEmpty() || ! m_sMD5.IsEmpty() )
	{
		sz.cy += 5 + 6;

		if ( ! m_sSHA1.IsEmpty() )
		{
			ExpandSize( dc, sz, m_sSHA1 );
			sz.cy += TIP_TEXTHEIGHT;
		}

		if ( ! m_sTiger.IsEmpty() )
		{
			ExpandSize( dc, sz, m_sTiger );
			sz.cy += TIP_TEXTHEIGHT;
		}

		if ( ! m_sED2K.IsEmpty() )
		{
			ExpandSize( dc, sz, m_sED2K );
			sz.cy += TIP_TEXTHEIGHT;
		}

		if ( ! m_sBTH.IsEmpty() )
		{
			ExpandSize( dc, sz, m_sBTH );
			sz.cy += TIP_TEXTHEIGHT;
		}

		if ( ! m_sMD5.IsEmpty() )
		{
			ExpandSize( dc, sz, m_sMD5 );
			sz.cy += TIP_TEXTHEIGHT;
		}
	}

	if ( m_pMetadata.GetCount() )
	{
		sz.cy += 5 + 6;

		int nValueWidth = 0;
		m_nKeyWidth = 0;

		m_pMetadata.ComputeWidth( &dc, m_nKeyWidth, nValueWidth );

		if ( m_nKeyWidth ) m_nKeyWidth += TIP_MARGIN;
		sz.cx = max( sz.cx, m_nKeyWidth + nValueWidth );
		sz.cy += LONG( TIP_TEXTHEIGHT * m_pMetadata.GetCount() );
	}

	// Busy/Firewalled/unstable warnings. Queue info.
	if ( ! m_sBusy.IsEmpty() || ! m_sPush.IsEmpty() || ! m_sUnstable.IsEmpty() || ! m_sQueue.IsEmpty() )
	{
		sz.cy += 11;

		if ( ! m_sBusy.IsEmpty() )
		{
			dc.SelectObject( &CoolInterface.m_fntBold );
			ExpandSize( dc, sz, m_sBusy, 20 );
			dc.SelectObject( &CoolInterface.m_fntNormal );
			sz.cy += TIP_ICONHEIGHT;
		}

		if ( ! m_sQueue.IsEmpty() )
		{
			if ( ! m_sBusy.IsEmpty() )				// Align queue info with above (if present)
				ExpandSize( dc, sz, m_sQueue, 20 );
			else
				ExpandSize( dc, sz, m_sQueue );
			sz.cy += TIP_TEXTHEIGHT;
		}

		if ( ! m_sPush.IsEmpty() )
		{
			dc.SelectObject( &CoolInterface.m_fntBold );
			ExpandSize( dc, sz, m_sPush, 20 );
			dc.SelectObject( &CoolInterface.m_fntNormal );
			sz.cy += TIP_ICONHEIGHT;
		}

		if ( ! m_sUnstable.IsEmpty() )
		{
			dc.SelectObject( &CoolInterface.m_fntBold );
			ExpandSize( dc, sz, m_sUnstable, 20 );
			dc.SelectObject( &CoolInterface.m_fntNormal );
			sz.cy += TIP_ICONHEIGHT;
		}
	}

	// Partial warning
	if ( ! m_sPartial.IsEmpty() )
	{
		sz.cy += 5 + 6;

		ExpandSize( dc, sz, m_sPartial );
		sz.cy += TIP_TEXTHEIGHT;
	}


	dc.SelectObject( pOldFont );

	sz.cx += TIP_MARGIN * 2;
	sz.cy += TIP_MARGIN * 2;

	return sz;
}

void CMatchTipCtrl::ExpandSize(CDC& dc, CSize& sz, const CString& strText, int nBase)
{
	CSize szText = dc.GetTextExtent( strText );
	szText.cx += nBase;
	sz.cx = max( sz.cx, szText.cx );
}

/////////////////////////////////////////////////////////////////////////////
// CMatchTipCtrl painting

BOOL CMatchTipCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CMatchTipCtrl::OnPaint()
{
	if ( ! IsWindow( GetSafeHwnd() ) || ! IsWindowVisible() ) return;

	CPaintDC dc( this );
	CRect rc;
	GetClientRect( &rc );

	dc.Draw3dRect( &rc, Colors.m_crTipBorder, Colors.m_crTipBorder );
	rc.DeflateRect( 1, 1 );

	if ( Skin.m_bmToolTip.m_hObject )	// Else paint solid color last
		CoolInterface.DrawWatermark( &dc, &rc, &Skin.m_bmToolTip, FALSE );	// TEMPORARY skinning, use flicker-free pMemDC

	CFont* pOldFont = (CFont*)dc.SelectObject( &CoolInterface.m_fntBold );
	CPoint pt( TIP_MARGIN, TIP_MARGIN );

	if ( Skin.m_bmToolTip.m_hObject )
		dc.SetBkMode( TRANSPARENT );
	else
		dc.SetBkColor( Colors.m_crTipBack );
	dc.SetTextColor( Colors.m_crTipText );
	COLORREF crBack = Skin.m_bmToolTip.m_hObject ? CLR_NONE : Colors.m_crTipBack;

	DrawText( dc, pt, m_sName );
	pt.y += TIP_TEXTHEIGHT;

	dc.SelectObject( &CoolInterface.m_fntNormal );

	if ( ! m_sUser.IsEmpty() )
	{
		DrawText( dc, pt, m_sUser );
		pt.y += TIP_TEXTHEIGHT;
	}

	if ( ! m_sCountry.IsEmpty() )
	{
		int nFlagIndex = Flags.GetFlagIndex( m_sCountryCode );
		if ( nFlagIndex >= 0 )
		{
			Flags.Draw( nFlagIndex, dc, pt.x, pt.y, crBack );
			dc.ExcludeClipRect( pt.x, pt.y, pt.x + 16, pt.y + 16 );
			pt.x += 20;
			pt.y += 2;
		}
		DrawText( dc, pt, m_sCountry );
		if ( nFlagIndex >= 0 )
		{
			pt.y -= 2;
			pt.x -= 20;
		}
		pt.y += TIP_TEXTHEIGHT;
	}

	pt.y += 5;
	dc.Draw3dRect( rc.left + 2, pt.y, rc.Width() - 4, 1, Colors.m_crTipBorder, Colors.m_crTipBorder );
	dc.ExcludeClipRect( rc.left + 2, pt.y, rc.right - 2, pt.y + 1 );
	pt.y += 6;

	if ( ! m_sStatus.IsEmpty() )
	{
		dc.SetTextColor( m_crStatus );
		dc.SelectObject( &CoolInterface.m_fntBold );
		DrawText( dc, pt, m_sStatus );
		dc.SelectObject( &CoolInterface.m_fntNormal );
		dc.SetTextColor( Colors.m_crTipText );
		pt.y += TIP_TEXTHEIGHT;

		pt.y += 5;
		dc.Draw3dRect( rc.left + 2, pt.y, rc.Width() - 4, 1, Colors.m_crTipBorder, Colors.m_crTipBorder );
		dc.ExcludeClipRect( rc.left + 2, pt.y, rc.right - 2, pt.y + 1 );
		pt.y += 6;
	}

	ShellIcons.Draw( &dc, m_nIcon, 32, pt.x, pt.y, crBack );
	dc.ExcludeClipRect( pt.x, pt.y, pt.x + 32, pt.y + 32 );

	if ( m_nRating > 1 )
	{
		CPoint ptStar( rc.right - 3, pt.y - 2 );

		for ( int nRating = m_nRating - 1 ; nRating ; nRating-- )
		{
			ptStar.x -= 16;
			CoolInterface.Draw( &dc, IDI_STAR, 16, ptStar.x, ptStar.y, crBack );
			dc.ExcludeClipRect( ptStar.x, ptStar.y, ptStar.x + 16, ptStar.y + 16 );
		}
	}

	pt.x += 40;
	CString str;
	LoadString( str, IDS_TIP_SIZE );
	str.Append( _T(": ") );
	DrawText( dc, pt, str );
	CSize sz = dc.GetTextExtent( str );
	pt.x += sz.cx;
	DrawText( dc, pt, m_sSize );
	pt.x -= sz.cx;
	if ( m_sSize.Find( _T(" B") ) < 1 && m_nRating < 2 )
	{
		CString sSize;
		sSize.Format( _T("(%I64i bytes)"), m_pFile->m_nSize );
		sz = dc.GetTextExtent( sSize );
		pt.x += ( rc.Width() - sz.cx - 50 );
		DrawText( dc, pt, sSize );
		pt.x -= ( rc.Width() - sz.cx - 50 );
	}
	pt.y += 16;
	LoadString( str, IDS_TIP_TYPE );
	str.Append( _T(": ") );
	DrawText( dc, pt, str );
	sz = dc.GetTextExtent( str );
	pt.x += sz.cx;
	DrawText( dc, pt, m_sType );
	pt.x -= sz.cx + 40;
	pt.y += 16;

	// Hashes
	if ( ! m_sSHA1.IsEmpty() || ! m_sTiger.IsEmpty() || ! m_sED2K.IsEmpty() ||
		! m_sBTH.IsEmpty() || ! m_sMD5.IsEmpty() )
	{
		pt.y += 5;
		dc.Draw3dRect( rc.left + 2, pt.y, rc.Width() - 4, 1, Colors.m_crTipBorder, Colors.m_crTipBorder );
		dc.ExcludeClipRect( rc.left + 2, pt.y, rc.right - 2, pt.y + 1 );
		pt.y += 6;

		if ( ! m_sSHA1.IsEmpty() )
		{
			DrawText( dc, pt, m_sSHA1 );
			pt.y += TIP_TEXTHEIGHT;
		}

		if ( ! m_sTiger.IsEmpty() )
		{
			DrawText( dc, pt, m_sTiger );
			pt.y += TIP_TEXTHEIGHT;
		}

		if ( ! m_sED2K.IsEmpty() )
		{
			DrawText( dc, pt, m_sED2K );
			pt.y += TIP_TEXTHEIGHT;
		}

		if ( ! m_sBTH.IsEmpty() )
		{
			DrawText( dc, pt, m_sBTH );
			pt.y += TIP_TEXTHEIGHT;
		}

		if ( ! m_sMD5.IsEmpty() )
		{
			DrawText( dc, pt, m_sMD5 );
			pt.y += TIP_TEXTHEIGHT;
		}
	}

	// Busy, firewalled, unstabled warnings. Queue info
	if ( ! m_sBusy.IsEmpty() || ! m_sPush.IsEmpty() || ! m_sUnstable.IsEmpty() || ! m_sQueue.IsEmpty() )
	{
		pt.y += 5;
		dc.Draw3dRect( rc.left + 2, pt.y, rc.Width() - 4, 1, Colors.m_crTipBorder, Colors.m_crTipBorder );
		if ( ! Skin.m_bmToolTip.m_hObject )
			dc.ExcludeClipRect( rc.left + 2, pt.y, rc.right - 2, pt.y + 1 );
		pt.y += 6;

		dc.SetTextColor( Colors.m_crTipWarnings );
		dc.SelectObject( &CoolInterface.m_fntBold );

		// Source busy warning
		if ( ! m_sBusy.IsEmpty() )
		{
			CoolInterface.Draw( &dc, IDI_BUSY, 16, pt.x, pt.y, crBack );
			if ( ! Skin.m_bmToolTip.m_hObject )
				dc.ExcludeClipRect( pt.x, pt.y, pt.x + 16, pt.y + 16 );

			CPoint ptTextWithIcon = pt;
			ptTextWithIcon.x += 20;
			ptTextWithIcon.y++;

			DrawText ( dc, ptTextWithIcon, m_sBusy);
			pt.y += TIP_ICONHEIGHT;
		}

		dc.SetTextColor( Colors.m_crTipText );
		dc.SelectObject( &CoolInterface.m_fntNormal );

		// Queue info
		if ( ! m_sQueue.IsEmpty() )
		{
			CPoint ptTextWithIcon = pt;
			ptTextWithIcon.x += 20;

			if ( ! m_sBusy.IsEmpty() )			// Align queue info with above (if present)
				DrawText( dc, ptTextWithIcon, m_sQueue );
			else
				DrawText( dc, pt, m_sQueue );

			pt.y += TIP_TEXTHEIGHT;
		}

		dc.SetTextColor( Colors.m_crTipWarnings );
		dc.SelectObject( &CoolInterface.m_fntBold );

		// Source firewalled warning
		if ( ! m_sPush.IsEmpty() )
		{
			CoolInterface.Draw( &dc, IDI_FIREWALLED, 16, pt.x, pt.y, crBack );
			if ( ! Skin.m_bmToolTip.m_hObject )
				dc.ExcludeClipRect( pt.x, pt.y, pt.x + 16, pt.y + 16 );

			CPoint ptTextWithIcon = pt;
			ptTextWithIcon.x += 20;
			ptTextWithIcon.y++;

			DrawText ( dc, ptTextWithIcon, m_sPush);
			pt.y += TIP_ICONHEIGHT;
		}

		// Source unstable warning
		if ( ! m_sUnstable.IsEmpty() )
		{
			CoolInterface.Draw( &dc, IDI_UNSTABLE, 16, pt.x, pt.y, crBack );
			if ( ! Skin.m_bmToolTip.m_hObject )
				dc.ExcludeClipRect( pt.x, pt.y, pt.x + 16, pt.y + 16 );

			CPoint ptTextWithIcon = pt;
			ptTextWithIcon.x += 20;
			ptTextWithIcon.y++;

			DrawText ( dc, ptTextWithIcon, m_sUnstable);
			pt.y += TIP_ICONHEIGHT;
		}
		dc.SetTextColor( Colors.m_crTipText );
		dc.SelectObject( &CoolInterface.m_fntNormal );
	}

	// Partial warning
	if ( ! m_sPartial.IsEmpty() )
	{
		pt.y += 5;
		dc.Draw3dRect( rc.left + 2, pt.y, rc.Width() - 4, 1, Colors.m_crTipBorder, Colors.m_crTipBorder );
		if ( ! Skin.m_bmToolTip.m_hObject )
			dc.ExcludeClipRect( rc.left + 2, pt.y, rc.right - 2, pt.y + 1 );
		pt.y += 6;

		DrawText( dc, pt, m_sPartial );
		pt.y += TIP_TEXTHEIGHT;
	}

	// Metadata
	if ( m_pMetadata.GetCount() )
	{
		pt.y += 5;
		dc.Draw3dRect( rc.left + 2, pt.y, rc.Width() - 4, 1, Colors.m_crTipBorder, Colors.m_crTipBorder );
		if ( ! Skin.m_bmToolTip.m_hObject )
			dc.ExcludeClipRect( rc.left + 2, pt.y, rc.right - 2, pt.y + 1 );
		pt.y += 6;

		for ( POSITION pos = m_pMetadata.GetIterator() ; pos ; )
		{
			CMetaItem* pItem = m_pMetadata.GetNext( pos );

			DrawText( dc, pt, Settings.General.LanguageRTL ? ':' + pItem->m_sKey : pItem->m_sKey + ':' );
			pt.x += m_nKeyWidth;
			DrawText( dc, pt, pItem->m_sValue );
			pt.x -= m_nKeyWidth;
			pt.y += TIP_TEXTHEIGHT;
		}
	}

	dc.SelectObject( pOldFont );
	if ( ! Skin.m_bmToolTip.m_hObject ) 	// Else watermark already painted
		dc.FillSolidRect( &rc, crBack );
}

void CMatchTipCtrl::DrawText(CDC& dc, CPoint& pt, const CString& strText)
{
	DWORD dwFlags = ( Settings.General.LanguageRTL ? ETO_RTLREADING : 0 );
	short nExtraPoint = ( Settings.General.LanguageRTL ? 1 : 0 );
	CSize sz = dc.GetTextExtent( strText );
	CRect rc( pt.x, pt.y, pt.x + sz.cx + nExtraPoint, pt.y + sz.cy );

	if ( ! Skin.m_bmToolTip.m_hObject )
		dc.SetBkColor( Colors.m_crTipBack );
	dc.ExtTextOut( pt.x, pt.y,
		ETO_CLIPPED|(Skin.m_bmToolTip.m_hObject ? 0 : ETO_OPAQUE)|dwFlags, &rc, strText, NULL );
	dc.ExcludeClipRect( &rc );
}

void CMatchTipCtrl::OnMouseMove(UINT /*nFlags*/, CPoint /*point*/)
{
	Hide();
}

void CMatchTipCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Hide();
	CWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}
