//
// PageFileComments.cpp
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
#include "Library.h"
#include "SharedFile.h"
#include "ShellIcons.h"
#include "Colors.h"
#include "CoolInterface.h"
#include "PageFileComments.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CFileCommentsPage, CFilePropertiesPage)

BEGIN_MESSAGE_MAP(CFileCommentsPage, CFilePropertiesPage)
	//{{AFX_MSG_MAP(CFileCommentsPage)
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileCommentsPage property page

CFileCommentsPage::CFileCommentsPage()
	: CFilePropertiesPage ( CFileCommentsPage::IDD )
	, m_sComments	( )
	, m_nRating 	( -1 )
{
}

CFileCommentsPage::~CFileCommentsPage()
{
}

void CFileCommentsPage::DoDataExchange(CDataExchange* pDX)
{
	CFilePropertiesPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileCommentsPage)
	DDX_Control(pDX, IDC_FILE_COMMENTS, m_wndComments);
	DDX_Control(pDX, IDC_FILE_RATING, m_wndRating);
	DDX_Text(pDX, IDC_FILE_COMMENTS, m_sComments);
	DDX_CBIndex(pDX, IDC_FILE_RATING, m_nRating);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFileCommentsPage message handlers

BOOL CFileCommentsPage::OnInitDialog()
{
	CFilePropertiesPage::OnInitDialog();

	CLibraryList* pFiles = GetList();
	if ( pFiles == NULL ) return TRUE;

	if ( pFiles->GetCount() == 1 )
	{
		CQuickLock oLock( Library.m_pSection );
		CLibraryFile* pFile = GetFile();
		if ( pFile == NULL ) return TRUE;

		m_nRating	= pFile->m_nRating;
		m_sComments	= pFile->m_sComments;
	}
	else
	{
		m_wndComments.EnableWindow( FALSE );

		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos = pFiles->GetIterator() ; pos ; )
		{
			if ( CLibraryFile* pFile = pFiles->GetNextFile( pos ) )
				m_nRating = pFile->m_nRating;
		}
	}

	UpdateData( FALSE );

	return TRUE;
}

void CFileCommentsPage::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemWidth	= 1024;
	lpMeasureItemStruct->itemHeight	= 18;
}

void CFileCommentsPage::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ( lpDrawItemStruct->itemID == (UINT)-1 ) return;
	if ( ( lpDrawItemStruct->itemAction & ODA_SELECT ) == 0 &&
		 ( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE ) == 0 ) return;

	CRect rcItem( &lpDrawItemStruct->rcItem );
	CPoint pt( rcItem.left + 1, rcItem.top + 1 );
	CDC dc;

	dc.Attach( lpDrawItemStruct->hDC );
	if ( Settings.General.LanguageRTL )
		SetLayout( dc.m_hDC, LAYOUT_RTL );

	int nRating = lpDrawItemStruct->itemID;

	CFont* pOldFont = (CFont*)dc.SelectObject( nRating > 0 ? &theApp.m_gdiFontBold : &theApp.m_gdiFont );
	dc.SetBkMode( TRANSPARENT );

	if ( lpDrawItemStruct->itemState & ODS_SELECTED )
	{
		dc.SetTextColor( Colors.m_crHiText );
		if ( Skin.m_bmSelected.m_hObject )
			CoolInterface.DrawWatermark( &dc, &rcItem, &Skin.m_bmSelected, FALSE ); 	// No overdraw
		else
			dc.FillSolidRect( &rcItem, Colors.m_crHighlight );
	}
	else // Unselected
	{
		dc.SetTextColor( Colors.m_crText );
		dc.FillSolidRect( &rcItem, Colors.m_crSysWindow );
	}

	rcItem.DeflateRect( 4, 1 );

	if ( nRating > 1 )
	{
		for ( int nStar = nRating - 1 ; nStar ; nStar-- )
		{
			rcItem.right -= 16;
			CoolInterface.Draw( &dc, IDI_STAR, 16, rcItem.right, rcItem.top, CLR_NONE,
				( lpDrawItemStruct->itemState & ODS_SELECTED ) );
			rcItem.right -= 2;
		}
	}
	else if ( nRating == 1 )
	{
		rcItem.right -= 16;
		CoolInterface.Draw( &dc, IDI_FAKE, 16, rcItem.right, rcItem.top, CLR_NONE,
			( lpDrawItemStruct->itemState & ODS_SELECTED ) );
	}

	if ( ( lpDrawItemStruct->itemState & ODS_SELECTED ) == 0 &&
		nRating >= 0 && nRating < 7 )
	{
		static COLORREF crRating[7] =
		{
			Colors.m_crRatingNull,	// Unrated
			Colors.m_crRating0,		// Fake
			Colors.m_crRating1,		// Poor
			Colors.m_crRating2,		// Average
			Colors.m_crRating3,		// Good
			Colors.m_crRating4,		// Very good
			Colors.m_crRating5,		// Excellent
		};

		dc.SetTextColor( crRating[ nRating ] );
	}

	CString str;
	m_wndRating.GetLBText( nRating, str );
	dc.DrawText( str, &rcItem, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX );

	dc.SelectObject( pOldFont );
	dc.Detach();
}

void CFileCommentsPage::OnOK()
{
	UpdateData();
	m_sComments.Trim();

	CLibraryList* pFiles = GetList();

	if ( pFiles == NULL || pFiles->GetCount() == 1 )
	{
		CQuickLock oLock( Library.m_pSection );
		if ( CLibraryFile* pFile = GetFile() )
		{
			pFile->m_nRating	= m_nRating;
			pFile->m_sComments	= m_sComments;

			pFile->ModifyMetadata();
			Library.Update();
		}
	}
	else
	{
		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos = pFiles->GetIterator() ; pos ; )
		{
			if ( CLibraryFile* pFile = pFiles->GetNextFile( pos ) )
				pFile->m_nRating = m_nRating;
		}

		Library.Update();
	}

	CFilePropertiesPage::OnOK();
}
