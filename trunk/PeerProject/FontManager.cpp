//
// FontManager.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "PeerProject.h"
#include "FontManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CFontManager::CFontManager(void)
{
	LOGFONT lf;
	theApp.m_gdiFont.GetLogFont( &lf );

	HDC hDC = GetDC( NULL );
	if ( lf.lfHeight < 0 )
		lf.lfHeight = -lf.lfHeight;

	m_nDefaultSize = (WORD)MulDiv( lf.lfHeight, 72, GetDeviceCaps( hDC, LOGPIXELSY ) );
	m_sFontName = lf.lfFaceName;

	ReleaseDC( NULL, hDC );
}

const DLGTEMPLATE* CFontManager::PreCreateDialog(_AFX_OCC_DIALOG_INFO* pOccDialogInfo, const DLGTEMPLATE* pOrigTemplate)
{
	const DLGTEMPLATE *lpNewTemplate = COccManager::PreCreateDialog( pOccDialogInfo, pOrigTemplate );

	if ( ! pOccDialogInfo->m_pNewTemplate )
	{
		CDialogTemplate temp( lpNewTemplate );

		temp.SetFont( m_sFontName, m_nDefaultSize );
		pOccDialogInfo->m_pNewTemplate = (DLGTEMPLATE*)temp.Detach();
		return pOccDialogInfo->m_pNewTemplate;
	}

	return lpNewTemplate;
}
