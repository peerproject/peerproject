//
// ClassFactory.cpp
//
//	Created by:		Rolandas Rudomanskis
//  Last change by:	thetruecamper
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2005.
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
#include "Globals.h"
#include "DocReader.h"
#include "PeerProject_i.c"

////////////////////////////////////////////////////////////////////////
// CDocumentClassFactory - IClassFactory Implementation
//
//  This is a fairly simple CF. We don't provide support for licensing
//  in this sample, nor aggregation. We just create and return a new 
//  CDocReader object.
//

////////////////////////////////////////////////////////////////////////
// QueryInterface
//

STDMETHODIMP CDocumentClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	ODS(_T("CDocumentClassFactory::QueryInterface\n"));

	CHECK_NULL_RETURN(ppv, E_POINTER);
	
	if ( CLSID_DocReader == riid )
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
STDMETHODIMP_(ULONG) CDocumentClassFactory::AddRef(void)
{
	TRACE1("CDocumentClassFactory::AddRef - %d\n", m_cRef + 1);
    return ++m_cRef;
}

////////////////////////////////////////////////////////////////////////
// Release
//
STDMETHODIMP_(ULONG) CDocumentClassFactory::Release(void)
{
	TRACE1("CDocumentClassFactory::Release - %d\n", m_cRef - 1);
    if ( 0 != --m_cRef ) return m_cRef;

	ODS(_T("CDocumentClassFactory delete\n"));

    LockServer(FALSE);
    return 0;
}

////////////////////////////////////////////////////////////////////////
// IClassFactory
//
////////////////////////////////////////////////////////////////////////
// CreateInstance
//
STDMETHODIMP CDocumentClassFactory::CreateInstance(LPUNKNOWN punk, REFIID riid, void** ppv)
{
	HRESULT hr;
//	CComObject<CDocReader>*pDocReader = NULL;

	ODS(_T("CFileClassFactory::CreateInstance\n"));

	CHECK_NULL_RETURN(ppv, E_POINTER);	*ppv = NULL;

 // This version does not support Aggregation...
	if (punk) return CLASS_E_NOAGGREGATION;

	if ( IID_ILibraryBuilderPlugin == riid || IID_IImageServicePlugin == riid )
	{
		CComObject<CDocReader>*pDocReader = new CComObject<CDocReader>;

		CHECK_NULL_RETURN(pDocReader, E_OUTOFMEMORY);
		hr = pDocReader->QueryInterface( IID_IUnknown, ppv );
		if ( SUCCEEDED(hr) )
		{
			pDocReader->Initialize( ( IID_IImageServicePlugin == riid ) );
			if ( IID_ILibraryBuilderPlugin == riid )
				*ppv = dynamic_cast<ILibraryBuilderPlugin*>(pDocReader);
			else
				*ppv = dynamic_cast<IImageServicePlugin*>(pDocReader);
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
STDMETHODIMP CDocumentClassFactory::LockServer(BOOL fLock)
{
	TRACE1("CDocumentClassFactory::LockServer - %d\n", fLock);
	if (fLock) DllAddRef();	else DllRelease();
	return S_OK;
}

