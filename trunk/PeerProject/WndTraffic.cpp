//
// WndTraffic.cpp
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
#include "WndTraffic.h"
#include "WndMain.h"
#include "DlgGraphList.h"
#include "GraphLine.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_SERIAL(CTrafficWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CTrafficWnd, CChildWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_TRAFFIC_GRID, OnUpdateTrafficGrid)
	ON_COMMAND(ID_TRAFFIC_GRID, OnTrafficGrid)
	ON_UPDATE_COMMAND_UI(ID_TRAFFIC_AXIS, OnUpdateTrafficAxis)
	ON_COMMAND(ID_TRAFFIC_AXIS, OnTrafficAxis)
	ON_UPDATE_COMMAND_UI(ID_TRAFFIC_LEGEND, OnUpdateTrafficLegend)
	ON_COMMAND(ID_TRAFFIC_LEGEND, OnTrafficLegend)
	ON_COMMAND(ID_TRAFFIC_SETUP, OnTrafficSetup)
	ON_COMMAND(ID_TRAFFIC_CLEAR, OnTrafficClear)
	ON_COMMAND(ID_TRAFFIC_WINDOW, OnTrafficWindow)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTrafficWnd construction

CTrafficWnd::CTrafficWnd(DWORD nUnique)
{
	m_nUnique = nUnique;
	m_pGraph  = new CLineGraph();

	Create( IDR_TRAFFICFRAME );
}

CTrafficWnd::~CTrafficWnd()
{
	if ( m_pGraph ) delete m_pGraph;
}

/////////////////////////////////////////////////////////////////////////////
// CTrafficWnd message handlers

int CTrafficWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	if ( m_nUnique == 0 )
		FindFreeUnique();

	if ( ! Serialize( FALSE ) && m_nUnique == 1 )
		m_pGraph->CreateDefaults();

	UpdateCaption();

	return 0;
}

void CTrafficWnd::OnDestroy()
{
	KillTimer( 2 );
	Serialize( TRUE );

	CChildWnd::OnDestroy();
}

void CTrafficWnd::FindFreeUnique()
{
	CWindowManager*	pWindows = GetManager();

	for ( m_nUnique = 1 ; m_nUnique < 1000 ; m_nUnique++ )
	{
		BOOL bAvailable		= TRUE;
		CTrafficWnd* pChild = NULL;

		while ( ( pChild = (CTrafficWnd*)pWindows->Find( RUNTIME_CLASS(CTrafficWnd), pChild ) ) != NULL )
		{
			if ( pChild != this && pChild->m_nUnique == m_nUnique )
			{
				bAvailable = FALSE;
				break;
			}
		}

		if ( bAvailable ) break;
	}
}

void CTrafficWnd::SetUpdateRate()
{
	KillTimer( 2 );

	CLineGraph* pGraph = (CLineGraph*)m_pGraph;
	SetTimer( 2, pGraph->m_nSpeed, NULL );
}

void CTrafficWnd::OnSize(UINT nType, int cx, int cy)
{
	CChildWnd::OnSize( nType, cx, cy );
	Invalidate();
}

void CTrafficWnd::OnPaint()
{
	CPaintDC dc( this );
	CRect rc;

	GetClientRect( &rc );
	m_pGraph->BufferedPaint( &dc, &rc );
}

void CTrafficWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 2 && m_pGraph->Update() )
		Invalidate();
}

void CTrafficWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if ( point.x == -1 && point.y == -1 )	// Keyboard fix
		ClientToScreen( &point );

	Skin.TrackPopupMenu( _T("CTrafficWnd"), point, ID_TRAFFIC_SETUP );
}

void CTrafficWnd::OnUpdateTrafficGrid(CCmdUI* pCmdUI)
{
	CLineGraph* pLineGraph = (CLineGraph*)m_pGraph;
	pCmdUI->SetCheck( pLineGraph->m_bShowGrid );
}

void CTrafficWnd::OnTrafficGrid()
{
	CLineGraph* pLineGraph = (CLineGraph*)m_pGraph;
	pLineGraph->m_bShowGrid = ! pLineGraph->m_bShowGrid;
	Invalidate();
}

void CTrafficWnd::OnUpdateTrafficAxis(CCmdUI* pCmdUI)
{
	CLineGraph* pLineGraph = (CLineGraph*)m_pGraph;
	pCmdUI->SetCheck( pLineGraph->m_bShowAxis );
}

void CTrafficWnd::OnTrafficAxis()
{
	CLineGraph* pLineGraph = (CLineGraph*)m_pGraph;
	pLineGraph->m_bShowAxis = ! pLineGraph->m_bShowAxis;
	Invalidate();
}

void CTrafficWnd::OnUpdateTrafficLegend(CCmdUI* pCmdUI)
{
	CLineGraph* pLineGraph = (CLineGraph*)m_pGraph;
	pCmdUI->SetCheck( pLineGraph->m_bShowLegend );
}

void CTrafficWnd::OnTrafficLegend()
{
	CLineGraph* pLineGraph = (CLineGraph*)m_pGraph;
	pLineGraph->m_bShowLegend = ! pLineGraph->m_bShowLegend;
	Invalidate();
}

void CTrafficWnd::OnTrafficSetup()
{
	CGraphListDlg dlg( this, (CLineGraph*)m_pGraph );

	dlg.m_sName = m_sName;

	if ( dlg.DoModal() == IDOK )
	{
		m_sName = dlg.m_sName;

		SetUpdateRate();
		UpdateCaption();
		Invalidate();
	}
}

void CTrafficWnd::OnTrafficClear()
{
	m_pGraph->Clear();
}

void CTrafficWnd::OnLButtonDblClk(UINT /*nFlags*/, CPoint /*point*/)
{
	PostMessage( WM_COMMAND, ID_TRAFFIC_SETUP );
}

void CTrafficWnd::OnTrafficWindow()
{
	new CTrafficWnd();
}

/////////////////////////////////////////////////////////////////////////////
// CTrafficWnd serialize

BOOL CTrafficWnd::Serialize(BOOL bSave)
{
	WINDOWPLACEMENT pPos = { sizeof( WINDOWPLACEMENT ) };

	CString strFile = Settings.General.DataPath + _T("Graph%.4u.dat");
	strFile.Format( strFile, m_nUnique );

	CFile pFile;
	if ( ! pFile.Open( strFile, bSave ? ( CFile::modeWrite | CFile::modeCreate ) : CFile::modeRead ) )
		return FALSE;

	try
	{
		CArchive ar( &pFile, bSave ? CArchive::store : CArchive::load );	// 4 KB buffer
		try
		{
			int nVersion = 0;

			if ( ar.IsStoring() )
			{
				nVersion = 0xFFFFFFFF;
				ar << nVersion;
				nVersion = 1;
				ar << nVersion;

				ar << m_nUnique;
				ar << m_sName;

				GetWindowPlacement( &pPos );
				ar.Write( &pPos, sizeof( pPos ) );
			}
			else // Loading
			{
				ar >> m_nUnique;

				if ( m_nUnique == 0xFFFFFFFF )
				{
					ar >> nVersion;
					ar >> m_nUnique;
				}

				if ( nVersion >= 1 )
					ar >> m_sName;

				ReadArchive( ar, &pPos, sizeof( pPos ) );
				if ( pPos.showCmd == SW_SHOWNORMAL )
					SetWindowPlacement( &pPos );
			}

			m_pGraph->Serialize( ar );

			ar.Close();
		}
		catch ( CException* pException )
		{
			ar.Abort();
			pFile.Abort();
			pException->Delete();
			return FALSE;
		}
		pFile.Close();
	}
	catch ( CException* pException )
	{
		pFile.Abort();
		pException->Delete();
		return FALSE;
	}

	if ( ! bSave )
		SetUpdateRate();

	return TRUE;
}

void CTrafficWnd::UpdateCaption()
{
	CString strCaption, strName;

	if ( ! m_sName.IsEmpty() )
		strName = _T(" : ") + m_sName;
	else if ( m_nUnique <= 26 )
		strName.Format( _T(" (%c)"), 'A' + m_nUnique - 1 );
	else
		strName.Format( _T(" (%lu)"), m_nUnique );

	LoadString( strCaption, IDR_TRAFFICFRAME );
	strCaption += strName;
	SetWindowText( strCaption );
}
