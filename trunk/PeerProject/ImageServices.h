//
// ImageServices.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#pragma once

#include "ThreadImpl.h"

class CImageFile;


class CImageServices : public CComObject, public CThreadImpl
{
	DECLARE_DYNCREATE(CImageServices)

public:
	CImageServices();
	virtual ~CImageServices();

	void		Clear();
	static BOOL	IsFileViewable(LPCTSTR pszPath);
	BOOL		LoadFromMemory(CImageFile* pFile, LPCTSTR pszType, LPCVOID pData, DWORD nLength, BOOL bScanOnly = FALSE, BOOL bPartialOk = FALSE);
	BOOL		LoadFromFile(CImageFile* pFile, LPCTSTR szFilename, BOOL bScanOnly = FALSE, BOOL bPartialOk = FALSE);
	BOOL		SaveToMemory(CImageFile* pFile, LPCTSTR pszType, int nQuality, LPBYTE* ppBuffer, DWORD* pnLength);
	BOOL		SaveToFile(CImageFile* pFile, LPCTSTR szFilename, int nQuality, DWORD* pnLength = NULL);

protected:
	typedef std::map< CLSID, DWORD > services_map;
	typedef std::list< std::pair< CLSID, CAdapt< CComPtr< IImageServicePlugin > > > > service_list;

	services_map	m_services;
	CMutex			m_pSection;
	CLSID			m_inCLSID;		// [in] Create interface
	CEvent			m_pReady;		// Ready event

	BOOL		LoadFromFileHelper(const CLSID& oCLSID, IImageServicePlugin* pService, CImageFile* pFile, LPCTSTR szFilename, BOOL bScanOnly, BOOL bPartialOk);
	BOOL		PostLoad(CImageFile* pFile, const IMAGESERVICEDATA* pParams, SAFEARRAY* pArray);
	static SAFEARRAY* ImageToArray(CImageFile* pFile);

	bool		LookupUniversalPlugins(service_list& oList);	// Get universal plugins
	static bool	GetService(const CLSID& oCLSID, IImageServicePlugin** pIImageServicePlugin); // Get plugin interface (and cache it)
	static bool	LoadService(const CLSID& oCLSID);				// (Re)Load plugin in plugin cache

	virtual void OnRun();

// IImageServicePlugin
	BEGIN_INTERFACE_PART(ImageService, IImageServicePlugin)
		STDMETHOD(LoadFromFile)( __in BSTR sFile, __inout IMAGESERVICEDATA* pParams, __out SAFEARRAY** ppImage );
		STDMETHOD(LoadFromMemory)( __in BSTR sType, __in SAFEARRAY* pMemory, __inout IMAGESERVICEDATA* pParams, __out SAFEARRAY** ppImage );
		STDMETHOD(SaveToFile)( __in BSTR sFile, __inout IMAGESERVICEDATA* pParams, __in SAFEARRAY* pImage);
		STDMETHOD(SaveToMemory)( __in BSTR sType, __out SAFEARRAY** ppMemory, __inout IMAGESERVICEDATA* pParams, __in SAFEARRAY* pImage);
	END_INTERFACE_PART(ImageService)

	DECLARE_INTERFACE_MAP()
};

extern CImageServices ImageServices;
