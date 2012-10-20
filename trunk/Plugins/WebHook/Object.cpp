//
// Object.cpp : Implementation of CWebHook
//
// This file is part of PeerProject (peerproject.org) � 2009,2012
// Portions Copyright Shareaza Development Team, 2009.
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
#include "Object.h"

// CWebHook

CWebHook::CWebHook()
	: m_dwCookie( 0 )
{
}

HRESULT CWebHook::FinalConstruct()
{
	if ( ! IsEnabled() )
		return E_FAIL;

	return CoCreateFreeThreadedMarshaler( GetControllingUnknown(), &m_pUnkMarshaler.p );
}

void CWebHook::FinalRelease()
{
	Disconnect();
	m_spUnkSite.Release();
	m_pUnkMarshaler.Release();
}

bool CWebHook::IsEnabled() const
{
	DWORD dwWebHookEnable = FALSE;
	DWORD dwLength = sizeof( dwWebHookEnable );
	SHRegGetUSValue( _T("Software\\PeerProject\\PeerProject\\Downloads"),
		_T("WebHookEnable"), NULL, &dwWebHookEnable,
		&dwLength, FALSE, &dwWebHookEnable, sizeof( dwWebHookEnable ) );
	return ( dwWebHookEnable != FALSE );
}

bool CWebHook::IsHooked(const CString& sExt) const
{
	CString sWebHookExtensions;
	DWORD dwLength = 1024;
	SHRegGetUSValue( _T("Software\\PeerProject\\PeerProject\\Downloads"),
		_T("WebHookExtensions"), NULL, sWebHookExtensions.GetBuffer( dwLength ),
		&dwLength, FALSE, _T(""), sizeof( TCHAR ) );
	sWebHookExtensions.ReleaseBuffer();
	sWebHookExtensions.MakeLower();
	return ( sWebHookExtensions.Find( CString( _T("|") ) + sExt + _T("|") ) != -1 );
}

void CWebHook::Connect()
{
	if ( m_spUnkSite )
	{
		CComQIPtr< IConnectionPointContainer > pContainer( m_spUnkSite );
		if ( pContainer )
		{
			CComPtr< IConnectionPoint > pPoint;
			if ( SUCCEEDED( pContainer->FindConnectionPoint( DIID_DWebBrowserEvents2, &pPoint ) ) )
				pPoint->Advise( static_cast< IDispatch* >( this ), &m_dwCookie );
		}
	}
}

void CWebHook::Disconnect()
{
	if ( m_spUnkSite )
	{
		CComQIPtr< IConnectionPointContainer > pContainer( m_spUnkSite );
		if ( pContainer )
		{
			CComPtr< IConnectionPoint > pPoint;
			if ( SUCCEEDED( pContainer->FindConnectionPoint( DIID_DWebBrowserEvents2, &pPoint ) ) )
			{
				pPoint->Unadvise( m_dwCookie );
				m_dwCookie = 0;
			}
		}
	}
}

void CWebHook::AddLink(const CString& sURL)
{
	ShellExecute( NULL, NULL, CString( _T("peerproject://url:") ) + sURL, NULL, NULL, SW_SHOWDEFAULT );
}

STDMETHODIMP CWebHook::Invoke(
	/*[in]*/ DISPID dispIdMember,
	/*[in]*/ REFIID riid,
	/*[in]*/ LCID lcid,
	/*[in]*/ WORD wFlags,
	/*[out][in]*/ DISPPARAMS *pDispParams,
	/*[out]*/ VARIANT* pVarResult,
	/*[out]*/ EXCEPINFO* pExcepInfo,
	/*[out]*/ UINT* puArgErr)
{
	if ( IsEnabled() )
	{
		switch ( dispIdMember )
		{
		case DISPID_BEFORENAVIGATE2:
			{
				ATLASSERT( pDispParams->cArgs == 7 );
				//ATLASSERT( pDispParams->rgvarg[ 6 ].vt == VT_DISPATCH );
				//CComQIPtr< IWebBrowser2 > pIWebBrowser2( pDispParams->rgvarg[ 6 ].pdispVal );
				ATLASSERT( pDispParams->rgvarg[ 5 ].vt == ( VT_BYREF | VT_VARIANT ) );
				ATLASSERT( pDispParams->rgvarg[ 5 ].pvarVal->vt == VT_BSTR );
				BSTR& bstrURL = pDispParams->rgvarg[ 5 ].pvarVal->bstrVal;

				ATLTRACE( "[Web Hook] Before navigate: %s\n", bstrURL );
				m_sURL.Empty();

				CString sURL( bstrURL );
				int nName = sURL.ReverseFind( _T('/') );
				CString sName = sURL.Mid( nName + 1 ).SpanExcluding( _T("?") );
				int nExt = sName.ReverseFind( _T('.') );
				CString sExt;
				if ( nExt != -1 )
					sExt = sName.Mid( nExt + 1 ).MakeLower();
				if ( ! sExt.IsEmpty() && IsHooked( sExt ) )
				{
					m_sURL = sURL;
				}
			}
			break;

		case DISPID_NAVIGATECOMPLETE2:
			m_sURL.Empty();
			break;

		case DISPID_FILEDOWNLOAD:
			{
				ATLASSERT( pDispParams->cArgs == 2 );
				ATLASSERT( pDispParams->rgvarg[ 0 ].vt == ( VT_BYREF | VT_BOOL ) );
				VARIANT_BOOL*& pCancel = pDispParams->rgvarg[ 0 ].pboolVal;

				if ( ! m_sURL.IsEmpty() )
				{
					ATLTRACE( "[Web Hook] File download: %s\n", m_sURL );
					AddLink( m_sURL );
					m_sURL.Empty();
					*pCancel = VARIANT_TRUE;
				}
			}
			break;
		}
	}

	return IWebHookDispatchImpl::Invoke( dispIdMember, riid, lcid, wFlags,
		pDispParams, pVarResult, pExcepInfo, puArgErr );
}

STDMETHODIMP CWebHook::SetSite(
	/*[in]*/ IUnknown* pUnkSite)
{
	Disconnect();

	HRESULT hr = IObjectWithSiteImpl< CWebHook >::SetSite( pUnkSite );

	Connect();

	return hr;
}

STDMETHODIMP CWebHook::AddLink(
	/*[in]*/ VARIANT oLink)
{
	if ( oLink.vt == VT_BSTR )
	{
		ATLTRACE( "[Web Hook] Menu call: %s\n", (LPCTSTR)oLink.bstrVal );
		AddLink( CString( oLink.bstrVal ) );
	}
	return S_OK;
}
