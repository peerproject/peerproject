//
// DiscoveryServices.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "HttpRequest.h"


// ToDo: Remove Network locks
// ToDo: Add support for concurrent webcache requests


class CDiscoveryService
{
public:
	enum Type
	{
		dsNull, dsGnutella, dsWebCache, dsServerMet, dsBlocked
	};

	enum SubType
	{
		dsOldBootStrap, dsGnutellaTCP, dsGnutella2TCP, dsGnutellaUDPHC, dsGnutella2UDPKHL
	};

	CDiscoveryService(Type nType = dsNull, LPCTSTR pszAddress = NULL);
	virtual ~CDiscoveryService();

	Type		m_nType;
	CString		m_sAddress;
	BOOL		m_bGnutella2;			// Webcache supports G2
	BOOL		m_bGnutella1;			// Webcache supports Gnutella
	DWORD		m_tCreated;
	DWORD		m_tAccessed;
	DWORD		m_nAccesses;
	DWORD		m_tUpdated;
	DWORD		m_nUpdates;
	DWORD		m_nHosts;
	DWORD		m_nTotalHosts;
	DWORD		m_nURLs;
	DWORD		m_nTotalURLs;
	DWORD		m_nFailures;
	DWORD		m_nAccessPeriod;
	DWORD		m_nUpdatePeriod;
	SubType		m_nSubType;
	IN_ADDR		m_pAddress;
	WORD		m_nPort;
	CString		m_sPong;

	void		Remove(BOOL bCheck = TRUE);
	void		OnSuccess();
	void		OnFailure();
	void		OnCopyGiven();			// Used in Datagrams.cpp

protected:
	void		OnAccess();
	void		OnGivenHosts();
	void		OnHostAdd(int nCount = 1);
	void		OnURLAdd(int nCount = 1);
	void		Serialize(CArchive& ar, int nVersion);
	BOOL		ResolveGnutella();

	friend class CDiscoveryServices;
};

class CDiscoveryServices :
	public CThreadImpl
{
public:
	CDiscoveryServices();
	virtual ~CDiscoveryServices();

	enum Mode
	{
		wcmHosts, wcmCaches, wcmUpdate, wcmSubmit, wcmServerMet
	};

protected:
	CList< CDiscoveryService* > m_pList;
	CHttpRequest		m_pRequest;
	CDiscoveryService*	m_pSubmit;
	CDiscoveryService*	m_pWebCache;
	Mode				m_nWebCache;
	PROTOCOLID			m_nLastQueryProtocol;		// Protocol that was queried most recently
	PROTOCOLID			m_nLastUpdateProtocol;		// Protocol that had a service update most recently
	DWORD				m_tUpdated;					// Time a webcache was last updated
	BOOL				m_bFirstTime;
	DWORD				m_tExecute;					// Time the Execute() function was last run
	DWORD				m_tQueried;					// Time a webcache/MET was last queried
	DWORD				m_tMetQueried;				// Time a MET was last queried

public:
	POSITION			GetIterator() const;
	CDiscoveryService*	GetNext(POSITION& pos) const;
	BOOL				Check(CDiscoveryService* pService, CDiscoveryService::Type nType = CDiscoveryService::dsNull) const;
	BOOL				Add(CDiscoveryService* pService);
	BOOL				Add(LPCTSTR pszAddress, int nType, PROTOCOLID nProtocol = PROTOCOL_NULL);
	BOOL				CheckMinimumServices();
//	DWORD				MetQueried() const;
	DWORD				LastExecute() const;
	CDiscoveryService*	GetByAddress(LPCTSTR pszAddress) const;
	CDiscoveryService*	GetByAddress(const IN_ADDR* pAddress, WORD nPort, CDiscoveryService::SubType nSubType );
	BOOL				Load();
	BOOL				Save();
	BOOL				Update();
	void				Stop();
	BOOL				Execute(BOOL bDiscovery, PROTOCOLID nProtocol, USHORT nForceDiscovery);
	BOOL				Execute(CDiscoveryService* pService, Mode nMode);

protected:
	void				Remove(CDiscoveryService* pService, BOOL bCheck = TRUE);
	DWORD				GetCount(int nType = 0, PROTOCOLID nProtocol = PROTOCOL_NULL) const;
	BOOL				CheckWebCacheValid(LPCTSTR pszAddress);
	void				Clear();
	int					ExecuteBootstraps( int nCount, BOOL bUDP = FALSE, PROTOCOLID nProtocol = PROTOCOL_NULL );
	void				Serialize(CArchive& ar);
	BOOL				RequestRandomService(PROTOCOLID nProtocol);
	CDiscoveryService*  GetRandomService(PROTOCOLID nProtocol);
	CDiscoveryService*	GetRandomWebCache(PROTOCOLID nProtocol, BOOL bWorkingOnly, CDiscoveryService* pExclude = NULL, BOOL bForUpdate = FALSE);
	BOOL				RequestWebCache(CDiscoveryService* pService, Mode nMode, PROTOCOLID nProtocol);
	void				OnRun();
	BOOL				RunWebCacheGet(BOOL bCache);
	BOOL				RunWebCacheUpdate();
	BOOL				RunServerMet();
	BOOL				SendWebCacheRequest(CString strURL, CString& strOutput);
	BOOL				EnoughServices() const;
	void				AddDefaults();
	void				MergeURLs();

	friend class CDiscoveryService;

private:
	CDiscoveryServices(const CDiscoveryServices&);
	CDiscoveryServices& operator=(const CDiscoveryServices&);
};

extern CDiscoveryServices DiscoveryServices;