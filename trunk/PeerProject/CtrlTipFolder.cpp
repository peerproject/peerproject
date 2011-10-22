//
// CtrlTipFolder.cpp
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
#include "Settings.h"
#include "PeerProject.h"
#include "CtrlTipFolder.h"

#include "Library.h"
#include "LibraryFolders.h"
#include "SharedFolder.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "ShellIcons.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CFolderTipCtrl, CCoolTipCtrl)

BEGIN_MESSAGE_MAP(CFolderTipCtrl, CCoolTipCtrl)
	//{{AFX_MSG_MAP(CFolderTipCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFolderTipCtrl construction

CFolderTipCtrl::CFolderTipCtrl()
	: m_pLibraryFolder( NULL )
{
}

CFolderTipCtrl::~CFolderTipCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CFolderTipCtrl prepare

BOOL CFolderTipCtrl::OnPrepare()
{
	CSingleLock pLock( &Library.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;

	if ( ! m_pLibraryFolder || ! LibraryFolders.CheckFolder( m_pLibraryFolder, TRUE ) ) return FALSE;

	m_sName = m_pLibraryFolder->m_sName;
	m_sPath = m_pLibraryFolder->m_sPath;

	m_sFiles.Format( _T("%lu"), m_pLibraryFolder->m_nFiles );
	m_sVolume = Settings.SmartVolume( m_pLibraryFolder->m_nVolume );

	QWORD nTotal;
	LibraryMaps.GetStatistics( NULL, &nTotal );

	if ( nTotal )
	{
		m_sPercentage.Format( _T("%.2f%% %s"),
			100.0 * ( m_pLibraryFolder->m_nVolume >> 10 ) / nTotal,
			(LPCTSTR)LoadString( IDS_TIP_LIBRARY_PERCENT ) );
	}
	else
	{
		m_sPercentage.Empty();
	}

	CalcSizeHelper();

	return m_sz.cx > 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFolderTipCtrl compute size

void CFolderTipCtrl::OnCalcSize(CDC* pDC)
{
	AddSize( pDC, m_sName );
	m_sz.cy += TIP_TEXTHEIGHT;
	pDC->SelectObject( &CoolInterface.m_fntNormal );
	AddSize( pDC, m_sPath );

	m_sz.cy += TIP_RULE;

	AddSize( pDC, m_sFiles, 120 );
	AddSize( pDC, m_sVolume, 120 );
	AddSize( pDC, m_sPercentage, 40 );

	m_sz.cy += TIP_TEXTHEIGHT * 4;
}

/////////////////////////////////////////////////////////////////////////////
// CFolderTipCtrl painting

void CFolderTipCtrl::OnPaint(CDC* pDC)
{
	CPoint pt( 0, 0 );

	DrawText( pDC, &pt, m_sName );
	pt.y += TIP_TEXTHEIGHT;
	pDC->SelectObject( &CoolInterface.m_fntNormal );
	DrawText( pDC, &pt, m_sPath );
	pt.y += TIP_TEXTHEIGHT;

	DrawRule( pDC, &pt );

	CoolInterface.Draw( pDC, IDI_FOLDER_OPEN, 32, pt.x, pt.y, Colors.m_crTipBack );
	pDC->ExcludeClipRect( pt.x, pt.y, pt.x + 32, pt.y + 32 );

	CString strText;
	LoadString( strText, IDS_TIP_TOTAL_FILES );
	DrawText( pDC, &pt, strText, 40 );
	DrawText( pDC, &pt, m_sFiles, 120 );
	pt.y += TIP_TEXTHEIGHT;
	LoadString( strText, IDS_TIP_TOTAL_VOLUME );
	DrawText( pDC, &pt, strText, 40 );
	DrawText( pDC, &pt, m_sVolume, 120 );
	pt.y += TIP_TEXTHEIGHT;
	DrawText( pDC, &pt, m_sPercentage, 40 );
	pt.y += TIP_TEXTHEIGHT;
}
