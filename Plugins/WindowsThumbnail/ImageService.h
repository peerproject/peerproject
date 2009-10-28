//
// ImageService.h : Declaration of the CImageService
//
// This file is part of PeerProject (peerproject.org) © 2009
// Portions Copyright Nikolay Raspopov, 2009.
//
// PeerProject is free software; you can redistribute it
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

#include "Resource.h"
#include "WindowsThumbnail.h"

class ATL_NO_VTABLE CImageService : 
	public CComObjectRootEx< CComMultiThreadModel >,
	public CComCoClass< CImageService, &CLSID_ImageService >,
	public IImageServicePlugin
{
public:
	CImageService() throw();

DECLARE_REGISTRY_RESOURCEID(IDR_IMAGESERVICE)

BEGIN_COM_MAP(CImageService)
	COM_INTERFACE_ENTRY(IImageServicePlugin)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() throw();
	void FinalRelease() throw();

protected:
	HMODULE m_hShell32;
	HRESULT (WINAPI *m_pfnSHCreateItemFromParsingName)(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv);
	CComPtr< IThumbnailCache > m_pCache;

	static HRESULT SafeGetThumbnail(IThumbnailCache* pCache, IShellItem* pItem, ISharedBitmap** ppBitmap) throw();
	static HRESULT LoadFromBitmap(HBITMAP hBitmap, IMAGESERVICEDATA* pParams, SAFEARRAY** ppImage) throw();

// IImageServicePlugin
public:
	STDMETHOD(LoadFromFile)(
		/* [in] */ BSTR sFile,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [out] */ SAFEARRAY** ppImage );
	STDMETHOD(LoadFromMemory)(
		/* [in] */ BSTR sType,
		/* [in] */ SAFEARRAY* pMemory,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [out] */ SAFEARRAY** ppImage );
	STDMETHOD(SaveToFile)(
		/* [in] */ BSTR sFile,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [in] */ SAFEARRAY* pImage);
	STDMETHOD(SaveToMemory)(
		/* [in] */ BSTR sType,
		/* [out] */ SAFEARRAY** ppMemory,
		/* [in,out] */ IMAGESERVICEDATA* pParams,
		/* [in] */ SAFEARRAY* pImage);
};

OBJECT_ENTRY_AUTO(__uuidof(ImageService), CImageService)
