//
// RichDocument.cpp
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
#include "Settings.h"
#include "PeerProject.h"
#include "CoolInterface.h"
#include "RichDocument.h"
#include "RichElement.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CRichDocument construction

CRichDocument::CRichDocument()
	: m_nCookie		( 0 )
	, m_szMargin	( 8, 8 )
	, m_crBackground( CoolInterface.m_crRichdocBack )
	, m_crText		( CoolInterface.m_crRichdocText )
	, m_crLink		( CoolInterface.m_crTextLink )
	, m_crHover		( CoolInterface.m_crTextLinkHot )
	, m_crHeading	( CoolInterface.m_crRichdocHeading )
{
	CreateFonts();
}

CRichDocument::~CRichDocument()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CRichDocument element access

POSITION CRichDocument::GetIterator() const
{
	return m_pElements.GetHeadPosition();
}

CRichElement* CRichDocument::GetNext(POSITION& pos) const
{
	return m_pElements.GetNext( pos );
}

CRichElement* CRichDocument::GetPrev(POSITION& pos) const
{
	return m_pElements.GetPrev( pos );
}

INT_PTR CRichDocument::GetCount() const
{
	return m_pElements.GetCount();
}

POSITION CRichDocument::Find(CRichElement* pElement) const
{
	return m_pElements.Find( pElement );
}

//////////////////////////////////////////////////////////////////////
// CRichDocument element modification

CRichElement* CRichDocument::Add(CRichElement* pElement, POSITION posBefore)
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( posBefore )
		m_pElements.InsertBefore( posBefore, pElement );
	else
		m_pElements.AddTail( pElement );

	pElement->m_pDocument = this;
	m_nCookie++;

	return pElement;
}

CRichElement* CRichDocument::Add(int nType, LPCTSTR pszText, LPCTSTR pszLink, DWORD nFlags, int nGroup, POSITION posBefore)
{
	CRichElement* pElement = new CRichElement( nType, pszText, pszLink, nFlags, nGroup );
	return Add( pElement, posBefore );
}

void CRichDocument::Remove(CRichElement* pElement)
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( POSITION pos = m_pElements.Find( pElement ) )
	{
		m_pElements.RemoveAt( pos );
		pElement->m_pDocument = NULL;
		m_nCookie++;
	}
}

void CRichDocument::ShowGroup(int nGroup, BOOL bShow)
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CRichElement* pElement = GetNext( pos );
		if ( pElement->m_nGroup == nGroup ) pElement->Show( bShow );
	}
}

void CRichDocument::ShowGroupRange(int nMin, int nMax, BOOL bShow)
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CRichElement* pElement = GetNext( pos );
		if ( pElement->m_nGroup >= nMin && pElement->m_nGroup <= nMax )
			pElement->Show( bShow );
	}
}

void CRichDocument::SetModified()
{
	m_nCookie++;
}

void CRichDocument::Clear()
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		delete GetNext( pos );
	}

	m_pElements.RemoveAll();
	m_nCookie++;
}

//////////////////////////////////////////////////////////////////////
// CRichDocument font construction

void CRichDocument::CreateFonts(const LOGFONT* lpDefault, const LOGFONT* lpHeading)
{
	CSingleLock pLock( &m_pSection, TRUE );

	LOGFONT lfDefault = {};
	if ( lpDefault )
		CopyMemory( &lfDefault, lpDefault, sizeof( lfDefault ) );
	else
		CoolInterface.m_fntRichDefault.GetLogFont( &lfDefault );

	lfDefault.lfWeight = FW_NORMAL;
	lfDefault.lfItalic = FALSE;
	lfDefault.lfUnderline = FALSE;
	if ( m_fntNormal.m_hObject ) m_fntNormal.DeleteObject();
	m_fntNormal.CreateFontIndirect( &lfDefault );

	lfDefault.lfWeight = FW_BOLD;
	if ( m_fntBold.m_hObject ) m_fntBold.DeleteObject();
	m_fntBold.CreateFontIndirect( &lfDefault );

	lfDefault.lfWeight = FW_NORMAL;
	lfDefault.lfItalic = TRUE;
	if ( m_fntItalic.m_hObject ) m_fntItalic.DeleteObject();
	m_fntItalic.CreateFontIndirect( &lfDefault );

	lfDefault.lfItalic = FALSE;
	lfDefault.lfUnderline = TRUE;
	if ( m_fntUnder.m_hObject ) m_fntUnder.DeleteObject();
	m_fntUnder.CreateFontIndirect( &lfDefault );

	lfDefault.lfWeight = FW_BOLD;
	if ( m_fntBoldUnder.m_hObject ) m_fntBoldUnder.DeleteObject();
	m_fntBoldUnder.CreateFontIndirect( &lfDefault );

	LOGFONT lfHeading = {};
	if ( lpHeading )
		CopyMemory( &lfHeading, lpHeading, sizeof( lfHeading) );
	else
		CoolInterface.m_fntRichHeading.GetLogFont( &lfHeading );

	if ( m_fntHeading.m_hObject ) m_fntHeading.DeleteObject();
	m_fntHeading.CreateFontIndirect( &lfHeading );
}

//////////////////////////////////////////////////////////////////////
// CRichDocument XML Load

BOOL CRichDocument::LoadXML(CXMLElement* pBase, CMap< CString, const CString&, CRichElement*, CRichElement* >* pMap, int nGroup)
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( pBase == NULL ) return FALSE;

	CString strTemp;

	if ( pBase->IsNamed( _T("document") ) )
	{
		strTemp = pBase->GetAttributeValue( _T("fontFace") );
		if ( strTemp.GetLength() )
		{
			// Change font face
			LOGFONT lfDefault = {};
			CoolInterface.m_fntRichDefault.GetLogFont( &lfDefault );
			_tcsncpy( lfDefault.lfFaceName, strTemp, LF_FACESIZE );

			LOGFONT lfHeading = {};
			CoolInterface.m_fntRichHeading.GetLogFont( &lfHeading );
			_tcsncpy( lfHeading.lfFaceName, strTemp, LF_FACESIZE );

			CreateFonts( &lfDefault, &lfHeading );
		}

		m_crBackground	= CoolInterface.m_crRichdocBack;
		m_crText		= CoolInterface.m_crRichdocText;
		m_crLink		= CoolInterface.m_crTextLink;
		m_crHover		= CoolInterface.m_crTextLinkHot;
		m_crHeading		= CoolInterface.m_crRichdocHeading;

		LoadXMLColor( pBase, _T("crBackground"), &m_crBackground );
		LoadXMLColor( pBase, _T("crText"), &m_crText );
		LoadXMLColor( pBase, _T("crLink"), &m_crLink );
		LoadXMLColor( pBase, _T("crHover"), &m_crHover );
		LoadXMLColor( pBase, _T("crHeading"), &m_crHeading );

		strTemp = pBase->GetAttributeValue( _T("leftMargin") );
		if ( strTemp.GetLength() ) _stscanf( strTemp, _T("%i"), &m_szMargin.cx );
		strTemp = pBase->GetAttributeValue( _T("topMargin") );
		if ( strTemp.GetLength() ) _stscanf( strTemp, _T("%i"), &m_szMargin.cy );
	}

	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML		= pBase->GetNextElement( pos );
		CRichElement* pElement	= NULL;

		if ( pXML->IsNamed( _T("text") ) )
		{
			pElement = new CRichElement( retText );
		}
		else if ( pXML->IsNamed( _T("link") ) )
		{
			pElement = new CRichElement( retLink );
		}
		else if ( pXML->IsNamed( _T("heading") ) )
		{
			pElement = new CRichElement( retHeading );
		}
		else if ( pXML->IsNamed( _T("newline") ) )
		{
			pElement = new CRichElement( retNewline );

			strTemp = pXML->GetAttributeValue( _T("gap") );

			if ( strTemp.GetLength() )
			{
				pElement->m_sText = strTemp;
				strTemp = pXML->GetAttributeValue( _T("indent") );
				if ( strTemp.GetLength() ) pElement->m_sText += '.' + strTemp;
			}
			else
			{
				strTemp = pXML->GetAttributeValue( _T("indent") );
				if ( strTemp.GetLength() ) pElement->m_sText = _T("0.") + strTemp;
			}
		}
		else if ( pXML->IsNamed( _T("gap") ) )
		{
			pElement = new CRichElement( retGap );

			strTemp = pXML->GetAttributeValue( _T("size") );
			if ( strTemp ) pElement->m_sText = strTemp;
		}
		else if ( pXML->IsNamed( _T("bitmap") ) )
		{
			pElement = new CRichElement( retBitmap );
		}
		else if ( pXML->IsNamed( _T("icon") ) )
		{
			pElement = new CRichElement( retIcon );
		}
		else if ( pXML->IsNamed( _T("anchor") ) )
		{
			pElement = new CRichElement( retAnchor );
		}
		else if ( pXML->IsNamed( _T("para") ) )
		{
			Add( pElement = new CRichElement( retAlign,
				pXML->GetAttributeValue( _T("align") ) ) );

			if ( pXML->GetElementCount() )
			{
				if ( ! LoadXML( pXML, pMap, nGroup ) ) return FALSE;
				if ( pElement->m_sText.CompareNoCase( _T("left") ) )
				{
					Add( new CRichElement( retAlign, _T("left") ) );
				}
			}

			continue;
		}
		else if ( pXML->IsNamed( _T("group") ) )
		{
			int nSubGroup = 0;
			if ( _stscanf( pXML->GetAttributeValue( _T("id") ), _T("%i"), &nSubGroup ) != 1 )
				return FALSE;
			if ( ! LoadXML( pXML, pMap, nSubGroup ) ) return FALSE;
			continue;
		}
		else if ( pXML->IsNamed( _T("styles") ) )
		{
			if ( ! LoadXMLStyles( pXML ) ) return FALSE;
		}
		else
		{
			return FALSE;
		}

		if ( pElement == NULL ) continue;

		strTemp = pXML->GetValue();
		if ( strTemp.GetLength() ) pElement->m_sText = strTemp;

		pElement->m_nGroup = nGroup;
		strTemp = pXML->GetAttributeValue( _T("group") );
		if ( strTemp.GetLength() ) _stscanf( strTemp, _T("%i"), &pElement->m_nGroup );

		strTemp = pXML->GetAttributeValue( _T("format") );
		ToLower( strTemp );

		if ( strTemp.Find( 'b' ) >= 0 )	pElement->m_nFlags |= retfBold;
		if ( strTemp.Find( 'i' ) >= 0 )	pElement->m_nFlags |= retfItalic;
		if ( strTemp.Find( 'u' ) >= 0 )	pElement->m_nFlags |= retfUnderline;

		strTemp = pXML->GetAttributeValue( _T("align") );
		ToLower( strTemp );

		if ( strTemp == _T("middle") ) pElement->m_nFlags |= retfMiddle;

		strTemp = pXML->GetAttributeValue( _T("color") );
		if ( strTemp.GetLength() == 6 )
		{
			pElement->m_nFlags |= retfColor;
			LoadXMLColor( pXML, _T("color"), &pElement->m_cColor );
		}
		else
		{
			strTemp = pXML->GetAttributeValue( _T("colour") );
			if ( strTemp.GetLength() == 6 )
			{
				pElement->m_nFlags |= retfColor;
				LoadXMLColor( pXML, _T("colour"), &pElement->m_cColor );
			}
		}

		if ( pElement->m_nType == retIcon )
		{
			strTemp = pXML->GetAttributeValue( _T("command") );
			if ( strTemp.GetLength() )
			{
				pElement->m_nType = retCmdIcon;
				pElement->m_sText = strTemp;
			}
		}

		if ( pElement->m_nType == retIcon || pElement->m_nType == retBitmap || pElement->m_nType == retAnchor )
		{
			strTemp = pXML->GetAttributeValue( _T("res") );
			if ( strTemp.GetLength() ) pElement->m_sText = strTemp;
			strTemp = pXML->GetAttributeValue( _T("path") );
			if ( strTemp.GetLength() ) pElement->m_sText = strTemp;

			strTemp = pXML->GetAttributeValue( _T("width") );
			if ( strTemp.GetLength() )
			{
				if ( pElement->m_sText.GetLength() ) pElement->m_sText += '.';
				pElement->m_sText += strTemp;
				strTemp = pXML->GetAttributeValue( _T("height") );
				if ( strTemp.GetLength() ) pElement->m_sText += '.' + strTemp;
			}
		}

		pElement->m_sLink = pXML->GetAttributeValue( _T("target") );

		if ( pMap )
		{
			strTemp = pXML->GetAttributeValue( _T("id") );
			if ( strTemp.GetLength() ) pMap->SetAt( strTemp, pElement );
		}

		Add( pElement );
	}

	return TRUE;
}

BOOL CRichDocument::LoadXMLStyles(CXMLElement* pParent)
{
	for ( POSITION pos = pParent->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pParent->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("style") ) ) continue;

		CString strName = pXML->GetAttributeValue( _T("name") );
		strName.MakeLower();
		bool bDefault = ( strName == _T("default") || strName.IsEmpty() );
		bool bHeading = ( strName == _T("heading") );

		LOGFONT lf = {};
		if ( bDefault )
			CoolInterface.m_fntRichDefault.GetLogFont( &lf );
		else if ( bHeading )
			CoolInterface.m_fntRichHeading.GetLogFont( &lf );

		if ( CXMLElement* pFont = pXML->GetElementByName( _T("font") ) )
		{
			CString strFontFace = pFont->GetAttributeValue( _T("face") );
			if ( ! strFontFace.IsEmpty() )
			{
				_tcsncpy( lf.lfFaceName, strFontFace, LF_FACESIZE );
			}
			CString strSize = pFont->GetAttributeValue( _T("size") );
			if ( ! strSize.IsEmpty() )
			{
				if ( _stscanf( strSize, _T("%i"), &lf.lfHeight ) == 1 )
					lf.lfHeight = - lf.lfHeight;
			}
			CString strWeight = pFont->GetAttributeValue( _T("weight") );
			if ( ! strWeight.IsEmpty() )
			{
				_stscanf( strWeight, _T("%i"), &lf.lfWeight );
			}
		}

		CXMLElement* pColors = pXML->GetElementByName( _T("colors") );
		if ( pColors == NULL ) pColors = pXML->GetElementByName( _T("colours") );
		if ( pColors == NULL ) pColors = pXML;

		if ( strName == _T("default") || strName.IsEmpty() )
		{
			LoadXMLColor( pColors, _T("text"), &m_crText );
			LoadXMLColor( pColors, _T("link"), &m_crLink );
			LoadXMLColor( pColors, _T("hover"), &m_crHover );

			// Create specified fonts (using default font as heading font)
			CreateFonts( &lf, NULL );
		}
		else if ( bHeading )
		{
			LoadXMLColor( pColors, _T("text"), &m_crHeading );

			// Create heading font
			if ( m_fntHeading.m_hObject ) m_fntHeading.DeleteObject();
			m_fntHeading.CreateFontIndirect( &lf );
		}
	}

	return TRUE;
}

BOOL CRichDocument::LoadXMLColor(CXMLElement* pXML, LPCTSTR pszName, COLORREF* pColor)
{
	CString str = pXML->GetAttributeValue( pszName );
	if ( str.GetLength() != 6 ) return FALSE;

	int nRed, nGreen, nBlue;

	if ( _stscanf( str.Mid( 0, 2 ), _T("%x"), &nRed ) != 1 ) return FALSE;
	if ( _stscanf( str.Mid( 2, 2 ), _T("%x"), &nGreen ) != 1 ) return FALSE;
	if ( _stscanf( str.Mid( 4, 2 ), _T("%x"), &nBlue ) != 1 ) return FALSE;

	*pColor = RGB( nRed, nGreen, nBlue );

	return TRUE;
}
