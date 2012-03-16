//
// MetaList.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "MetaList.h"
#include "Library.h"
#include "LibraryFolders.h"
#include "WndLibrary.h"
#include "WndMain.h"
#include "Schema.h"
#include "SchemaMember.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

#define METAPANEL_KEY_WIDTH	120
#define MUSICBRAINZ_HEIGHT	31
#define MUSICBRAINZ_WIDTH	200


//////////////////////////////////////////////////////////////////////
// CMetaList construction

CMetaList::CMetaList()
	: m_nHeight( 0 )
{
	m_bmMusicBrainz.LoadBitmap( IDB_MUSICBRAINZ_LOGO );
}

CMetaList::~CMetaList()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CMetaList clear (remove all)

void CMetaList::Clear()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		delete GetNext( pos );
	}

	m_bMusicBrainz = FALSE;
	m_pItems.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CMetaList add simple item

CMetaItem* CMetaList::Add(LPCTSTR pszKey, LPCTSTR pszValue)
{
	CMetaItem* pItem = new CMetaItem( NULL );
	pItem->m_sKey	= pszKey;
	pItem->m_sValue	= pszValue;
	m_pItems.AddTail( pItem );
	return pItem;
}

//////////////////////////////////////////////////////////////////////
// CMetaList find a key by name

CMetaItem* CMetaList::Find(LPCTSTR pszKey) const
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		if ( pItem->m_sKey.CompareNoCase( pszKey ) == 0 ) return pItem;
	}

	return NULL;
}

BOOL CMetaList::IsMusicBrainz() const
{
	CString strMultiple = LoadString( IDS_MULTIPLE );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		if ( pItem->m_pMember && ! pItem->m_sValue.IsEmpty() && pItem->m_sValue != strMultiple )
		{
			if ( pItem->m_pMember->m_sName == L"mbalbumid" ||
				 pItem->m_pMember->m_sName == L"mbartistid" ||
				 pItem->m_pMember->m_sName == L"mbpuid" )
				return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMetaList remove a key by name

void CMetaList::Remove(LPCTSTR pszKey)
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posCur = pos;

		CMetaItem* pItem = GetNext( pos );

		if ( pItem->m_sKey.CompareNoCase( pszKey ) == 0 )
		{
			m_pItems.RemoveAt( posCur );
			delete pItem;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CMetaList shuffle

void CMetaList::Shuffle()
{
	if ( m_pItems.GetCount() < 2 ) return;

	CMetaItem* pItem = (CMetaItem*)m_pItems.RemoveHead();
	m_pItems.AddTail( pItem );
}

//////////////////////////////////////////////////////////////////////
// CMetaList setup schema

void CMetaList::Setup(CSchemaPtr pSchema, BOOL bClear)
{
	if ( bClear )
		Clear();
	if ( ! pSchema ) return;

	for ( POSITION pos = pSchema->GetMemberIterator() ; pos ; )
	{
		CSchemaMember* pMember = pSchema->GetNextMember( pos );
		m_pItems.AddTail( new CMetaItem( pMember ) );
	}

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		if ( ! pItem->m_sValue.IsEmpty() )
			pItem->m_bValueDefined = TRUE;
	}
}

///////////////////////////////////////////////////////////////////////////
// Overwrites metadata list with a specified list keeping old members intact
// Doesn't verify schema members, just creates key-value pairs
void CMetaList::Setup(const CMetaList* pMetaList)
{
	if ( pMetaList == NULL ) return;

	for ( POSITION posOther = pMetaList->GetIterator() ; posOther ; )
	{
		const CMetaItem* pItemOther = pMetaList->GetNext( posOther );
		CMetaItem* pItem = Find( pItemOther->m_sKey );

		if ( pItem == NULL )
		{
			if ( ! pItemOther->m_sValue.IsEmpty() )
			{
				pItem = new CMetaItem( pItemOther->m_pMember );
				pItem->m_sKey	= pItemOther->m_sKey;
				pItem->m_sValue	= pItemOther->m_sValue;
				m_pItems.AddTail( pItem );
			}
		}
		else
		{
			pItem->m_sValue = pItemOther->m_sValue;
		}

		if ( pItem )
			pItem->m_bValueDefined = ! pItemOther->m_sValue.IsEmpty();
	}

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		const CMetaItem* pItemOther = pMetaList->Find( pItem->m_sKey );
		if ( pItemOther == NULL )
		{
			pItem->m_sValue.Empty();
			pItem->m_bValueDefined = FALSE;
			pItem->m_bLink = FALSE;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CMetaList combination

void CMetaList::Combine(const CXMLElement* pXML)
{
	if ( pXML == NULL ) return;

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		GetNext( pos )->Combine( pXML );
	}

	m_bMusicBrainz = IsMusicBrainz();
}

//////////////////////////////////////////////////////////////////////
// CMetaList vote (common)

void CMetaList::Vote()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		GetNext( pos )->Vote();
	}
}

//////////////////////////////////////////////////////////////////////
// CMetaList identify links

void CMetaList::CreateLinks()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		pItem->CreateLink();

		int nLink = pItem->m_sKey.Find( _T("Link") );

		if ( nLink > 0 )
		{
			CString strBase( pItem->m_sKey.Left( nLink ) );
			strBase.TrimRight();

			if ( CMetaItem* pBase = Find( strBase ) )
			{
				if ( ! pBase->m_sValue.IsEmpty() )
				{
					pItem->m_sKey	= pBase->m_sKey;
					pItem->m_sValue = pBase->m_sValue;
					pBase->m_sKey.Empty();
					pBase->m_sValue.Empty();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CMetaList clean (remove empty)

void CMetaList::Clean(int nMaxLength)
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posCur = pos;
		CMetaItem* pItem = GetNext( pos );

		if ( ! pItem->Limit( nMaxLength ) )
		{
			m_pItems.RemoveAt( posCur );
			delete pItem;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CMetaList compute the combined widths of the meta information

void CMetaList::ComputeWidth(CDC* pDC, int& nKeyWidth, int& nValueWidth)
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem	= GetNext( pos );
		if ( pItem->m_pMember && pItem->m_pMember->m_bHidden ) continue;

		CSize szKey			= pDC->GetTextExtent( pItem->m_sKey + ':' );
		CSize szValue		= pDC->GetTextExtent( pItem->m_sValue );

		nKeyWidth			= max( nKeyWidth, int(szKey.cx) );
		nValueWidth			= max( nValueWidth, int(szValue.cx) );
	}
}

INT_PTR CMetaList::GetCount(BOOL bVisibleOnly) const
{
	if ( ! bVisibleOnly ) return GetCount();

	INT_PTR nTotal = 0;

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		if ( pItem->m_pMember && pItem->m_pMember->m_bHidden ) continue;
		nTotal++;
	}

	return nTotal;
}

//////////////////////////////////////////////////////////////////////
// CMetaList hit testing

CMetaItem* CMetaList::HitTest(const CPoint& point, BOOL bLinksOnly)
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		if ( pItem->m_pMember && pItem->m_pMember->m_bHidden ) continue;

		if ( pItem->PtInRect( point ) )
		{
			if ( bLinksOnly && ! pItem->m_bLink ) return NULL;
			return pItem;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CMetaList pre-built set cursor handler

BOOL CMetaList::OnSetCursor(CWnd* pWnd)
{
	CPoint point;
	GetCursorPos( &point );
	pWnd->ScreenToClient( &point );

	if ( HitTest( point, TRUE ) == NULL ) return FALSE;

	SetCursor( AfxGetApp()->LoadCursor( IDC_HAND ) );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CMetaList layout (Was CMetaPanel)

int CMetaList::Layout(CDC* pDC, int nWidth)
{
	int nSmall = ( nWidth >= 400 ) ? nWidth / 2 - METAPANEL_KEY_WIDTH - 3 : 0;
	int nLarge = nWidth - METAPANEL_KEY_WIDTH - 3;

	m_nHeight = 0;

	CFont* pOld = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CMetaItem* pItem = GetNext( pos );
		if ( pItem->m_pMember && pItem->m_pMember->m_bHidden ) continue;

		CSize sz = pDC->GetTextExtent( pItem->GetDisplayValue() );

		if ( sz.cx <= nSmall )
		{
			pItem->m_bFullWidth	= FALSE;
			pItem->m_nHeight	= 18;

			if ( CMetaItem* pNext = GetNext( pos ) )
			{
				while ( pNext && pNext->m_pMember && pNext->m_pMember->m_bHidden )
				{
					pNext = GetNext( pos );
				}

				if ( pNext == NULL )
				{
					pItem->m_bFullWidth = TRUE;
					m_nHeight += 20;
					break;
				}

				sz = pDC->GetTextExtent( pNext->GetDisplayValue() );

				if ( sz.cx <= nSmall )
				{
					pNext->m_bFullWidth	= FALSE;
					pNext->m_nHeight	= 18;
				}
				else if ( pos )
				{
					pItem->m_bFullWidth = TRUE;
					m_pItems.GetPrev( pos );
				}
				else
				{
					pItem->m_bFullWidth = TRUE;
					pos = m_pItems.GetTailPosition();
				}
			}
			else
			{
				pItem->m_bFullWidth = TRUE;
			}

			m_nHeight += 20;
		}
		else
		{
			if ( sz.cx > nLarge )
			{
				CRect rcText( 0, 0, nLarge, 0xFFFF );
				Skin.DrawWrappedText( pDC, &rcText, pItem->GetDisplayValue(), NULL, FALSE );
				pItem->m_bFullWidth	= TRUE+TRUE;
				pItem->m_nHeight	= rcText.top + 4;
				m_nHeight += pItem->m_nHeight + 2;
			}
			else
			{
				pItem->m_bFullWidth = TRUE;
				pItem->m_nHeight	= 18;
				m_nHeight += 20;
			}
		}
	}

	pDC->SelectObject( pOld );

	return m_nHeight;
}

//////////////////////////////////////////////////////////////////////
// CMetaList paint  (Was CMetaPanel)

void CMetaList::Paint(CDC* pDC, const CRect* prcArea)
{
	CRect rcWork( prcArea );

	if ( m_bMusicBrainz )
	{
		pDC->FillSolidRect( rcWork.left, rcWork.top, rcWork.Width(), MUSICBRAINZ_HEIGHT + 4, Colors.m_crWindow );

		CDC dcMem;
		dcMem.CreateCompatibleDC( pDC );

		CBitmap* pOld = (CBitmap*)dcMem.SelectObject( &m_bmMusicBrainz );
		pDC->BitBlt( rcWork.left, rcWork.top, MUSICBRAINZ_WIDTH, MUSICBRAINZ_HEIGHT,
			&dcMem, 0, 0, SRCCOPY );
		pDC->ExcludeClipRect( rcWork.left, rcWork.top, rcWork.right, rcWork.top + MUSICBRAINZ_HEIGHT + 4 );

		dcMem.SelectObject( pOld );

		rcWork.top += MUSICBRAINZ_HEIGHT + 4;
	}

	POSITION pos = GetIterator();
	DWORD dwFlags = ( Settings.General.LanguageRTL ? ETO_RTLREADING : 0 );

	for ( int nRow = 0 ; pos ; nRow++ )
	{
		pDC->SetBkColor( Colors.m_crSchemaRow[ nRow & 1 ] );
		int nHeight = 0;

		for ( int nColumn = 0 ; nColumn < 2 && pos ; nColumn++ )
		{
			CMetaItem* pItem = GetNext( pos );

			CRect rcValue( rcWork.left, rcWork.top, rcWork.left, rcWork.top + pItem->m_nHeight );

			if ( pItem->m_bFullWidth )
			{
				if ( nColumn > 0 )
				{
					if ( pos ) m_pItems.GetPrev( pos ); else pos = m_pItems.GetTailPosition();
					break;
				}

				rcValue.right	= rcWork.right;
			}
			else
			{
				rcValue.left	+= nColumn * rcWork.Width() / 2 + 1;
				rcValue.right	+= ( nColumn + 1 ) * rcWork.Width() / 2 - 1;
			}

			CRect rcKey( rcValue.left, rcValue.top, rcValue.left + METAPANEL_KEY_WIDTH, rcValue.bottom );
			rcValue.left = rcKey.right;

			pDC->SetTextColor( Colors.m_crText );
			pDC->SelectObject( &CoolInterface.m_fntBold );

			CString strKey( pItem->m_sKey );
			strKey.TrimRight( L" \x00A0" );
			pDC->ExtTextOut( rcKey.left + 3, rcKey.top + 2, ETO_CLIPPED|ETO_OPAQUE, &rcKey, strKey + ':', NULL );

			if ( pItem->m_bLink )
			{
				pDC->SetTextColor( Colors.m_crTextLink );
				pDC->SelectObject( &CoolInterface.m_fntUnder );
			}
			else
			{
				pDC->SelectObject( &CoolInterface.m_fntNormal );
			}

			if ( pItem->m_bFullWidth == 2 )
			{
				CRect rcText( &rcValue );
				rcText.DeflateRect( 3, 2 );
				Skin.DrawWrappedText( pDC, &rcText, pItem->GetDisplayValue(), NULL, TRUE );
				pDC->ExtTextOut( rcValue.left, rcValue.top, ETO_OPAQUE|dwFlags, &rcValue, NULL, 0, NULL );
				pItem->CopyRect( &rcValue );
			}
			else
			{
				pDC->ExtTextOut( rcValue.left + 3, rcValue.top + 2, ETO_CLIPPED|ETO_OPAQUE|dwFlags,
					&rcValue, pItem->GetDisplayValue(), NULL );

				pItem->CopyRect( &rcValue );

				pItem->right = pItem->left + 6 +
					pDC->GetTextExtent( pItem->GetDisplayValue() ).cx;
			}

			pDC->ExcludeClipRect( &rcKey );
			pDC->ExcludeClipRect( &rcValue );

			nHeight = pItem->m_nHeight;
			if ( pItem->m_bFullWidth ) break;
		}

		rcWork.top += nHeight + 2;
	}
}

//////////////////////////////////////////////////////////////////////
// CMetaList click handler  (Was CMetaListPanel)

BOOL CMetaList::OnClick(const CPoint& point)
{
	if ( CMetaItem* pItem = HitTest( point, TRUE ) )
	{
		CQuickLock oLock( Library.m_pSection );

		if ( CAlbumFolder* pFolder = pItem->GetLinkTarget() )
		{
			CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
			if ( CLibraryWnd* pLibraryWnd = (CLibraryWnd*)pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CLibraryWnd) ) )
			{
				CLibraryFrame* pFrame = &pLibraryWnd->m_wndFrame;
				ASSERT_KINDOF(CLibraryFrame, pFrame );
				pFrame->Display( pFolder );
				return TRUE;
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMetaItem construction

CMetaItem::CMetaItem(CSchemaMember* pMember)
	: CRect()
	, m_pMember 	( pMember )
	, m_bLink		( FALSE )
	, m_bValueDefined ( FALSE )
	, m_sKey		( pMember ? pMember->m_sTitle : L"" )
	, m_bFullWidth	( FALSE )
	, m_nHeight 	( 0 )
{
}

//////////////////////////////////////////////////////////////////////
// CMetaItem combination

BOOL CMetaItem::Combine(const CXMLElement* pXML)
{
	if ( ! m_pMember )
		return FALSE;

	CString strValue = m_pMember->GetValueFrom( pXML, NULL, TRUE );
	strValue.Trim();

	if ( ! m_bValueDefined )
		m_sValue = strValue;
	else if ( m_sValue != strValue )
		m_sValue = LoadString( IDS_MULTIPLE );

	m_bValueDefined = TRUE;
	int nVote = 1;
	if ( m_pVote.Lookup( strValue, nVote ) ) nVote ++;
	m_pVote.SetAt( strValue, nVote );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CMetaItem voting

void CMetaItem::Vote()
{
	if ( m_sValue != LoadString( IDS_MULTIPLE ) ) return;

	int nBest = 0;

	for ( POSITION pos = m_pVote.GetStartPosition() ; pos ; )
	{
		CString strValue;
		int nVote;

		m_pVote.GetNextAssoc( pos, strValue, nVote );

		if ( nVote > nBest )
		{
			nBest = nVote;
			m_sValue = strValue;
		}
	}

	m_pVote.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CMetaItem cleaning

BOOL CMetaItem::Limit(int nMaxLength)
{
	if ( m_sValue.IsEmpty() )
		return FALSE;
	if ( nMaxLength > 0 && m_sValue.GetLength() > nMaxLength )
		m_sValue = m_sValue.Left( nMaxLength ) + _T('\x2026');

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CMetaItem linking

BOOL CMetaItem::CreateLink()
{
	if ( StartsWith( m_sValue, _PT("http://") ) || StartsWith( m_sValue, _PT("www.") ) )
	{
		m_bLink = TRUE;

		int curPos = 0;
		m_sLink = m_sValue.Tokenize( L"|", curPos );
		if ( curPos != -1 )
			m_sLinkName = m_sValue.Tokenize( L"|", curPos );

		return TRUE;
	}

	if ( m_pMember == NULL ) return FALSE;

	if ( m_pMember->m_sLinkURI.IsEmpty() ) return FALSE;
	if ( m_pMember->m_sLinkName.IsEmpty() ) return FALSE;

	{
		CQuickLock oLock( Library.m_pSection );		// ASSUME_LOCKs

		m_bLink = LibraryFolders.GetAlbumTarget( m_pMember->m_sLinkURI, m_pMember->m_sLinkName, m_sValue ) != NULL;
	}

	if ( m_bLink ) m_sLink = m_sValue;

	return m_bLink;
}

//////////////////////////////////////////////////////////////////////
// CMetaItem link targets

CAlbumFolder* CMetaItem::GetLinkTarget(BOOL bHTTP) const
{
	if ( bHTTP )
	{
		if ( StartsWith( m_sLink, _PT("http://") ) )
		{
			ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("open"),
				m_sLink, NULL, NULL, SW_SHOWNORMAL );
			return NULL;
		}
		if ( StartsWith( m_sLink, _PT("www.") ) )
		{
			ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("open"),
				_T("http://") + m_sLink, NULL, NULL, SW_SHOWNORMAL );
			return NULL;
		}
	}

	if ( m_pMember == NULL || ! m_bLink ) return NULL;
	if ( m_pMember->m_sLinkURI.IsEmpty() ) return NULL;
	if ( m_pMember->m_sLinkName.IsEmpty() ) return NULL;

	return LibraryFolders.GetAlbumTarget( m_pMember->m_sLinkURI, m_pMember->m_sLinkName, m_sLink );
}

CString CMetaItem::GetMusicBrainzLink() const
{
	if ( m_pMember == NULL || m_sValue.IsEmpty() )
		return CString();

	if ( m_pMember->m_sName == L"mbalbumid" )
		return L"http://musicbrainz.org/release/" + m_sValue + L".html";
	if ( m_pMember->m_sName == L"mbartistid" )
		return L"http://musicbrainz.org/artist/" + m_sValue + L".html";
	if ( m_pMember->m_sName == L"mbpuid" )
		return L"http://musicbrainz.org/show/puid/?matchesonly=0&amp;puid=" + m_sValue;

	return CString();
}
