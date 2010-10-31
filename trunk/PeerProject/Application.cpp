//
// Application.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "PeerProject.h"
#include "Settings.h"
#include "Application.h"
#include "CoolInterface.h"
#include "ImageServices.h"
#include "Skin.h"
#include "Library.h"
#include "Plugins.h"
#include "ComMenu.h"
#include "ComToolbar.h"
#include "WndMain.h"
#include "WndChild.h"
#include "WndPlugin.h"

#include "XML.h"
#include "XMLCOM.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CApplication, CComObject)

// {E9B2EF9B-4A0C-451e-801F-257861B87FAD}
IMPLEMENT_OLECREATE_FLAGS(CApplication, "PeerProject.Application",
	afxRegFreeThreading|afxRegApartmentThreading,
	0xe9b2ef9b, 0x4a0c, 0x451e, 0x80, 0x1f, 0x25, 0x78, 0x61, 0xb8, 0x7f, 0xad)

BEGIN_MESSAGE_MAP(CApplication, CComObject)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CApplication, CComObject)
	INTERFACE_PART(CApplication, IID_IApplication, Application)
	INTERFACE_PART(CApplication, IID_IUserInterface, UserInterface)
	INTERFACE_PART(CApplication, IID_ISettings, Settings)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CApplication construction

CApplication::CApplication()
{
	EnableDispatch( IID_IApplication );
	EnableDispatch( IID_IUserInterface );
	EnableDispatch( IID_ISettings );
}

CApplication::~CApplication()
{
}

/////////////////////////////////////////////////////////////////////////////
// CApplication operations

HRESULT CApplication::GetApp(IApplication** ppIApplication) throw()
{
	return CoCreateInstance( CLSID_PeerProjectApplication, NULL, CLSCTX_ALL,
		IID_IApplication, (void**)ppIApplication );
}

HRESULT CApplication::GetUI(IUserInterface** ppIUserInterface) throw()
{
	return CoCreateInstance( CLSID_PeerProjectApplication, NULL, CLSCTX_ALL,
		IID_IUserInterface, (void**)ppIUserInterface );
}

HRESULT CApplication::GetSettings(ISettings** ppISettings) throw()
{
	return CoCreateInstance( CLSID_PeerProjectApplication, NULL, CLSCTX_ALL,
		IID_ISettings, (void**)ppISettings );
}

/////////////////////////////////////////////////////////////////////////////
// CApplication IApplication

IMPLEMENT_DISPATCH(CApplication, Application)

STDMETHODIMP CApplication::XApplication::get_Application(IApplication FAR* FAR* ppApplication)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( ppApplication == NULL ) return E_INVALIDARG;
	*ppApplication = (IApplication*)pThis->GetInterface( IID_IApplication, TRUE );
	return S_OK;
}

STDMETHODIMP CApplication::XApplication::get_Version(BSTR FAR* psVersion)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( psVersion == NULL ) return E_INVALIDARG;
	*psVersion = CComBSTR( theApp.m_sVersion ).Detach();
	return S_OK;
}

STDMETHODIMP CApplication::XApplication::CheckVersion(BSTR sVersion)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( sVersion == NULL ) return E_INVALIDARG;

	int nDesired[4];

	if ( swscanf_s( sVersion, L"%i.%i.%i.%i", &nDesired[3], &nDesired[2],
		&nDesired[1], &nDesired[0] ) != 4 ) return E_INVALIDARG;

	// Note: Assumes each version component is 8 bit
	BOOL bOk = ( theApp.m_nVersion[0] << 24 ) + ( theApp.m_nVersion[1] << 16 ) + ( theApp.m_nVersion[2] << 8 ) + theApp.m_nVersion[3]
			>= ( nDesired[3] << 24 ) + ( nDesired[2] << 16 ) + ( nDesired[1] << 8 ) + nDesired[0];

	return bOk ? S_OK : S_FALSE;
}

STDMETHODIMP CApplication::XApplication::CreateXML(ISXMLElement FAR* FAR* ppXML)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( ppXML == NULL ) return E_INVALIDARG;
	CXMLElement* pXML = new CXMLElement();
	*ppXML = (ISXMLElement*)CXMLCOM::Wrap( pXML, IID_ISXMLElement );
	return S_OK;
}

STDMETHODIMP CApplication::XApplication::get_UserInterface(IUserInterface FAR* FAR* ppUserInterface)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( ppUserInterface == NULL ) return E_INVALIDARG;
	*ppUserInterface = (IUserInterface*)pThis->GetInterface( IID_IUserInterface, TRUE );
	return S_OK;
}

STDMETHODIMP CApplication::XApplication::get_Library(ILibrary FAR* FAR* ppLibrary)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( ppLibrary == NULL ) return E_INVALIDARG;
	*ppLibrary = (ILibrary*)Library.GetInterface( IID_ILibrary, TRUE );
	return S_OK;
}

STDMETHODIMP CApplication::XApplication::get_Settings(ISettings FAR* FAR* ppSettings)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( ppSettings == NULL ) return E_INVALIDARG;
	*ppSettings = (ISettings*)pThis->GetInterface( IID_ISettings, TRUE );
	return S_OK;
}

STDMETHODIMP CApplication::XApplication::get_ImageService(IImageServicePlugin FAR* FAR* ppIImageService)
{
	METHOD_PROLOGUE( CApplication, Application )
	if ( ppIImageService == NULL ) return E_INVALIDARG;
	*ppIImageService = (IImageServicePlugin*)ImageServices.GetInterface( IID_IImageServicePlugin, TRUE );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CApplication ISettings

IMPLEMENT_DISPATCH(CApplication, Settings)

STDMETHODIMP CApplication::XSettings::GetValue(VARIANT* value)
{
	METHOD_PROLOGUE( CApplication, Settings )

	if ( value == NULL || value->vt != VT_BSTR )
		return E_INVALIDARG;

	CString strPath( value->bstrVal );

	if ( strPath.IsEmpty() )
		return E_INVALIDARG;

	SysFreeString( value->bstrVal );
	value->vt = VT_EMPTY;

	if ( Settings.GetValue( strPath, value ) )
		return S_OK;

	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// CApplication IUserInterface

IMPLEMENT_DISPATCH(CApplication, UserInterface)

STDMETHODIMP CApplication::XUserInterface::get_Application(IApplication FAR* FAR* ppApplication)
{
	METHOD_PROLOGUE( CApplication, UserInterface )
	if ( ppApplication == NULL ) return E_INVALIDARG;
	*ppApplication = (IApplication*)pThis->GetInterface( IID_IApplication, TRUE );
	return S_OK;
}

STDMETHODIMP CApplication::XUserInterface::get_UserInterface(IUserInterface FAR* FAR* ppUserInterface)
{
	METHOD_PROLOGUE( CApplication, UserInterface )
	if ( ppUserInterface == NULL ) return E_INVALIDARG;
	*ppUserInterface = (IUserInterface*)pThis->GetInterface( IID_IUserInterface, TRUE );
	return S_OK;
}

STDMETHODIMP CApplication::XUserInterface::NewWindow(BSTR bsName, IPluginWindowOwner FAR* pOwner, IPluginWindow FAR* FAR* ppWindow)
{
	METHOD_PROLOGUE( CApplication, UserInterface )

	if ( bsName == NULL || pOwner == NULL || ppWindow == NULL ) return E_INVALIDARG;
	if ( theApp.SafeMainWnd() == NULL ) return E_UNEXPECTED;

	IPluginWindowOwner* pOwner2;
	if ( FAILED( pOwner->QueryInterface( IID_IPluginWindowOwner, (void**)&pOwner2 ) ) ) return E_NOINTERFACE;

	CPluginWnd* pWnd = new CPluginWnd( CString( bsName ), pOwner2 );
	pOwner2->Release();

	*ppWindow = (IPluginWindow*)pWnd->GetInterface( &IID_IPluginWindow );

	return S_OK;
}

STDMETHODIMP CApplication::XUserInterface::get_MainWindowHwnd(HWND FAR* phWnd)
{
	METHOD_PROLOGUE( CApplication, UserInterface )
	if ( phWnd == NULL ) return E_INVALIDARG;
	if ( theApp.SafeMainWnd() == NULL ) return E_UNEXPECTED;
	*phWnd = theApp.SafeMainWnd()->GetSafeHwnd();
	return S_OK;
}

STDMETHODIMP CApplication::XUserInterface::get_ActiveView(IGenericView FAR* FAR* ppView)
{
	METHOD_PROLOGUE( CApplication, UserInterface )

	if ( ppView == NULL ) return E_INVALIDARG;
	*ppView = NULL;

	CMainWnd* pMainWnd = (CMainWnd*)theApp.SafeMainWnd();
	if ( pMainWnd == NULL ) return E_UNEXPECTED;
	CChildWnd* pChildWnd = pMainWnd->m_pWindows.GetActive();
	if ( pChildWnd == NULL ) return S_FALSE;

	return pChildWnd->GetGenericView( ppView );
}

STDMETHODIMP CApplication::XUserInterface::RegisterCommand(BSTR bsName, HICON hIcon, UINT* pnCommandID)
{
	METHOD_PROLOGUE( CApplication, UserInterface )
	if ( pnCommandID == NULL ) return E_INVALIDARG;
	UINT nID = CoolInterface.NameToID( CString( bsName ) );
	if ( nID > 0 && nID < Plugins.m_nCommandID )
	{
		// Name exists, reuse existing ID (commmon/conflict)
		*pnCommandID = nID;
	}
	else
	{
		*pnCommandID = Plugins.GetCommandID();
		if ( bsName != NULL ) CoolInterface.NameCommand( *pnCommandID, CString( bsName ) );
	}
	if ( hIcon ) CoolInterface.AddIcon( *pnCommandID, hIcon );
	return S_OK;
}

STDMETHODIMP CApplication::XUserInterface::AddFromString(BSTR sXML)
{
	METHOD_PROLOGUE( CApplication, UserInterface )
	if ( sXML == NULL ) return E_INVALIDARG;
	return Skin.LoadFromString( CString( sXML ), Settings.General.Path + '\\' ) ? S_OK : E_FAIL;
}

STDMETHODIMP CApplication::XUserInterface::AddFromResource(HINSTANCE hInstance, UINT nID)
{
	METHOD_PROLOGUE( CApplication, UserInterface )
	if ( hInstance == NULL || nID == 0 ) return E_INVALIDARG;
	return Skin.LoadFromResource( hInstance, nID ) ? S_OK : E_FAIL;
}

STDMETHODIMP CApplication::XUserInterface::AddFromXML(ISXMLElement FAR* pXML)
{
	METHOD_PROLOGUE( CApplication, UserInterface )
	CXMLElement* pBase = CXMLCOM::Unwrap( pXML );
	if ( pBase == NULL ) return E_INVALIDARG;
	return Skin.LoadFromXML( pBase, Settings.General.Path + '\\' ) ? S_OK : E_FAIL;
}

STDMETHODIMP CApplication::XUserInterface::GetMenu(BSTR bsName, VARIANT_BOOL bCreate, ISMenu FAR* FAR* ppMenu)
{
	METHOD_PROLOGUE( CApplication, UserInterface )

	if ( bsName == NULL || ppMenu == NULL ) return E_INVALIDARG;
	*ppMenu = NULL;

	CMenu* pMenu = Skin.GetMenu( CString( bsName ) );

	if ( pMenu == NULL )
	{
		if ( bCreate == VARIANT_FALSE ) return E_FAIL;
		pMenu = Skin.CreatePopupMenu( CString( bsName ) );
	}

	*ppMenu = CComMenu::Wrap( pMenu->GetSafeHmenu() );

	return S_OK;
}

STDMETHODIMP CApplication::XUserInterface::GetToolbar(BSTR bsName, VARIANT_BOOL bCreate, ISToolbar FAR* FAR* ppToolbar)
{
	METHOD_PROLOGUE( CApplication, UserInterface )

	if ( bsName == NULL || ppToolbar == NULL ) return E_INVALIDARG;
	*ppToolbar = NULL;

	CCoolBarCtrl* pBar = Skin.GetToolBar( CString( bsName ) );

	if ( pBar == NULL )
	{
		if ( bCreate == VARIANT_FALSE ) return E_FAIL;
		pBar = Skin.CreateToolBar( CString( bsName ) );
	}

	*ppToolbar = CComToolbar::Wrap( pBar );

	return S_OK;
}
