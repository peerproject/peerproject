//
// SWFBuilder.cpp : Implementation of CSWFBuilder
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Copyright (c) Nikolay Raspopov, 2005.
//
// GFL Library, GFL SDK and XnView
// Copyright (c) 1991-2004 Pierre-E Gougelet
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

#include "stdafx.h"
#include "SWFBuilder.h"

HRESULT CSWFBuilder::FinalConstruct () throw()
{
	return CoCreateFreeThreadedMarshaler (GetControllingUnknown(), &m_pUnkMarshaler.p);
}

void CSWFBuilder::FinalRelease () throw()
{
	m_pUnkMarshaler.Release ();
}

STDMETHODIMP CSWFBuilder::Process(/*[in]*/ BSTR /*sFile*/, /*[in]*/ ISXMLElement* pXML)
{
	if ( ! pXML )
		return E_POINTER;

	CComPtr <ISXMLElements> pISXMLRootElements;
	HRESULT hr = pXML->get_Elements(&pISXMLRootElements);
	if (FAILED (hr)) return hr;

	CComPtr <ISXMLElement> pXMLRootElement;
	hr = pISXMLRootElements->Create (CComBSTR ("videos"), &pXMLRootElement);
	if (FAILED (hr)) return hr;

	CComPtr <ISXMLAttributes> pISXMLRootAttributes;
	hr = pXMLRootElement->get_Attributes(&pISXMLRootAttributes);
	if (FAILED (hr)) return hr;

	pISXMLRootAttributes->Add (CComBSTR ("xmlns:xsi"),
		CComBSTR ("http://www.w3.org/2001/XMLSchema-instance"));
	pISXMLRootAttributes->Add (CComBSTR ("xsi:noNamespaceSchemaLocation"),
		CComBSTR ("http://www.limewire.com/schemas/video.xsd"));

	CComPtr <ISXMLElements> pISXMLElements;
	hr = pXMLRootElement->get_Elements(&pISXMLElements);
	if (FAILED (hr)) return hr;

	CComPtr <ISXMLElement> pXMLElement;
	hr = pISXMLElements->Create (CComBSTR ("video"), &pXMLElement);
	if (FAILED (hr)) return hr;

	CComPtr <ISXMLAttributes> pISXMLAttributes;
	hr = pXMLElement->get_Attributes(&pISXMLAttributes);
	if (FAILED (hr)) return hr;

	pISXMLAttributes->Add (CComBSTR ("type"), CComBSTR ("Shockwave Flash"));
	pISXMLAttributes->Add (CComBSTR ("codec"), CComBSTR ("SWF"));
	CString tmp;
	tmp.Format (_T("%lu"), cx);
	pISXMLAttributes->Add (CComBSTR ("width"), CComBSTR (tmp));
	tmp.Format (_T("%lu"), cy);
	pISXMLAttributes->Add (CComBSTR ("height"), CComBSTR (tmp));

	return hr;
}
