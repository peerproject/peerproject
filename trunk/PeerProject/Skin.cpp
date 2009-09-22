//
// Skin.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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
#include "CoolMenu.h"
#include "CtrlCoolBar.h"
#include "Skin.h"
#include "SkinWindow.h"
#include "ImageServices.h"
#include "ImageFile.h"
#include "Plugins.h"
#include "XML.h"
#include "WndChild.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSkin Skin;


//////////////////////////////////////////////////////////////////////
// CSkin construction

CSkin::CSkin()
{
	// experimental values
	m_pStrings.InitHashTable( 1531 );
	m_pMenus.InitHashTable( 83 );
	m_pToolbars.InitHashTable( 61 );
	m_pDocuments.InitHashTable( 61 );
	m_pWatermarks.InitHashTable( 31 );
	m_pLists.InitHashTable( 31 );
	m_pDialogs.InitHashTable( 127 );

	CreateDefaultColors();

	m_brDialog.CreateSolidBrush( m_crDialog );
}

CSkin::~CSkin()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CSkin apply

void CSkin::Apply()
{
	Clear();

	Settings.General.Language = _T("en");

	CreateDefault();

	ApplyRecursive( L"Languages\\" );
	ApplyRecursive( NULL );

	m_brDialog.CreateSolidBrush( m_crDialog );

	if ( HBITMAP hPanelMark = GetWatermark( _T("CPanelWnd.Caption") ) )
		m_bmPanelMark.Attach( hPanelMark );
	else if ( m_crPanelBack == RGB( 60, 60, 60 ) )
		m_bmPanelMark.LoadBitmap( IDB_PANEL_MARK );

	CoolMenu.SetWatermark( GetWatermark( _T("CCoolMenu") ) );

	Plugins.OnSkinChanged();
}

//////////////////////////////////////////////////////////////////////
// CSkin default skin

void CSkin::CreateDefault()
{
	CreateDefaultColors();

	CoolInterface.CreateFonts();

	// Default Options

	m_nToolbarHeight = 28;
	m_nSidebarWidth = 200;
	m_bMenuBorders = TRUE;
	m_bMenuGripper = TRUE;
	m_bDropMenu = FALSE;
	m_rcNavBarOffset = CRect( 0, 0, 0, 0 );

	// Command Icons

	HICON hIcon = theApp.LoadIcon( IDI_CHECKMARK );
	if ( hIcon )
	{
		if ( Settings.General.LanguageRTL ) hIcon = CreateMirroredIcon( hIcon );
		CoolInterface.AddIcon( ID_CHECKMARK, hIcon );
		VERIFY( DestroyIcon( hIcon ) );
	}

	// Load Definitions

	LoadFromResource( NULL, IDR_XML_DEFINITIONS );
	LoadFromResource( NULL, IDR_XML_DEFAULT );

	// Copying
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_GUIDE );
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_UPDATE );

	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_1 );
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_2 );
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_3 );
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_4 );
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_5 );
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_6 );

	// Plugins
	Plugins.RegisterCommands();
	Plugins.InsertCommands();
}

void CSkin::CreateDefaultColors()
{
	CoolInterface.CalculateColors( FALSE );

	// Colour Scheme

	m_crDialog					= CoolInterface.CalculateColor(
		GetSysColor( COLOR_BTNFACE ), GetSysColor( COLOR_WINDOW ), 200 );

	m_crPanelBack				= RGB( 60, 60, 60 );
	m_crPanelText				= RGB( 255, 255, 255 );
	m_crPanelBorder				= RGB( 0, 0, 0 );

	m_crBannerBack				= RGB( 122, 161, 230 );
	m_crBannerText				= RGB( 250, 250, 255 );

	m_crSchemaRow[0]			= RGB( 245, 245, 255 );
	m_crSchemaRow[1]			= RGB( 214, 223, 247 );

	// NavBar

	m_crNavBarText				= CLR_NONE;
	m_crNavBarTextUp			= m_crNavBarText;
	m_crNavBarTextDown			= m_crNavBarText;
	m_crNavBarTextHover			= m_crNavBarText;
	m_crNavBarTextChecked		= m_crNavBarText;
	m_crNavBarShadow			= CLR_NONE;
	m_crNavBarShadowUp			= m_crNavBarShadow;
	m_crNavBarShadowDown		= m_crNavBarShadow;
	m_crNavBarShadowHover		= m_crNavBarShadow;
	m_crNavBarShadowChecked		= m_crNavBarShadow;
	m_crNavBarOutline			= CLR_NONE;
	m_crNavBarOutlineUp			= m_crNavBarOutline;
	m_crNavBarOutlineDown		= m_crNavBarOutline;
	m_crNavBarOutlineHover		= m_crNavBarOutline;
	m_crNavBarOutlineChecked	= m_crNavBarOutline;
}

//////////////////////////////////////////////////////////////////////
// CSkin clear

void CSkin::Clear()
{
	CString strName;
	POSITION pos;

	for ( pos = m_pMenus.GetStartPosition() ; pos ; )
	{
		CMenu* pMenu;
		m_pMenus.GetNextAssoc( pos, strName, pMenu );
		delete pMenu;
	}

	for ( pos = m_pToolbars.GetStartPosition() ; pos ; )
	{
		CCoolBarCtrl* pBar;
		m_pToolbars.GetNextAssoc( pos, strName, pBar );
		delete pBar;
	}

	for ( pos = m_pDialogs.GetStartPosition() ; pos ; )
	{
		CXMLElement* pXML;
		m_pDialogs.GetNextAssoc( pos, strName, pXML );
		delete pXML;
	}

	for ( pos = m_pDocuments.GetStartPosition() ; pos ; )
	{
		CXMLElement* pXML;
		m_pDocuments.GetNextAssoc( pos, strName, pXML );
		delete pXML;
	}

	for ( pos = m_pSkins.GetHeadPosition() ; pos ; )
	{
		delete m_pSkins.GetNext( pos );
	}

	for ( pos = m_pFontPaths.GetHeadPosition() ; pos ; )
	{
		RemoveFontResourceEx( m_pFontPaths.GetNext( pos ), FR_PRIVATE, NULL );
	}

	m_pStrings.RemoveAll();
	m_pControlTips.RemoveAll();
	m_pMenus.RemoveAll();
	m_pToolbars.RemoveAll();
	m_pDocuments.RemoveAll();
	m_pWatermarks.RemoveAll();
	m_pLists.RemoveAll();
	m_pDialogs.RemoveAll();
	m_pSkins.RemoveAll();
	m_pFontPaths.RemoveAll();
	m_pImages.RemoveAll();

	if ( m_brDialog.m_hObject != NULL ) m_brDialog.DeleteObject();
	if ( m_bmPanelMark.m_hObject != NULL ) m_bmPanelMark.DeleteObject();

	CoolInterface.Clear();
}

//////////////////////////////////////////////////////////////////////
// CSkin caption selector

BOOL CSkin::SelectCaption(CWnd* pWnd, int nIndex)
{
	CString strCaption;
	pWnd->GetWindowText( strCaption );

	if ( SelectCaption( strCaption, nIndex ) )
	{
		pWnd->SetWindowText( strCaption );
		return TRUE;
	}

	return FALSE;
}

BOOL CSkin::SelectCaption(CString& strCaption, int nIndex)
{
	for ( strCaption += '|' ; ; nIndex-- )
	{
		CString strSection = strCaption.SpanExcluding( _T("|") );
		strCaption = strCaption.Mid( strSection.GetLength() + 1 );
		if ( strSection.IsEmpty() ) break;

		if ( nIndex <= 0 )
		{
			strCaption = strSection;
			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CSkin recursive folder applicator

void CSkin::ApplyRecursive(LPCTSTR pszPath)
{
	WIN32_FIND_DATA pFind;
	HANDLE hSearch;

	CString strPath;
	strPath.Format( _T("%s\\Skins\\%s*.*"), (LPCTSTR)Settings.General.Path,
		pszPath ? pszPath : _T("") );

	hSearch = FindFirstFile( strPath, &pFind );

	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.cFileName[0] == '.' ) continue;

			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( pszPath == NULL && _tcsicmp( pFind.cFileName, L"languages" ) ||
					 pszPath != NULL && _tcsistr( pszPath, L"languages" ) == NULL )
				{
					strPath.Format( L"%s%s\\", pszPath ? pszPath : L"", pFind.cFileName );
					ApplyRecursive( strPath );
				}
			}
			else if (	_tcsistr( pFind.cFileName, L".xml" ) != NULL &&
						_tcsicmp( pFind.cFileName, L"Definitions.xml" ) )
			{
				strPath.Format( L"%s%s", pszPath ? pszPath : L"", pFind.cFileName );

				if ( theApp.GetProfileInt( L"Skins", strPath, FALSE ) )
					LoadFromFile( Settings.General.Path + L"\\Skins\\" + strPath );
			}
		}
		while ( FindNextFile( hSearch, &pFind ) );

		FindClose( hSearch );
	}
}

//////////////////////////////////////////////////////////////////////
// CSkin root load

BOOL CSkin::LoadFromFile(LPCTSTR pszFile)
{
	TRACE( _T("Loading skin file: %s\n"), pszFile );

	CXMLElement* pXML = CXMLElement::FromFile( pszFile );
	if ( pXML == NULL ) return FALSE;

	CString strPath = pszFile;
	int nSlash = strPath.ReverseFind( '\\' );
	if ( nSlash >= 0 ) strPath = strPath.Left( nSlash + 1 );

	BOOL bResult = LoadFromXML( pXML, strPath );

	delete pXML;

	return bResult;
}

BOOL CSkin::LoadFromResource(HINSTANCE hInstance, UINT nResourceID)
{
	HMODULE hModule = ( hInstance != NULL ) ? hInstance : GetModuleHandle( NULL );
	CString strBody( ::LoadHTML( hModule, nResourceID ) );
	CString strPath;
	strPath.Format( _T("%p$"), hModule );
	return LoadFromString( strBody, strPath );
}

BOOL CSkin::LoadFromString(const CString& strXML, const CString& strPath)
{
	CXMLElement* pXML = CXMLElement::FromString( strXML, TRUE );
	if ( pXML == NULL ) return FALSE;

	BOOL bSuccess = LoadFromXML( pXML, strPath );

	delete pXML;
	return bSuccess;
}

BOOL CSkin::LoadFromXML(CXMLElement* pXML, const CString& strPath)
{
	if ( ! pXML->IsNamed( _T("skin") ) )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("Unknown [skin] element"), pXML->ToString() );
		return FALSE;
	}

	BOOL bSuccess = FALSE;

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSub = pXML->GetNextElement( pos );
		bSuccess = FALSE;

		if ( pSub->IsNamed( _T("commandImages") ) )
		{
			if ( ! LoadCommandImages( pSub, strPath ) ) break;
		}
		else if ( pSub->IsNamed( _T("watermarks" ) ) )
		{
			if ( ! LoadWatermarks( pSub, strPath ) ) break;
		}
		else if ( pSub->IsNamed( _T("windowSkins" ) ) )
		{
			if ( ! LoadWindowSkins( pSub, strPath ) ) break;
		}
		else if ( pSub->IsNamed( _T("colorScheme") ) || pSub->IsNamed( _T("colourScheme") ) )
		{
			if ( ! LoadColorScheme( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("menus") ) )
		{
			if ( ! LoadMenus( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("toolbars") ) )
		{
			if ( ! LoadToolbars( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("dialogs") ) )
		{
			if ( ! LoadDialogs( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("strings") ) || pSub->IsNamed( _T("commandTips") ) )
		{
			if ( ! LoadStrings( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("controltips") ) )
		{
			if ( ! LoadControlTips( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("commandMap") ) || pSub->IsNamed( _T("tipMap") ) ||
				  pSub->IsNamed( _T("resourceMap") ) )
		{
			if ( ! LoadResourceMap( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("listColumns") ) )
		{
			if ( ! LoadListColumns( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("fonts" ) ) )
		{
			if ( ! LoadFonts( pSub, strPath ) ) break;
		}
		else if ( pSub->IsNamed( _T("documents" ) ) )
		{
			if ( ! LoadDocuments( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("options") ) )
		{
			if ( ! LoadOptions( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("navbar") ) )
		{
			if ( ! LoadNavBar( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("manifest") ) )
		{
			CString strType = pSub->GetAttributeValue( _T("type") );
			ToLower( strType );

			if ( strType == _T("language") )
			{
				Settings.General.Language = pSub->GetAttributeValue( _T("language"), _T("en") );
				Settings.General.LanguageRTL = ( pSub->GetAttributeValue( _T("dir"), _T("ltr") ) == "rtl" );
				TRACE( _T("Loading language: %s\r\n"), Settings.General.Language );
				TRACE( _T("RTL: %d\r\n"), Settings.General.LanguageRTL );
			}
			else if ( strType == _T("skin") )
			{
				CString strSkinName = pSub->GetAttributeValue( _T("name"), _T("") );
				theApp.Message( MSG_NOTICE, IDS_SKIN_LOAD, strSkinName );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Unknown [type] attribute in [manifest] element"), pSub->ToString() );
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [skin] element"), pSub->ToString() );
		}

		bSuccess = TRUE;
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CSkin strings

void CSkin::AddString(const CString& strString, UINT nStringID)
{
	m_pStrings.SetAt( nStringID, strString );
}

BOOL CSkin::LoadString(CString& str, UINT nStringID) const
{
	if ( nStringID < 10 )
		// Popup menus
		return FALSE;

	if ( m_pStrings.Lookup( nStringID, str ) ||
		( IS_INTRESOURCE( nStringID ) && str.LoadString( nStringID ) ) )
		return TRUE;

	HWND hWnd = (HWND)UIntToPtr( nStringID );
	if ( IsWindow( hWnd ) )
	{
		CWnd::FromHandle( hWnd )->GetWindowText( str );
		return TRUE;
	}

#ifdef _DEBUG
	theApp.Message( MSG_ERROR, _T("Failed to load string %d."), nStringID );
#endif // _DEBUG

	str.Empty();
	return FALSE;
}

BOOL CSkin::LoadStrings(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("string") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CString strValue = pXML->GetAttributeValue( _T("value") );

				for (;;)
				{
					int nPos = strValue.Find( _T("\\n") );
					if ( nPos < 0 ) break;
					strValue = strValue.Left( nPos ) + _T("\n") + strValue.Mid( nPos + 2 );
				}

				// Hack for I64 compliance

				if ( nID == IDS_DOWNLOAD_FRAGMENT_REQUEST || nID == IDS_DOWNLOAD_USEFUL_RANGE ||
					 nID == IDS_UPLOAD_CONTENT || nID == IDS_UPLOAD_PARTIAL_CONTENT ||
					 nID == IDS_DOWNLOAD_VERIFY_DROP )
				{
					strValue.Replace( _T("%lu"), _T("%I64i") );
				}

				m_pStrings.SetAt( nID, strValue );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Unknown [id] attribute in [string] element"), pXML->ToString() );
			}
		}
		else if ( pXML->IsNamed( _T("tip") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CString strMessage = pXML->GetAttributeValue( _T("message") );
				CString strTip = pXML->GetAttributeValue( _T("tip") );
				if ( strTip.GetLength() ) strMessage += '\n' + strTip;
				m_pStrings.SetAt( nID, strMessage );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Unknown [id] attribute in [tip] element"), pXML->ToString() );
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [strings] element"), pXML->ToString() );
		}
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CSkin custom options

BOOL CSkin::LoadOptions(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );


		if ( pXML->IsNamed( _T("option") ) )
		{
			CString strName		= pXML->GetAttributeValue( _T("name") );
			CString strValue	= pXML->GetAttributeValue( _T("value") );
			CString strHeight	= pXML->GetAttributeValue( _T("height") );
			CString strWidth	= pXML->GetAttributeValue( _T("width") );

			if ( strName.CompareNoCase( _T("navbar") ) == 0 )
			{
				if ( ! LoadNavBar( pXML ) )
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Skin Option [Navbar] Failed"), pXML->ToString() );
			}
			else if ( strName.CompareNoCase( _T("dropmenu") ) == 0 )
			{
				if ( strValue.CompareNoCase( _T("false") ) == 0 )
					m_bDropMenu = FALSE;
				else if ( strValue.CompareNoCase( _T("true") ) == 0 )
					m_bDropMenu = TRUE;
			}
			else if ( strName.CompareNoCase( _T("menuborders") ) == 0 ||  strName.CompareNoCase( _T("menubevel") ) == 0 )
			{
				if ( strValue.CompareNoCase( _T("false") ) == 0 )
					m_bMenuBorders = FALSE;
				else if ( strValue.CompareNoCase( _T("true") ) == 0 )
					m_bMenuBorders = TRUE;
			}
			else if ( strName.CompareNoCase( _T("menugripper") ) == 0 )
			{
				if ( strValue.CompareNoCase( _T("false") ) == 0 )
					m_bMenuGripper = FALSE;
				else if ( strValue.CompareNoCase( _T("true") ) == 0 )
					m_bMenuGripper = TRUE;
			}
			else if ( strName.CompareNoCase( _T("toolbar") ) == 0 || strName.CompareNoCase( _T("toolbars") ) == 0 )
			{
				if ( strValue.GetLength() )
					m_nToolbarHeight = _wtoi(strValue);
				else if ( strHeight.GetLength() )
					m_nToolbarHeight = _wtoi(strHeight);
			}
			else if ( strName.CompareNoCase( _T("sidebar") ) == 0 || strName.CompareNoCase( _T("sidepanel") ) == 0 )
			{
				if ( strValue.GetLength() )
					m_nSidebarWidth = _wtoi(strValue);
				else if ( strWidth.GetLength() )
					m_nSidebarWidth = _wtoi(strWidth);
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in skin [Options]"), pXML->ToString() );
		}
	}

	return TRUE;
}

BOOL CSkin::LoadNavBar(CXMLElement* pBase)
{
	CString strValue = pBase->GetAttributeValue( _T("offset") );
	if ( strValue.GetLength() )
	{
		_stscanf( strValue, _T("%i,%i"),
			&m_rcNavBarOffset.left, &m_rcNavBarOffset.top );
	}

	strValue = pBase->GetAttributeValue( _T("mode") );
	if ( ! strValue.GetLength() )
		strValue = pBase->GetAttributeValue( _T("case") );

	if ( strValue.CompareNoCase( _T("upper") ) == 0 )
		m_NavBarMode = NavBarUpper;
	else if ( strValue.CompareNoCase( _T("lower") ) == 0 )
		m_NavBarMode = NavBarLower;
	else
		m_NavBarMode = NavBarNormal;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CSkin dialog control tips

BOOL CSkin::LoadControlTip(CString& str, UINT nCtrlID)
{
	if ( m_pControlTips.Lookup( nCtrlID, str ) ) return TRUE;
	str.Empty();
	return FALSE;
}

BOOL CSkin::LoadControlTips(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("tip") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CString strMessage = pXML->GetAttributeValue( _T("message") );
				strMessage.Replace( _T("{n}"), _T("\r\n") );
				m_pControlTips.SetAt( nID, strMessage );
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin menus

CMenu* CSkin::GetMenu(LPCTSTR pszName) const
{
	ASSERT( Settings.General.GUIMode == GUI_WINDOWED ||
		Settings.General.GUIMode == GUI_TABBED ||
		Settings.General.GUIMode == GUI_BASIC );
	ASSERT( pszName != NULL );
	CString strName( pszName );
	CMenu* pMenu = NULL;
	for ( int nModeTry = 0 ;
		m_pszModeSuffix[ Settings.General.GUIMode ][ nModeTry ] ; nModeTry++ )
	{
		if ( m_pMenus.Lookup( strName +
			m_pszModeSuffix[ Settings.General.GUIMode ][ nModeTry ], pMenu ) )
		{
			break;
		}
	}
	ASSERT_VALID( pMenu );
	ASSERT( pMenu->GetMenuItemCount() > 0 );
	return pMenu;
}

BOOL CSkin::LoadMenus(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		if ( pXML->IsNamed( _T("menu") ) )
		{
			if ( ! LoadMenu( pXML ) )
				return FALSE;
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [menu] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

BOOL CSkin::LoadMenu(CXMLElement* pXML)
{
	CString strName = pXML->GetAttributeValue( _T("name") );
	if ( strName.IsEmpty() )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("No [name] attribute in [menu] element"), pXML->ToString() );
		return FALSE;
	}

	CMenu* pOldMenu = NULL;
	if ( m_pMenus.Lookup( strName, pOldMenu ) )
	{
		ASSERT_VALID( pOldMenu );
		delete pOldMenu;
		m_pMenus.RemoveKey( strName );
	}

	auto_ptr< CMenu > pMenu( new CMenu() );
	ASSERT_VALID( pMenu.get() );
	if ( ! pMenu.get() )
		return FALSE;

	if ( pXML->GetAttributeValue( _T("type"), _T("popup") ).CompareNoCase( _T("bar") ) == 0 )
	{
		if( ! pMenu->CreateMenu() )
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Cannot create menu"), pXML->ToString() );
			return FALSE;
		}
	}
	else if( ! pMenu->CreatePopupMenu() )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("Cannot create popup menu"), pXML->ToString() );
		return FALSE;
	}

	if ( ! CreateMenu( pXML, pMenu->GetSafeHmenu() ) )
		return FALSE;

	m_pMenus.SetAt( strName, pMenu.release() );

	return TRUE;
}

CMenu* CSkin::CreatePopupMenu(LPCTSTR pszName)
{
	CMenu* pMenu = new CMenu();
	if ( pMenu )
	{
		if ( pMenu->CreatePopupMenu() )
		{
			m_pMenus.SetAt( pszName, pMenu );
			return pMenu;
		}
		delete pMenu;
	}
	return NULL;
}

BOOL CSkin::CreateMenu(CXMLElement* pRoot, HMENU hMenu)
{
	for ( POSITION pos = pRoot->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML	= pRoot->GetNextElement( pos );
		CString strText		= pXML->GetAttributeValue( _T("text") );

		int nAmp = strText.Find( '_' );
		if ( nAmp >= 0 ) strText.SetAt( nAmp, '&' );

		if ( pXML->IsNamed( _T("item") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CString strKeys = pXML->GetAttributeValue( _T("shortcut") );

				if ( strKeys.GetLength() ) strText += '\t' + strKeys;

				VERIFY( AppendMenu( hMenu, MF_STRING, nID, strText ) );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Unknown [id] attribute in menu [item] element"), pXML->ToString() );
			}
		}
		else if ( pXML->IsNamed( _T("menu") ) )
		{
			HMENU hSubMenu = ::CreatePopupMenu();
			ASSERT( hSubMenu );
			if ( ! CreateMenu( pXML, hSubMenu ) )
			{
				DestroyMenu( hSubMenu );
				return FALSE;
			}

			VERIFY( AppendMenu( hMenu, MF_STRING|MF_POPUP, (UINT_PTR)hSubMenu, strText ) );
		}
		else if ( pXML->IsNamed( _T("separator") ) )
		{
			VERIFY( AppendMenu( hMenu, MF_SEPARATOR, ID_SEPARATOR, NULL ) );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [menu] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin toolbars

BOOL CSkin::CreateToolBar(LPCTSTR pszName, CCoolBarCtrl* pBar)
{
	if ( pszName == NULL ) return FALSE;

	if (pBar->m_hWnd)
	for ( CWnd* pChild = pBar->GetWindow( GW_CHILD ) ; pChild ; pChild = pChild->GetNextWindow() )
	{
		pChild->ShowWindow( SW_HIDE );
	}

	CString strPath = pszName;
	strPath += _T(".Toolbar");

	if ( m_pWatermarks.Lookup( strPath, strPath ) )
	{
		HBITMAP hBitmap = LoadBitmap( strPath );
		pBar->SetWatermark( hBitmap );
	}
	else
	{
		pBar->SetWatermark( NULL );
	}

	pBar->Clear();

	ASSERT( Settings.General.GUIMode == GUI_WINDOWED ||
		Settings.General.GUIMode == GUI_TABBED ||
		Settings.General.GUIMode == GUI_BASIC );
	LPCTSTR* pszModeSuffix = m_pszModeSuffix[ Settings.General.GUIMode ];
	CCoolBarCtrl* pBase = NULL;
	CString strName( pszName );

	for ( int nModeTry = 0 ; nModeTry < 3 && pszModeSuffix[ nModeTry ] ; nModeTry++ )
	{
		if ( m_pToolbars.Lookup( strName + pszModeSuffix[ nModeTry ], pBase ) ) break;
	}

	if ( pBase != NULL )
	{
		pBar->Copy( pBase );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CCoolBarCtrl* CSkin::GetToolBar(LPCTSTR pszName) const
{
	ASSERT( Settings.General.GUIMode == GUI_WINDOWED ||
		Settings.General.GUIMode == GUI_TABBED ||
		Settings.General.GUIMode == GUI_BASIC );

	LPCTSTR* pszModeSuffix = m_pszModeSuffix[ Settings.General.GUIMode ];
	CCoolBarCtrl* pBar = NULL;
	CString strName( pszName );

	for ( int nModeTry = 0 ; nModeTry < 3 && pszModeSuffix[ nModeTry ] ; nModeTry++ )
	{
		if ( m_pToolbars.Lookup( strName + pszModeSuffix[ nModeTry ], pBar ) )
			return pBar;
	}

	return NULL;
}

BOOL CSkin::LoadToolbars(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("toolbar") ) )
		{
			if ( ! CreateToolBar( pXML ) )
				return FALSE;
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [toolbars] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

CCoolBarCtrl* CSkin::CreateToolBar(LPCTSTR pszName)
{
	CCoolBarCtrl* pBar = new CCoolBarCtrl();
	if ( pBar )
	{
		m_pToolbars.SetAt( pszName, pBar );
		return pBar;
	}
	return NULL;
}

BOOL CSkin::CreateToolBar(CXMLElement* pBase)
{
	CCoolBarCtrl* pBar = new CCoolBarCtrl();

	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("button") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CCoolBarItem* pItem = pBar->Add( nID, pXML->GetAttributeValue( _T("text") ) );
				CString strTemp = pXML->GetAttributeValue( _T("color") );
				if ( ! strTemp )
					strTemp = pXML->GetAttributeValue( _T("colour") );

				if ( strTemp.GetLength() == 6 )
				{
					int nRed, nGreen, nBlue;
					_stscanf( strTemp.Mid( 0, 2 ), _T("%x"), &nRed );
					_stscanf( strTemp.Mid( 2, 2 ), _T("%x"), &nGreen );
					_stscanf( strTemp.Mid( 4, 2 ), _T("%x"), &nBlue );
					pItem->m_crText = RGB( nRed, nGreen, nBlue );
				}

				strTemp = pXML->GetAttributeValue( L"tip" );
				if ( strTemp.GetLength() )
					pItem->SetTip( strTemp );

				strTemp = pXML->GetAttributeValue( L"visible", L"true" );
				if ( strTemp.CompareNoCase( L"false" ) == 0 )
					pItem->Show( FALSE );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Unknown [id] attribute in [button] element"), pXML->ToString() );
			}
		}
		else if ( pXML->IsNamed( _T("separator") ) )
		{
			pBar->Add( ID_SEPARATOR );
		}
		else if ( pXML->IsNamed( _T("rightalign") ) )
		{
			pBar->Add( UINT( ID_RIGHTALIGN ) );
		}
		else if ( pXML->IsNamed( _T("control") ) )
		{
			UINT nWidth, nHeight = 0;
			CString strTemp;

			UINT nID = LookupCommandID( pXML );
			if ( nID )
			{
				strTemp = pXML->GetAttributeValue( _T("width") );
				CCoolBarItem* pItem = NULL;

				if ( _stscanf( strTemp, _T("%lu"), &nWidth ) == 1 )
				{
					strTemp = pXML->GetAttributeValue( _T("height") );
					_stscanf( strTemp, _T("%lu"), &nHeight );
					pItem = pBar->Add( nID, nWidth, nHeight );
				}

				if ( pItem )
				{
					strTemp = pXML->GetAttributeValue( L"checked", L"false" );

					if ( strTemp.CompareNoCase( L"true" ) == 0 )
					{
						pItem->m_bCheckButton = TRUE;
						pItem->m_bEnabled = FALSE;
					}

					strTemp = pXML->GetAttributeValue( _T("text") );
					pItem->SetText( strTemp );
				}
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Unknown [id] attribute in [control] element"), pXML->ToString() );
			}

		}
		else if ( pXML->IsNamed( _T("label") ) )
		{
			CCoolBarItem* pItem = pBar->Add( 1, pXML->GetAttributeValue( _T("text") ) );
			pItem->m_crText = 0;
			pItem->SetTip( pXML->GetAttributeValue( _T("tip") ) );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [toolbar] element"), pXML->ToString() );
		}
	}

	CString strName = pBase->GetAttributeValue( _T("name") );

	CCoolBarCtrl* pOld = NULL;
	if ( m_pToolbars.Lookup( strName, pOld ) && pOld ) delete pOld;

	m_pToolbars.SetAt( strName, pBar );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin documents

BOOL CSkin::LoadDocuments(CXMLElement* pBase)
{
	for ( POSITION posDoc = pBase->GetElementIterator() ; posDoc ; )
	{
		CXMLElement* pDoc = pBase->GetNextElement( posDoc );

		if ( pDoc->IsNamed( _T("document") ) )
		{
			CString strName = pDoc->GetAttributeValue( _T("name") );

			CXMLElement* pOld = NULL;
			if ( m_pDocuments.Lookup( strName, pOld ) ) delete pOld;

			m_pDocuments.SetAt( strName, pDoc->Detach() );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [documents] element"), pDoc->ToString() );
		}
	}

	return TRUE;
}

CXMLElement* CSkin::GetDocument(LPCTSTR pszName)
{
	CXMLElement* pXML = NULL;

	if ( m_pDocuments.Lookup( pszName, pXML ) ) return pXML;

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CSkin watermarks

HBITMAP CSkin::GetWatermark(LPCTSTR pszName)
{
	CString strPath;
	if ( m_pWatermarks.Lookup( pszName, strPath ) ) return LoadBitmap( strPath );
	return NULL;
}

BOOL CSkin::GetWatermark(CBitmap* pBitmap, LPCTSTR pszName)
{
	ASSERT( pBitmap != NULL );
	if ( pBitmap->m_hObject != NULL ) pBitmap->DeleteObject();
	HBITMAP hBitmap = GetWatermark( pszName );
	if ( hBitmap != NULL ) pBitmap->Attach( hBitmap );
	return ( hBitmap != NULL );
}

BOOL CSkin::LoadWatermarks(CXMLElement* pSub, const CString& strPath)
{
	for ( POSITION posMark = pSub->GetElementIterator() ; posMark ; )
	{
		CXMLElement* pMark = pSub->GetNextElement( posMark );

		if ( pMark->IsNamed( _T("watermark") ) )
		{
			CString strName	= pMark->GetAttributeValue( _T("target") );
			CString strFile	= pMark->GetAttributeValue( _T("path") );

			if ( strName.GetLength() && strFile.GetLength() )
			{
				strFile = strPath + strFile;
				m_pWatermarks.SetAt( strName, strFile );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Missed [target] and/or [path] attributes in [watermark] element"), pMark->ToString() );
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [watermarks] element"), pMark->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin list column translations

BOOL CSkin::Translate(LPCTSTR pszName, CHeaderCtrl* pCtrl)
{
	CString strEdit;

	if ( ! m_pLists.Lookup( pszName, strEdit ) ) return FALSE;

	TCHAR szColumn[128] = {};
	HD_ITEM pColumn = {};

	if ( Settings.General.LanguageRTL )
		pCtrl->ModifyStyleEx( 0, WS_EX_LAYOUTRTL, 0 );
	pColumn.mask		= HDI_TEXT;
	pColumn.pszText		= szColumn;
	pColumn.cchTextMax	= 126;

	for ( int nItem = 0 ; nItem < pCtrl->GetItemCount() ; nItem++ )
	{
		*szColumn = _T('\0');
		pCtrl->GetItem( nItem, &pColumn );

		_tcscat( szColumn, _T("=") );

		LPCTSTR pszFind = _tcsistr( strEdit, szColumn );

		if ( pszFind )
		{
			pszFind += _tcslen( szColumn );

			CString strNew = pszFind;
			strNew = strNew.SpanExcluding( _T("|") );

			_tcsncpy( szColumn, strNew, _countof( szColumn ) );
			pCtrl->SetItem( nItem, &pColumn );
		}
	}

	return TRUE;
}

CString CSkin::GetHeaderTranslation(LPCTSTR pszClassName, LPCTSTR pszHeaderName)
{
	CString strEdit;
	if ( ! m_pLists.Lookup( pszClassName, strEdit ) )
		return CString( pszHeaderName );

	CString strOriginal( pszHeaderName );
	strOriginal += L"=";
	LPCTSTR pszFind = _tcsistr( strEdit, strOriginal );

	if ( pszFind )
	{
		pszFind += strOriginal.GetLength();
		CString strNew = pszFind;
		strNew = strNew.SpanExcluding( _T("|") );
		return strNew;
	}
	return CString( pszHeaderName );
}

BOOL CSkin::LoadListColumns(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("list") ) )
		{
			CString strName = pXML->GetAttributeValue( _T("name") );
			if ( strName.IsEmpty() ) continue;

			CString strEdit;
			for ( POSITION posCol = pXML->GetElementIterator() ; posCol ; )
			{
				CXMLElement* pCol = pXML->GetNextElement( posCol );
				if ( pCol->IsNamed( _T("column") ) )
				{
					CString strFrom	= pCol->GetAttributeValue( _T("from") );
					CString strTo	= pCol->GetAttributeValue( _T("to") );

					if ( strFrom.IsEmpty() || strTo.IsEmpty() ) continue;

					if ( strEdit.GetLength() ) strEdit += '|';
					strEdit += strFrom;
					strEdit += '=';
					strEdit += strTo;
				}
				else
				{
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
						_T("Unknown element in [list] element"), pCol->ToString() );
				}
			}

			m_pLists.SetAt( strName, strEdit );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [listColumns] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin dialogs

BOOL CSkin::Apply(LPCTSTR pszName, CDialog* pDialog, UINT nIconID, CToolTipCtrl* pWndTooltips)
{
	if ( nIconID )
		CoolInterface.SetIcon( nIconID, FALSE, FALSE, pDialog );

	CString strName;

	if ( pszName == NULL )
		strName = pDialog->GetRuntimeClass()->m_lpszClassName;
	else
		strName = pszName;

	CString strCaption, strTip;

	for ( CWnd* pWnd = pDialog->GetWindow( GW_CHILD ) ; pWnd ; pWnd = pWnd->GetNextWindow() )
	{
		pWnd->SetFont( &CoolInterface.m_fntNormal );

		TCHAR szClass[3] = { 0, 0, 0 };
		LoadControlTip( strTip, pWnd->GetDlgCtrlID() );

		if ( pWndTooltips && strTip.GetLength() )
		{
			pWndTooltips->AddTool( pWnd, strTip );
		}

		GetClassName( pWnd->GetSafeHwnd(), szClass, 3 );

		// Skip added banner
		if ( _tcsnicmp( szClass, _T("St"), 3 ) == 0 &&
			IDC_BANNER == pWnd->GetDlgCtrlID() &&
			pWnd->GetNextWindow() == NULL )
			break;

		strCaption += szClass;
	}

	if ( Settings.General.DialogScan )
	{
		CFile pFile;

		if ( pFile.Open( _T("\\Dialogs.xml"), CFile::modeReadWrite ) )
		{
			pFile.Seek( 0, CFile::end );
		}
		else if ( ! pFile.Open( _T("\\Dialogs.xml"), CFile::modeWrite|CFile::modeCreate ) )
		{
			return FALSE;
		}
		else
			pFile.Write( "<dialogs>\r\n", 11 );

		pFile.Write( "\t<dialog name=\"", 15 );
		int nBytes = WideCharToMultiByte( CP_ACP, 0, strName, strName.GetLength(), NULL, 0, NULL, NULL );
		LPSTR pBytes = new CHAR[nBytes];
		WideCharToMultiByte( CP_ACP, 0, strName, strName.GetLength(), pBytes, nBytes, NULL, NULL );
		pFile.Write( pBytes, nBytes );
		delete [] pBytes;

		pFile.Write( "\" cookie=\"", 10 );
		nBytes = WideCharToMultiByte( CP_ACP, 0, strCaption, strCaption.GetLength(), NULL, 0, NULL, NULL );
		pBytes = new CHAR[nBytes];
		WideCharToMultiByte( CP_ACP, 0, strCaption, strCaption.GetLength(), pBytes, nBytes, NULL, NULL );
		pFile.Write( pBytes, nBytes );
		delete [] pBytes;

		pFile.Write( "\" caption=\"", 11 );
		pDialog->GetWindowText( strCaption );
		strCaption.Replace( _T("\n"), _T("{n}") );
		strCaption.Replace( _T("\r"), _T("") );
		strCaption.Replace( _T("&"), _T("_") );
		CString strTemp;
		CXMLNode::ValueToString( strCaption, strTemp );
		strCaption = strTemp;
		nBytes = WideCharToMultiByte( CP_ACP, 0, strCaption, strCaption.GetLength(), NULL, 0, NULL, NULL );
		pBytes = new CHAR[nBytes];
		WideCharToMultiByte( CP_ACP, 0, strCaption, strCaption.GetLength(), pBytes, nBytes, NULL, NULL );
		pFile.Write( pBytes, nBytes );
		delete [] pBytes;

		pFile.Write( "\">\r\n", 4 );

		for ( CWnd* pWnd = pDialog->GetWindow( GW_CHILD ) ; pWnd ; pWnd = pWnd->GetNextWindow() )
		{
			TCHAR szClass[64];

			GetClassName( pWnd->GetSafeHwnd(), szClass, 64 );
			strCaption.Empty();

			// Skip added banner- Todo: Remove this
			if ( _tcsnicmp( szClass, _T("St"), 3 ) == 0 &&
				IDC_BANNER == pWnd->GetDlgCtrlID() &&
				pWnd->GetNextWindow() == NULL )
				break;

			if ( _tcsistr( szClass, _T("Static") ) ||
				 _tcsistr( szClass, _T("Button") ) )
			{
				pWnd->GetWindowText( strCaption );
			}
			else if ( _tcsistr( szClass, _T("ListBox") ) )
			{
				CListBox* pListBox = static_cast< CListBox* >( pWnd );
				for ( int i = 0; i < pListBox->GetCount(); ++i )
				{
					CString strTemp;
					pListBox->GetText( i, strTemp );
					if ( ! strCaption.IsEmpty() )
						strCaption += _T('|');
					strCaption += strTemp;
				}
			}
			else if ( _tcsistr( szClass, _T("ComboBox") ) )
			{
				CComboBox* pComboBox = static_cast< CComboBox* >( pWnd );
				for ( int i = 0; i < pComboBox->GetCount(); ++i )
				{
					CString strTemp;
					pComboBox->GetLBText( i, strTemp );
					if ( ! strCaption.IsEmpty() )
						strCaption += _T('|');
					strCaption += strTemp;
				}
			}

			if ( strCaption.GetLength() )
			{
				strCaption.Replace( _T("\n"), _T("{n}") );
				strCaption.Replace( _T("\r"), _T("") );
				strCaption.Replace( _T("&"), _T("_") );
				CString strTemp;
				CXMLNode::ValueToString( strCaption, strTemp );
				strCaption = strTemp;
				pFile.Write( "\t\t<control caption=\"", 20 );
				int nBytes = WideCharToMultiByte( CP_ACP, 0, strCaption, strCaption.GetLength(), NULL, 0, NULL, NULL );
				LPSTR pBytes = new CHAR[nBytes];
				WideCharToMultiByte( CP_ACP, 0, strCaption, strCaption.GetLength(), pBytes, nBytes, NULL, NULL );
				pFile.Write( pBytes, nBytes );
				delete [] pBytes;

				pFile.Write( "\"/>\r\n", 5 );
			}
			else
			{
				pFile.Write( "\t\t<control/>\r\n", 10+4 );
			}
		}

		pFile.Write( "\t</dialog>\r\n", 12 );
		pFile.Close();

		return TRUE;
	}

	CXMLElement* pBase = NULL;
	if ( ! m_pDialogs.Lookup( strName, pBase ) )
		// Naked dialog
		return FALSE;

	if ( strCaption != pBase->GetAttributeValue( _T("cookie") ) )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("Invalid [cookie] attribute in [dialog] element"), pBase->ToString() );
		return FALSE;
	}

	strCaption = pBase->GetAttributeValue( _T("caption") );
	if ( strCaption.GetLength() ) pDialog->SetWindowText( strCaption );

	CWnd* pWnd = pDialog->GetWindow( GW_CHILD );

	for ( POSITION pos = pBase->GetElementIterator() ; pos && pWnd ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		TCHAR szClass[3] = { 0, 0, 0 };
		GetClassName( pWnd->GetSafeHwnd(), szClass, 3 );

		// Needed for some controls like Schema combo box
		if ( Settings.General.LanguageRTL && (CString)szClass != "Ed" )
			pWnd->ModifyStyleEx( 0, WS_EX_LAYOUTRTL|WS_EX_RTLREADING, 0 );

		if ( pXML->IsNamed( _T("control") ) )
		{
			strCaption = pXML->GetAttributeValue( _T("caption") );
			strTip = pXML->GetAttributeValue( L"tip" );
			if ( pWndTooltips && strTip.GetLength() )
			{
				pWndTooltips->AddTool( pWnd, strTip );
			}

			strCaption.Replace( _T("{n}"), _T("\r\n") );

			if ( strCaption.GetLength() )
			{
				if ( (CString) szClass != "Co" )
				{
					int nPos = strCaption.Find( '_' );
					if ( nPos >= 0 ) strCaption.SetAt( nPos, '&' );
					pWnd->SetWindowText( strCaption );
				}
				else
				{
					CStringArray pItems;
					CString strTemp;
					int nNum;

					Split( strCaption, _T('|'), pItems, TRUE );
					CComboBox* pCombo = (CComboBox*) pWnd;
					nNum = pCombo->GetCount();
					if ( nNum == pItems.GetSize() )
					{
						for ( int nCount = 0; nCount < nNum; nCount++ )
						{
							pCombo->DeleteString( 0 );
							strTemp = pItems.GetAt( nCount );
							pCombo->AddString( (LPCTSTR) strTemp );
						}
					}
				}
			}

			pWnd = pWnd->GetNextWindow();
		}
	}

	return TRUE;
}

CString CSkin::GetDialogCaption(LPCTSTR pszName)
{
	CXMLElement* pBase = NULL;
	CString strCaption;

	if ( m_pDialogs.Lookup( pszName, pBase ) )
	{
		strCaption = pBase->GetAttributeValue( _T("caption") );
	}

	return strCaption;
}

BOOL CSkin::LoadDialogs(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("dialog") ) )
		{
			CString strName = pXML->GetAttributeValue( _T("name") );
			CXMLElement* pOld;

			if ( m_pDialogs.Lookup( strName, pOld ) ) delete pOld;

			pXML->Detach();
			m_pDialogs.SetAt( strName, pXML );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [dialogs] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin window skins

CSkinWindow* CSkin::GetWindowSkin(LPCTSTR pszWindow, LPCTSTR pszAppend)
{
	CString strWindow;
	strWindow.Format( _T("|%s%s|"), pszWindow, pszAppend ? pszAppend : _T("") );

	for ( POSITION pos = m_pSkins.GetHeadPosition() ; pos ; )
	{
		CSkinWindow* pSkin = m_pSkins.GetNext( pos );
		if ( pSkin->m_sTargets.Find( strWindow ) >= 0 ) return pSkin;
	}

	return NULL;
}

CSkinWindow* CSkin::GetWindowSkin(CWnd* pWnd)
{
	ASSERT( Settings.General.GUIMode == GUI_WINDOWED ||
		Settings.General.GUIMode == GUI_TABBED ||
		Settings.General.GUIMode == GUI_BASIC );
	LPCTSTR* pszModeSuffix = m_pszModeSuffix[ Settings.General.GUIMode ];
	BOOL bPanel = FALSE;

	ASSERT(pWnd != NULL);

	if ( pWnd->IsKindOf( RUNTIME_CLASS(CChildWnd) ) )
	{
		CChildWnd* pChild = (CChildWnd*)pWnd;
		bPanel = pChild->m_bPanelMode;
	}

	#ifdef _AFXDLL	// ToDo: Clean this up?
	for ( CRuntimeClass* pClass = pWnd->GetRuntimeClass() ; pClass && pClass->m_pfnGetBaseClass ; pClass = pClass->m_pfnGetBaseClass() )
	#else
	for ( CRuntimeClass* pClass = pWnd->GetRuntimeClass() ; pClass ; pClass = pClass->m_pBaseClass )
	#endif
	{
		if ( bPanel )
		{
			CSkinWindow* pSkin = GetWindowSkin( CString( pClass->m_lpszClassName ), _T(".Panel") );
			if ( pSkin != NULL ) return pSkin;
		}

		for ( int nSuffix = 0 ; nSuffix < 3 && pszModeSuffix[ nSuffix ] != NULL ; nSuffix ++ )
		{
			if ( pszModeSuffix[ nSuffix ][0] != 0 || ! bPanel )
			{
				CSkinWindow* pSkin = GetWindowSkin(
					CString( pClass->m_lpszClassName ), pszModeSuffix[ nSuffix ] );
				if ( pSkin != NULL ) return pSkin;
			}
		}
	}

	return NULL;
}

BOOL CSkin::LoadWindowSkins(CXMLElement* pSub, const CString& strPath)
{
	for ( POSITION posSkin = pSub->GetElementIterator() ; posSkin ; )
	{
		CXMLElement* pSkinElement = pSub->GetNextElement( posSkin );

		if ( pSkinElement->IsNamed( _T("windowSkin") ) )
		{
			CSkinWindow* pSkin = new CSkinWindow();

			if ( pSkin->Parse( pSkinElement, strPath ) )
				m_pSkins.AddHead( pSkin );
			else
				delete pSkin;
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [windowSkins] element"),	pSkinElement->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin Color Scheme

BOOL CSkin::LoadColorScheme(CXMLElement* pBase)
{
	CMapStringToPtr pColors;

	pColors.SetAt( _T("system.base.window"), &CoolInterface.m_crWindow );
	pColors.SetAt( _T("system.base.midtone"), &CoolInterface.m_crMidtone );
	pColors.SetAt( _T("system.base.text"), &CoolInterface.m_crText );
	pColors.SetAt( _T("system.base.hitext"), &CoolInterface.m_crHiText );
	pColors.SetAt( _T("system.base.hiborder"), &CoolInterface.m_crHiBorder );
	pColors.SetAt( _T("system.base.highlight"), &CoolInterface.m_crHighlight );
	pColors.SetAt( _T("system.back.normal"), &CoolInterface.m_crBackNormal );
	pColors.SetAt( _T("system.back.selected"), &CoolInterface.m_crBackSel );
	pColors.SetAt( _T("system.back.checked"), &CoolInterface.m_crBackCheck );
	pColors.SetAt( _T("system.back.checked.selected"), &CoolInterface.m_crBackCheckSel );
	pColors.SetAt( _T("system.margin"), &CoolInterface.m_crMargin );
	pColors.SetAt( _T("system.border"), &CoolInterface.m_crBorder );
	pColors.SetAt( _T("system.shadow"), &CoolInterface.m_crShadow );
	pColors.SetAt( _T("system.text"), &CoolInterface.m_crCmdText );
	pColors.SetAt( _T("system.text.selected"), &CoolInterface.m_crCmdTextSel );
	pColors.SetAt( _T("system.disabled"), &CoolInterface.m_crDisabled );

	pColors.SetAt( _T("tooltip.back"), &CoolInterface.m_crTipBack );
	pColors.SetAt( _T("tooltip.text"), &CoolInterface.m_crTipText );
	pColors.SetAt( _T("tooltip.border"), &CoolInterface.m_crTipBorder );
	pColors.SetAt( _T("tooltip.warnings"), &CoolInterface.m_crTipWarnings );

	pColors.SetAt( _T("taskpanel.back"), &CoolInterface.m_crTaskPanelBack );
	pColors.SetAt( _T("taskbox.caption.back"), &CoolInterface.m_crTaskBoxCaptionBack );
	pColors.SetAt( _T("taskbox.caption.text"), &CoolInterface.m_crTaskBoxCaptionText );
	pColors.SetAt( _T("taskbox.primary.back"), &CoolInterface.m_crTaskBoxPrimaryBack );
	pColors.SetAt( _T("taskbox.primary.text"), &CoolInterface.m_crTaskBoxPrimaryText );
	pColors.SetAt( _T("taskbox.caption.hover"), &CoolInterface.m_crTaskBoxCaptionHover );
	pColors.SetAt( _T("taskbox.client"), &CoolInterface.m_crTaskBoxClient );

	pColors.SetAt( _T("dialog.back"), &m_crDialog );
	pColors.SetAt( _T("panel.caption.back"), &m_crPanelBack );
	pColors.SetAt( _T("panel.caption.text"), &m_crPanelText );
	pColors.SetAt( _T("panel.caption.border"), &m_crPanelBorder );
	pColors.SetAt( _T("banner.back"), &m_crBannerBack );
	pColors.SetAt( _T("banner.text"), &m_crBannerText );
	pColors.SetAt( _T("schema.row1"), &m_crSchemaRow[0] );
	pColors.SetAt( _T("schema.row2"), &m_crSchemaRow[1] );

//	Active window color is controlled by media player plugin, thus we can not skin it?
	pColors.SetAt( _T("media.window"), &CoolInterface.m_crMediaWindowBack );
	pColors.SetAt( _T("media.window.back"), &CoolInterface.m_crMediaWindowBack );
	pColors.SetAt( _T("media.window.text"), &CoolInterface.m_crMediaWindowText );
	pColors.SetAt( _T("media.status"), &CoolInterface.m_crMediaStatusBack );
	pColors.SetAt( _T("media.status.back"), &CoolInterface.m_crMediaStatusBack );
	pColors.SetAt( _T("media.status.text"), &CoolInterface.m_crMediaStatusText );
	pColors.SetAt( _T("media.panel"), &CoolInterface.m_crMediaPanelBack );
	pColors.SetAt( _T("media.panel.back"), &CoolInterface.m_crMediaPanelBack );
	pColors.SetAt( _T("media.panel.text"), &CoolInterface.m_crMediaPanelText );
	pColors.SetAt( _T("media.panel.active"), &CoolInterface.m_crMediaPanelActiveBack );
	pColors.SetAt( _T("media.panel.active.back"), &CoolInterface.m_crMediaPanelActiveBack );
	pColors.SetAt( _T("media.panel.active.text"), &CoolInterface.m_crMediaPanelActiveText );
	pColors.SetAt( _T("media.panel.caption"), &CoolInterface.m_crMediaPanelCaptionBack );
	pColors.SetAt( _T("media.panel.caption.back"), &CoolInterface.m_crMediaPanelCaptionBack );
	pColors.SetAt( _T("media.panel.caption.text"), &CoolInterface.m_crMediaPanelCaptionText );

	pColors.SetAt( _T("traffic.window.back"), &CoolInterface.m_crTrafficWindowBack );
	pColors.SetAt( _T("traffic.window.text"), &CoolInterface.m_crTrafficWindowText );
	pColors.SetAt( _T("traffic.window.grid"), &CoolInterface.m_crTrafficWindowGrid );

	pColors.SetAt( _T("monitor.history.back"), &CoolInterface.m_crMonitorHistoryBack );
	pColors.SetAt( _T("monitor.history.back.max"), &CoolInterface.m_crMonitorHistoryBackMax );
	pColors.SetAt( _T("monitor.history.text"), &CoolInterface.m_crMonitorHistoryText );
	pColors.SetAt( _T("monitor.download.line"), &CoolInterface.m_crMonitorDownloadLine );
	pColors.SetAt( _T("monitor.upload.line"), &CoolInterface.m_crMonitorUploadLine );
	pColors.SetAt( _T("monitor.download.bar"), &CoolInterface.m_crMonitorDownloadBar );
	pColors.SetAt( _T("monitor.upload.bar"), &CoolInterface.m_crMonitorUploadBar );

	pColors.SetAt( _T("schema.rating"), &CoolInterface.m_crRatingNull );
	pColors.SetAt( _T("schema.rating0"), &CoolInterface.m_crRating0 );
	pColors.SetAt( _T("schema.rating1"), &CoolInterface.m_crRating1 );
	pColors.SetAt( _T("schema.rating2"), &CoolInterface.m_crRating2 );
	pColors.SetAt( _T("schema.rating3"), &CoolInterface.m_crRating3 );
	pColors.SetAt( _T("schema.rating4"), &CoolInterface.m_crRating4 );
	pColors.SetAt( _T("schema.rating5"), &CoolInterface.m_crRating5 );

	pColors.SetAt( _T("richdoc.back"), &CoolInterface.m_crRichdocBack );
	pColors.SetAt( _T("richdoc.text"), &CoolInterface.m_crRichdocText );
	pColors.SetAt( _T("richdoc.heading"), &CoolInterface.m_crRichdocHeading );

	pColors.SetAt( _T("system.textalert"), &CoolInterface.m_crTextAlert );
	pColors.SetAt( _T("system.textstatus"), &CoolInterface.m_crTextStatus );
	pColors.SetAt( _T("system.textlink"), &CoolInterface.m_crTextLink );
	pColors.SetAt( _T("system.textlink.selected"), &CoolInterface.m_crTextLinkHot );

	pColors.SetAt( _T("system.base.chat.in"), &CoolInterface.m_crChatIn );
	pColors.SetAt( _T("system.base.chat.out"), &CoolInterface.m_crChatOut );
	pColors.SetAt( _T("system.base.chat.null"), &CoolInterface.m_crChatNull );
	pColors.SetAt( _T("system.base.search.null"), &CoolInterface.m_crSearchNull );
	pColors.SetAt( _T("system.base.search.exists"), &CoolInterface.m_crSearchExists );
	pColors.SetAt( _T("system.base.search.exists.hit"), &CoolInterface.m_crSearchExistsHit );
	pColors.SetAt( _T("system.base.search.exists.selected"), &CoolInterface.m_crSearchExistsSelected );
	pColors.SetAt( _T("system.base.search.queued"), &CoolInterface.m_crSearchQueued );
	pColors.SetAt( _T("system.base.search.queued.hit"), &CoolInterface.m_crSearchQueuedHit );
	pColors.SetAt( _T("system.base.search.queued.selected"), &CoolInterface.m_crSearchQueuedSelected );
	pColors.SetAt( _T("system.base.search.ghostrated"), &CoolInterface.m_crSearchGhostrated );
	pColors.SetAt( _T("system.base.search.highrated"), &CoolInterface.m_crSearchHighrated );
	pColors.SetAt( _T("system.base.search.collection"), &CoolInterface.m_crSearchCollection );
	pColors.SetAt( _T("system.base.search.torrent"), &CoolInterface.m_crSearchTorrent );
	pColors.SetAt( _T("system.base.transfer.source"), &CoolInterface.m_crTransferSource );
	pColors.SetAt( _T("system.base.transfer.ranges"), &CoolInterface.m_crTransferRanges );
	pColors.SetAt( _T("system.base.transfer.completed"), &CoolInterface.m_crTransferCompleted );
	pColors.SetAt( _T("system.base.transfer.seeding"), &CoolInterface.m_crTransferVerifyPass );
	pColors.SetAt( _T("system.base.transfer.failed"), &CoolInterface.m_crTransferVerifyFail );
	pColors.SetAt( _T("system.base.transfer.completed.selected"), &CoolInterface.m_crTransferCompletedSelected );
	pColors.SetAt( _T("system.base.transfer.seeding.selected"), &CoolInterface.m_crTransferVerifyPassSelected );
	pColors.SetAt( _T("system.base.transfer.failed.selected"), &CoolInterface.m_crTransferVerifyFailSelected );
	pColors.SetAt( _T("system.base.network.null"), &CoolInterface.m_crNetworkNull );
	pColors.SetAt( _T("system.base.network.g1"), &CoolInterface.m_crNetworkG1 );
	pColors.SetAt( _T("system.base.network.g2"), &CoolInterface.m_crNetworkG2 );
	pColors.SetAt( _T("system.base.network.ed2k"), &CoolInterface.m_crNetworkED2K );
	pColors.SetAt( _T("system.base.network.up"), &CoolInterface.m_crNetworkUp );
	pColors.SetAt( _T("system.base.network.down"), &CoolInterface.m_crNetworkDown );
	pColors.SetAt( _T("system.base.security.allow"), &CoolInterface.m_crSecurityAllow );
	pColors.SetAt( _T("system.base.security.deny"), &CoolInterface.m_crSecurityDeny );

	pColors.SetAt( _T("dropdownbox.back"), &CoolInterface.m_crDropdownBox );
	pColors.SetAt( _T("dropdownbox.text"), &CoolInterface.m_crDropdownText );
	pColors.SetAt( _T("resizebar.edge"), &CoolInterface.m_crResizebarEdge );
	pColors.SetAt( _T("resizebar.face"), &CoolInterface.m_crResizebarFace );
	pColors.SetAt( _T("resizebar.shadow"), &CoolInterface.m_crResizebarShadow );
	pColors.SetAt( _T("resizebar.highlight"), &CoolInterface.m_crResizebarHighlight );
	pColors.SetAt( _T("fragmentbar.shaded"), &CoolInterface.m_crFragmentShaded );
	pColors.SetAt( _T("fragmentbar.complete"), &CoolInterface.m_crFragmentComplete );
	pColors.SetAt( _T("fragmentbar.source1"), &CoolInterface.m_crFragmentSource1 );
	pColors.SetAt( _T("fragmentbar.source2"), &CoolInterface.m_crFragmentSource2 );
	pColors.SetAt( _T("fragmentbar.source3"), &CoolInterface.m_crFragmentSource3 );
	pColors.SetAt( _T("fragmentbar.source4"), &CoolInterface.m_crFragmentSource4 );
	pColors.SetAt( _T("fragmentbar.source5"), &CoolInterface.m_crFragmentSource5 );
	pColors.SetAt( _T("fragmentbar.source6"), &CoolInterface.m_crFragmentSource6 );
	pColors.SetAt( _T("fragmentbar.pass"), &CoolInterface.m_crFragmentPass );
	pColors.SetAt( _T("fragmentbar.fail"), &CoolInterface.m_crFragmentFail );
	pColors.SetAt( _T("fragmentbar.request"), &CoolInterface.m_crFragmentRequest );
	pColors.SetAt( _T("fragmentbar.border"), &CoolInterface.m_crFragmentBorder );
	pColors.SetAt( _T("fragmentbar.border.selected"), &CoolInterface.m_crFragmentBorderSelected );
	pColors.SetAt( _T("fragmentbar.border.simplebar"), &CoolInterface.m_crFragmentBorderSimpleBar );
	pColors.SetAt( _T("fragmentbar.border.simplebar.selected"), &CoolInterface.m_crFragmentBorderSimpleBarSelected );

	pColors.SetAt( _T("system.environment.borders"), &CoolInterface.m_crSysBorders );
	pColors.SetAt( _T("system.environment.window"), &CoolInterface.m_crSysWindow );
	pColors.SetAt( _T("system.environment.btnface"), &CoolInterface.m_crSysBtnFace );
	pColors.SetAt( _T("system.environment.3dshadow"), &CoolInterface.m_crSys3DShadow );
	pColors.SetAt( _T("system.environment.3dhighlight"), &CoolInterface.m_crSys3DHighlight );
	pColors.SetAt( _T("system.environment.activecaption"), &CoolInterface.m_crSysActiveCaption );

	pColors.SetAt( _T("navbar.text"), &m_crNavBarText );
	pColors.SetAt( _T("navbar.text.up"), &m_crNavBarTextUp );
	pColors.SetAt( _T("navbar.text.down"), &m_crNavBarTextDown );
	pColors.SetAt( _T("navbar.text.hover"), &m_crNavBarTextHover );
	pColors.SetAt( _T("navbar.text.checked"), &m_crNavBarTextChecked );
	pColors.SetAt( _T("navbar.shadow"), &m_crNavBarShadow );
	pColors.SetAt( _T("navbar.shadow.up"), &m_crNavBarShadowUp );
	pColors.SetAt( _T("navbar.shadow.down"), &m_crNavBarShadowDown );
	pColors.SetAt( _T("navbar.shadow.hover"), &m_crNavBarShadowHover );
	pColors.SetAt( _T("navbar.shadow.checked"), &m_crNavBarShadowChecked );
	pColors.SetAt( _T("navbar.outline"), &m_crNavBarOutline );
	pColors.SetAt( _T("navbar.outline.up"), &m_crNavBarOutlineUp );
	pColors.SetAt( _T("navbar.outline.down"), &m_crNavBarOutlineDown );
	pColors.SetAt( _T("navbar.outline.hover"), &m_crNavBarOutlineHover );
	pColors.SetAt( _T("navbar.outline.checked"), &m_crNavBarOutlineChecked );

	BOOL bSystem = FALSE, bNonBase = FALSE;

	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		if ( pXML->IsNamed( _T("color") ) ||
			 pXML->IsNamed( _T("colour") ) )
		{
			CString strName		= pXML->GetAttributeValue( _T("name") );
			CString strValue	= pXML->GetAttributeValue( _T("value") );
			ToLower( strName );

			COLORREF* pColor;

			if ( pColors.Lookup( strName, (void*&)pColor ) )
			{
				if ( strValue.GetLength() == 6 )
				{
					int nRed, nGreen, nBlue;

					_stscanf( strValue.Mid( 0, 2 ), _T("%x"), &nRed );
					_stscanf( strValue.Mid( 2, 2 ), _T("%x"), &nGreen );
					_stscanf( strValue.Mid( 4, 2 ), _T("%x"), &nBlue );

					if ( strName.Find( _T("system.") ) >= 0 )
					{
						bSystem = TRUE;

						if ( ! bNonBase && strName.Find( _T(".base.") ) < 0 )
						{
							bNonBase = TRUE;
							CoolInterface.CalculateColors( TRUE );
						}
					}

					*pColor = RGB( nRed, nGreen, nBlue );
				}
				else if ( strValue.GetLength() == 0 )
				{
					*pColor = CLR_NONE;
				}
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Unknown [name] attribute in [colorScheme] element"), pXML->ToString() );
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [colorScheme] element"), pXML->ToString() );
		}
	}

	if ( bSystem && ! bNonBase ) CoolInterface.CalculateColors( TRUE );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin command lookup helper

UINT CSkin::LookupCommandID(CXMLElement* pXML, LPCTSTR pszName) const
{
	return CoolInterface.NameToID( pXML->GetAttributeValue( pszName ) );
}

//////////////////////////////////////////////////////////////////////
// CSkin command map

BOOL CSkin::LoadResourceMap(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( L"command" ) ||
			 pXML->IsNamed( L"control" ) ||
			 pXML->IsNamed( L"resource" ) )
		{
			CString strTemp = pXML->GetAttributeValue( _T("code") );
			UINT nID;

			if ( _stscanf( strTemp, _T("%lu"), &nID ) != 1 )
				return FALSE;

			CoolInterface.NameCommand( nID, pXML->GetAttributeValue( _T("id") ) );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [...Map] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin fonts

BOOL CSkin::LoadFonts(CXMLElement* pBase, const CString& strPath)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("font") ) )
		{
			CString strName		= pXML->GetAttributeValue( _T("name") );
			CString strLanguage	= pXML->GetAttributeValue( _T("language") );
			CString strFace		= pXML->GetAttributeValue( _T("face") );
			CString strSize		= pXML->GetAttributeValue( _T("size") );
			CString strWeight	= pXML->GetAttributeValue( _T("weight") );

			if ( ( Settings.General.Language.CompareNoCase( strLanguage ) == 0 ) ||
				 strLanguage.IsEmpty() )
			{
				CFont* pFont = NULL;

				if ( strName.CompareNoCase( _T("system.plain") ) == 0 )
				{
					pFont = &CoolInterface.m_fntNormal;
				}
				else if ( strName.CompareNoCase( _T("system.bold") ) == 0 )
				{
					pFont = &CoolInterface.m_fntBold;
				}
				else if ( strName.CompareNoCase( _T("panel.caption") ) == 0 )
				{
					pFont = &CoolInterface.m_fntCaption;
				}
				else if ( strName.CompareNoCase( _T("navbar.caption") ) == 0 )
				{
					pFont = &CoolInterface.m_fntNavBar;
				}
				else if ( strName.CompareNoCase( _T("richdoc.default") ) == 0 || strName.CompareNoCase( _T("rich.default") ) == 0 )
				{
					pFont = &CoolInterface.m_fntRichDefault;
				}
				else if ( strName.CompareNoCase( _T("richdoc.heading") ) == 0 || strName.CompareNoCase( _T("rich.heading") ) == 0 )
				{
					pFont = &CoolInterface.m_fntRichHeading;
				}
				else
				{
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
						_T("Unknown Font"), pXML->ToString() );
					continue;
				}

				if ( pFont->m_hObject ) pFont->DeleteObject();

				if ( strFace.IsEmpty() )
					strFace = Settings.Fonts.DefaultFont;

				if ( strWeight.CompareNoCase( _T("bold") ) == 0 )
					strWeight = _T("700");
				else if ( strWeight.IsEmpty() )
					strWeight = _T("400");

				int nFontSize = Settings.Fonts.FontSize, nFontWeight = FW_NORMAL;

				_stscanf( strSize, _T("%i"), &nFontSize );
				_stscanf( strWeight, _T("%i"), &nFontWeight );

				pFont->CreateFontW( -nFontSize, 0, 0, 0, nFontWeight, FALSE, FALSE, FALSE,
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					theApp.m_bIsVistaOrNewer ? DEFAULT_QUALITY : ANTIALIASED_QUALITY,
					DEFAULT_PITCH|FF_DONTCARE, strFace );

				if ( strName.CompareNoCase( _T("system.plain") ) == 0 )
				{
					pFont = &CoolInterface.m_fntUnder;
					if ( pFont->m_hObject ) pFont->DeleteObject();

					pFont->CreateFontW( -nFontSize, 0, 0, 0, nFontWeight, FALSE, TRUE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							theApp.m_bIsVistaOrNewer ? DEFAULT_QUALITY : ANTIALIASED_QUALITY,
							DEFAULT_PITCH|FF_DONTCARE, strFace );

					pFont = &CoolInterface.m_fntItalic;
					if ( pFont->m_hObject ) pFont->DeleteObject();

					pFont->CreateFontW( -nFontSize, 0, 0, 0, nFontWeight, TRUE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							theApp.m_bIsVistaOrNewer ? DEFAULT_QUALITY : ANTIALIASED_QUALITY,
							DEFAULT_PITCH|FF_DONTCARE, strFace );
				}
				else if ( strName.CompareNoCase( _T("system.bold") ) == 0 )
				{
					pFont = &CoolInterface.m_fntBoldItalic;
					if ( pFont->m_hObject ) pFont->DeleteObject();

					pFont->CreateFontW( -nFontSize, 0, 0, 0, nFontWeight, TRUE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							theApp.m_bIsVistaOrNewer ? DEFAULT_QUALITY : ANTIALIASED_QUALITY,
							DEFAULT_PITCH|FF_DONTCARE, strFace );
				}
			}
		}
		else if ( pXML->IsNamed( _T("import") ) )
		{
			CString strFile = strPath + pXML->GetAttributeValue( _T("path") );

			BOOL bSuccess = AddFontResourceEx( strFile, FR_PRIVATE, NULL );

			if ( bSuccess )
				m_pFontPaths.AddTail( strFile );
			else
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
					_T("Failed to import font"), pXML->ToString() );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [fonts] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin command images

CString	CSkin::GetImagePath(UINT nImageID) const
{
	CString strPath;
	if ( ! m_pImages.Lookup( nImageID, strPath ) )
		strPath.Format( _T("\"%s\",-%u"), theApp.m_strBinaryPath, nImageID );
	return strPath;
}

BOOL CSkin::LoadCommandImages(CXMLElement* pBase, const CString& strPath)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("icon") ) )
		{
			if ( ! LoadCommandIcon( pXML, strPath ) )
				return FALSE;
		}
		else if ( pXML->IsNamed( _T("bitmap") ) )
		{
			if ( ! LoadCommandBitmap( pXML, strPath ) )
				return FALSE;
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Unknown element in [commandImages] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

BOOL CSkin::LoadCommandIcon(CXMLElement* pXML, const CString& strPath)
{
	// strPath is:
	// 1) when loading from resource: "module instance$" or ...
	// 2) when loading from file: "root skin path\".

	CString strFile = strPath +
		pXML->GetAttributeValue( _T("res") ) +
		pXML->GetAttributeValue( _T("path") );

	UINT nIconID = LookupCommandID( pXML, _T("res") );
	HINSTANCE hInstance( NULL );
	if ( nIconID )
		_stscanf( strPath, _T("%p"), &hInstance );

	UINT nID = LookupCommandID( pXML );
	if ( nID == 0 )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("Unknown [id] attribute in [icon] element"), pXML->ToString() );
		return FALSE;
	}

	// Is this a RTL-enabled icon? (default: "0" - no)
	BOOL bRTL = ( pXML->GetAttributeValue( _T("rtl"), _T("0") ) == _T("1") );

	// Icon types (default: "16" - 16x16 icon only)
	CString strTypes = pXML->GetAttributeValue( _T("types"), _T("16") );
	int curPos = 0;
	CString strSize;
	while ( ( strSize = strTypes.Tokenize( _T(","), curPos ) ).GetLength() )
	{
		int cx = _tstoi( strSize );
		int nType;
		switch ( cx )
		{
		case 16:
			nType = LVSIL_SMALL;
			break;
		case 32:
			nType = LVSIL_NORMAL;
			break;
		case 48:
			nType = LVSIL_BIG;
			break;
		default:
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Icon has invalid size"), pXML->ToString() );
			return FALSE;
		}

		HICON hIcon = NULL;
		if ( nIconID && hInstance )
		{
			hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( nIconID ),
				IMAGE_ICON, cx, cx, 0 );
		}
		else
		{
			if ( LoadIcon( strFile,
				( ( nType == LVSIL_SMALL ) ? &hIcon : NULL ),
				( ( nType == LVSIL_NORMAL ) ? &hIcon : NULL ),
				( ( nType == LVSIL_BIG )? &hIcon : NULL ) ) )
			{
				m_pImages.SetAt( nID, strFile );
			}
		}
		if ( hIcon )
		{
			if ( bRTL && Settings.General.LanguageRTL )
				hIcon = CreateMirroredIcon( hIcon );
			CoolInterface.AddIcon( nID, hIcon, nType );
			VERIFY( DestroyIcon( hIcon ) );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
				_T("Failed to load icon"), pXML->ToString() );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CSkin::LoadCommandBitmap(CXMLElement* pBase, const CString& strPath)
{
	CString strFile;
	UINT nID = LookupCommandID( pBase );
	// If nID is 0 then we don't want to include it in strFile because
	// strFile must be a file system path rather than a resource path.
	if ( nID )
		strFile.Format( _T("%s%lu%s"), strPath, nID, pBase->GetAttributeValue( _T("path") ) );
	else
		strFile.Format( _T("%s%s"), strPath, pBase->GetAttributeValue( _T("path") ) );

	CString strMask = pBase->GetAttributeValue( _T("mask"), _T("00FF00") );
	COLORREF crMask;
	int nRed = 0, nGreen = 0, nBlue = 0;
	if ( strMask.GetLength() == 6 &&
		_stscanf( strMask.Mid( 0, 2 ), _T("%x"), &nRed ) == 1 &&
		_stscanf( strMask.Mid( 2, 2 ), _T("%x"), &nGreen ) == 1 &&
		_stscanf( strMask.Mid( 4, 2 ), _T("%x"), &nBlue ) == 1 )
	{
		crMask = RGB( nRed, nGreen, nBlue );
	}
	else
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("Image has invalid mask"), pBase->ToString() );
		return FALSE;
	}

	HBITMAP hBitmap = LoadBitmap( strFile );
	if ( hBitmap == NULL )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("Failed to load image"), pBase->ToString() );
		return FALSE;
	}
	if ( Settings.General.LanguageRTL )
		hBitmap = CreateMirroredBitmap( hBitmap );

	BOOL bResult = CoolInterface.Add( this, pBase, hBitmap, crMask );
	DeleteObject( hBitmap );
	if ( ! bResult )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR,
			_T("Failed to add image"), pBase->ToString() );
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin popup menu helper

UINT_PTR CSkin::TrackPopupMenu(LPCTSTR pszMenu, const CPoint& point,
	UINT nDefaultID, UINT nFlags, const CStringList& oFiles, CWnd* pWnd) const
{
	CMenu* pPopup = GetMenu( pszMenu );
	if ( pPopup == NULL )
		return 0;

	if ( nDefaultID != 0 )
		pPopup->SetDefaultItem( nDefaultID );

	if ( oFiles.GetCount() )
	{
		// Change ID_SHELL_MENU item to shell submenu
		MENUITEMINFO pInfo = {};
		pInfo.cbSize = sizeof( pInfo );
		pInfo.fMask = MIIM_SUBMENU | MIIM_STATE;
		pInfo.fState = MFS_ENABLED;
		HMENU hSubMenu = pInfo.hSubMenu = ::CreatePopupMenu();
		ASSERT( hSubMenu );
		if( pPopup->SetMenuItemInfo( ID_SHELL_MENU, &pInfo ) )
		{
			UINT_PTR nCmd = CoolMenu.DoExplorerMenu( pWnd->GetSafeHwnd(), oFiles,
				point, pPopup->GetSafeHmenu(), pInfo.hSubMenu,
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | nFlags );

			// Change ID_SHELL_MENU back
			pInfo.hSubMenu = NULL;
			VERIFY( pPopup->SetMenuItemInfo( ID_SHELL_MENU, &pInfo ) );

			return nCmd;
		}
		VERIFY( DestroyMenu( hSubMenu ) );
	}

	return pPopup->TrackPopupMenu(
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | nFlags,
		point.x, point.y, pWnd );
}

//////////////////////////////////////////////////////////////////////
// CSkin draw wrapped text

// GetTextFlowChange determines the direction of text and its change at word boundaries.
// Returns the direction of the first "words iceland" and the position
// where the next "iceland" starts at the word boundary.
// If there is no change in direction it returns 0.

int CSkin::GetTextFlowChange(LPCTSTR pszText, BOOL* bIsRTL)
{
	TRISTATE bTextIsRTL = TRI_UNKNOWN;
	BOOL bChangeFound   = FALSE;
	LPCTSTR pszWord = pszText;
	LPCTSTR pszScan = pszText;

	int nPos;
	for ( nPos = 0; ; pszScan++, nPos++ )
	{
		// get the first word with punctuation marks and whitespaces
		if ( (unsigned short)*pszScan > 32 && (unsigned short)*pszScan != 160 ) continue;

		if ( pszWord < pszScan )
		{
			int nLen = static_cast< int >( pszScan - pszWord );
			WORD* nCharType = new WORD[ nLen + 1 ];

			TCHAR* pszTestWord = new TCHAR[ nLen + 1 ];
			_tcsncpy( pszTestWord, pszWord, nLen );
			pszTestWord[ nLen ] = 0;

			GetStringTypeEx( LOCALE_NEUTRAL, CT_CTYPE2, pszTestWord, nLen + 1, (LPWORD)nCharType );
			delete [] pszTestWord;

			for ( int i = 0 ; i < nLen ; i++ )
			{
				if ( nCharType[ i ] == C2_LEFTTORIGHT )
				{
					if ( bTextIsRTL == TRI_UNKNOWN )
					{
						bTextIsRTL = TRI_FALSE;
						*bIsRTL = FALSE;
					}
					else if ( bTextIsRTL == TRI_TRUE )
					{
						bChangeFound = TRUE;
						break;
					}
				}
				else if ( nCharType[ i ] == C2_RIGHTTOLEFT )
				{
					if ( bTextIsRTL == TRI_UNKNOWN )
					{
						bTextIsRTL = TRI_TRUE;
						*bIsRTL = TRUE;
					}
					else if ( bTextIsRTL == TRI_FALSE )
					{
						bChangeFound = TRUE;
						break;
					}
				}
			}
			BOOL bLeadingWhiteSpace = ( nCharType[ 0 ] == C2_WHITESPACE );
			delete [] nCharType;

			if ( bChangeFound ) return nPos - nLen + ( bLeadingWhiteSpace ? 1 : 0 );
			pszWord = pszScan;
		}
		if ( ! *pszScan ) break;
	}
	return 0;
}

void CSkin::DrawWrappedText(CDC* pDC, CRect* pBox, LPCTSTR pszText, CPoint ptStart, BOOL bExclude)
{
	// TODO: Wrap mixed text in RTL and LTR layouts correctly

	if ( pszText == NULL ) return;
	if ( ptStart.x == 0 && ptStart.y == 0 ) ptStart = pBox->TopLeft();

	UINT nAlignOptionsOld = pDC->GetTextAlign(); // backup settings
	UINT nFlags = ETO_CLIPPED | ( bExclude ? ETO_OPAQUE : 0 );

	unsigned short nLenFull = static_cast< unsigned short >( _tcslen( pszText ) );

	// Collect stats about the text from the start
	BOOL bIsRTLStart = FALSE;
	int nTestStart	= GetTextFlowChange( pszText, &bIsRTLStart );

	// Guess text direction ( not always works )
	BOOL bNormalFlow = Settings.General.LanguageRTL ? bIsRTLStart : !bIsRTLStart;

	TCHAR* pszSource = NULL;
	LPCTSTR pszWord	 = NULL;
	LPCTSTR pszScan  = NULL;

	if ( nTestStart )
	{
		// Get the source string to draw and truncate initial string to pass it recursively
		pszSource = new TCHAR[ nTestStart + 1 ];
		_tcsncpy( pszSource, pszText, nTestStart );
		pszSource[ nTestStart ] = 0;
		if ( !bNormalFlow )
		{
			// swap whitespaces
			CString str = pszSource;
			if ( pszSource[ 0 ] == ' ' || (unsigned short)pszSource[ 0 ] == 160 )
			{
				str = str + _T(" ");
				str = str.Right( nTestStart );
			}
			else if ( pszSource[ nTestStart - 1 ] == ' ' ||
					  (unsigned short)pszSource[ nTestStart - 1 ] == 160 )
			{
				str = _T(" ") + str;
				str = str.Left( nTestStart );
			}
			_tcsncpy( pszSource, str.GetBuffer( nTestStart ), nTestStart );
		}
		nLenFull = static_cast< unsigned short >( nTestStart );
		pszText += nTestStart;
	}
	else
		pszSource = (TCHAR*)pszText;

	pszWord = pszSource;
	pszScan = pszSource;

	if ( !bNormalFlow )
	{
		if ( ( bIsRTLStart != FALSE ) != Settings.General.LanguageRTL )
			pDC->SetTextAlign( nAlignOptionsOld ^ TA_RTLREADING );
		pszScan += nLenFull - 1;
		pszWord += nLenFull;
		for ( int nEnd = nLenFull - 1; nEnd >= 0 ; nEnd-- )
		{
			if ( nEnd ) pszScan--;
			if ( nEnd && (unsigned short)*pszScan > 32 && (unsigned short)*pszScan != 160 ) continue;

			if ( pszWord >= pszScan )
			{
				int nLen = static_cast< int >( pszWord - pszScan );
				CSize sz;
				GetTextExtentPoint32( pDC->m_hAttribDC, pszScan, nLen, &sz );

				if ( ptStart.x > pBox->left && ptStart.x + sz.cx > pBox->right )
				{
					ptStart.x = pBox->left;
					ptStart.y += sz.cy;
				}

				// Add extra point in x-axis; it cuts off the 1st word character otherwise
				const short nExtraPoint = ( Settings.General.LanguageRTL ) ? 1 : 0;
				CRect rc( ptStart.x, ptStart.y, ptStart.x + sz.cx + nExtraPoint, ptStart.y + sz.cy );

				pDC->ExtTextOut( ptStart.x, ptStart.y, nFlags, &rc,
					pszScan, nLen, NULL );
				if ( bExclude ) pDC->ExcludeClipRect( &rc );

				ptStart.x += sz.cx + nExtraPoint;
				pBox->top = ptStart.y + sz.cy;
			}
			pszWord = pszScan;
		}
	}
	else
	{
		for ( ; ; pszScan++ )
		{
			if ( *pszScan != NULL && (unsigned short)*pszScan > 32 &&
				 (unsigned short)*pszScan != 160 ) continue;

			if ( pszWord <= pszScan )
			{
				int nLen = static_cast< int >( pszScan - pszWord + ( *pszScan ? 1 : 0 ) );
				CSize sz = pDC->GetTextExtent( pszWord, nLen );

				if ( ptStart.x > pBox->left && ptStart.x + sz.cx > pBox->right )
				{
					ptStart.x = pBox->left;
					ptStart.y += sz.cy;
				}

				// Add extra point in x-axis; it cuts off the 1st word character otherwise
				const short nExtraPoint = ( Settings.General.LanguageRTL ) ? 1 : 0;

				CRect rc( ptStart.x, ptStart.y, ptStart.x + sz.cx + nExtraPoint, ptStart.y + sz.cy );

				pDC->ExtTextOut( ptStart.x, ptStart.y, nFlags, &rc,
					pszWord, nLen, NULL );
				if ( bExclude ) pDC->ExcludeClipRect( &rc );

				ptStart.x += sz.cx + nExtraPoint;
				pBox->top = ptStart.y + sz.cy;
			}

			pszWord = pszScan + 1;
			if ( ! *pszScan ) break;
		}
	}
	if ( nTestStart ) delete [] pszSource;
	// reset align options back
	pDC->SetTextAlign( nAlignOptionsOld );
	if ( nTestStart ) DrawWrappedText( pDC, pBox, pszText, ptStart, bExclude );
}

//////////////////////////////////////////////////////////////////////
// CSkin load bitmap helper

HBITMAP CSkin::LoadBitmap(const CString& strName)
{
	int nPos = strName.Find( '$' );
	if ( nPos < 0 )
	{
		return CImageFile::LoadBitmapFromFile( strName );
	}
	else
	{
		HINSTANCE hInstance = NULL;
		if ( _stscanf( (LPCTSTR)strName, _T("%p"), &hInstance ) != 1 )
			return NULL;

		UINT nID = 0;
		if ( _stscanf( (LPCTSTR)strName + nPos + 1, _T("%lu"), &nID ) != 1 )
			return NULL;

		nPos = strName.ReverseFind( '.' );
		return CImageFile::LoadBitmapFromResource( nID,
			( nPos < 0 ? RT_BITMAP : ( (LPCTSTR)strName + nPos + 1 ) ), hInstance );
	}
}

//////////////////////////////////////////////////////////////////////
// CSkin mode suffixes

LPCTSTR CSkin::m_pszModeSuffix[3][4] =
{
	{ _T(".Windowed"), _T(""), NULL, NULL },			// GUI_WINDOWED
	{ _T(".Tabbed"), _T(""), NULL, NULL },				// GUI_TABBED
	{ _T(".Basic"), _T(".Tabbed"), _T(""), NULL }		// GUI_BASIC
};
