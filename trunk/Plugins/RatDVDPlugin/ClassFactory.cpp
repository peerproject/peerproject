//
// ClassFactory.cpp
//
//	Date:			"$Date: $"
//	Revision:		"$Revision: 1.0 $"
//  Last change by:	"$Author: rolandas $"
//	Created by:		Rolandas Rudomanskis
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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

#pragma once

#include "stdafx.h"
#include "RatDVDPlugin.h"

////////////////////////////////////////////////////////////////////////
// CRatDVDClassFactory - IClassFactory Implementation
//
//  This is a fairly simple CF. We don't provide support for licensing
//  in this sample, nor aggregation. We just create and return a new 
//  CRatDVDPlugin object.
//

////////////////////////////////////////////////////////////////////////
// QueryInterface
//

STDMETHODIMP CRatDVDClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	ODS(_T("CRatDVDClassFactory::QueryInterface\n"));

	CHECK_NULL_RETURN(ppv, E_POINTER);
	
	if ( CLSID_RatDVDReader == riid )
	{
		*ppv = this;
		this->AddRef();
		return S_OK;
	}
	*ppv = NULL;
	return E_NOINTERFACE;
}

////////////////////////////////////////////////////////////////////////
// AddRef
//
STDMETHODIMP_(ULONG) CRatDVDClassFactory::AddRef(void)
{
    return ++m_cRef;
}

////////////////////////////////////////////////////////////////////////
// Release
//
STDMETHODIMP_(ULONG) CRatDVDClassFactory::Release(void)
{
    if ( 0 != --m_cRef ) return m_cRef;

	ODS(_T("CRatDVDClassFactory delete\n"));

    LockServer(FALSE);
    return 0;
}

////////////////////////////////////////////////////////////////////////
// IClassFactory
//
////////////////////////////////////////////////////////////////////////
// CreateInstance
//
STDMETHODIMP CRatDVDClassFactory::CreateInstance(LPUNKNOWN punk, REFIID riid, void** ppv)
{
	HRESULT hr;

	ODS(_T("CFileClassFactory::CreateInstance\n"));

	CHECK_NULL_RETURN(ppv, E_POINTER);	*ppv = NULL;

 // This version does not support Aggregation...
	if (punk) return CLASS_E_NOAGGREGATION;

	if ( IID_ILibraryBuilderPlugin == riid || IID_IImageServicePlugin == riid )
	{
		CComObject<CRatDVDPlugin>*pRatDVDPlugin = new CComObject<CRatDVDPlugin>;

		CHECK_NULL_RETURN(pRatDVDPlugin, E_OUTOFMEMORY);
		hr = pRatDVDPlugin->QueryInterface( IID_IUnknown, ppv );
		if ( SUCCEEDED(hr) )
		{
			if ( IID_ILibraryBuilderPlugin == riid )
				*ppv = dynamic_cast<ILibraryBuilderPlugin*>(pRatDVDPlugin);
			else
				*ppv = dynamic_cast<IImageServicePlugin*>(pRatDVDPlugin);
		}
		else return hr;
	}
	else return E_NOINTERFACE;

	LockServer(TRUE); // on success, bump up the lock count

	return hr;
}

////////////////////////////////////////////////////////////////////////
// LockServer
//
STDMETHODIMP CRatDVDClassFactory::LockServer(BOOL fLock)
{
	if (fLock) DllAddRef();	else DllRelease();
	return S_OK;
}

