//
// Skin.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "Skin.h"
#include "SkinWindow.h"
#include "CtrlCoolBar.h"
#include "CoolMenu.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "ImageServices.h"
#include "ImageFile.h"
#include "Images.h"
#include "Buffer.h"
#include "Plugins.h"
#include "WndChild.h"
#include "WndSettingsPage.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CSkin Skin;

BOOL CSkin::m_bSkinChanging = FALSE;	// Static system state indicator (Active CMainWnd::OnSkinChanged)


//////////////////////////////////////////////////////////////////////
// CSkin construction

CSkin::CSkin()
{
	// Experimental values (?)
	m_pStrings.InitHashTable( 1531 );
	m_pMenus.InitHashTable( 83 );
	m_pToolbars.InitHashTable( 61 );
	m_pDocuments.InitHashTable( 61 );
	m_pWatermarks.InitHashTable( 31 );
	m_pLists.InitHashTable( 31 );
	m_pDialogs.InitHashTable( 127 );

	CreateDefaultColors();
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

	CreateDefault();

	ApplyRecursive( L"Languages\\" );

	Plugins.RegisterCommands();
	Plugins.InsertCommands();

	ApplyRecursive( NULL );

	CoolMenu.SetWatermark( GetWatermark( _T("CCoolMenu") ) );

	Plugins.OnSkinChanged();
}

//////////////////////////////////////////////////////////////////////
// CSkin default skin

void CSkin::CreateDefault()
{
	CreateDefaultColors();

	CoolInterface.CreateFonts();

	Settings.General.Language = _T("en");
	Settings.General.LanguageRTL = false;
	Settings.General.LanguageDefault = true;

	// Default Skin Options:
	Settings.SetDefault( &Settings.Skin.MenubarHeight );	// 28
	Settings.SetDefault( &Settings.Skin.ToolbarHeight );	// 28
	Settings.SetDefault( &Settings.Skin.TaskbarHeight );	// 26
	Settings.SetDefault( &Settings.Skin.TaskbarTabWidth );	// 0	// 200/140 set in WndMain
	Settings.SetDefault( &Settings.Skin.GroupsbarHeight );	// 24
	Settings.SetDefault( &Settings.Skin.HeaderbarHeight );	// 64
	Settings.SetDefault( &Settings.Skin.MonitorbarWidth );	// 120
	Settings.SetDefault( &Settings.Skin.SidebarWidth ); 	// 200
	Settings.SetDefault( &Settings.Skin.SidebarPadding );	// 12
	Settings.SetDefault( &Settings.Skin.Splitter ); 		// 6
	Settings.SetDefault( &Settings.Skin.ButtonEdge );		// 4
	Settings.SetDefault( &Settings.Skin.LibIconsX );		// 220
	Settings.SetDefault( &Settings.Skin.LibIconsY );		// 56
	Settings.SetDefault( &Settings.Skin.MenuBorders );		// true
	Settings.SetDefault( &Settings.Skin.MenuGripper );		// true
	Settings.SetDefault( &Settings.Skin.RoundedSelect );	// false
	Settings.SetDefault( &Settings.Skin.DropMenu ); 		// false
	Settings.SetDefault( &Settings.Skin.DropMenuLabel ); 	// true
	m_ptNavBarOffset = CPoint( 0, 0 );

	// Command Icons
	//if ( HICON hIcon = theApp.LoadIcon( IDI_CHECKMARK ) )
	//{
	//	//if ( Settings.General.LanguageRTL ) hIcon = CreateMirroredIcon( hIcon );	// Impossible?
	//	CoolInterface.AddIcon( ID_CHECKMARK, hIcon );
	//	VERIFY( DestroyIcon( hIcon ) );
	//}

	// Load Definitions
	LoadFromResource( NULL, IDR_XML_DEFINITIONS );
	LoadFromResource( NULL, IDR_XML_DEFAULT );

	// Copying
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_GUIDE );
	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_UPDATE );

//	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_1 );
//	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_2 );
//	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_3 );
//	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_4 );
//	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_5 );
//	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_6 );
//	CoolInterface.CopyIcon( ID_HELP_FAQ, ID_HELP_WEB_SKINS );
}

void CSkin::CreateDefaultColors()
{
	Colors.CalculateColors( FALSE );

	// Blank NavBar Workaround (Initialize here only)
	Colors.m_crNavBarText			= CLR_NONE;
	Colors.m_crNavBarTextUp			= CLR_NONE;
	Colors.m_crNavBarTextDown		= CLR_NONE;
	Colors.m_crNavBarTextHover		= CLR_NONE;
	Colors.m_crNavBarTextChecked	= CLR_NONE;
	Colors.m_crNavBarShadow			= CLR_NONE;
	Colors.m_crNavBarShadowUp		= CLR_NONE;
	Colors.m_crNavBarShadowDown		= CLR_NONE;
	Colors.m_crNavBarShadowHover	= CLR_NONE;
	Colors.m_crNavBarShadowChecked	= CLR_NONE;
	Colors.m_crNavBarOutline		= CLR_NONE;
	Colors.m_crNavBarOutlineUp		= CLR_NONE;
	Colors.m_crNavBarOutlineDown	= CLR_NONE;
	Colors.m_crNavBarOutlineHover	= CLR_NONE;
	Colors.m_crNavBarOutlineChecked	= CLR_NONE;
}

//////////////////////////////////////////////////////////////////////
// CSkin clear

void CSkin::Clear()
{
	//CQuickLock oLock( m_pSection );

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

//	if ( m_brDialog.m_hObject ) m_brDialog.DeleteObject();
//	if ( m_brDialogPanel.m_hObject ) m_brDialogPanel.DeleteObject();
//	if ( m_brMediaSlider.m_hObject ) m_brMediaSlider.DeleteObject();

//	if ( m_bmDialog.m_hObject ) m_bmDialog.DeleteObject();
//	if ( m_bmDialogPanel.m_hObject ) m_bmDialogPanel.DeleteObject();
//	if ( m_bmToolTip.m_hObject ) m_bmToolTip.DeleteObject();
//	if ( m_bmSelected.m_hObject ) m_bmSelected.DeleteObject();

//	if ( m_bmPanelMark.m_hObject ) m_bmPanelMark.DeleteObject();
//	if ( m_bmBanner.m_hObject ) m_bmBanner.DeleteObject();

	Images.DeleteObjects();

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
				if ( pszPath == NULL && _tcsicmp( pFind.cFileName, L"languages" ) != 0 ||
					 pszPath != NULL && _tcsistr( pszPath, L"languages" ) == NULL )
				{
					strPath.Format( L"%s%s\\", pszPath ? pszPath : L"", pFind.cFileName );
					ApplyRecursive( strPath );
				}
			}
			else if ( _tcsistr( pFind.cFileName, L".xml" ) != NULL &&
					  _tcsicmp( pFind.cFileName, L"Definitions.xml" ) != 0 )
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
	BOOL bSuccess = FALSE;

	if ( ! pXML->IsNamed( _T("skin") ) )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [skin] root element"), pXML->ToString() );
		return bSuccess;
	}

	// XML Root Elements:
	SwitchMap( Text )
	{
		Text[ L"manifest" ]		= 'm';
		Text[ L"windows" ]		= 'w';
		Text[ L"windowskins" ]	= 'w';
		Text[ L"watermarks" ]	= 'e';
		Text[ L"images" ]		= 'e';
		Text[ L"icons" ] 		= 'i';
		Text[ L"commandimages" ] = 'i';
		Text[ L"colors" ]		= 'c';
		Text[ L"colours" ]		= 'c';
		Text[ L"colorscheme" ]	= 'c';
		Text[ L"colourscheme" ]	= 'c';
		Text[ L"toolbars" ]		= 't';
		Text[ L"menus" ]		= 'u';
		Text[ L"dialogs" ]		= 'a';
		Text[ L"documents" ] 	= 'd';
		Text[ L"listcolumns" ]	= 'l';
		Text[ L"options" ]		= 'o';
		Text[ L"navbar" ]		= 'v';	// Legacy
		Text[ L"fonts" ] 		= 'f';
		Text[ L"strings" ]		= 'r';
		Text[ L"commandtips" ]	= 'r';
		Text[ L"controltips" ]	= 'n';
		Text[ L"commandmap" ]	= 'p';
		Text[ L"resourcemap" ]	= 'p';
		Text[ L"tipmap" ]		= 'p';
	}

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSub = pXML->GetNextElement( pos );
		CString strElement = pSub->GetName();
		strElement.MakeLower();
		bSuccess = FALSE;

		switch ( Text[ strElement ] )
		{
		case 'w':	// windowSkins, windows
			if ( ! LoadWindowSkins( pSub, strPath ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("WindowSkins") );
			break;
		case 'e':	// watermarks, images
			if ( ! LoadWatermarks( pSub, strPath ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Watermarks") );
			break;
		case 'i':	// commandImages, icons
			if ( ! LoadCommandImages( pSub, strPath ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("CommandImages") );
			break;
		case 'c':	// colorScheme, colourScheme, colors
			if ( ! LoadColorScheme( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("ColorScheme") );
			break;
		case 't':	// toolbars
			if ( ! LoadToolbars( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Toolbars") );
			break;
		case 'u':	// menus
			if ( ! LoadMenus( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Menus") );
			break;
		case 'a':	// dialogs
			if ( ! LoadDialogs( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Dialogs") );
			break;
		case 'd':	// documents
			if ( ! LoadDocuments( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Documents") );
			break;
		case 'r':	// strings, commandTips
			if ( ! LoadStrings( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Strings") );
			break;
		case 'n':	// controltips
			if ( ! LoadControlTips( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("ControlTips") );
			break;
		case 'p':	// commandMap, resourceMap, tipMap
			if ( ! LoadResourceMap( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("ResourceMap") );
			break;
		case 'l':	// listColumns
			if ( ! LoadListColumns( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("ListColumns") );
			break;
		case 'f':	// fonts
			if ( ! LoadFonts( pSub, strPath ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Fonts") );
			break;
		case 'o':	// options
			if ( ! LoadOptions( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("Options") );
			break;
		case 'v':	// navbar  (Shareaza import only)
			if ( ! LoadNavBar( pSub ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed section"), _T("NavBar (Deprecated)") );
			break;

		case 'm':	// manifest
			if ( pSub->GetAttributeValue( _T("type") ).CompareNoCase( _T("skin") ) == 0 )
			{
				CString strSkinName = pSub->GetAttributeValue( _T("name"), _T("") );
				theApp.Message( MSG_NOTICE, IDS_SKIN_LOAD, strSkinName );
			}
			else if ( pSub->GetAttributeValue( _T("type") ).CompareNoCase( _T("language") ) == 0 )
			{
				Settings.General.Language = pSub->GetAttributeValue( _T("language"), _T("en") );
				Settings.General.LanguageRTL = ( pSub->GetAttributeValue( _T("dir"), _T("ltr") ) == "rtl" );
				Settings.General.LanguageDefault = Settings.General.Language.Left(2) == _T("en");
				TRACE( _T("Loading language: %s\r\n"), Settings.General.Language );
				TRACE( _T("RTL: %d\r\n"), Settings.General.LanguageRTL );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [type] attribute in [manifest] element"), pSub->ToString() );
			}
			break;

		default:
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in root [skin] element"), pSub->ToString() );
			continue;
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
		return FALSE;	// Popup menus

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

				for ( ;; )
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
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [id] attribute in [string] element"), pXML->ToString() );
			}
		}
		else if ( pXML->IsNamed( _T("tip") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CString strMessage = pXML->GetAttributeValue( _T("message") );
				CString strTip = pXML->GetAttributeValue( _T("tip") );
				if ( ! strTip.IsEmpty() ) strMessage += '\n' + strTip;
				m_pStrings.SetAt( nID, strMessage );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [id] attribute in [tip] element"), pXML->ToString() );
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [strings] element"), pXML->ToString() );
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

		if ( ! pXML->IsNamed( _T("option") ) )
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in skin [Options]"), pXML->ToString() );
			continue;		// Failed, but keep trying
		}

		const CString strName	= pXML->GetAttributeValue( _T("name") ).MakeLower();
		const CString strValue	= pXML->GetAttributeValue( _T("value") ).MakeLower();
		const CString strHeight	= pXML->GetAttributeValue( _T("height") );
		const CString strWidth	= pXML->GetAttributeValue( _T("width") );

		// Skin Options:
		SwitchMap( Text )
		{
			Text[ L"navbar" ]		= 'n';
			Text[ L"dropmenu" ]		= 'd';
			Text[ L"dropdownmenu" ]	= 'd';
			Text[ L"submenu" ]		= 'd';
			Text[ L"menubar" ]		= 'm';
			Text[ L"menubars" ]		= 'm';
			Text[ L"menubarbevel" ]	= 'b';
			Text[ L"menuborders" ]	= 'b';
			Text[ L"menugripper" ]	= 'p';
			Text[ L"grippers" ] 	= 'p';
			Text[ L"toolbar" ]		= 't';
			Text[ L"toolbars" ]		= 't';
			Text[ L"taskbar" ]		= 'k';
			Text[ L"tabbar" ]		= 'k';
			Text[ L"sidebar" ]		= 's';
			Text[ L"sidepanel" ] 	= 's';
			Text[ L"taskpanel" ] 	= 's';
			Text[ L"taskboxpadding" ] = 'a';
			Text[ L"sidebarpadding" ] = 'a';
			Text[ L"sidebarmargin" ]  = 'a';
			Text[ L"titlebar" ]		= 'h';
			Text[ L"headerpanel" ]	= 'h';
			Text[ L"groupsbar" ] 	= 'g';
			Text[ L"downloadgroups" ] = 'g';
			Text[ L"bandwidthwidget" ] = 'o';
			Text[ L"monitorbar" ]	= 'o';
			Text[ L"dragbar" ]		= 'r';
			Text[ L"splitter" ]		= 'r';
			Text[ L"listitem" ]		= 'w';
			Text[ L"rowsize" ]		= 'w';
			Text[ L"roundedselect" ] = 'c';
			Text[ L"highlightchamfer" ] = 'c';
			Text[ L"buttonedge" ]	= 'e';
			Text[ L"buttonmap" ] 	= 'e';
			Text[ L"icongrid" ]		= 'i';
			Text[ L"librarytiles" ]	= 'i';
		}

		switch ( Text[ strName ] )
		{
		case 'n':	// "Navbar"
			if ( ! LoadNavBar( pXML ) )
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Skin Option [Navbar] Failed"), pXML->ToString() );
			break;
		case 'd':	// "DropMenu" or "SubMenu"
			if ( strValue == _T("true") )
				Settings.Skin.DropMenu = true;
			else if ( strValue == _T("false") )
				Settings.Skin.DropMenu = false;
			else if ( strValue == _T("on") )
				Settings.Skin.DropMenu = true;
			else if ( strValue == _T("off") )
				Settings.Skin.DropMenu = false;
			else if ( strValue == _T("1") )
				Settings.Skin.DropMenu = true;
			else if ( strValue == _T("0") )
				Settings.Skin.DropMenu = false;
			else if ( ! strValue.IsEmpty() && strValue.GetLength() < 3 )
				Settings.Skin.DropMenuLabel = _wtoi(strValue);
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.DropMenuLabel = _wtoi(strWidth);
			if ( Settings.Skin.DropMenuLabel > 100 )
				Settings.Skin.DropMenuLabel = 0;
			else if ( Settings.Skin.DropMenuLabel > 1 )
				Settings.Skin.DropMenu = true;
			break;
		case 'b':	// "MenuBorders" or "MenubarBevel"
			if ( strValue == _T("true") )
				Settings.Skin.MenuBorders = true;
			else if ( strValue == _T("false") )
				Settings.Skin.MenuBorders = false;
			else if ( strValue == _T("on") )
				Settings.Skin.MenuBorders = true;
			else if ( strValue == _T("off") )
				Settings.Skin.MenuBorders = false;
			else if ( strValue == _T("1") )
				Settings.Skin.MenuBorders = true;
			else if ( strValue == _T("0") )
				Settings.Skin.MenuBorders = false;
			break;
		case 'p':	// "MenuGripper" or "Grippers"
			if ( strValue == _T("true") )
				Settings.Skin.MenuGripper = true;
			else if ( strValue == _T("false") )
				Settings.Skin.MenuGripper = false;
			else if ( strValue == _T("on") )
				Settings.Skin.MenuGripper = true;
			else if ( strValue == _T("off") )
				Settings.Skin.MenuGripper = false;
			else if ( strValue == _T("1") )
				Settings.Skin.MenuGripper = true;
			else if ( strValue == _T("0") )
				Settings.Skin.MenuGripper = false;
			break;
		case 'c':	// "RoundedSelect" or "HighlightChamfer"
			if ( strValue == _T("true") )
				Settings.Skin.RoundedSelect = true;
			else if ( strValue == _T("false") )
				Settings.Skin.RoundedSelect = false;
			else if ( strValue == _T("on") )
				Settings.Skin.RoundedSelect = true;
			else if ( strValue == _T("off") )
				Settings.Skin.RoundedSelect = false;
			else if ( strValue == _T("1") )
				Settings.Skin.RoundedSelect = true;
			else if ( strValue == _T("0") )
				Settings.Skin.RoundedSelect = false;
			break;
		case 'm':	// "Menubar" or "Menubars"
			if ( ! strHeight.IsEmpty() )
				Settings.Skin.MenubarHeight = _wtoi(strHeight);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.MenubarHeight = _wtoi(strValue);
			break;
		case 't':	// "Toolbar" or "Toolbars"
			if ( ! strHeight.IsEmpty() )
				Settings.Skin.ToolbarHeight = _wtoi(strHeight);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.ToolbarHeight = _wtoi(strValue);
			break;
		case 'k':	// "Taskbar" or "TabBar"
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.TaskbarTabWidth = _wtoi(strWidth);
			if ( ! strHeight.IsEmpty() )
				Settings.Skin.TaskbarHeight = _wtoi(strHeight);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.TaskbarHeight = _wtoi(strValue);
			break;
		case 's':	// "Sidebar" or "SidePanel" or "TaskPanel"
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.SidebarWidth = _wtoi(strWidth);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.SidebarWidth = _wtoi(strValue);
			break;
		case 'a':	// "SidebarMargin" or "SidebarPadding" or "TaskPanelPadding"
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.SidebarPadding = _wtoi(strWidth);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.SidebarPadding = _wtoi(strValue);
			break;
		case 'h':	// "Titlebar" or "HeaderPanel"
			if ( ! strHeight.IsEmpty() )
				Settings.Skin.HeaderbarHeight = _wtoi(strHeight);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.HeaderbarHeight = _wtoi(strValue);
			break;
		case 'g':	// "Groupsbar" or "DownloadGroups"
			if ( ! strHeight.IsEmpty() )
				Settings.Skin.GroupsbarHeight = _wtoi(strHeight);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.GroupsbarHeight = _wtoi(strValue);
			break;
		case 'o':	// "Monitorbar" or "BandwidthWidget"
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.MonitorbarWidth = _wtoi(strWidth);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.MonitorbarWidth = _wtoi(strValue);
			break;
		case 'r':	// "Dragbar" or "Splitter"
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.Splitter = _wtoi(strWidth);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.Splitter = _wtoi(strValue);
			break;
		case 'e':	// "ButtonEdge" or "ButtonMap"
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.ButtonEdge = _wtoi(strWidth);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.ButtonEdge = _wtoi(strValue);
			break;
		case 'i':	// "IconGrid" or "LibraryTiles"
			if ( ! strHeight.IsEmpty() )
				Settings.Skin.LibIconsY = _wtoi(strHeight);
			if ( ! strWidth.IsEmpty() )
				Settings.Skin.LibIconsX = _wtoi(strWidth);
			else if ( ! strValue.IsEmpty() )
				Settings.Skin.LibIconsX = _wtoi(strValue);
			break;
		case 'w':	// "RowSize" or "ListItem"
			{
				int nSize;
				if ( ! strHeight.IsEmpty() )
					nSize = _wtoi(strHeight);
				else if ( ! strValue.IsEmpty() )
					nSize = _wtoi(strValue);
				else
					break;
				if ( nSize >= 16 && nSize <= 20 )
					Settings.Skin.RowSize = nSize;
			}
			break;
		}
	}

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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [menu] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

BOOL CSkin::LoadMenu(CXMLElement* pXML)
{
	CString strName = pXML->GetAttributeValue( _T("name") );
	if ( strName.IsEmpty() )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("No [name] attribute in [menu] element"), pXML->ToString() );
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
		if ( ! pMenu->CreateMenu() )
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Cannot create menu"), pXML->ToString() );
			return FALSE;
		}
	}
	else if ( ! pMenu->CreatePopupMenu() )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Cannot create popup menu"), pXML->ToString() );
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

				if ( ! strKeys.IsEmpty() ) strText += '\t' + strKeys;

				VERIFY( AppendMenu( hMenu, MF_STRING, nID, strText ) );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [id] attribute in menu [item] element"), pXML->ToString() );
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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [menu] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin toolbars

BOOL CSkin::LoadNavBar(CXMLElement* pBase)
{
	CString strValue = pBase->GetAttributeValue( _T("offset") );
	if ( ! strValue.IsEmpty() )
	{
		if ( _stscanf( strValue, _T("%i,%i"), &m_ptNavBarOffset.x, &m_ptNavBarOffset.y ) != 2 )
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Bad [offset] attribute in [navbar] element"), pBase->ToString() );
	}

	strValue = pBase->GetAttributeValue( _T("mode") );
	if ( strValue.IsEmpty() )
		strValue = pBase->GetAttributeValue( _T("case") );

	if ( strValue.CompareNoCase( _T("upper") ) == 0 )
		m_NavBarMode = NavBarUpper;
	else if ( strValue.CompareNoCase( _T("lower") ) == 0 )
		m_NavBarMode = NavBarLower;
	else
		m_NavBarMode = NavBarNormal;

	return TRUE;
}

BOOL CSkin::CreateToolBar(LPCTSTR pszName, CCoolBarCtrl* pBar)
{
	//CQuickLock oLock( m_pSection );

	if ( pszName == NULL ) return FALSE;

	if ( pBar->m_hWnd )
	{
		for ( CWnd* pChild = pBar->GetWindow( GW_CHILD ) ; pChild ; pChild = pChild->GetNextWindow() )
		{
			pChild->ShowWindow( SW_HIDE );
		}
	}
	pBar->SetWatermark( NULL );
	pBar->Clear();

	ASSERT( Settings.General.GUIMode == GUI_WINDOWED ||
		Settings.General.GUIMode == GUI_TABBED ||
		Settings.General.GUIMode == GUI_BASIC );
	LPCTSTR* pszModeSuffix = m_pszModeSuffix[ Settings.General.GUIMode ];
	CString strClassName( pszName );

	CCoolBarCtrl* pBase = NULL;
	for ( int nModeTry = 0 ; nModeTry < 3 && pszModeSuffix[ nModeTry ] ; nModeTry++ )
	{
		CString strName( strClassName + pszModeSuffix[ nModeTry ] );
		if ( m_pToolbars.Lookup( strName, pBase ) )
		{
		//	if ( strName.Left( 10 ) == _T("CSearchWnd") )
		//		continue; 	// Crash Workaround?
			if ( HBITMAP hBitmap = GetWatermark( strName + _T(".Toolbar") ) )
				pBar->SetWatermark( hBitmap );
			else if ( HBITMAP hBitmap = GetWatermark( strClassName + _T(".Toolbar") ) )
				pBar->SetWatermark( hBitmap );
			else if ( HBITMAP hBitmap = GetWatermark( _T("System.Toolbars") ) )		// ToDo: Images.m_bmToolbar
				pBar->SetWatermark( hBitmap );

			pBar->Copy( pBase );
			return TRUE;
		}
	}

//	ASSERT( pBase != NULL );

	return FALSE;
}

CCoolBarCtrl* CSkin::GetToolBar(LPCTSTR pszName) const
{
	//CQuickLock oLock( m_pSection );

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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [toolbars] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

CCoolBarCtrl* CSkin::CreateToolBar(LPCTSTR pszName)
{
	//CQuickLock oLock( m_pSection );

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
				if ( ! strTemp.IsEmpty() )
					pItem->SetTip( strTemp );

				strTemp = pXML->GetAttributeValue( L"visible", L"true" );
				if ( strTemp.CompareNoCase( L"false" ) == 0 )
					pItem->Show( FALSE );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [id] attribute in [button] element"), pXML->ToString() );
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
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [id] attribute in [control] element"), pXML->ToString() );
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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [toolbar] element"), pXML->ToString() );
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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [documents] element"), pDoc->ToString() );
		}
	}

	return TRUE;
}

CXMLElement* CSkin::GetDocument(LPCTSTR pszName)
{
	//CQuickLock oLock( m_pSection );

	CXMLElement* pXML = NULL;

	if ( m_pDocuments.Lookup( pszName, pXML ) ) return pXML;

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CSkin watermarks

HBITMAP CSkin::GetWatermark(LPCTSTR pszName)
{
	//CQuickLock oLock( m_pSection );

	CString strPath;
	if ( m_pWatermarks.Lookup( pszName, strPath ) && ! strPath.IsEmpty() )
	{
		if ( HBITMAP hBitmap = LoadBitmap( strPath ) )
			return hBitmap;

		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed to load watermark"), CString( pszName ) + _T(". File: ") + strPath );
	}
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

		if ( pMark->IsNamed( _T("watermark") ) || pMark->IsNamed( _T("image") ) )
		{
			CString strName = pMark->GetAttributeValue( _T("target") );
			CString strFile = pMark->GetAttributeValue( _T("path") );

			if ( ! strName.IsEmpty() )
			{
				if ( ! strFile.IsEmpty() )
					strFile = strPath + strFile;
				m_pWatermarks.SetAt( strName, strFile );
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Missing [target] attribute in [watermark] element"), pMark->ToString() );
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [watermarks] element"), pMark->ToString() );
		}
	}


	// Common system-wide volatile bitmaps (buttons):
	Images.Load();

// Obsolete: Moved to Images

//	if ( m_bmSelected.m_hObject ) m_bmSelected.DeleteObject();
//	if ( HBITMAP hSelected = GetWatermark( _T("System.Highlight") ) )
//		m_bmSelected.Attach( hSelected );
//	else if ( HBITMAP hSelected = GetWatermark( _T("CTransfers.Selected") ) )
//		m_bmSelected.Attach( hSelected );
//
//	if ( m_bmSelectedGrey.m_hObject ) m_bmSelectedGrey.DeleteObject();
//	if ( HBITMAP hSelected = GetWatermark( _T("System.Highlight.Inactive") ) )
//		m_bmSelectedGrey.Attach( hSelected );
//	else if ( HBITMAP hSelected = GetWatermark( _T("CTransfers.Selected.Inactive") ) )
//		m_bmSelectedGrey.Attach( hSelected );
//
//	if ( m_bmToolTip.m_hObject ) m_bmToolTip.DeleteObject();
//	if ( HBITMAP hToolTip = GetWatermark( _T("System.ToolTip") ) )
//		m_bmToolTip.Attach( hToolTip );
//	else if ( HBITMAP hToolTip = GetWatermark( _T("System.Tooltips") ) )
//		m_bmToolTip.Attach( hToolTip );
//
//	if ( m_bmDialog.m_hObject ) m_bmDialog.DeleteObject();
//	if ( HBITMAP hDialog = GetWatermark( _T("System.Dialogs") ) )
//		m_bmDialog.Attach( hDialog );
//	else if ( HBITMAP hDialog = GetWatermark( _T("CDialog") ) )
//		m_bmDialog.Attach( hDialog );
//
//	if ( m_bmDialogPanel.m_hObject ) m_bmDialogPanel.DeleteObject();
//	if ( HBITMAP hDialog = GetWatermark( _T("System.DialogPanels") ) )
//		m_bmDialogPanel.Attach( hDialog );
//	else if ( HBITMAP hDialog = GetWatermark( _T("CDialog.Panel") ) )
//		m_bmDialogPanel.Attach( hDialog );
//
//	if ( m_bmPanelMark.m_hObject != NULL ) m_bmPanelMark.DeleteObject();
//	if ( HBITMAP hPanelMark = GetWatermark( _T("CPanelWnd.Caption") ) )
//		m_bmPanelMark.Attach( hPanelMark );
//	else if ( Colors.m_crPanelBack == RGB_DEFAULT_CASE )
//		m_bmPanelMark.LoadBitmap( IDB_PANEL_MARK );				// Default resource handling

	// Related brushes:

//	// Skinnable Dialogs  (This brush applies to text bg.  Body in DlgSkinDialog, WndSettingPage, etc.)
//	if ( Images.m_brDialog.m_hObject ) Images.m_brDialog.DeleteObject();
//	if ( Images.m_bmDialog.m_hObject )
//		Images.m_brDialog.CreatePatternBrush( & Images.m_bmDialog );	//Attach( (HBRUSH)GetStockObject( NULL_BRUSH ) );
//	else
//		Images.m_brDialog.CreateSolidBrush( Colors.m_crDialog );
//
//	if ( Images.m_brDialogPanel.m_hObject ) Images.m_brDialogPanel.DeleteObject();
//	if ( Images.m_bmDialogPanel.m_hObject )
//		Images.m_brDialogPanel.CreatePatternBrush( & Images.m_bmDialogPanel );
//	else
//		Images.m_brDialogPanel.CreateSolidBrush( Colors.m_crDialogPanel );
//
//	if ( Images.m_brMediaSlider.m_hObject ) Images.m_brMediaSlider.DeleteObject();
//	if ( HBITMAP hSlider = GetWatermark( _T("CCoolbar.Control") ) )
//	{
//		CBitmap bmSlider;
//		bmSlider.Attach( hSlider );
//		Images.m_brMediaSlider.CreatePatternBrush( &bmSlider );
//	}
//	else if ( HBITMAP hSlider = GetWatermark( _T("CMediaFrame.Slider") ) )
//	{
//		CBitmap bmSlider;
//		bmSlider.Attach( hSlider );
//		Images.m_brMediaSlider.CreatePatternBrush( &bmSlider );
//	}
//	else
//		Images.m_brMediaSlider.CreateSolidBrush( Colors.m_crMidtone );

	// Dialog Header:

//	m_nBanner = 0;	// Defined in Images
//	if ( m_bmBanner.m_hObject ) m_bmBanner.DeleteObject();
//	if ( HBITMAP hBanner = GetWatermark( _T("System.Header") ) )
//	{
//		BITMAP bmInfo;
//		m_bmBanner.Attach( hBanner );
//		m_bmBanner.GetObject( sizeof( BITMAP ), &bmInfo );
//		m_bmBanner.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
//		m_nBanner = bmInfo.bmHeight;
//	}
//	else if ( HBITMAP hBanner = GetWatermark( _T("Banner") ) )
//	{
//		BITMAP bmInfo;
//		m_bmBanner.Attach( hBanner );
//		m_bmBanner.GetObject( sizeof( BITMAP ), &bmInfo );
//		m_bmBanner.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
//		m_nBanner = bmInfo.bmHeight;
//	}

	// "System.Toolbars" fallback at toolbar creation
	// Button states in CImages

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

					if ( ! strEdit.IsEmpty() ) strEdit += '|';
					strEdit += strFrom;
					strEdit += '=';
					strEdit += strTo;
				}
				else
				{
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [list] element"), pCol->ToString() );
				}
			}

			m_pLists.SetAt( strName, strEdit );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [listColumns] element"), pXML->ToString() );
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

	if ( pszName )
		strName = pszName;
	else
		strName = pDialog->GetRuntimeClass()->m_lpszClassName;

	if ( Settings.General.DialogScan )
		return Dialogscan( pDialog, strName );

	CXMLElement* pBase = NULL;
	if ( ! m_pDialogs.Lookup( strName, pBase ) )
		return FALSE;	// Naked dialog

	CString strCookie = GetDialogCookie( pDialog, pWndTooltips );	// Also parses default tips

	if ( strCookie != pBase->GetAttributeValue( _T("cookie") ) )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Invalid [cookie] attribute in [dialog] element, use: ") + strCookie, pBase->ToString() );
		return FALSE;
	}

	CString strTip;
	CString strCaption = pBase->GetAttributeValue( _T("caption") );
	if ( ! strCaption.IsEmpty() ) pDialog->SetWindowText( strCaption );

	CWnd* pWnd = pDialog->GetWindow( GW_CHILD );

	for ( POSITION pos = pBase->GetElementIterator() ; pos && pWnd ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		TCHAR szClass[3] = { 0, 0, 0 };
		GetClassName( pWnd->GetSafeHwnd(), szClass, 3 );

		// Skip added banner
		if ( _tcsnicmp( szClass, _T("St"), 3 ) == 0 &&
			IDC_BANNER == pWnd->GetDlgCtrlID() )
		{
			pWnd = pWnd->GetNextWindow();
			if ( ! pWnd )
				break;
		}

		// Needed for some controls like Schema combo box
		if ( Settings.General.LanguageRTL && (CString)szClass != "Ed" )
			pWnd->ModifyStyleEx( 0, WS_EX_LAYOUTRTL|WS_EX_RTLREADING, 0 );

		if ( pXML->IsNamed( _T("control") ) )
		{
			if ( pWndTooltips )
			{
				strTip = pXML->GetAttributeValue( L"tip" );
				if ( ! strTip.IsEmpty() )
					pWndTooltips->AddTool( pWnd, strTip );
			}

			strCaption = pXML->GetAttributeValue( _T("caption") );
			if ( ! strCaption.IsEmpty() )
			{
				strCaption.Replace( _T("{n}"), _T("\r\n") );

				if ( (CString)szClass != "Co" )
				{
					int nPos = strCaption.Find( '_' );
					if ( nPos >= 0 ) strCaption.SetAt( nPos, '&' );
					pWnd->SetWindowText( strCaption );
				}
				else
				{
					CComboBox* pCombo = (CComboBox*)pWnd;
					int nNum = pCombo->GetCount();

					CStringArray pItems;
					Split( strCaption, _T('|'), pItems, TRUE );

					if ( nNum == pItems.GetSize() )
					{
						int nCurSel = pCombo->GetCurSel();
						pCombo->ResetContent();
						for ( int i = 0 ; i < nNum ; ++i )
						{
							pCombo->AddString( pItems.GetAt( i ) );
						}
						pCombo->SetCurSel( nCurSel );
					}
				}
			}

			pWnd = pWnd->GetNextWindow();
		}
	}

	return TRUE;
}

BOOL CSkin::Dialogscan(CDialog* pDialog, CString sName /*=""*/)
{
	CStdioFile pFile;

	if ( pFile.Open( Settings.General.Path + _T("\\Dialogs.xml"), CFile::modeReadWrite ) )
		pFile.Seek( 0, CFile::end );
	else if ( pFile.Open( Settings.General.Path + _T("\\Dialogs.xml"), CFile::modeWrite|CFile::modeCreate ) )
		pFile.WriteString( _T("<dialogs>\r\n") );
	else
		return FALSE;

	// Obsolete CFile method, for reference:
	//pFile.Write( "\t<dialog name=\"", 15 );
	//int nBytes = WideCharToMultiByte( CP_ACP, 0, strName, strName.GetLength(), NULL, 0, NULL, NULL );
	//LPSTR pBytes = new CHAR[nBytes];
	//WideCharToMultiByte( CP_ACP, 0, strName, strName.GetLength(), pBytes, nBytes, NULL, NULL );
	//pFile.Write( pBytes, nBytes );
	//delete [] pBytes;

	if ( sName.IsEmpty() )
		sName = pDialog->GetRuntimeClass()->m_lpszClassName;

	CString strCaption, strTip;
	pDialog->GetWindowText( strCaption );
	strCaption.Replace( _T("\n"), _T("{n}") );
	strCaption.Replace( _T("\r"), _T("") );
	strCaption.Replace( _T("&"), _T("_") );
	strCaption = Escape( strCaption );

	pFile.WriteString( _T("\t<dialog name=\"") );
	pFile.WriteString( sName );

	pFile.WriteString( _T("\" cookie=\"") );
	pFile.WriteString( GetDialogCookie( pDialog ) );

	pFile.WriteString( _T("\" caption=\"") );
	pFile.WriteString( strCaption );

	pFile.WriteString( _T("\">\r\n") );

	for ( CWnd* pWnd = pDialog->GetWindow( GW_CHILD ) ; pWnd ; pWnd = pWnd->GetNextWindow() )
	{
		TCHAR szClass[64];
		GetClassName( pWnd->GetSafeHwnd(), szClass, 64 );

		// Skip added banner
		if ( _tcsnicmp( szClass, _T("St"), 3 ) == 0 &&
			 pWnd->GetDlgCtrlID() == IDC_BANNER )
			continue;

		strCaption.Empty();
		strTip.Empty();
		LoadControlTip( strTip, pWnd->GetDlgCtrlID() );

		if ( _tcsistr( szClass, _T("Static") ) ||
			 _tcsistr( szClass, _T("Button") ) )
		{
			pWnd->GetWindowText( strCaption );
		}
		else if ( _tcsistr( szClass, _T("ListBox") ) )
		{
			CListBox* pListBox = static_cast< CListBox* >( pWnd );
			for ( int i = 0 ; i < pListBox->GetCount() ; ++i )
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
			for ( int i = 0 ; i < pComboBox->GetCount() ; ++i )
			{
				CString strTemp;
				pComboBox->GetLBText( i, strTemp );
				if ( ! strCaption.IsEmpty() )
					strCaption += _T('|');
				strCaption += strTemp;
			}
		}

		pFile.WriteString( _T("\t\t<control") );

		if ( ! strCaption.IsEmpty() )
		{
			strCaption.Replace( _T("\n"), _T("{n}") );
			strCaption.Replace( _T("\r"), _T("") );
			strCaption.Replace( _T("&"), _T("_") );
			strCaption = Escape( strCaption );
			pFile.WriteString( _T(" caption=\"") );
			pFile.WriteString( strCaption );
			pFile.WriteString( _T("\"") );
		}

		if ( ! strTip.IsEmpty() )
		{
			strTip.Replace( _T("\n"), _T("{n}") );
			strTip.Replace( _T("\r"), _T("") );
			pFile.WriteString( _T(" tip=\"") );
			pFile.WriteString( strTip );
			pFile.WriteString( _T("\"") );
		}

		pFile.WriteString( _T("/>\r\n") );
	}

	pFile.WriteString( _T("\t</dialog>\r\n") );
	//pFile.Close();

	return TRUE;
}

CString CSkin::GetDialogCookie(CDialog* pDialog, CToolTipCtrl* pWndTooltips /*=NULL*/)
{
	CString strCookie, strTip;

	for ( CWnd* pWnd = pDialog->GetWindow( GW_CHILD ) ; pWnd ; pWnd = pWnd->GetNextWindow() )
	{
		pWnd->SetFont( &CoolInterface.m_fntNormal );

		if ( pWndTooltips )
		{
			LoadControlTip( strTip, pWnd->GetDlgCtrlID() );
			if ( ! strTip.IsEmpty() )
				pWndTooltips->AddTool( pWnd, strTip );
		}

		TCHAR szClass[3] = { 0, 0, 0 };
		GetClassName( pWnd->GetSafeHwnd(), szClass, 3 );

		// Skip added banner
		if ( _tcsnicmp( szClass, _T("St"), 3 ) == 0 &&
			 pWnd->GetDlgCtrlID() == IDC_BANNER )
			continue;

		// Skip settings pages
		if ( pWnd->IsKindOf( RUNTIME_CLASS( CSettingsPage ) ) )
			continue;

		strCookie += szClass;	// Cookie
	}

	return strCookie;
}

CString CSkin::GetDialogCaption(LPCTSTR pszName)
{
	//CQuickLock oLock( m_pSection );

	CXMLElement* pBase = NULL;
	CString strCaption;

	if ( m_pDialogs.Lookup( pszName, pBase ) )
		strCaption = pBase->GetAttributeValue( _T("caption") );

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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [dialogs] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin window skins

CSkinWindow* CSkin::GetWindowSkin(LPCTSTR pszWindow, LPCTSTR pszAppend)
{
	//CQuickLock oLock( m_pSection );

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
		CString strClassName( pClass->m_lpszClassName );

		if ( bPanel )
		{
			CSkinWindow* pSkin = GetWindowSkin( (LPCTSTR)strClassName, _T(".Panel") );
			if ( pSkin != NULL ) return pSkin;
		}

		for ( int nSuffix = 0 ; nSuffix < 3 && pszModeSuffix[ nSuffix ] != NULL ; nSuffix ++ )
		{
			if ( pszModeSuffix[ nSuffix ][0] != 0 || ! bPanel )
			{
				CSkinWindow* pSkin = GetWindowSkin( (LPCTSTR)strClassName, pszModeSuffix[ nSuffix ] );
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

		if ( pSkinElement->IsNamed( _T("windowSkin") ) || pSkinElement->IsNamed( _T("window") ) )
		{
			CSkinWindow* pSkin = new CSkinWindow();

			if ( pSkin->Parse( pSkinElement, strPath ) )
				m_pSkins.AddHead( pSkin );
			else
				delete pSkin;
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [windowSkins] element"), pSkinElement->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin Color Scheme

BOOL CSkin::LoadColorScheme(CXMLElement* pBase)
{
	CMapStringToPtr pColors;

	pColors.SetAt( _T("system.base.window"), &Colors.m_crWindow );
	pColors.SetAt( _T("system.base.midtone"), &Colors.m_crMidtone );
	pColors.SetAt( _T("system.base.text"), &Colors.m_crText );
	pColors.SetAt( _T("system.base.hitext"), &Colors.m_crHiText );
	pColors.SetAt( _T("system.base.hiborder"), &Colors.m_crHiBorder );
	pColors.SetAt( _T("system.base.hiborderin"), &Colors.m_crHiBorderIn );
	pColors.SetAt( _T("system.base.highlight"), &Colors.m_crHighlight );
	pColors.SetAt( _T("system.back.normal"), &Colors.m_crBackNormal );
	pColors.SetAt( _T("system.back.selected"), &Colors.m_crBackSel );
	pColors.SetAt( _T("system.back.checked"), &Colors.m_crBackCheck );
	pColors.SetAt( _T("system.back.checked.selected"), &Colors.m_crBackCheckSel );
	pColors.SetAt( _T("system.margin"), &Colors.m_crMargin );
	pColors.SetAt( _T("system.border"), &Colors.m_crBorder );
	pColors.SetAt( _T("system.shadow"), &Colors.m_crShadow );
	pColors.SetAt( _T("system.text"), &Colors.m_crCmdText );
	pColors.SetAt( _T("system.text.selected"), &Colors.m_crCmdTextSel );
	pColors.SetAt( _T("system.disabled"), &Colors.m_crDisabled );

	pColors.SetAt( _T("tooltip.back"), &Colors.m_crTipBack );
	pColors.SetAt( _T("tooltip.text"), &Colors.m_crTipText );
	pColors.SetAt( _T("tooltip.graph"), &Colors.m_crTipGraph );
	pColors.SetAt( _T("tooltip.grid"), &Colors.m_crTipGraphGrid );
	pColors.SetAt( _T("tooltip.border"), &Colors.m_crTipBorder );
	pColors.SetAt( _T("tooltip.warnings"), &Colors.m_crTipWarnings );

	pColors.SetAt( _T("taskpanel.back"), &Colors.m_crTaskPanelBack );
	pColors.SetAt( _T("taskbox.caption.back"), &Colors.m_crTaskBoxCaptionBack );
	pColors.SetAt( _T("taskbox.caption.text"), &Colors.m_crTaskBoxCaptionText );
	pColors.SetAt( _T("taskbox.caption.hover"), &Colors.m_crTaskBoxCaptionHover );
	pColors.SetAt( _T("taskbox.primary.back"), &Colors.m_crTaskBoxPrimaryBack );
	pColors.SetAt( _T("taskbox.primary.text"), &Colors.m_crTaskBoxPrimaryText );
	pColors.SetAt( _T("taskbox.client"), &Colors.m_crTaskBoxClient );	// Deprecated
	pColors.SetAt( _T("taskbox.back"), &Colors.m_crTaskBoxClient );
	pColors.SetAt( _T("taskbox.text"), &Colors.m_crTaskBoxText );

	pColors.SetAt( _T("dialog.back"), &Colors.m_crDialog );
	pColors.SetAt( _T("dialog.text"), &Colors.m_crDialogText );
	pColors.SetAt( _T("dialog.menu.back"), &Colors.m_crDialogMenu );
	pColors.SetAt( _T("dialog.menu.text"), &Colors.m_crDialogMenuText );
	pColors.SetAt( _T("dialog.panel.back"), &Colors.m_crDialogPanel );
	pColors.SetAt( _T("dialog.panel.text"), &Colors.m_crDialogPanelText );
	pColors.SetAt( _T("panel.caption.back"), &Colors.m_crPanelBack );
	pColors.SetAt( _T("panel.caption.text"), &Colors.m_crPanelText );
	pColors.SetAt( _T("panel.caption.border"), &Colors.m_crPanelBorder );
	pColors.SetAt( _T("banner.back"), &Colors.m_crBannerBack );
	pColors.SetAt( _T("banner.text"), &Colors.m_crBannerText );
	pColors.SetAt( _T("schema.row1"), &Colors.m_crSchemaRow[0] );
	pColors.SetAt( _T("schema.row2"), &Colors.m_crSchemaRow[1] );

//	Active window color is controlled by media player plugin, thus we can not skin it?
	pColors.SetAt( _T("media.window"), &Colors.m_crMediaWindowBack );
	pColors.SetAt( _T("media.window.back"), &Colors.m_crMediaWindowBack );
	pColors.SetAt( _T("media.window.text"), &Colors.m_crMediaWindowText );
	pColors.SetAt( _T("media.status"), &Colors.m_crMediaStatusBack );
	pColors.SetAt( _T("media.status.back"), &Colors.m_crMediaStatusBack );
	pColors.SetAt( _T("media.status.text"), &Colors.m_crMediaStatusText );
	pColors.SetAt( _T("media.panel"), &Colors.m_crMediaPanelBack );
	pColors.SetAt( _T("media.panel.back"), &Colors.m_crMediaPanelBack );
	pColors.SetAt( _T("media.panel.text"), &Colors.m_crMediaPanelText );
	pColors.SetAt( _T("media.panel.active"), &Colors.m_crMediaPanelActiveBack );
	pColors.SetAt( _T("media.panel.active.back"), &Colors.m_crMediaPanelActiveBack );
	pColors.SetAt( _T("media.panel.active.text"), &Colors.m_crMediaPanelActiveText );
	pColors.SetAt( _T("media.panel.caption"), &Colors.m_crMediaPanelCaptionBack );
	pColors.SetAt( _T("media.panel.caption.back"), &Colors.m_crMediaPanelCaptionBack );
	pColors.SetAt( _T("media.panel.caption.text"), &Colors.m_crMediaPanelCaptionText );

	pColors.SetAt( _T("traffic.window.back"), &Colors.m_crTrafficWindowBack );
	pColors.SetAt( _T("traffic.window.text"), &Colors.m_crTrafficWindowText );
	pColors.SetAt( _T("traffic.window.grid"), &Colors.m_crTrafficWindowGrid );

	pColors.SetAt( _T("monitor.history.back"), &Colors.m_crMonitorHistoryBack );
	pColors.SetAt( _T("monitor.history.back.max"), &Colors.m_crMonitorHistoryBackMax );
	pColors.SetAt( _T("monitor.history.text"), &Colors.m_crMonitorHistoryText );
	pColors.SetAt( _T("monitor.download.line"), &Colors.m_crMonitorDownloadLine );
	pColors.SetAt( _T("monitor.upload.line"), &Colors.m_crMonitorUploadLine );
	pColors.SetAt( _T("monitor.download.bar"), &Colors.m_crMonitorDownloadBar );
	pColors.SetAt( _T("monitor.upload.bar"), &Colors.m_crMonitorUploadBar );
	pColors.SetAt( _T("monitor.graph.border"), &Colors.m_crMonitorGraphBorder );
	pColors.SetAt( _T("monitor.graph.back"), &Colors.m_crMonitorGraphBack );
	pColors.SetAt( _T("monitor.graph.grid"), &Colors.m_crMonitorGraphGrid );
	pColors.SetAt( _T("monitor.graph.line"), &Colors.m_crMonitorGraphLine );

	pColors.SetAt( _T("schema.rating"), &Colors.m_crRatingNull );
	pColors.SetAt( _T("schema.rating0"), &Colors.m_crRating0 );
	pColors.SetAt( _T("schema.rating1"), &Colors.m_crRating1 );
	pColors.SetAt( _T("schema.rating2"), &Colors.m_crRating2 );
	pColors.SetAt( _T("schema.rating3"), &Colors.m_crRating3 );
	pColors.SetAt( _T("schema.rating4"), &Colors.m_crRating4 );
	pColors.SetAt( _T("schema.rating5"), &Colors.m_crRating5 );

	pColors.SetAt( _T("meta.row"), &Colors.m_crSchemaRow[0] );
	pColors.SetAt( _T("meta.row"), &Colors.m_crSchemaRow[1] );
	pColors.SetAt( _T("meta.row.alt"), &Colors.m_crSchemaRow[1] );
	pColors.SetAt( _T("meta.row.odd"), &Colors.m_crSchemaRow[0] );
	pColors.SetAt( _T("meta.row.even"), &Colors.m_crSchemaRow[1] );
	pColors.SetAt( _T("meta.rating"),  &Colors.m_crRatingNull );
	pColors.SetAt( _T("meta.rating0"), &Colors.m_crRating0 );
	pColors.SetAt( _T("meta.rating1"), &Colors.m_crRating1 );
	pColors.SetAt( _T("meta.rating2"), &Colors.m_crRating2 );
	pColors.SetAt( _T("meta.rating3"), &Colors.m_crRating3 );
	pColors.SetAt( _T("meta.rating4"), &Colors.m_crRating4 );
	pColors.SetAt( _T("meta.rating5"), &Colors.m_crRating5 );

	pColors.SetAt( _T("richdoc.back"), &Colors.m_crRichdocBack );
	pColors.SetAt( _T("richdoc.text"), &Colors.m_crRichdocText );
	pColors.SetAt( _T("richdoc.heading"), &Colors.m_crRichdocHeading );

	pColors.SetAt( _T("system.textalert"), &Colors.m_crTextAlert );
	pColors.SetAt( _T("system.textstatus"), &Colors.m_crTextStatus );
	pColors.SetAt( _T("system.textlink"), &Colors.m_crTextLink );
	pColors.SetAt( _T("system.textlink.selected"), &Colors.m_crTextLinkHot );

	// Deprecated System.Base. first
	pColors.SetAt( _T("system.base.chat.in"), &Colors.m_crChatIn );
	pColors.SetAt( _T("system.base.chat.out"), &Colors.m_crChatOut );
	pColors.SetAt( _T("system.base.chat.null"), &Colors.m_crChatNull );
	pColors.SetAt( _T("system.base.search.null"), &Colors.m_crSearchNull );
	pColors.SetAt( _T("system.base.search.exists"), &Colors.m_crSearchExists );
	pColors.SetAt( _T("system.base.search.exists.hit"), &Colors.m_crSearchExistsHit );
	pColors.SetAt( _T("system.base.search.exists.selected"), &Colors.m_crSearchExistsSelected );
	pColors.SetAt( _T("system.base.search.queued"), &Colors.m_crSearchQueued );
	pColors.SetAt( _T("system.base.search.queued.hit"), &Colors.m_crSearchQueuedHit );
	pColors.SetAt( _T("system.base.search.queued.selected"), &Colors.m_crSearchQueuedSelected );
	pColors.SetAt( _T("system.base.search.ghostrated"), &Colors.m_crSearchGhostrated );
	pColors.SetAt( _T("system.base.search.highrated"), &Colors.m_crSearchHighrated );
	pColors.SetAt( _T("system.base.search.collection"), &Colors.m_crSearchCollection );
	pColors.SetAt( _T("system.base.search.torrent"), &Colors.m_crSearchTorrent );
	pColors.SetAt( _T("system.base.transfer.source"), &Colors.m_crTransferSource );
	pColors.SetAt( _T("system.base.transfer.ranges"), &Colors.m_crTransferRanges );
	pColors.SetAt( _T("system.base.transfer.completed"), &Colors.m_crTransferCompleted );
	pColors.SetAt( _T("system.base.transfer.seeding"), &Colors.m_crTransferVerifyPass );
	pColors.SetAt( _T("system.base.transfer.failed"), &Colors.m_crTransferVerifyFail );
	pColors.SetAt( _T("system.base.transfer.completed.selected"), &Colors.m_crTransferCompletedSelected );
	pColors.SetAt( _T("system.base.transfer.seeding.selected"), &Colors.m_crTransferVerifyPassSelected );
	pColors.SetAt( _T("system.base.transfer.failed.selected"), &Colors.m_crTransferVerifyFailSelected );
	pColors.SetAt( _T("system.base.network.null"), &Colors.m_crNetworkNull );
	pColors.SetAt( _T("system.base.network.gnutella"), &Colors.m_crNetworkG1 );
	pColors.SetAt( _T("system.base.network.gnutella2"), &Colors.m_crNetworkG2 );
	pColors.SetAt( _T("system.base.network.g1"), &Colors.m_crNetworkG1 );
	pColors.SetAt( _T("system.base.network.g2"), &Colors.m_crNetworkG2 );
	pColors.SetAt( _T("system.base.network.ed2k"), &Colors.m_crNetworkED2K );
	pColors.SetAt( _T("system.base.network.dc"), &Colors.m_crNetworkDC );
	pColors.SetAt( _T("system.base.network.up"), &Colors.m_crNetworkUp );
	pColors.SetAt( _T("system.base.network.down"), &Colors.m_crNetworkDown );
	pColors.SetAt( _T("system.base.network.in"), &Colors.m_crNetworkDown );
	pColors.SetAt( _T("system.base.network.out"), &Colors.m_crNetworkUp );
	pColors.SetAt( _T("system.base.security.allow"), &Colors.m_crSecurityAllow );
	pColors.SetAt( _T("system.base.security.deny"), &Colors.m_crSecurityDeny );

	// Preferred System. second
	pColors.SetAt( _T("system.chat.in"), &Colors.m_crChatIn );
	pColors.SetAt( _T("system.chat.out"), &Colors.m_crChatOut );
	pColors.SetAt( _T("system.chat.null"), &Colors.m_crChatNull );
	pColors.SetAt( _T("system.search.null"), &Colors.m_crSearchNull );
	pColors.SetAt( _T("system.search.exists"), &Colors.m_crSearchExists );
	pColors.SetAt( _T("system.search.exists.hit"), &Colors.m_crSearchExistsHit );
	pColors.SetAt( _T("system.search.exists.selected"), &Colors.m_crSearchExistsSelected );
	pColors.SetAt( _T("system.search.queued"), &Colors.m_crSearchQueued );
	pColors.SetAt( _T("system.search.queued.hit"), &Colors.m_crSearchQueuedHit );
	pColors.SetAt( _T("system.search.queued.selected"), &Colors.m_crSearchQueuedSelected );
	pColors.SetAt( _T("system.search.ghostrated"), &Colors.m_crSearchGhostrated );
	pColors.SetAt( _T("system.search.highrated"), &Colors.m_crSearchHighrated );
	pColors.SetAt( _T("system.search.collection"), &Colors.m_crSearchCollection );
	pColors.SetAt( _T("system.search.torrent"), &Colors.m_crSearchTorrent );
	pColors.SetAt( _T("system.transfer.source"), &Colors.m_crTransferSource );
	pColors.SetAt( _T("system.transfer.ranges"), &Colors.m_crTransferRanges );
	pColors.SetAt( _T("system.transfer.completed"), &Colors.m_crTransferCompleted );
	pColors.SetAt( _T("system.transfer.seeding"), &Colors.m_crTransferVerifyPass );
	pColors.SetAt( _T("system.transfer.failed"), &Colors.m_crTransferVerifyFail );
	pColors.SetAt( _T("system.transfer.completed.selected"), &Colors.m_crTransferCompletedSelected );
	pColors.SetAt( _T("system.transfer.seeding.selected"), &Colors.m_crTransferVerifyPassSelected );
	pColors.SetAt( _T("system.transfer.failed.selected"), &Colors.m_crTransferVerifyFailSelected );
	pColors.SetAt( _T("system.library"), &Colors.m_crLibraryShared );
	pColors.SetAt( _T("system.library.shared"), &Colors.m_crLibraryShared );
	pColors.SetAt( _T("system.library.unshared"), &Colors.m_crLibraryUnshared );
	pColors.SetAt( _T("system.library.unscanned"), &Colors.m_crLibraryUnscanned );
	pColors.SetAt( _T("system.library.unsafe"), &Colors.m_crLibraryUnsafe );
	pColors.SetAt( _T("system.log.debug"), &Colors.m_crLogDebug );
	pColors.SetAt( _T("system.log.info"), &Colors.m_crLogInfo );
	pColors.SetAt( _T("system.log.notice"), &Colors.m_crLogNotice );
	pColors.SetAt( _T("system.log.warning"), &Colors.m_crLogWarning );
	pColors.SetAt( _T("system.log.error"), &Colors.m_crLogError );
	pColors.SetAt( _T("system.network.null"), &Colors.m_crNetworkNull );
	pColors.SetAt( _T("system.network.gnutella"), &Colors.m_crNetworkG1 );
	pColors.SetAt( _T("system.network.gnutella2"), &Colors.m_crNetworkG2 );
	pColors.SetAt( _T("system.network.edonkey"), &Colors.m_crNetworkED2K );
	pColors.SetAt( _T("system.network.g1"), &Colors.m_crNetworkG1 );
	pColors.SetAt( _T("system.network.g2"), &Colors.m_crNetworkG2 );
	pColors.SetAt( _T("system.network.ed2k"), &Colors.m_crNetworkED2K );
	pColors.SetAt( _T("system.network.dc"), &Colors.m_crNetworkDC );
	pColors.SetAt( _T("system.network.up"), &Colors.m_crNetworkUp );
	pColors.SetAt( _T("system.network.down"), &Colors.m_crNetworkDown );
	pColors.SetAt( _T("system.network.in"), &Colors.m_crNetworkDown );
	pColors.SetAt( _T("system.network.out"), &Colors.m_crNetworkUp );
	pColors.SetAt( _T("system.security.allow"), &Colors.m_crSecurityAllow );
	pColors.SetAt( _T("system.security.deny"), &Colors.m_crSecurityDeny );

	pColors.SetAt( _T("dropdownbox.back"), &Colors.m_crDropdownBox );
	pColors.SetAt( _T("dropdownbox.text"), &Colors.m_crDropdownText );
	pColors.SetAt( _T("resizebar.edge"), &Colors.m_crResizebarEdge );
	pColors.SetAt( _T("resizebar.face"), &Colors.m_crResizebarFace );
	pColors.SetAt( _T("resizebar.shadow"), &Colors.m_crResizebarShadow );
	pColors.SetAt( _T("resizebar.highlight"), &Colors.m_crResizebarHighlight );
	pColors.SetAt( _T("fragmentbar.shaded"), &Colors.m_crFragmentShaded );
	pColors.SetAt( _T("fragmentbar.complete"), &Colors.m_crFragmentComplete );
	pColors.SetAt( _T("fragmentbar.source1"), &Colors.m_crFragmentSource1 );
	pColors.SetAt( _T("fragmentbar.source2"), &Colors.m_crFragmentSource2 );
	pColors.SetAt( _T("fragmentbar.source3"), &Colors.m_crFragmentSource3 );
	pColors.SetAt( _T("fragmentbar.source4"), &Colors.m_crFragmentSource4 );
	pColors.SetAt( _T("fragmentbar.source5"), &Colors.m_crFragmentSource5 );
	pColors.SetAt( _T("fragmentbar.source6"), &Colors.m_crFragmentSource6 );
	pColors.SetAt( _T("fragmentbar.pass"), &Colors.m_crFragmentPass );
	pColors.SetAt( _T("fragmentbar.fail"), &Colors.m_crFragmentFail );
	pColors.SetAt( _T("fragmentbar.request"), &Colors.m_crFragmentRequest );
	pColors.SetAt( _T("fragmentbar.border"), &Colors.m_crFragmentBorder );
	pColors.SetAt( _T("fragmentbar.border.selected"), &Colors.m_crFragmentBorderSelected );

	pColors.SetAt( _T("system.environment.borders"), &Colors.m_crSysDisabled );
	pColors.SetAt( _T("system.environment.disabled"), &Colors.m_crSysDisabled );
	pColors.SetAt( _T("system.environment.window"), &Colors.m_crSysWindow );
	pColors.SetAt( _T("system.environment.btnface"), &Colors.m_crSysBtnFace );
	pColors.SetAt( _T("system.environment.3dshadow"), &Colors.m_crSys3DShadow );
	pColors.SetAt( _T("system.environment.3dhighlight"), &Colors.m_crSys3DHighlight );
	pColors.SetAt( _T("system.environment.activecaption"), &Colors.m_crSysActiveCaption );
	pColors.SetAt( _T("system.environment.menutext"), &Colors.m_crSysMenuText );

	pColors.SetAt( _T("navbar.text"), &Colors.m_crNavBarText );
	pColors.SetAt( _T("navbar.text.up"), &Colors.m_crNavBarTextUp );
	pColors.SetAt( _T("navbar.text.down"), &Colors.m_crNavBarTextDown );
	pColors.SetAt( _T("navbar.text.hover"), &Colors.m_crNavBarTextHover );
	pColors.SetAt( _T("navbar.text.checked"), &Colors.m_crNavBarTextChecked );
	pColors.SetAt( _T("navbar.shadow"), &Colors.m_crNavBarShadow );
	pColors.SetAt( _T("navbar.shadow.up"), &Colors.m_crNavBarShadowUp );
	pColors.SetAt( _T("navbar.shadow.down"), &Colors.m_crNavBarShadowDown );
	pColors.SetAt( _T("navbar.shadow.hover"), &Colors.m_crNavBarShadowHover );
	pColors.SetAt( _T("navbar.shadow.checked"), &Colors.m_crNavBarShadowChecked );
	pColors.SetAt( _T("navbar.outline"), &Colors.m_crNavBarOutline );
	pColors.SetAt( _T("navbar.outline.up"), &Colors.m_crNavBarOutlineUp );
	pColors.SetAt( _T("navbar.outline.down"), &Colors.m_crNavBarOutlineDown );
	pColors.SetAt( _T("navbar.outline.hover"), &Colors.m_crNavBarOutlineHover );
	pColors.SetAt( _T("navbar.outline.checked"), &Colors.m_crNavBarOutlineChecked );

	// "system.base.xxx" colors trigger recalculating (needs review)
	BOOL bSystem  = FALSE;
	BOOL bNonBase = FALSE;

	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		if ( pXML->IsNamed( _T("color") ) ||
			 pXML->IsNamed( _T("colour") ) )
		{
			CString strName  = pXML->GetAttributeValue( _T("name") );
			CString strValue = pXML->GetAttributeValue( _T("value") );
			strName.MakeLower();

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
							Colors.CalculateColors( TRUE );
						}
					}

					*pColor = RGB( nRed, nGreen, nBlue );
				}
				else if ( strValue.IsEmpty() )
				{
					*pColor = CLR_NONE;
				}
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [name] attribute in [colorScheme] element"), pXML->ToString() );
			}
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [colorScheme] element"), pXML->ToString() );
		}
	}

	if ( bSystem && ! bNonBase )
		Colors.CalculateColors( TRUE );

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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [...Map] element"), pXML->ToString() );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin fonts

BOOL CSkin::LoadFonts(CXMLElement* pBase, const CString& strPath)
{
	bool bRichDefault = false, bRichHeading = false;

	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if ( pXML->IsNamed( _T("font") ) )
		{
			CString strLanguage = pXML->GetAttributeValue( _T("language") );

			if ( strLanguage.IsEmpty() ||
				( Settings.General.Language.CompareNoCase( strLanguage ) == 0 ) )
			{
				CString strName		= pXML->GetAttributeValue( _T("name") ).MakeLower();
				CString strFace		= pXML->GetAttributeValue( _T("face") );
				CString strSize		= pXML->GetAttributeValue( _T("size") );
				CString strWeight	= pXML->GetAttributeValue( _T("weight") );
				CFont* pFont		= NULL;

				if ( strName.GetLength() < 6 ) continue;

				// Specifiable Fonts:
				SwitchMap( Font )
				{
					Font[ L"system" ]			= 'd';
					Font[ L"system.default" ]	= 'd';
					Font[ L"system.plain" ]		= 'd';
					Font[ L"system.bold" ]		= 'b';
					Font[ L"panel.caption" ]	= 'p';
					Font[ L"navbar.caption" ]	= 'n';
					Font[ L"richdoc.default" ]	= 'r';
					Font[ L"rich.default" ]		= 'r';
					Font[ L"richdoc.heading" ]	= 'h';
					Font[ L"rich.heading" ]		= 'h';
				}

				switch ( Font[ strName ] )
				{
				case 'd':	// system.default, system.plain, system
					pFont = &CoolInterface.m_fntNormal;
					break;
				case 'b':	// system.bold
					pFont = &CoolInterface.m_fntBold;
					break;
				case 'p':	// panel.caption
					pFont = &CoolInterface.m_fntCaption;
					break;
				case 'n':	// navbar.caption
					pFont = &CoolInterface.m_fntNavBar;
					break;
				case 'r':	// richdoc.default, rich.default
					pFont = &CoolInterface.m_fntRichDefault;
					bRichDefault = true;
					break;
				case 'h':	// richdoc.heading, rich.heading
					pFont = &CoolInterface.m_fntRichHeading;
					bRichHeading = true;
					break;
				default:
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown font name"), pXML->ToString() );
					continue;
				}

				if ( pFont->m_hObject ) pFont->DeleteObject();

				if ( strFace.IsEmpty() )
					strFace = Settings.Fonts.DefaultFont;

				if ( strWeight.CompareNoCase( _T("bold") ) == 0 )
					strWeight = _T("700");
				else if ( strWeight.IsEmpty() || strWeight.CompareNoCase( _T("normal") ) == 0 )
					strWeight = _T("400");

				int nFontSize = Settings.Fonts.DefaultSize, nFontWeight = FW_NORMAL;

				if ( strSize.GetLength() && _stscanf( strSize, _T("%i"), &nFontSize ) != 1 )
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Bad [size] attribute in [font] element"), pXML->ToString() );

				if ( strWeight.GetLength() > 2 && _stscanf( strWeight, _T("%i"), &nFontWeight ) != 1 )
					theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Bad [weight] attribute in [font] element"), pXML->ToString() );

				pFont->CreateFont( -nFontSize, 0, 0, 0, nFontWeight, FALSE, FALSE, FALSE,
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					theApp.m_nFontQuality, DEFAULT_PITCH|FF_DONTCARE, strFace );

				if ( strName.CompareNoCase( _T("system.plain") ) == 0 )
				{
					pFont = &CoolInterface.m_fntUnder;
					if ( pFont->m_hObject ) pFont->DeleteObject();

					pFont->CreateFont( -nFontSize, 0, 0, 0, nFontWeight, FALSE, TRUE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							theApp.m_nFontQuality, DEFAULT_PITCH|FF_DONTCARE, strFace );

					pFont = &CoolInterface.m_fntItalic;
					if ( pFont->m_hObject ) pFont->DeleteObject();

					pFont->CreateFont( -nFontSize, 0, 0, 0, nFontWeight, TRUE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							theApp.m_nFontQuality, DEFAULT_PITCH|FF_DONTCARE, strFace );
				}
				else if ( strName.CompareNoCase( _T("system.bold") ) == 0 )
				{
					pFont = &CoolInterface.m_fntBoldItalic;
					if ( pFont->m_hObject ) pFont->DeleteObject();

					pFont->CreateFont( -nFontSize, 0, 0, 0, nFontWeight, TRUE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							theApp.m_nFontQuality, DEFAULT_PITCH|FF_DONTCARE, strFace );
				}
			}
		}
		else if ( pXML->IsNamed( _T("import") ) )
		{
			CString strFile = strPath + pXML->GetAttributeValue( _T("path") );

			if ( AddFontResourceEx( strFile, FR_PRIVATE, NULL ) )
				m_pFontPaths.AddTail( strFile );
			else
				theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed to import font"), pXML->ToString() );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [fonts] element"), pXML->ToString() );
		}
	}

	// Create Rich Default font based on Normal font, if absent
	if ( ! bRichDefault )
	{
		LOGFONT lfDefault = {};
		CoolInterface.m_fntNormal.GetLogFont( &lfDefault );
		lfDefault.lfHeight -= 1;
		lfDefault.lfWeight += 300;
		if ( CoolInterface.m_fntRichDefault.m_hObject )
			CoolInterface.m_fntRichDefault.DeleteObject();
		CoolInterface.m_fntRichDefault.CreateFontIndirect( &lfDefault );
	}

	// Create Rich Heading font based on Rich Default font, if absent
	if ( ! bRichHeading )
	{
		LOGFONT lfDefault = {};
		CoolInterface.m_fntRichDefault.GetLogFont( &lfDefault );
		lfDefault.lfHeight -= 5;
		lfDefault.lfWeight += 100;
		if ( CoolInterface.m_fntRichHeading.m_hObject )
			CoolInterface.m_fntRichHeading.DeleteObject();
		CoolInterface.m_fntRichHeading.CreateFontIndirect( &lfDefault );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin command images

CString	CSkin::GetImagePath(UINT nImageID) const
{
	//CQuickLock oLock( m_pSection );

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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown element in [commandImages] element"), pXML->ToString() );
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
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Unknown [id] attribute in [icon] element"), pXML->ToString() );
		return TRUE;	// Skip icon and load remaining skin
	}

	// Is this a RTL-enabled icon? (default: "0" - no)
	const BOOL bRTL = Settings.General.LanguageRTL && pXML->GetAttributeValue( _T("rtl"), _T("0") ) == _T("1");

	// Icon types (default: "16" - 16x16 icon only)
	CString strTypes = pXML->GetAttributeValue( _T("types"), _T("16") );
	int curPos = 0;
	CString strSize;
	while ( ! ( strSize = strTypes.Tokenize( _T(","), curPos ) ).IsEmpty() )
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
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Icon has invalid size"), pXML->ToString() );
			return FALSE;
		}

		HICON hIcon = NULL;
		if ( nIconID && hInstance )
		{
			hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( nIconID ),
				IMAGE_ICON, cx, cx, 0 );
		}
		else if ( LoadIcon( strFile,
			( ( nType == LVSIL_SMALL ) ? &hIcon : NULL ),
			( ( nType == LVSIL_NORMAL ) ? &hIcon : NULL ),
			( ( nType == LVSIL_BIG ) ? &hIcon : NULL ) ) )
		{
			m_pImages.SetAt( nID, strFile );
		}
		if ( hIcon )
		{
			if ( bRTL )
				hIcon = CreateMirroredIcon( hIcon );
			CoolInterface.AddIcon( nID, hIcon, nType );
			VERIFY( DestroyIcon( hIcon ) );
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed to load icon"), pXML->ToString() );
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

	COLORREF crMask = NULL;
	CString strMask = pBase->GetAttributeValue( _T("mask"), _T("00FF00") );
	if ( strMask.GetLength() >= 6 )
		crMask = GetColor( strMask );

	if ( crMask == NULL )
	{
		// ToDo: Auto set mask to CLR_NONE for alpha PNGs, and cull this list
		strMask.MakeLower();
		if ( strMask == _T("alpha") ||
			 strMask == _T("transparent") ||
			 strMask == _T("clr_none") ||
			 strMask == _T("none") ||
			 strMask == _T("null") ||
			 strMask == _T("png") )
		{
			crMask = CLR_NONE; 	// 0 Alpha (or Black)
		}
		else
		{
			theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Image has invalid mask"), pBase->ToString() );
			crMask = RGB( 0, 255, 0 );
		}
	}

	HBITMAP hBitmap;
	if ( crMask == CLR_NONE )
	{
		CImageFile pFile;
		if ( nID > 100 )
			pFile.LoadFromResource( AfxGetResourceHandle(), nID, RT_PNG );
		else
			pFile.LoadFromFile( strFile );
#ifndef WIN64
		if ( theApp.m_bIsWin2000 ) pFile.EnsureRGB();
#endif
		hBitmap = pFile.CreateBitmap();
	}
	else
		hBitmap = LoadBitmap( strFile );

	if ( ! hBitmap )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed to load image"), pBase->ToString() );
		return FALSE;
	}
	if ( Settings.General.LanguageRTL )
		hBitmap = CreateMirroredBitmap( hBitmap );

	BOOL bResult = CoolInterface.Add( this, pBase, hBitmap, crMask );
	DeleteObject( hBitmap );
	if ( ! bResult )
	{
		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Failed to add image"), pBase->ToString() );
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin popup menu helper

void CSkin::TrackPopupMenu(LPCTSTR pszMenu, const CPoint& point,
	UINT nDefaultID, const CStringList& oFiles, CWnd* pWnd) const
{
	CMenu* pPopup = GetMenu( pszMenu );
	if ( pPopup == NULL )
		return;

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
		if ( pPopup->SetMenuItemInfo( ID_SHELL_MENU, &pInfo ) )
		{
			CoolMenu.DoExplorerMenu( pWnd->GetSafeHwnd(), oFiles,
				point, pPopup->GetSafeHmenu(), pInfo.hSubMenu,
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON );

			// Change ID_SHELL_MENU back
			pInfo.hSubMenu = NULL;
			VERIFY( pPopup->SetMenuItemInfo( ID_SHELL_MENU, &pInfo ) );

			return;
		}
		VERIFY( DestroyMenu( hSubMenu ) );
	}

	__try	// Fix for strange TrackPopupMenu crash inside GUI
	{
		pPopup->TrackPopupMenu(
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
			point.x, point.y, pWnd );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

//////////////////////////////////////////////////////////////////////
// CSkin draw wrapped text

// GetTextFlowChange determines the direction of text and its change at word boundaries.
// Returns the direction of the first "words iceland" and the position
// where the next "iceland" starts at the word boundary.
// If there is no change in direction it returns 0.

int CSkin::GetTextFlowChange(LPCTSTR pszText, BOOL* bIsRTL)
{
	TRISTATE bTextIsRTL	= TRI_UNKNOWN;
	BOOL bChangeFound	= FALSE;
	LPCTSTR pszWord = pszText;
	LPCTSTR pszScan = pszText;

	int nPos;
	for ( nPos = 0 ; ; pszScan++, nPos++ )
	{
		// Get the first word with punctuation marks and whitespaces
		if ( (unsigned short)*pszScan > 32 && (unsigned short)*pszScan != 160 ) continue;

		if ( pszWord < pszScan )
		{
			int nLen = static_cast< int >( pszScan - pszWord );
			WORD* nCharType = new WORD[ nLen + 1 ];

			TCHAR* pszTestWord = new TCHAR[ nLen + 1 ];
			_tcsncpy_s( pszTestWord, nLen + 1, pszWord, nLen );
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
	// ToDo: Wrap mixed text in RTL and LTR layouts correctly

	if ( pszText == NULL ) return;
	if ( ptStart.x == 0 && ptStart.y == 0 ) ptStart = pBox->TopLeft();

	UINT nAlignOptionsOld = pDC->GetTextAlign();	// Backup settings
	UINT nFlags = ETO_CLIPPED | ( bExclude ? ETO_OPAQUE : 0 );

	unsigned short nLenFull = static_cast< unsigned short >( _tcslen( pszText ) );

	// Collect stats about the text from the start
	BOOL bIsRTLStart = FALSE;
	int nTestStart = GetTextFlowChange( pszText, &bIsRTLStart );

	// Guess text direction ( not always works )
	BOOL bNormalFlow = Settings.General.LanguageRTL ? bIsRTLStart : ! bIsRTLStart;

	TCHAR* pszSource = NULL;
	LPCTSTR pszWord  = NULL;
	LPCTSTR pszScan  = NULL;

	if ( nTestStart )
	{
		// Get the source string to draw and truncate initial string to pass it recursively
		pszSource = new TCHAR[ nTestStart + 1 ];
		_tcsncpy_s( pszSource, nTestStart + 1, pszText, nTestStart );
		pszSource[ nTestStart ] = 0;
		if ( ! bNormalFlow )
		{
			// Swap whitespaces
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
			_tcsncpy_s( pszSource, nTestStart + 1, str.GetBuffer( nTestStart ), nTestStart );
		}
		nLenFull = static_cast< unsigned short >( nTestStart );
		pszText += nTestStart;
	}
	else
		pszSource = (TCHAR*)pszText;

	pszWord = pszSource;
	pszScan = pszSource;

	if ( ! bNormalFlow )
	{
		if ( ( bIsRTLStart != FALSE ) != Settings.General.LanguageRTL )
			pDC->SetTextAlign( nAlignOptionsOld ^ TA_RTLREADING );
		pszScan += nLenFull - 1;
		pszWord += nLenFull;
		for ( int nEnd = nLenFull - 1 ; nEnd >= 0 ; nEnd-- )
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
	// Reset align options back
	pDC->SetTextAlign( nAlignOptionsOld );
	if ( nTestStart ) DrawWrappedText( pDC, pBox, pszText, ptStart, bExclude );
}

//////////////////////////////////////////////////////////////////////
// CSkin hex color utility

BOOL CSkin::LoadColor(CXMLElement* pXML, LPCTSTR pszName, COLORREF* pColor)
{
	CString str = pXML->GetAttributeValue( pszName );
	if ( ! str.IsEmpty() )
	{
		*pColor = GetColor( str );
		if ( *pColor || str == _T("000000") )
			return TRUE;

		theApp.Message( MSG_ERROR, IDS_SKIN_ERROR, _T("Bad color attribute (") + str + _T(")"), pXML->ToString() );
	}

	return FALSE;
}

COLORREF CSkin::GetColor(CString sColor)
{
	sColor.Trim( _T(" #") );

	const int nLength = sColor.GetLength();
	if ( nLength < 6 || nLength > 14 )
		return NULL;

	int nRed = 0, nGreen = 0, nBlue = 0;
	if ( nLength == 6 &&
		_stscanf( sColor.Mid( 0, 2 ), _T("%x"), &nRed ) == 1 &&
		_stscanf( sColor.Mid( 2, 2 ), _T("%x"), &nGreen ) == 1 &&
		_stscanf( sColor.Mid( 4, 2 ), _T("%x"), &nBlue ) == 1 )
	{
		return RGB( nRed, nGreen, nBlue );
	}

	if ( _stscanf( (LPCTSTR)sColor, _T("%i, %i, %i"), &nRed, &nGreen, &nBlue ) == 3 &&
		 nRed < 256 && nGreen < 256 && nBlue < 256 )
		return RGB( nRed, nGreen, nBlue );

	if ( sColor == L"CLR_NONE" )
		return CLR_NONE;

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CSkin load bitmap helper

HBITMAP CSkin::LoadBitmap(const CString& strName)
{
	//HBITMAP hBitmap = NULL;
	//if ( m_pBitmaps.Lookup( strName, hBitmap ) )
	//	return hBitmap;

	const int nPos = strName.Find( '$' );
	if ( nPos < 0 )
		return CImageFile::LoadBitmapFromFile( strName );

	HINSTANCE hInstance = NULL;
	if ( _stscanf( (LPCTSTR)strName, _T("%p"), &hInstance ) != 1 )
		return NULL;

	UINT nID = 0;
	if ( _stscanf( (LPCTSTR)strName + nPos + 1, _T("%lu"), &nID ) != 1 )
		return NULL;

	return CImageFile::LoadBitmapFromResource( nID, hInstance );

	//if ( hBitmap )
	//	m_pBitmaps.SetAt( strName, hBitmap );
	//return hBitmap;
}

HBITMAP CSkin::LoadBitmap(UINT nID)
{
	CString strName;
	strName.Format( _T("%p$%lu"), (HINSTANCE)GetModuleHandle( NULL ), nID );
	return LoadBitmap( strName );
}

//////////////////////////////////////////////////////////////////////
// CSkin mode suffixes

LPCTSTR CSkin::m_pszModeSuffix[3][4] =
{
	{ _T(".Windowed"), _T(""), NULL, NULL },			// GUI_WINDOWED
	{ _T(".Tabbed"), _T(""), NULL, NULL },				// GUI_TABBED
	{ _T(".Basic"), _T(".Tabbed"), _T(""), NULL }		// GUI_BASIC
};
