//
// ImageServices.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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


class CImageServices : public CThreadImpl
{
public:
	CImageServices();
	virtual ~CImageServices();

	void		Clear();
	static BOOL	IsFileViewable(LPCTSTR pszPath);
	BOOL		LoadFromMemory(CImageFile* pFile, LPCTSTR pszType, LPCVOID pData, DWORD nLength, BOOL bScanOnly = FALSE, BOOL bPartialOk = FALSE);
	BOOL		LoadFromFile(CImageFile* pFile, LPCTSTR szFilename, BOOL bScanOnly = FALSE, BOOL bPartialOk = FALSE);
	BOOL		SaveToMemory(CImageFile* pFile, LPCTSTR pszType, int nQuality, LPBYTE* ppBuffer, DWORD* pnLength);
//	BOOL		SaveToFile(CImageFile* pFile, LPCTSTR pszType, int nQuality, HANDLE hFile, DWORD* pnLength = NULL);

protected:
	typedef std::map< CLSID, DWORD > services_map;

	services_map	m_services;
	CMutex			m_pSection;

	CLSID			m_inCLSID;		// [in] Create interface
	CEvent			m_pReady;		// Ready event

	BOOL		PostLoad(CImageFile* pFile, const IMAGESERVICEDATA* pParams, SAFEARRAY* pArray);
	SAFEARRAY*	ImageToArray(CImageFile* pFile);

	// Get plugin interface for specified extension
	bool		GetService(LPCTSTR pszType, IImageServicePlugin** pIImageServicePlugin);

	// (Re)Load plugin in plugin cache for specified file extension
	bool		LoadService(LPCTSTR pszType);

	virtual void OnRun();
};

extern CImageServices ImageServices;
