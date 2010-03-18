//
// SkinWindow.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-20078.
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
#include "CoolInterface.h"
#include "SkinWindow.h"
#include "ImageFile.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define BORDER_WIDTH			GetSystemMetrics( SM_CXSIZEFRAME )
#define SIZEBOX_WIDTH			GetSystemMetrics( SM_CXSIZE )


//////////////////////////////////////////////////////////////////////
// CSkinWindow construction

CSkinWindow::CSkinWindow() :
	m_rcCaption( 0, 0, 0, 0 )
{
	m_bPart		= new BOOL[ SKINPART_COUNT ];
	m_rcPart	= new CRect[ SKINPART_COUNT ];
	m_nPart		= new int[ SKINPART_COUNT ];
	m_bAnchor	= new BOOL[ SKINANCHOR_COUNT ];
	m_rcAnchor	= new CRect[ SKINANCHOR_COUNT ];

	ZeroMemory( m_bPart, sizeof(BOOL) * SKINPART_COUNT );
	ZeroMemory( m_nPart, sizeof(int) * SKINPART_COUNT );
	ZeroMemory( m_bAnchor, sizeof(BOOL) * SKINANCHOR_COUNT );

	m_szMinSize.cx = m_szMinSize.cy = 0;
	m_rcMaximise.SetRect( -1, 0, -1, -1 );
	m_rcResize.SetRect( BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH );

	m_hoSkin			= NULL;
	m_bCaption			= FALSE;
	m_bCaptionCaps		= FALSE;
	m_crCaptionText		= RGB( 255, 255, 255 );
	m_crCaptionInactive	= RGB( 128, 128, 128 );
	m_crCaptionShadow	= CLR_NONE;
	m_crCaptionOutline	= CLR_NONE;
	m_nCaptionAlign		= 0;

	m_pRegionXML	= NULL;

	m_nHoverAnchor	= 0;
	m_nDownAnchor	= 0;
	m_nMirror = 0;
	m_rcMirror.SetRectEmpty();
}

CSkinWindow::~CSkinWindow()
{
	if ( m_dcSkin.m_hDC != NULL )
	{
		if ( m_hoSkin != NULL ) m_dcSkin.SelectObject( CBitmap::FromHandle( m_hoSkin ) );
		m_dcSkin.DeleteDC();
	}

	if ( m_bmSkin.m_hObject != NULL ) m_bmSkin.DeleteObject();

	if ( m_pRegionXML ) delete m_pRegionXML;

	for ( POSITION pos = m_pPartList.GetStartPosition() ; pos ; )
	{
		CRect* pRect;
		CString str;
		m_pPartList.GetNextAssoc( pos, str, pRect );
		delete pRect;
	}

	for ( POSITION pos = m_pAnchorList.GetStartPosition() ; pos ; )
	{
		CRect* pRect;
		CString str;
		m_pAnchorList.GetNextAssoc( pos, str, pRect );
		delete pRect;
	}

	delete [] m_bPart;
	delete [] m_rcPart;
	delete [] m_nPart;
	delete [] m_bAnchor;
	delete [] m_rcAnchor;
}

//////////////////////////////////////////////////////////////////////
// CSkinWindow parse XML

BOOL CSkinWindow::Parse(CXMLElement* pBase, const CString& strPath)
{
	static LPCTSTR pszPart[] =
	{
		_T("TopLeft"), _T("Top"), _T("TopRight"),
		_T("LeftTop"), _T("Left"), _T("LeftBottom"),
		_T("RightTop"), _T("Right"), _T("RightBottom"),
		_T("BottomLeft"), _T("Bottom"), _T("BottomRight"),
		_T("TopLeftAlt"), _T("TopAlt"), _T("TopRightAlt"),
		_T("LeftTopAlt"), _T("LeftAlt"), _T("LeftBottomAlt"),
		_T("RightTopAlt"), _T("RightAlt"), _T("RightBottomAlt"),
		_T("BottomLeftAlt"), _T("BottomAlt"), _T("BottomRightAlt"),
		_T("System"), _T("SystemHover"), _T("SystemDown"),
		_T("Minimise"), _T("MinimiseHover"), _T("MinimiseDown"),
		_T("Maximise"), _T("MaximiseHover"), _T("MaximiseDown"),
		_T("Close"), _T("CloseHover"), _T("CloseDown"),
		NULL
	};

	static LPCTSTR pszAnchor[] =
	{
		_T("Icon"), _T("System"), _T("Minimise"), _T("Maximise"), _T("Close"),
		NULL
	};

	CString str;
	CRect rc;

	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pGroup = pBase->GetNextElement( pos );

		if ( pGroup->IsNamed( _T("target") ) )
		{
			CString strTarget = pGroup->GetAttributeValue( _T("window") );

			if ( strTarget == _T("CMainTabBarCtrl") )
				strTarget = strTarget;

			if ( strTarget.GetLength() )
			{
				m_sTargets += '|';
				m_sTargets += strTarget;
				m_sTargets += '|';
			}
		}
		else if ( pGroup->IsNamed( _T("parts") ) )
		{
			for ( POSITION posInner = pGroup->GetElementIterator() ; posInner ; )
			{
				CXMLElement* pXML = pGroup->GetNextElement( posInner );
				if ( ! pXML->IsNamed( _T("part") ) )
				{
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [parts] element"), pXML->ToString() );
					continue;
				}
				if ( ! ParseRect( pXML, &rc ) ) continue;
				if ( ! rc.Width() ) rc.right++;
				if ( ! rc.Height() ) rc.bottom++;

				CString strMode = pXML->GetAttributeValue( _T("mode") );
				int nMode = SKINPARTMODE_TILE;

				if ( strMode.CompareNoCase( _T("tile") ) == 0 )
					nMode = SKINPARTMODE_TILE;
				else if ( strMode.CompareNoCase( _T("stretch") ) == 0 )
					nMode = SKINPARTMODE_STRETCH;

				CString strName = pXML->GetAttributeValue( _T("name") );
				if ( strName.IsEmpty() ) continue;

				int nPart = 0;
				for ( ; pszPart[ nPart ] ; nPart++ )
				{
					if ( _tcsicmp( strName, pszPart[ nPart ] ) == 0 )
					{
						m_bPart[ nPart ]	= TRUE;
						m_nPart[ nPart ]	= nMode;
						m_rcPart[ nPart ]	= rc;
						break;
					}
				}

				if ( pszPart[ nPart ] == NULL )
				{
					CRect* pRect;

					if ( m_pPartList.Lookup( strName, pRect ) )
					{
						*pRect = rc;
					}
					else
					{
						pRect = new CRect( &rc );
						m_pPartList.SetAt( strName, pRect );
					}
				}
			}
		}
		else if ( pGroup->IsNamed( _T("anchors") ) )
		{
			for ( POSITION posInner = pGroup->GetElementIterator() ; posInner ; )
			{
				CXMLElement* pXML = pGroup->GetNextElement( posInner );
				if ( ! pXML->IsNamed( _T("anchor") ) )
				{
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [anchors] element"), pXML->ToString() );
					continue;
				}

				if ( ! ParseRect( pXML, &rc ) ) continue;

				CString strName = pXML->GetAttributeValue( _T("name") );

				int nAnchor = 0;
				for ( ; pszAnchor[ nAnchor ] ; nAnchor++ )
				{
					if ( _tcsicmp( strName, pszAnchor[ nAnchor ] ) == 0 )
					{
						m_bAnchor[ nAnchor ]	= TRUE;
						m_rcAnchor[ nAnchor ]	= rc;
						break;
					}
				}

				if ( pszAnchor[ nAnchor ] == NULL )
				{
					CRect* pRect;
					if ( m_pAnchorList.Lookup( strName, pRect ) )
					{
						*pRect = rc;
					}
					else
					{
						pRect = new CRect( &rc );
						m_pAnchorList.SetAt( strName, pRect );
					}

					m_rcMirror = pRect;

					if ( strName == "Mirror" )
						m_nMirror = 1;
					else if ( strName == "MirrorFull" )
						m_nMirror = 2;
				}
			}
		}
		else if ( pGroup->IsNamed( _T("region") ) )
		{
			if ( m_pRegionXML ) delete m_pRegionXML;
			m_pRegionXML = pGroup->Detach();
		}
		else if ( pGroup->IsNamed( _T("caption") ) )
		{
			m_bCaption = ParseRect( pGroup, &m_rcCaption );

			CString strFont = pGroup->GetAttributeValue( _T("fontFace") );
			CString strSize = pGroup->GetAttributeValue( _T("fontSize") );
			CString strBold = pGroup->GetAttributeValue( _T("fontWeight") );

			if ( strBold.CompareNoCase( _T("bold") ) == 0 )
				strBold = _T("700");
			else if ( strBold.CompareNoCase( _T("normal") ) == 0 )
				strBold = _T("400");

			int nFontSize = 13, nFontWeight = FW_BOLD;
			_stscanf( strSize, _T("%i"), &nFontSize );
			_stscanf( strBold, _T("%i"), &nFontWeight );

			LOGFONT lf = {};
			lf.lfHeight			= nFontSize;
			lf.lfWeight			= nFontWeight;
			lf.lfCharSet		= DEFAULT_CHARSET;
			lf.lfQuality		= theApp.m_nFontQuality; // DEFAULT_QUALITY
			lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
			lf.lfPitchAndFamily	= DEFAULT_PITCH|FF_DONTCARE;
			_tcsncpy( lf.lfFaceName, ( strFont.IsEmpty() ?
				Settings.Fonts.DefaultFont : strFont ), LF_FACESIZE );

			if ( _tcsistr( strSize, _T("pt") ) != NULL )
			{
				m_fnCaption.CreatePointFontIndirect( &lf );
			}
			else
			{
				lf.lfHeight = -lf.lfHeight;
				m_fnCaption.CreateFontIndirect( &lf );
			}

			str = pGroup->GetAttributeValue( _T("colour") );
			ParseColor( str, m_crCaptionText );
			str = pGroup->GetAttributeValue( _T("color") );
			ParseColor( str, m_crCaptionText );
			str = pGroup->GetAttributeValue( _T("inactiveColour") );
			ParseColor( str, m_crCaptionInactive );
			str = pGroup->GetAttributeValue( _T("inactiveColor") );
			ParseColor( str, m_crCaptionInactive );
			str = pGroup->GetAttributeValue( _T("outlineColour") );
			ParseColor( str, m_crCaptionOutline );
			str = pGroup->GetAttributeValue( _T("outlineColor") );
			ParseColor( str, m_crCaptionOutline );
			str = pGroup->GetAttributeValue( _T("shadowColour") );
			ParseColor( str, m_crCaptionShadow );
			str = pGroup->GetAttributeValue( _T("shadowColor") );
			ParseColor( str, m_crCaptionShadow );

			str = pGroup->GetAttributeValue( _T("caps") );
			m_bCaptionCaps = str.GetLength() > 0;

			str = pGroup->GetAttributeValue( _T("align") );
			if ( str.CompareNoCase( _T("left") ) == 0 )
				m_nCaptionAlign = 0;
			else if ( str.CompareNoCase( _T("center") ) == 0 )
				m_nCaptionAlign = 1;
			else if ( str.CompareNoCase( _T("right") ) == 0 )
				m_nCaptionAlign = 2;

			if ( m_bCaption && m_fnCaption.m_hObject == NULL )
			{
				NONCLIENTMETRICS pMetrics = { sizeof( NONCLIENTMETRICS ) };
				SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &pMetrics, 0 );
				m_fnCaption.CreateFontIndirect( &pMetrics.lfCaptionFont );
			}
		}
		else if ( pGroup->IsNamed( _T("image") ) )
		{
			str = pGroup->GetAttributeValue( _T("language") );

			if ( str.GetLength() > 0 )
			{
				if ( str.CompareNoCase( Settings.General.Language ) != 0 ) continue;
				m_sLanguage = str;
			}
			else
			{
				m_sLanguage = Settings.General.Language;
			}

			CString strRes	= pGroup->GetAttributeValue( _T("res") );
			CString strFile	= pGroup->GetAttributeValue( _T("path") );
			HBITMAP hBitmap = NULL;

			if ( strFile.GetLength() > 0 )
			{
				strFile = strPath + strFile;
				hBitmap = CImageFile::LoadBitmapFromFile( strFile );

			// ToDo: Alternate PNG Alpha Handling
			//	if ( strFile.Right( 4 ) == ".bmp" )
			//	{
			//		hBitmap = (HBITMAP)LoadImage( AfxGetInstanceHandle(), strFile,
			//					IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
			//	}
			//	else //.png
			//	{
			//		CImageFile pFile;
			//
			//		pFile.LoadFromFile( strFile );
			//		pFile.EnsureRGB();	// Removes Alpha, do reverse
			//		hBitmap = pFile.CreateBitmap();
			//	}
			}
			else if ( strRes.GetLength() > 0 )
			{
				UINT nResID = 0;
				if ( _stscanf( strRes, _T("%lu"), &nResID ) != 1 )
				{
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [res] attribute in [image] element"), pGroup->ToString() );
					return FALSE;
				}

				if ( Settings.General.LanguageRTL )
				{
					if ( nResID == IDB_NAVBAR_IMAGE )
						nResID = IDB_NAVBAR_IMAGE_RTL;
					else if ( nResID == IDB_NAVBAR_ALPHA )
						nResID = IDB_NAVBAR_ALPHA_RTL;
				}
				else // Is this check ever necessary?
				{
					if ( nResID == IDB_NAVBAR_IMAGE_RTL )
						nResID = IDB_NAVBAR_IMAGE;
					else if ( nResID == IDB_NAVBAR_ALPHA_RTL )
						nResID = IDB_NAVBAR_ALPHA;
				}

				hBitmap = (HBITMAP)LoadImage( AfxGetInstanceHandle(),
					MAKEINTRESOURCE(nResID), IMAGE_BITMAP, 0, 0, 0 );
				//	ToDo: Convert Resources to .PNG ?
				//	hBitmap = CImageFile::LoadBitmapFromFile( nResID, RT_PNG );
				}

			if ( hBitmap == NULL )
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Cannot load image"), pGroup->ToString() );
				return FALSE;
			}

			str = pGroup->GetAttributeValue( _T("type") );
			ToLower( str );

			//ToDo: Add PNG Transparency Support

			if ( str == _T("watermark") )
			{
				if ( m_bmWatermark.m_hObject != NULL ) m_bmWatermark.DeleteObject();
				m_bmWatermark.Attach( hBitmap );
			}
			else if ( str == _T("alpha") )
			{
				if ( m_bmAlpha.m_hObject != NULL ) m_bmAlpha.DeleteObject();
				m_bmAlpha.Attach( hBitmap );
			}
			else	// type="image"
			{
				if ( m_bmSkin.m_hObject != NULL ) m_bmSkin.DeleteObject();
				m_bmSkin.Attach( hBitmap );
			}
		}
		else if ( pGroup->IsNamed( _T("maximiseCrop") ) )
		{
			ParseRect( pGroup, &m_rcMaximise );
			m_rcMaximise.right -= m_rcMaximise.left;
			m_rcMaximise.bottom -= m_rcMaximise.top;
		}
		else if ( pGroup->IsNamed( _T("resizeBorder") ) )
		{
			ParseRect( pGroup, &m_rcResize );
			m_rcResize.right -= m_rcResize.left;
			m_rcResize.bottom -= m_rcResize.top;
		}
		else if ( pGroup->IsNamed( _T("minimumSize") ) )
		{
			CString str = pGroup->GetAttributeValue( _T("width") );
			_stscanf( str, _T("%i"), &m_szMinSize.cx );
			str = pGroup->GetAttributeValue( _T("height") );
			_stscanf( str, _T("%i"), &m_szMinSize.cy );
		}
		else
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [windowSkin] element"), pGroup->ToString() );
	}

	return ( m_bmSkin.m_hObject != NULL );
}

//////////////////////////////////////////////////////////////////////
// CSkinWindow parse helpers

BOOL CSkinWindow::ParseRect(CXMLElement* pXML, CRect* pRect)
{
	CString strValue = pXML->GetAttributeValue( _T("rect") );

	if ( strValue.GetLength() )
	{
		int x, y, cx, cy;
		if ( _stscanf( strValue, _T("%i,%i,%i,%i"), &x, &y, &cx, &cy ) != 4 )
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Invalid [rect] attribute"), pXML->ToString() );
			return FALSE;
		}
		pRect->left = x;
		pRect->top = y;
		pRect->right = x + cx;
		pRect->bottom = y + cy;
		return TRUE;
	}

	strValue = pXML->GetAttributeValue( _T("point") );

	if ( strValue.GetLength() )
	{
		int x, y;
		if ( _stscanf( strValue, _T("%i,%i"), &x, &y ) != 2 )
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Invalid [point] attribute"), pXML->ToString() );
			return FALSE;
		}
		pRect->left = x;
		pRect->top = y;
		pRect->right = pRect->bottom = 0;
		return TRUE;
	}

	return FALSE;
}

BOOL CSkinWindow::ParseColor(const CString& str, COLORREF& cr)
{
	if ( str.GetLength() != 6 ) return FALSE;

	int nRed, nGreen, nBlue;

	if ( _stscanf( str.Mid( 0, 2 ), _T("%x"), &nRed ) != 1 ) return FALSE;
	if ( _stscanf( str.Mid( 2, 2 ), _T("%x"), &nGreen ) != 1 ) return FALSE;
	if ( _stscanf( str.Mid( 4, 2 ), _T("%x"), &nBlue ) != 1 ) return FALSE;

	cr = RGB( nRed, nGreen, nBlue );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkinWindow window hooks

void CSkinWindow::CalcWindowRect(RECT* pRect, BOOL bToClient, BOOL /*bZoomed*/)
{
	CRect rcAdjust( 0, 0, 0, 0 );

	if ( m_bPart[ SKINPART_TOP_LEFT ] )
		rcAdjust.top = max( rcAdjust.top, m_rcPart[ SKINPART_TOP_LEFT ].Height() );
	if ( m_bPart[ SKINPART_TOP ] )
		rcAdjust.top = max( rcAdjust.top, m_rcPart[ SKINPART_TOP ].Height() );
	if ( m_bPart[ SKINPART_TOP_RIGHT ] )
		rcAdjust.top = max( rcAdjust.top, m_rcPart[ SKINPART_TOP_RIGHT ].Height() );

	if ( m_bPart[ SKINPART_LEFT ] )
		rcAdjust.left = max( rcAdjust.left, m_rcPart[ SKINPART_LEFT ].Width() );
	if ( m_bPart[ SKINPART_RIGHT ] )
		rcAdjust.right = max( rcAdjust.right, m_rcPart[ SKINPART_RIGHT ].Width() );

	if ( m_bPart[ SKINPART_BOTTOM_LEFT ] )
		rcAdjust.bottom = max( rcAdjust.bottom, m_rcPart[ SKINPART_BOTTOM_LEFT ].Height() );
	if ( m_bPart[ SKINPART_BOTTOM ] )
		rcAdjust.bottom = max( rcAdjust.bottom, m_rcPart[ SKINPART_BOTTOM ].Height() );
	if ( m_bPart[ SKINPART_BOTTOM_RIGHT ] )
		rcAdjust.bottom = max( rcAdjust.bottom, m_rcPart[ SKINPART_BOTTOM_RIGHT ].Height() );

	if ( bToClient )
	{
		pRect->left		+= rcAdjust.left;
		pRect->top		+= rcAdjust.top;
		pRect->right	-= rcAdjust.right;
		pRect->bottom	-= rcAdjust.bottom;
	}
	else
	{
		pRect->left		-= rcAdjust.left;
		pRect->top		-= rcAdjust.top;
		pRect->right	+= rcAdjust.right;
		pRect->bottom	+= rcAdjust.bottom;
	}
}

void CSkinWindow::OnNcCalcSize(CWnd* pWnd, BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	CalcWindowRect( &lpncsp->rgrc[0], TRUE, pWnd->IsZoomed() );
}

void CSkinWindow::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	HMONITOR hMonitor = MonitorFromWindow( AfxGetMainWnd()->GetSafeHwnd(),
		MONITOR_DEFAULTTOPRIMARY );

	MONITORINFO oMonitor = {0};
	oMonitor.cbSize = sizeof( MONITORINFO );
	GetMonitorInfo( hMonitor, &oMonitor );

	CRect rcWork = oMonitor.rcWork;
	rcWork.OffsetRect( -rcWork.left, -rcWork.top );

	CRect rcAdjusted( &rcWork );
	CalcWindowRect( &rcAdjusted, FALSE, TRUE );

	if ( m_rcMaximise.left < 0 )
		rcWork.left = rcAdjusted.left;
	else
		rcWork.left -= m_rcMaximise.left;

	if ( m_rcMaximise.top < 0 )
		rcWork.top = rcAdjusted.top;
	else
		rcWork.top -= m_rcMaximise.top;

	if ( m_rcMaximise.right < 0 )
		rcWork.right = rcAdjusted.right;
	else
		rcWork.right += m_rcMaximise.right;

	if ( m_rcMaximise.bottom < 0 )
		rcWork.bottom = rcAdjusted.bottom;
	else
		rcWork.bottom += m_rcMaximise.bottom;

	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	lpMMI->ptMinTrackSize.x	= max( lpMMI->ptMinTrackSize.x, m_szMinSize.cx );
	lpMMI->ptMinTrackSize.y	= max( lpMMI->ptMinTrackSize.y, m_szMinSize.cy );
}

UINT CSkinWindow::OnNcHitTest(CWnd* pWnd, CPoint point, BOOL bResizable)
{
	CRect rc, rcAnchor;
	int nPointX = 0;

	pWnd->GetWindowRect( &rc );
	if ( Settings.General.LanguageRTL )
	{
		nPointX = point.x;
		point.x = 2 * rc.left + rc.Width() - point.x;
	}

	if ( m_bAnchor[ SKINANCHOR_SYSTEM ] )
	{
		ResolveAnchor( rc, rcAnchor, SKINANCHOR_SYSTEM );
		if ( rcAnchor.PtInRect( point ) ) return HTSYSMENU;
	}

	if ( m_bAnchor[ SKINANCHOR_MINIMISE ] )
	{
		ResolveAnchor( rc, rcAnchor, SKINANCHOR_MINIMISE );
		if ( rcAnchor.PtInRect( point ) ) return HTREDUCE;
	}

	if ( m_bAnchor[ SKINANCHOR_MAXIMISE ] )
	{
		ResolveAnchor( rc, rcAnchor, SKINANCHOR_MAXIMISE );
		if ( rcAnchor.PtInRect( point ) ) return HTZOOM;
	}

	if ( m_bAnchor[ SKINANCHOR_CLOSE ] )
	{
		ResolveAnchor( rc, rcAnchor, SKINANCHOR_CLOSE );
		if ( rcAnchor.PtInRect( point ) ) return HTCLOSE;
	}

	if ( Settings.General.LanguageRTL )
		point.x = nPointX;

	if ( bResizable && ! pWnd->IsZoomed() )
	{
		if ( point.x >= rc.right - SIZEBOX_WIDTH && point.y >= rc.bottom - SIZEBOX_WIDTH )
			return HTBOTTOMRIGHT;

		if ( point.x < rc.left + m_rcResize.left )
		{
			if ( point.y < rc.top + m_rcResize.top ) return HTTOPLEFT;
			else if ( point.y >= rc.bottom - m_rcResize.bottom ) return HTBOTTOMLEFT;
			else return HTLEFT;
		}
		else if ( point.x >= rc.right - m_rcResize.right )
		{
			if ( point.y < rc.top + m_rcResize.top ) return HTTOPRIGHT;
			else if ( point.y >= rc.bottom - m_rcResize.bottom ) return HTBOTTOMRIGHT;
			else return HTRIGHT;
		}
		else if ( point.y < rc.top + m_rcResize.top )
		{
			if ( point.x < rc.left + m_rcResize.left ) return HTTOPLEFT;
			else if ( point.x >= rc.right - m_rcResize.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if ( point.y >= rc.bottom - m_rcResize.bottom )
		{
			if ( point.x < rc.left + m_rcResize.left ) return HTBOTTOMLEFT;
			else if ( point.x >= rc.right - m_rcResize.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
	}

	OnNcCalcSize( pWnd, FALSE, (NCCALCSIZE_PARAMS*)&rc );

	if ( point.y < rc.top ) return HTCAPTION;

	return rc.PtInRect( point ) ? HTCLIENT : HTBORDER;
}

void CSkinWindow::OnNcPaint(CWnd* pWnd)
{
	Paint( pWnd );
}

BOOL CSkinWindow::OnNcActivate(CWnd* pWnd, BOOL bActive)
{
	Paint( pWnd, bActive ? TRI_TRUE : TRI_FALSE );

	return FALSE;
}

void CSkinWindow::OnSetText(CWnd* pWnd)
{
	Paint( pWnd );
}

void CSkinWindow::OnSize(CWnd* pWnd)
{
	if ( pWnd->IsIconic() ) return;

	if ( pWnd->IsZoomed() )	// Fullscreen Resize
	{
		CRect rcWnd;
		SystemParametersInfo( SPI_GETWORKAREA, 0, rcWnd, 0 );

		HMONITOR hMonitor = MonitorFromWindow( pWnd->GetSafeHwnd(),
			MONITOR_DEFAULTTONEAREST );

		MONITORINFO oMonitor = {0};
		oMonitor.cbSize = sizeof( MONITORINFO );
		GetMonitorInfo( hMonitor, &oMonitor );

		if ( oMonitor.dwFlags & MONITORINFOF_PRIMARY )
		{
			SetWindowPos( pWnd->GetSafeHwnd(), HWND_TOP, rcWnd.left, rcWnd.top,
				rcWnd.Width(), rcWnd.Height(), 0 );
		}

		pWnd->SetWindowRgn( NULL, TRUE );
	}
	else if ( m_pRegionXML )
	{
		SelectRegion( pWnd );				// Redraw = true
	}
	else if ( ! theApp.m_bIsWin2000 )
	{
		CRect rcWnd;

		pWnd->GetWindowRect( &rcWnd );
		rcWnd.OffsetRect( -rcWnd.left, -rcWnd.top );
		rcWnd.right++; rcWnd.bottom++;

		HRGN hRgn = CreateRectRgnIndirect( &rcWnd );
		pWnd->SetWindowRgn( hRgn, TRUE );	// Redraw = true
	}
}

BOOL CSkinWindow::OnEraseBkgnd(CWnd* pWnd, CDC* pDC)
{
	if ( m_bmWatermark.m_hObject == NULL ) return FALSE;

	if ( ! m_dcSkin.m_hDC ) m_dcSkin.CreateCompatibleDC( pDC );
	CBitmap* pOldImage = (CBitmap*)m_dcSkin.SelectObject( &m_bmWatermark );

	BITMAP pWatermark;
	CRect rc;

	pWnd->GetClientRect( &rc );
	m_bmWatermark.GetBitmap( &pWatermark );

	for ( int nY = rc.top ; nY < rc.bottom ; nY += pWatermark.bmHeight )
	{
		for ( int nX = rc.left ; nX < rc.right ; nX += pWatermark.bmWidth )
		{
			pDC->BitBlt( nX, nY, pWatermark.bmWidth, pWatermark.bmHeight, &m_dcSkin, 0, 0, SRCCOPY );
		}
	}

	m_dcSkin.SelectObject( pOldImage );
	return TRUE;
}

void CSkinWindow::OnNcMouseMove(CWnd* pWnd, UINT nHitTest, CPoint /*point*/)
{
	int nAnchor = 0;
	if ( nHitTest == HTSYSMENU ) nAnchor = SKINANCHOR_SYSTEM;
	else if ( nHitTest == HTREDUCE ) nAnchor = SKINANCHOR_MINIMISE;
	else if ( nHitTest == HTZOOM ) nAnchor = SKINANCHOR_MAXIMISE;
	else if ( nHitTest == HTCLOSE ) nAnchor = SKINANCHOR_CLOSE;

	if ( m_nDownAnchor && m_nDownAnchor != nAnchor )
	{
		if ( ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) == 0 )
			m_nDownAnchor = 0;
		nAnchor = 0;
	}

	m_nHoverAnchor = nAnchor;

	Paint( pWnd );
}

BOOL CSkinWindow::OnNcLButtonDown(CWnd* pWnd, UINT nHitTest, CPoint point)
{
	int nAnchor = 0;
	if ( nHitTest == HTSYSMENU ) nAnchor = SKINANCHOR_SYSTEM;
	else if ( nHitTest == HTREDUCE ) nAnchor = SKINANCHOR_MINIMISE;
	else if ( nHitTest == HTZOOM ) nAnchor = SKINANCHOR_MAXIMISE;
	else if ( nHitTest == HTCLOSE ) nAnchor = SKINANCHOR_CLOSE;
	else return FALSE;

	m_nHoverAnchor = m_nDownAnchor = nAnchor;

	if ( nAnchor == SKINANCHOR_SYSTEM )
	{
		CRect rcWindow, rcSystem;

		pWnd->GetWindowRect( &rcWindow );
		ResolveAnchor( rcWindow, rcSystem, SKINANCHOR_SYSTEM );
		CMenu* pPopup = pWnd->GetSystemMenu( FALSE );

		Paint( pWnd );

		DWORD nTime = GetTickCount();

		UINT nCmdID = pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,
			Settings.General.LanguageRTL ? rcWindow.right - rcSystem.left + rcWindow.left : rcSystem.left,
			rcSystem.bottom, pWnd, NULL );

		m_nHoverAnchor = m_nDownAnchor = 0;

		if ( nCmdID )
		{
			pWnd->PostMessage( WM_SYSCOMMAND, nCmdID, MAKELONG( rcSystem.left, rcSystem.bottom ) );
		}
		else if ( GetTickCount() - nTime < 300 )
		{
			GetCursorPos( &point );
			if ( OnNcHitTest( pWnd, point, FALSE ) == HTSYSMENU )
				pWnd->PostMessage( WM_SYSCOMMAND, SC_CLOSE, MAKELONG( rcSystem.left, rcSystem.bottom ) );
		}
	}

	Paint( pWnd );

	return TRUE;
}

BOOL CSkinWindow::OnNcLButtonUp(CWnd* pWnd, UINT /*nHitTest*/, CPoint /*point*/)
{
	if ( m_nDownAnchor )
	{
		if ( m_nDownAnchor == m_nHoverAnchor )
		{
			switch ( m_nDownAnchor )
			{
			case SKINANCHOR_MINIMISE:
				pWnd->PostMessage( WM_SYSCOMMAND, SC_MINIMIZE );
				break;
			case SKINANCHOR_MAXIMISE:
				pWnd->PostMessage( WM_SYSCOMMAND, pWnd->IsZoomed() ? SC_RESTORE : SC_MAXIMIZE );
				break;
			case SKINANCHOR_CLOSE:
				pWnd->PostMessage( WM_SYSCOMMAND, SC_CLOSE );
				break;
			}
		}
		m_nHoverAnchor = 0;
		m_nDownAnchor = 0;
	}

	Paint( pWnd );

	return FALSE;
}

BOOL CSkinWindow::OnNcLButtonDblClk(CWnd* pWnd, UINT nHitTest, CPoint /*point*/)
{
	if ( nHitTest == HTSYSMENU )
	{
		pWnd->PostMessage( WM_SYSCOMMAND, SC_CLOSE );
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CSkinWindow paint

void CSkinWindow::Prepare(CDC* pDC)
{
	if ( m_dcSkin.m_hDC == NULL )
		m_dcSkin.CreateCompatibleDC( pDC );
	if ( m_hoSkin == NULL )
		m_hoSkin = (HBITMAP)m_dcSkin.SelectObject( &m_bmSkin )->GetSafeHandle();
	if ( Settings.General.LanguageRTL )
		SetLayout( m_dcSkin.m_hDC, LAYOUT_BITMAPORIENTATIONPRESERVED );
}

void CSkinWindow::Paint(CWnd* pWnd, TRISTATE bActive)
{
	// ToDo: Should most of this work be moved to OnSkin/OnSize?

	HICON hIcon = NULL;
	CString strCaption;
	CRect rc, rcItem;

	CWindowDC dc( pWnd );

	dc.SetLayout( Settings.General.LanguageRTL ? LAYOUT_RTL : 0 );

	Prepare( &dc );

	pWnd->GetWindowRect( &rc );
	rc.OffsetRect( -rc.left, -rc.top );

	if ( bActive == TRI_UNKNOWN )
	{
		if ( pWnd->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) ) )
		{
			CMDIFrameWnd* pFrame = ((CMDIChildWnd*)pWnd)->GetMDIFrame();
			bActive = pFrame->MDIGetActive() == pWnd ? TRI_TRUE : TRI_FALSE;
		}
		else
		{
			bActive = CWnd::GetForegroundWindow() == pWnd ? TRI_TRUE : TRI_FALSE;
		}
	}

	//	BOOL bZoomed = pWnd->IsZoomed();

	// Caption Bar Setup:

	if ( m_bCaption )
	{
		pWnd->GetWindowText( strCaption );
		if ( m_bCaptionCaps )
		{
			CharUpper( strCaption.GetBuffer() );
			strCaption.ReleaseBuffer();
		}
	}

	if ( m_bAnchor[ SKINANCHOR_ICON ] )
	{
		hIcon = pWnd->GetIcon( FALSE );
		if ( hIcon == NULL ) hIcon = pWnd->GetIcon( TRUE );
	}

	int nCaptionHeight = 0;
	if ( m_bPart[ SKINPART_TOP ] ) nCaptionHeight = max( nCaptionHeight, m_rcPart[ SKINPART_TOP ].Height() );
	if ( m_bPart[ SKINPART_TOP_LEFT ] ) nCaptionHeight = max( nCaptionHeight, m_rcPart[ SKINPART_TOP_LEFT ].Height() );
	else if ( m_bPart[ SKINPART_TOP_RIGHT ] ) nCaptionHeight = max( nCaptionHeight, m_rcPart[ SKINPART_TOP_RIGHT ].Height() );

	CSize size( rc.Width(), nCaptionHeight );
	CDC* pDC = CoolInterface.GetBuffer( dc, size );
	pDC->SetLayout( Settings.General.LanguageRTL ? LAYOUT_RTL : 0 );
	COLORREF crOldTextColor = pDC->GetTextColor();

	// Window Buttons

	for ( int nAnchor = SKINANCHOR_SYSTEM ; nAnchor <= SKINANCHOR_CLOSE ; nAnchor++ )
	{
		if ( m_bAnchor[ nAnchor ] )
		{
			int nPart = ( nAnchor - SKINANCHOR_SYSTEM ) * 3 + SKINPART_SYSTEM;

			if ( m_nHoverAnchor == nAnchor )
				nPart += ( m_nDownAnchor == nAnchor ? 2 : 1 );
			else
				nPart += ( m_nDownAnchor == nAnchor ? 0 : 0 );

			if ( m_bPart[ nPart ] )
			{
				ResolveAnchor( rc, rcItem, nAnchor );

				// ToDo: Buttons like "Close" are all mirrored for RTL layout
				// Maybe <anchor name="Close" rect="-30,5,20,20" rtl="true"/>
				// should be used instead? Small design flaws appear sometimes with current implementation.

				if ( Settings.General.LanguageRTL && m_bPart[ SKINPART_TOP ] && nAnchor == SKINANCHOR_CLOSE )
					pDC->StretchBlt( m_rcPart[ nPart ].Width() + rcItem.left - 1, rcItem.top,
						-m_rcPart[ nPart ].Width(), m_rcPart[ nPart ].Height(),
						&m_dcSkin, m_rcPart[ nPart ].left, m_rcPart[ nPart ].top,
						m_rcPart[ nPart ].Width(), m_rcPart[ nPart ].Height(), SRCCOPY );
				else
					pDC->BitBlt( rcItem.left, rcItem.top,
						m_rcPart[ nPart ].Width(), m_rcPart[ nPart ].Height(), &m_dcSkin,
						m_rcPart[ nPart ].left, m_rcPart[ nPart ].top, SRCCOPY );

				pDC->ExcludeClipRect( rcItem.left, rcItem.top,
					rcItem.left + m_rcPart[ nPart ].Width(),
					rcItem.top + m_rcPart[ nPart ].Height() );

				// ToDo: Add other button support? (mediaplayer etc.)
			}
		}
	}

	// Frames (Active/Inactive):

	CRect rcLeft( &rc ), rcTop( &rc ), rcRight( &rc ), rcBottom( &rc );
	int nTotalWidth, nCaptionWidth, nSystemOffset;
	int nSystemWidth, nCaptionOffset, nRestOffset;
	nCaptionOffset = m_rcMirror.left;
	nCaptionWidth = m_rcMirror.top;
	nSystemOffset = m_rcMirror.right - m_rcMirror.left;
	nSystemWidth = m_rcMirror.bottom - m_rcMirror.top;

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_TOP_LEFT_ALT ] )
	{
		pDC->BitBlt( 0, 0, m_rcPart[ SKINPART_TOP_LEFT_ALT ].Width(),
			m_rcPart[ SKINPART_TOP_LEFT_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_TOP_LEFT_ALT ].left,
			m_rcPart[ SKINPART_TOP_LEFT_ALT ].top, SRCCOPY );

		nTotalWidth = m_rcPart[ SKINPART_TOP_LEFT_ALT ].Width();
		nRestOffset = nTotalWidth - nSystemWidth - nSystemOffset - nCaptionWidth - nCaptionOffset;

		// Inactive main window caption mirroring for RTL
		if ( Settings.General.LanguageRTL && m_sTargets == "|CMainWnd|" && m_nMirror != 0 )
		{
			pDC->StretchBlt( nTotalWidth - nCaptionOffset, 0, -nCaptionWidth,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].Height(), &m_dcSkin,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].left + nRestOffset + nSystemWidth + nSystemOffset,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].top, nCaptionWidth,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].Height(), SRCCOPY );
			if ( m_nMirror == 2 )
				pDC->StretchBlt( nRestOffset + nSystemWidth, 0, -nSystemWidth,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].Height(), &m_dcSkin,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].left + nRestOffset,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].top, nSystemWidth,
				m_rcPart[ SKINPART_TOP_LEFT_ALT ].Height(), SRCCOPY );
		}

		rcLeft.top += m_rcPart[ SKINPART_TOP_LEFT_ALT ].Height();
		rcTop.left += m_rcPart[ SKINPART_TOP_LEFT_ALT ].Width();
	}
	else if ( m_bPart[ SKINPART_TOP_LEFT ] )
	{
		pDC->BitBlt( 0, 0, m_rcPart[ SKINPART_TOP_LEFT ].Width(),
			m_rcPart[ SKINPART_TOP_LEFT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_TOP_LEFT ].left,
			m_rcPart[ SKINPART_TOP_LEFT ].top, SRCCOPY );

		// Active main window caption mirroring for RTL
		if ( Settings.General.LanguageRTL && m_sTargets == "|CMainWnd|" && m_nMirror != 0 )
		{
			nTotalWidth = m_rcPart[ SKINPART_TOP_LEFT ].Width();
			nRestOffset = nTotalWidth - nSystemWidth - nSystemOffset - nCaptionWidth - nCaptionOffset;
			pDC->StretchBlt( nTotalWidth - nCaptionOffset, 0, -nCaptionWidth,
				m_rcPart[ SKINPART_TOP_LEFT ].Height(), &m_dcSkin,
				m_rcPart[ SKINPART_TOP_LEFT ].left + nRestOffset + nSystemWidth + nSystemOffset,
				m_rcPart[ SKINPART_TOP_LEFT ].top, nCaptionWidth,
				m_rcPart[ SKINPART_TOP_LEFT ].Height(), SRCCOPY );
			if ( m_nMirror == 2 )
				pDC->StretchBlt( nRestOffset + nSystemWidth, 0, -nSystemWidth,
				m_rcPart[ SKINPART_TOP_LEFT ].Height(), &m_dcSkin,
				m_rcPart[ SKINPART_TOP_LEFT ].left + nRestOffset,
				m_rcPart[ SKINPART_TOP_LEFT ].top, nSystemWidth,
				m_rcPart[ SKINPART_TOP_LEFT ].Height(), SRCCOPY );
		}

		rcLeft.top += m_rcPart[ SKINPART_TOP_LEFT ].Height();
		rcTop.left += m_rcPart[ SKINPART_TOP_LEFT ].Width();
	}

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_TOP_RIGHT_ALT ] )
	{
		pDC->BitBlt( rc.Width() - m_rcPart[ SKINPART_TOP_RIGHT_ALT ].Width(), 0,
			m_rcPart[ SKINPART_TOP_RIGHT_ALT ].Width(),
			m_rcPart[ SKINPART_TOP_RIGHT_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_TOP_RIGHT_ALT ].left,
			m_rcPart[ SKINPART_TOP_RIGHT_ALT ].top, SRCCOPY );

		rcTop.right -= m_rcPart[ SKINPART_TOP_RIGHT_ALT ].Width();
		rcRight.top += m_rcPart[ SKINPART_TOP_RIGHT_ALT ].Height();
	}
	else if ( m_bPart[ SKINPART_TOP_RIGHT ] )
	{
		pDC->BitBlt( rc.Width() - m_rcPart[ SKINPART_TOP_RIGHT ].Width(), 0,
			m_rcPart[ SKINPART_TOP_RIGHT ].Width(),
			m_rcPart[ SKINPART_TOP_RIGHT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_TOP_RIGHT ].left,
			m_rcPart[ SKINPART_TOP_RIGHT ].top, SRCCOPY );

		rcTop.right -= m_rcPart[ SKINPART_TOP_RIGHT ].Width();
		rcRight.top += m_rcPart[ SKINPART_TOP_RIGHT ].Height();
	}

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_BOTTOM_LEFT_ALT ] )
	{
		dc.BitBlt( 0, rc.Height() - m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].Height(),
			m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].Width(),
			m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].left,
			m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].top, SRCCOPY );

		rcBottom.left += m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].Width();
		rcLeft.bottom -= m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].Height();
	}
	else if ( m_bPart[ SKINPART_BOTTOM_LEFT ] )
	{
		dc.BitBlt( 0, rc.Height() - m_rcPart[ SKINPART_BOTTOM_LEFT ].Height(),
			m_rcPart[ SKINPART_BOTTOM_LEFT ].Width(),
			m_rcPart[ SKINPART_BOTTOM_LEFT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_BOTTOM_LEFT ].left,
			m_rcPart[ SKINPART_BOTTOM_LEFT ].top, SRCCOPY );

		rcBottom.left += m_rcPart[ SKINPART_BOTTOM_LEFT ].Width();
		rcLeft.bottom -= m_rcPart[ SKINPART_BOTTOM_LEFT ].Height();
	}

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_BOTTOM_RIGHT_ALT ] )
	{
		dc.BitBlt( 0, rc.Height() - m_rcPart[ SKINPART_BOTTOM_LEFT_ALT ].Height(),
			m_rcPart[ SKINPART_BOTTOM_RIGHT_ALT ].Width(),
			m_rcPart[ SKINPART_BOTTOM_RIGHT_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_BOTTOM_RIGHT_ALT ].left,
			m_rcPart[ SKINPART_BOTTOM_RIGHT_ALT ].top, SRCCOPY );

		rcBottom.left += m_rcPart[ SKINPART_BOTTOM_RIGHT_ALT ].Width();
		rcLeft.bottom -= m_rcPart[ SKINPART_BOTTOM_RIGHT_ALT ].Height();
	}
	else if ( m_bPart[ SKINPART_BOTTOM_RIGHT ] )
	{
		dc.BitBlt( rc.Width() - m_rcPart[ SKINPART_BOTTOM_RIGHT ].Width(),
			rc.Height() - m_rcPart[ SKINPART_BOTTOM_RIGHT ].Height(),
			m_rcPart[ SKINPART_BOTTOM_RIGHT ].Width(),
			m_rcPart[ SKINPART_BOTTOM_RIGHT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_BOTTOM_RIGHT ].left,
			m_rcPart[ SKINPART_BOTTOM_RIGHT ].top, SRCCOPY );

		rcRight.bottom -= m_rcPart[ SKINPART_BOTTOM_RIGHT ].Height();
		rcBottom.right -= m_rcPart[ SKINPART_BOTTOM_RIGHT ].Width();
	}

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_LEFT_TOP_ALT ] )
	{
		dc.BitBlt( 0, rcLeft.top, m_rcPart[ SKINPART_LEFT_TOP_ALT ].Width(),
			m_rcPart[ SKINPART_LEFT_TOP_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_LEFT_TOP_ALT ].left,
			m_rcPart[ SKINPART_LEFT_TOP_ALT ].top, SRCCOPY );

		rcLeft.top += m_rcPart[ SKINPART_LEFT_TOP ].Height();
	}
	else if ( m_bPart[ SKINPART_LEFT_TOP ] )
	{
		dc.BitBlt( 0, rcLeft.top, m_rcPart[ SKINPART_LEFT_TOP ].Width(),
			m_rcPart[ SKINPART_LEFT_TOP ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_LEFT_TOP ].left,
			m_rcPart[ SKINPART_LEFT_TOP ].top, SRCCOPY );

		rcLeft.top += m_rcPart[ SKINPART_LEFT_TOP ].Height();
	}

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_LEFT_BOTTOM_ALT ] )
	{
		dc.BitBlt( 0, rcLeft.bottom - m_rcPart[ SKINPART_LEFT_BOTTOM_ALT ].Height(),
			m_rcPart[ SKINPART_LEFT_BOTTOM_ALT ].Width(),
			m_rcPart[ SKINPART_LEFT_BOTTOM_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_LEFT_BOTTOM_ALT ].left,
			m_rcPart[ SKINPART_LEFT_BOTTOM_ALT ].top, SRCCOPY );

		rcLeft.bottom -= m_rcPart[ SKINPART_LEFT_BOTTOM_ALT ].Height();
	}
	else if ( m_bPart[ SKINPART_LEFT_BOTTOM ] )
	{
		dc.BitBlt( 0, rcLeft.bottom - m_rcPart[ SKINPART_LEFT_BOTTOM ].Height(),
			m_rcPart[ SKINPART_LEFT_BOTTOM ].Width(),
			m_rcPart[ SKINPART_LEFT_BOTTOM ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_LEFT_BOTTOM ].left,
			m_rcPart[ SKINPART_LEFT_BOTTOM ].top, SRCCOPY );

		rcLeft.bottom -= m_rcPart[ SKINPART_LEFT_BOTTOM ].Height();
	}

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_RIGHT_TOP_ALT ] )
	{
		dc.BitBlt( rcRight.right - m_rcPart[ SKINPART_RIGHT_TOP_ALT ].Width(),
			rcRight.top, m_rcPart[ SKINPART_RIGHT_TOP_ALT ].Width(),
			m_rcPart[ SKINPART_RIGHT_TOP_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_RIGHT_TOP_ALT ].left,
			m_rcPart[ SKINPART_RIGHT_TOP_ALT ].top, SRCCOPY );

		rcRight.top += m_rcPart[ SKINPART_RIGHT_TOP_ALT ].Height();
	}
	else if ( m_bPart[ SKINPART_RIGHT_TOP ] )
	{
		dc.BitBlt( rcRight.right - m_rcPart[ SKINPART_RIGHT_TOP ].Width(),
			rcRight.top, m_rcPart[ SKINPART_RIGHT_TOP ].Width(),
			m_rcPart[ SKINPART_RIGHT_TOP ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_RIGHT_TOP ].left,
			m_rcPart[ SKINPART_RIGHT_TOP ].top, SRCCOPY );

		rcRight.top += m_rcPart[ SKINPART_RIGHT_TOP ].Height();
	}

	if ( bActive == TRI_FALSE && m_bPart[ SKINPART_RIGHT_BOTTOM_ALT ] )
	{
		dc.BitBlt( rcRight.right - m_rcPart[ SKINPART_RIGHT_BOTTOM_ALT ].Width(),
			rcRight.bottom - m_rcPart[ SKINPART_RIGHT_BOTTOM_ALT ].Height(),
			m_rcPart[ SKINPART_RIGHT_BOTTOM_ALT ].Width(),
			m_rcPart[ SKINPART_RIGHT_BOTTOM_ALT ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_RIGHT_BOTTOM_ALT ].left,
			m_rcPart[ SKINPART_RIGHT_BOTTOM_ALT ].top, SRCCOPY );

		rcRight.bottom -= m_rcPart[ SKINPART_RIGHT_BOTTOM_ALT ].Height();
	}
	else if ( m_bPart[ SKINPART_RIGHT_BOTTOM ] )
	{
		dc.BitBlt( rcRight.right - m_rcPart[ SKINPART_RIGHT_BOTTOM ].Width(),
			rcRight.bottom - m_rcPart[ SKINPART_RIGHT_BOTTOM ].Height(),
			m_rcPart[ SKINPART_RIGHT_BOTTOM ].Width(),
			m_rcPart[ SKINPART_RIGHT_BOTTOM ].Height(), &m_dcSkin,
			m_rcPart[ SKINPART_RIGHT_BOTTOM ].left,
			m_rcPart[ SKINPART_RIGHT_BOTTOM ].top, SRCCOPY );

		rcRight.bottom -= m_rcPart[ SKINPART_RIGHT_BOTTOM ].Height();
	}

	if ( rcTop.left < rcTop.right )
	{
		if ( bActive == TRI_FALSE && m_bPart[ SKINPART_TOP_ALT ] )
		{
			CRect* pRect = &m_rcPart[ SKINPART_TOP_ALT ];

			if ( m_nPart[ SKINPART_TOP_ALT ] == SKINPARTMODE_STRETCH )
			{
				pDC->SetStretchBltMode( STRETCH_DELETESCANS );
				pDC->StretchBlt( rcTop.left, 0, rcTop.Width(), pRect->Height(),
					&m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nX = rcTop.left ; nX < rcTop.right ; nX += pRect->Width() )
				{
					pDC->BitBlt( nX, 0, min( pRect->Width(), rcTop.right - nX ),
						pRect->Height(), &m_dcSkin, pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
		else if ( m_bPart[ SKINPART_TOP ] )
		{
			CRect* pRect = &m_rcPart[ SKINPART_TOP ];

			if ( m_nPart[ SKINPART_TOP ] == SKINPARTMODE_STRETCH )
			{
				pDC->SetStretchBltMode( STRETCH_DELETESCANS );
				pDC->StretchBlt( rcTop.left, 0, rcTop.Width(), pRect->Height(),
					&m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nX = rcTop.left ; nX < rcTop.right ; nX += pRect->Width() )
				{
					pDC->BitBlt( nX, 0, min( pRect->Width(), rcTop.right - nX ),
						pRect->Height(), &m_dcSkin, pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
	}

	if ( rcLeft.top < rcLeft.bottom )
	{
		if ( bActive == TRI_FALSE && m_bPart[ SKINPART_LEFT_ALT ] )
		{
			CRect* pRect = &m_rcPart[ SKINPART_LEFT_ALT ];

			if ( m_nPart[ SKINPART_LEFT_ALT ] == SKINPARTMODE_STRETCH )
			{
				dc.SetStretchBltMode( STRETCH_DELETESCANS );
				dc.StretchBlt( 0, rcLeft.top, pRect->Width(), rcLeft.Height(),
					&m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nY = rcLeft.top ; nY < rcLeft.bottom ; nY += pRect->Height() )
				{
					dc.BitBlt( 0, nY, pRect->Width(), min( pRect->Height(), int(rcLeft.bottom - nY) ),
						&m_dcSkin, pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
		else if ( m_bPart[ SKINPART_LEFT ] )
		{
			CRect* pRect = &m_rcPart[ SKINPART_LEFT ];

			if ( m_nPart[ SKINPART_LEFT ] == SKINPARTMODE_STRETCH )
			{
				dc.SetStretchBltMode( STRETCH_DELETESCANS );
				dc.StretchBlt( 0, rcLeft.top, pRect->Width(), rcLeft.Height(),
					&m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nY = rcLeft.top ; nY < rcLeft.bottom ; nY += pRect->Height() )
				{
					dc.BitBlt( 0, nY, pRect->Width(), min( pRect->Height(), int(rcLeft.bottom - nY) ),
						&m_dcSkin, pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
	}

	if ( rcRight.top < rcRight.bottom )
	{
		if ( bActive == TRI_FALSE && m_bPart[ SKINPART_RIGHT_ALT ] )
		{
			CRect* pRect = &m_rcPart[ SKINPART_RIGHT_ALT ];

			if ( m_nPart[ SKINPART_RIGHT_ALT ] == SKINPARTMODE_STRETCH )
			{
				dc.SetStretchBltMode( STRETCH_DELETESCANS );
				dc.StretchBlt( rc.right - pRect->Width(), rcRight.top, pRect->Width(),
					rcRight.Height(), &m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nY = rcRight.top ; nY < rcRight.bottom ; nY += pRect->Height() )
				{
					dc.BitBlt( rc.right - pRect->Width(), nY, pRect->Width(),
						min( pRect->Height(), rcRight.bottom - nY ),
						&m_dcSkin, 	pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
		else if ( m_bPart[ SKINPART_RIGHT ] && rcRight.top < rcRight.bottom )
		{
			CRect* pRect = &m_rcPart[ SKINPART_RIGHT ];

			if ( m_nPart[ SKINPART_RIGHT ] == SKINPARTMODE_STRETCH )
			{
				dc.SetStretchBltMode( STRETCH_DELETESCANS );
				dc.StretchBlt( rc.right - pRect->Width(), rcRight.top, pRect->Width(),
					rcRight.Height(), &m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nY = rcRight.top ; nY < rcRight.bottom ; nY += pRect->Height() )
				{
					dc.BitBlt( rc.right - pRect->Width(), nY, pRect->Width(),
						min( pRect->Height(), rcRight.bottom - nY ),
						&m_dcSkin, 	pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
	}

	if ( rcBottom.left < rcBottom.right )
	{
		if ( bActive == TRI_FALSE && m_bPart[ SKINPART_BOTTOM_ALT ] )
		{
			CRect* pRect = &m_rcPart[ SKINPART_BOTTOM_ALT ];

			if ( m_nPart[ SKINPART_BOTTOM_ALT ] == SKINPARTMODE_STRETCH )
			{
				dc.SetStretchBltMode( STRETCH_DELETESCANS );
				dc.StretchBlt( rcBottom.left, rc.bottom - pRect->Height(),
					rcBottom.Width(), pRect->Height(),
					&m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nX = rcBottom.left ; nX < rcBottom.right ; nX += pRect->Width() )
				{
					dc.BitBlt( nX, rc.bottom - pRect->Height(),
						min( pRect->Width(), rcBottom.right - nX ), pRect->Height(),
						&m_dcSkin, pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
		else if ( m_bPart[ SKINPART_BOTTOM ] )
		{
			CRect* pRect = &m_rcPart[ SKINPART_BOTTOM ];

			if ( m_nPart[ SKINPART_BOTTOM ] == SKINPARTMODE_STRETCH )
			{
				dc.SetStretchBltMode( STRETCH_DELETESCANS );
				dc.StretchBlt( rcBottom.left, rc.bottom - pRect->Height(),
					rcBottom.Width(), pRect->Height(),
					&m_dcSkin, pRect->left, pRect->top,
					pRect->Width(), pRect->Height(), SRCCOPY );
			}
			else
			{
				for ( int nX = rcBottom.left ; nX < rcBottom.right ; nX += pRect->Width() )
				{
					dc.BitBlt( nX, rc.bottom - pRect->Height(),
						min( pRect->Width(), rcBottom.right - nX ), pRect->Height(),
						&m_dcSkin, pRect->left, pRect->top, SRCCOPY );
				}
			}
		}
	}

	// Caption Text:

	if ( hIcon != NULL )
	{
		ResolveAnchor( rc, rcItem, SKINANCHOR_ICON );
		DrawIconEx( pDC->GetSafeHdc(), rcItem.left, rcItem.top, hIcon, 16, 16, 0, NULL, DI_NORMAL );
	}

	if ( m_bCaption && strCaption.GetLength() )
	{
		CFont* pOldFont	= (CFont*)pDC->SelectObject( &m_fnCaption );
		CSize sz		= pDC->GetTextExtent( strCaption );
		CPoint ptCap;

		rcItem.left		= m_rcCaption.left + ( m_rcCaption.left >= 0 ? rc.left : rc.right );
		//if ( m_rcPart[ SKINPART_TOP_RIGHT ] )
		//	rcItem.right = rc.right - m_rcPart[ SKINPART_TOP_RIGHT ].Width();
		//else
			rcItem.right = m_rcCaption.Width() + ( m_rcCaption.Width() >= 0 ? rc.left : rc.right );
		rcItem.top		= rc.top + m_rcCaption.top;
		rcItem.bottom	= rc.top + m_rcCaption.bottom;

		switch ( m_nCaptionAlign )
		{
		case 0:
			ptCap.x = rcItem.left + 1;
			break;
		case 1:
			ptCap.x = ( rcItem.left + rcItem.right ) / 2 - sz.cx / 2;
			ptCap.x = max( ptCap.x, rcItem.left + 1 );
			break;
		case 2:
			ptCap.x = rcItem.right - sz.cx - 1;
			ptCap.x = max( ptCap.x, rcItem.left + 1 );
			break;
		}

		ptCap.y = ( rcItem.top + rcItem.bottom ) / 2 - sz.cy / 2;

		pDC->SetBkMode( TRANSPARENT );

		if ( m_crCaptionOutline != CLR_NONE )
		{
			pDC->SetTextColor( m_crCaptionOutline );
			pDC->ExtTextOut( ptCap.x - 1, ptCap.y - 1, ETO_CLIPPED, &rcItem, strCaption, NULL );
			pDC->ExtTextOut( ptCap.x, ptCap.y - 1, ETO_CLIPPED, &rcItem, strCaption, NULL );
			pDC->ExtTextOut( ptCap.x + 1, ptCap.y - 1, ETO_CLIPPED, &rcItem, strCaption, NULL );
			pDC->ExtTextOut( ptCap.x - 1, ptCap.y, ETO_CLIPPED, &rcItem, strCaption, NULL );
			pDC->ExtTextOut( ptCap.x + 1, ptCap.y, ETO_CLIPPED, &rcItem, strCaption, NULL );
			pDC->ExtTextOut( ptCap.x - 1, ptCap.y + 1, ETO_CLIPPED, &rcItem, strCaption, NULL );
			pDC->ExtTextOut( ptCap.x, ptCap.y + 1, ETO_CLIPPED, &rcItem, strCaption, NULL );
			pDC->ExtTextOut( ptCap.x + 1, ptCap.y + 1, ETO_CLIPPED, &rcItem, strCaption, NULL );
		}

		if ( m_crCaptionShadow != CLR_NONE )
		{
			pDC->SetTextColor( m_crCaptionShadow );
			pDC->ExtTextOut( ptCap.x + 1, ptCap.y + 1, ETO_CLIPPED, &rcItem, strCaption, NULL );
		}

		pDC->SetTextColor( bActive == TRI_TRUE ? m_crCaptionText : m_crCaptionInactive );
		pDC->ExtTextOut( ptCap.x, ptCap.y, ETO_CLIPPED, &rcItem, strCaption, NULL );
		pDC->SelectObject( pOldFont );
	}

	pDC->SetTextColor( crOldTextColor );

	dc.BitBlt( 0, 0, rc.Width(), nCaptionHeight, pDC, 0, 0, SRCCOPY );

	dc.SelectObject( GetStockObject( ANSI_VAR_FONT ) ); // Comctl32.dll font leak fix
}

//////////////////////////////////////////////////////////////////////
// CSkinWindow part and anchor access

BOOL CSkinWindow::GetPart(LPCTSTR pszName, CRect& rcPart)
{
	CRect* pRect;
	if ( ! m_pPartList.Lookup( pszName, pRect ) ) return FALSE;
	rcPart = *pRect;
	return TRUE;
}

BOOL CSkinWindow::GetAnchor(LPCTSTR pszName, CRect& rcAnchor)
{
	CRect* pRect;
	if ( ! m_pAnchorList.Lookup( pszName, pRect ) ) return FALSE;
	rcAnchor = *pRect;
	return TRUE;
}

BOOL CSkinWindow::GetAnchor(LPCTSTR pszName, const CRect& rcClient, CRect& rcAnchor)
{
	CRect* pRect;
	if ( ! m_pAnchorList.Lookup( pszName, pRect ) ) return FALSE;
	rcAnchor = *pRect;
	rcAnchor.OffsetRect( rcAnchor.left < 0 ? rcClient.right : rcClient.left, 0 );
	rcAnchor.OffsetRect( 0, rcAnchor.top < 0 ? rcClient.bottom : rcClient.top );
	return TRUE;
}

BOOL CSkinWindow::PaintPartOnAnchor(CDC* pDC, const CRect& rcClient, LPCTSTR pszPart, LPCTSTR pszAnchor)
{
	CRect rcPart, rcAnchor;

	if ( ! GetPart( pszPart, rcPart ) ) return FALSE;
	if ( ! GetAnchor( pszAnchor, rcClient, rcAnchor ) ) return FALSE;
	if ( m_dcSkin.m_hDC == NULL ) Prepare( pDC );

	pDC->BitBlt( rcAnchor.left, rcAnchor.top, rcPart.Width(), rcPart.Height(),
		&m_dcSkin, rcPart.left, rcPart.top, SRCCOPY );
	pDC->ExcludeClipRect( &rcAnchor );

	return TRUE;
}

void CSkinWindow::ResolveAnchor(const CRect& rcClient, CRect& rcAnchor, int nAnchor)
{
	rcAnchor = m_rcAnchor[ nAnchor ];
	rcAnchor.OffsetRect( rcAnchor.left < 0 ? rcClient.right : rcClient.left, 0 );
	rcAnchor.OffsetRect( 0, rcAnchor.top < 0 ? rcClient.bottom : rcClient.top );
}

//////////////////////////////////////////////////////////////////////
// CSkinWindow region builder

void CSkinWindow::SelectRegion(CWnd* pWnd)
{
	CRect rcWnd, rcPart;
	HRGN hRgn = NULL;

	pWnd->GetWindowRect( &rcWnd );
	rcWnd.OffsetRect( -rcWnd.left, -rcWnd.top );
	rcWnd.right++; rcWnd.bottom++;

	for ( POSITION pos = m_pRegionXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = m_pRegionXML->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("shape") ) ) continue;

		if ( ParseRect( pXML, &rcPart ) )
		{
			rcPart.right	-= rcPart.left;
			rcPart.bottom	-= rcPart.top;
			rcPart.left		+= rcPart.left >= 0 ? rcWnd.left : rcWnd.right + 1;
			rcPart.top		+= rcPart.top >= 0 ? rcWnd.top : rcWnd.bottom + 1;
			rcPart.right	+= rcPart.right >= 0 ? rcWnd.left : rcWnd.right + 1;
			rcPart.bottom	+= rcPart.bottom >= 0 ? rcWnd.top : rcWnd.bottom + 1;
		}
		else
		{
			rcPart.CopyRect( &rcWnd );
		}

		CString strType = pXML->GetAttributeValue( _T("type") );
		HRGN hPart = NULL;

		if ( strType.CompareNoCase( _T("rectangle") ) == 0 )
		{
			hPart = CreateRectRgnIndirect( &rcPart );
		}
		else if ( strType.CompareNoCase( _T("ellipse") ) == 0 )
		{
			hPart = CreateEllipticRgnIndirect( &rcPart );
		}
		else if ( strType.CompareNoCase( _T("roundRect") ) == 0 )
		{
			int nWidth, nHeight;
			_stscanf( pXML->GetAttributeValue( _T("size") ), _T("%i,%i"), &nWidth, &nHeight );
			hPart = CreateRoundRectRgn( rcPart.left, rcPart.top, rcPart.right, rcPart.bottom,
				nWidth, nHeight );
		}
		else
		{
			continue;
		}

		if ( hPart == NULL )
		{
			if ( hRgn ) DeleteObject( hRgn );
			pWnd->SetWindowRgn( NULL, TRUE );
			return;
		}

		if ( hRgn )
		{
			strType = pXML->GetAttributeValue( _T("combine") );

			if ( strType.CompareNoCase( _T("and") ) == 0 )
				CombineRgn( hRgn, hPart, hRgn, RGN_AND );
			else if ( strType.CompareNoCase( _T("copy") ) == 0 )
				CombineRgn( hRgn, hPart, hRgn, RGN_COPY );
			else if ( strType.CompareNoCase( _T("diff") ) == 0 )
				CombineRgn( hRgn, hRgn, hPart, RGN_DIFF );
			else if ( strType.CompareNoCase( _T("or") ) == 0 )
				CombineRgn( hRgn, hPart, hRgn, RGN_OR );
			else if ( strType.CompareNoCase( _T("xor") ) == 0 )
				CombineRgn( hRgn, hPart, hRgn, RGN_XOR );

			DeleteObject( hPart );
		}
		else
		{
			hRgn = hPart;
		}
	}

	if ( hRgn ) pWnd->SetWindowRgn( hRgn, TRUE );	// Redraw = true
}

CSize CSkinWindow::GetRegionSize()
{
	if ( ! m_pRegionXML ) return CSize( 0, 0 );

	CRect rcTotal( 0, 0, 0, 0 );

	for ( POSITION pos = m_pRegionXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = m_pRegionXML->GetNextElement( pos );
		CRect rcPart;

		if ( pXML->IsNamed( _T("shape") ) && ParseRect( pXML, &rcPart ) )
			rcTotal.UnionRect( &rcTotal, &rcPart );
	}

	return rcTotal.Size();
}

//////////////////////////////////////////////////////////////////////
// CSkinWindow pre-blend

BOOL CSkinWindow::PreBlend(CBitmap* pbmTarget, const CRect& rcTarget, const CRect& rcSource)
{
	// Currently Navbar transparency only
	// ToDo: Add transparency support for PNGs and frames, etc.?

	BITMAPINFO pImageInfo = {};
	BITMAPINFO pAlphaInfo = {};
	BITMAPINFO pCacheInfo = {};

	pImageInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pAlphaInfo.bmiHeader.biSize	= sizeof(BITMAPINFOHEADER);
	pCacheInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	HDC hDC = ::GetDC( 0 );
	SetLayout( hDC, Settings.General.LanguageRTL ? LAYOUT_BITMAPORIENTATIONPRESERVED : 0 );

	if ( 0 == GetDIBits( hDC, m_bmSkin, 0, 0, NULL, &pImageInfo, DIB_RGB_COLORS ) ||
		 0 == GetDIBits( hDC, *pbmTarget, 0, 0, NULL, &pCacheInfo, DIB_RGB_COLORS ) )
	{
		::ReleaseDC( 0, hDC );
		return FALSE;
	}

	BOOL bAlpha = m_bmAlpha.m_hObject &&
		GetDIBits( hDC, m_bmAlpha, 0, 0, NULL, &pAlphaInfo, DIB_RGB_COLORS );
	if ( ! bAlpha )
		CopyMemory( &pAlphaInfo, &pImageInfo, sizeof(pAlphaInfo) );

	int nImagePitch = ( ( pImageInfo.bmiHeader.biWidth * 3 ) + 3 ) & ~3;
	int nAlphaPitch = ( ( pAlphaInfo.bmiHeader.biWidth * 3 ) + 3 ) & ~3;
	int nCachePitch = ( ( pCacheInfo.bmiHeader.biWidth * 3 ) + 3 ) & ~3;

	pImageInfo.bmiHeader.biHeight		= -abs( pImageInfo.bmiHeader.biHeight );
	pImageInfo.bmiHeader.biBitCount		= 24;
	pImageInfo.bmiHeader.biCompression	= BI_RGB;
	pImageInfo.bmiHeader.biSizeImage	= -pImageInfo.bmiHeader.biHeight * nImagePitch;
	pAlphaInfo.bmiHeader.biHeight		= -abs( pAlphaInfo.bmiHeader.biHeight );
	pAlphaInfo.bmiHeader.biBitCount		= 24;
	pAlphaInfo.bmiHeader.biCompression	= BI_RGB;
	pAlphaInfo.bmiHeader.biSizeImage	= -pAlphaInfo.bmiHeader.biHeight * nAlphaPitch;
	pCacheInfo.bmiHeader.biHeight		= -abs( pCacheInfo.bmiHeader.biHeight );
	pCacheInfo.bmiHeader.biBitCount		= 24;
	pCacheInfo.bmiHeader.biCompression	= BI_RGB;
	pCacheInfo.bmiHeader.biSizeImage	= -pCacheInfo.bmiHeader.biHeight * nCachePitch;

	auto_array< BYTE > pCacheData( new BYTE[ pCacheInfo.bmiHeader.biSizeImage ] );
	auto_array< BYTE > pImageData( new BYTE[ pImageInfo.bmiHeader.biSizeImage ] );
	auto_array< BYTE > pAlphaData( bAlpha ? new BYTE[ pAlphaInfo.bmiHeader.biSizeImage ] : NULL );

	GetDIBits( hDC, *pbmTarget, 0, -pCacheInfo.bmiHeader.biHeight, pCacheData.get(), &pCacheInfo, DIB_RGB_COLORS );
	GetDIBits( hDC, m_bmSkin, 0, -pImageInfo.bmiHeader.biHeight, pImageData.get(), &pImageInfo, DIB_RGB_COLORS );
	if ( bAlpha )
		GetDIBits( hDC, m_bmAlpha, 0, -pAlphaInfo.bmiHeader.biHeight, pAlphaData.get(), &pAlphaInfo, DIB_RGB_COLORS );

	int nSrcY = rcSource.top, nSrcLeft = rcSource.left * 3;
	int nDstY = rcTarget.top, nDstLeft = rcTarget.left * 3;

	int nWidth = min( rcSource.Width(), rcTarget.Width() );
	nWidth = min( nWidth, pImageInfo.bmiHeader.biWidth - rcSource.left );
	nWidth = min( nWidth, pAlphaInfo.bmiHeader.biWidth - rcSource.left );
	nWidth = min( nWidth, pCacheInfo.bmiHeader.biWidth - rcTarget.left );
	if ( nWidth > 0 )
	{
		for ( int nY = min( rcTarget.Height(), rcSource.Height() ) ; nY ; nY--, nSrcY++, nDstY++ )
		{
			BYTE* pCachePtr = pCacheData.get() + nDstY * nCachePitch + nDstLeft;
			BYTE* pImagePtr = pImageData.get() + nSrcY * nImagePitch + nSrcLeft;

			if ( nDstY < 0 || nDstY >= -pCacheInfo.bmiHeader.biHeight )
			{
				// Out of bounds on destination
			}
			else if ( nSrcY < 0 || nSrcY >= -pImageInfo.bmiHeader.biHeight )
			{
				// Out of bounds on source
			}
			else if ( bAlpha && nSrcY < -pAlphaInfo.bmiHeader.biHeight )
			{
				BYTE* pAlphaPtr = pAlphaData.get() + nSrcY * nAlphaPitch + nSrcLeft;
				for ( int nX = nWidth ; nX ; nX-- )
				{
					register BYTE nAlpha = *pAlphaPtr; pAlphaPtr += 3;
					*pCachePtr = (BYTE)( ( (DWORD)(*pCachePtr) * ( 255 - nAlpha ) + (*pImagePtr) * nAlpha ) / 255 );
					pCachePtr++; pImagePtr++;
					*pCachePtr = (BYTE)( ( (DWORD)(*pCachePtr) * ( 255 - nAlpha ) + (*pImagePtr) * nAlpha ) / 255 );
					pCachePtr++; pImagePtr++;
					*pCachePtr = (BYTE)( ( (DWORD)(*pCachePtr) * ( 255 - nAlpha ) + (*pImagePtr) * nAlpha ) / 255 );
					pCachePtr++; pImagePtr++;
				}
			}
			else
			{
				CopyMemory( pCachePtr, pImagePtr, nWidth * 3 );
			}
		}
	}

	SetDIBits( hDC, *pbmTarget, 0, -pCacheInfo.bmiHeader.biHeight, pCacheData.get(),
		&pCacheInfo, DIB_RGB_COLORS );

	::ReleaseDC( 0, hDC );

	return TRUE;
}
