//
// Builder.cpp : Implementation of CBuilder
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Nikolay Raspopov, 2005.
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
#include "Builder.h"

HRESULT CBuilder::FinalConstruct() throw()
{
	return CoCreateFreeThreadedMarshaler (GetControllingUnknown(), &m_pUnkMarshaler.p);
}

void CBuilder::FinalRelease() throw()
{
	m_pUnkMarshaler.Release();
}

STDMETHODIMP CBuilder::Process(
	/* [in] */ BSTR sFile,
	/* [in] */ ISXMLElement* pXML)
{
	if ( ! pXML )
		return E_POINTER;

	CComPtr <ISXMLElements> pISXMLRootElements;
	HRESULT hr = pXML->get_Elements(&pISXMLRootElements);
	if (FAILED (hr))
		return hr;
	CComPtr <ISXMLElement> pXMLRootElement;
	hr = pISXMLRootElements->Create(CComBSTR("images"), &pXMLRootElement);
	if (FAILED (hr))
		return hr;
	CComPtr <ISXMLAttributes> pISXMLRootAttributes;
	hr = pXMLRootElement->get_Attributes(&pISXMLRootAttributes);
	if (FAILED (hr))
		return hr;
	pISXMLRootAttributes->Add (CComBSTR("xmlns:xsi"),
		CComBSTR("http://www.w3.org/2001/XMLSchema-instance"));
	pISXMLRootAttributes->Add (CComBSTR("xsi:noNamespaceSchemaLocation"),
		CComBSTR("http://www.shareaza.com/schemas/image.xsd"));

	CComPtr <ISXMLElements> pISXMLElements;
	hr = pXMLRootElement->get_Elements(&pISXMLElements);
	if (FAILED (hr))
		return hr;
	CComPtr <ISXMLElement> pXMLElement;
	hr = pISXMLElements->Create(CComBSTR("image"), &pXMLElement);
	if (FAILED (hr))
		return hr;
	CComPtr <ISXMLAttributes> pISXMLAttributes;
	hr = pXMLElement->get_Attributes(&pISXMLAttributes);
	if (FAILED (hr))
		return hr;

	GFL_FILE_INFORMATION inf = { 0 };
	WCHAR pszPath[MAX_PATH] = { 0 };

	GFL_ERROR err = gflGetFileInformationW( sFile, -1, &inf );

	if ( err != GFL_NO_ERROR )
	{
		if ( GetShortPathNameW( sFile, pszPath, MAX_PATH ) )
			err = gflGetFileInformationW( pszPath, -1, &inf );
		else
			err = GFL_ERROR_FILE_OPEN;
	}

	if ( err == GFL_NO_ERROR )
	{
		CString tmp;

		tmp.Format (_T("%lu"), inf.Height);
		pISXMLAttributes->Add(CComBSTR("height"), CComBSTR(tmp));

		tmp.Format (_T("%lu"), inf.Width);
		pISXMLAttributes->Add(CComBSTR("width"), CComBSTR(tmp));

		pISXMLAttributes->Add(CComBSTR("description"), CComBSTR(inf.Description));

		CString colors;
		const int bits = inf.ComponentsPerPixel * inf.BitsPerComponent;
		if ( inf.ColorModel == GFL_CM_GREY )
			colors = _T("Greyscale");
		else
		{
			if (bits <= 4)
				colors = _T("16");
			else if (bits <= 8)
				colors = _T("256");
			else if (bits <= 16)
				colors = _T("64K");
			else
				colors = _T("16.7M");
		}
		pISXMLAttributes->Add(CComBSTR("colors"), CComBSTR(colors));
	}
	else
		hr = E_FAIL;

	return hr;
}
