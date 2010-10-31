//
// PeerProjectURL.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#pragma once

#include "QuerySearch.h"

class CBTInfo;


class CPeerProjectURL : public CPeerProjectFile
{
// Construction
public:
	CPeerProjectURL(LPCTSTR pszURL = NULL);
	CPeerProjectURL(CBTInfo* pTorrent);
	CPeerProjectURL(const CPeerProjectURL& pURL);
	virtual ~CPeerProjectURL();

// Attributes
public:
	enum URI_TYPE
	{
		uriNull, uriSource, uriDownload, uriSearch,
		uriHost, uriBrowse, uriDonkeyServer, uriDiscovery
	};

	PROTOCOLID	m_nProtocol;
	URI_TYPE	m_nAction;
	CBTInfo*	m_pTorrent;
	CString		m_sAddress;
	IN_ADDR		m_pAddress;
	IN_ADDR		m_pServerAddress;
	WORD		m_nServerPort;
	WORD		m_nPort;
	BOOL		m_bSize;
	CString		m_sLogin;
	CString		m_sPassword;
	Hashes::BtGuid m_oBTC;

// Operations
public:
	BOOL	Parse(const CString& sText, CList< CString >& pURLs, BOOL bResolve = FALSE);	// Parse URL list
	BOOL	Parse(LPCTSTR pszURL, BOOL bResolve = TRUE);	// Parse single URL
	CQuerySearchPtr ToQuery() const;						// Construct CQuerySearch object

protected:
	void	Clear();

	BOOL	ParseRoot(LPCTSTR pszURL, BOOL bResolve);
	BOOL	ParseHTTP(LPCTSTR pszURL, BOOL bResolve);
	BOOL	ParseFTP(LPCTSTR pszURL, BOOL bResolve);
	BOOL	ParseED2KFTP(LPCTSTR pszURL, BOOL bResolve);
	BOOL	ParseBTC(LPCTSTR pszURL, BOOL bResolve);
	BOOL	ParseMagnet(LPCTSTR pszURL);
	BOOL	ParsePeerProject(LPCTSTR pszURL);
	BOOL	ParsePeerProjectHost(LPCTSTR pszURL, BOOL bBrows);
	BOOL	ParsePeerProjectFile(LPCTSTR pszURL);
	BOOL	ParseDonkey(LPCTSTR pszURL);
	BOOL	ParseDonkeyFile(LPCTSTR pszURL);
	BOOL	ParseDonkeyServer(LPCTSTR pszURL);
	BOOL	ParsePiolet(LPCTSTR pszURL);
	BOOL	ParsePioletFile(LPCTSTR pszURL);
	BOOL	ParseDiscovery(LPCTSTR pszURL, int nType);

	static void	SkipSlashes(LPCTSTR& pszURL, int nAdd = 0);
	static void	SafeString(CString& strInput);

// Registration Operations
public:
	static void	Register(BOOL bOnStartup = FALSE);
	static BOOL	RegisterShellType(LPCTSTR pszRoot, LPCTSTR pszProtocol,
		LPCTSTR pszName, LPCTSTR pszType, LPCTSTR pszApplication, LPCTSTR pszTopic, UINT nIDIcon, BOOL bOverwrite = TRUE);
	static BOOL	RegisterMagnetHandler(LPCTSTR pszID, LPCTSTR pszName, LPCTSTR pszDescription, LPCTSTR pszApplication, UINT nIDIcon);
	static BOOL	IsRegistered(LPCTSTR pszProtocol);
	static BOOL	UnregisterShellType(LPCTSTR pszRoot, LPCTSTR pszProtocol);
	static void DeleteKey(HKEY hParent, LPCTSTR pszKey);
};
