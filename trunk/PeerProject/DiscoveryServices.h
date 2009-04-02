//
// DiscoveryServices.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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
	BOOL		m_bGnutella2;			// Webcache supports Gnutella 2
	BOOL		m_bGnutella1;			// Webcache supports Gnutella
	DWORD		m_tCreated;
	DWORD		m_tAccessed;
	DWORD		m_nAccesses;
	DWORD		m_tUpdated;
	DWORD		m_nUpdates;
	DWORD		m_nHosts;
	DWORD		m_nFailures;
	DWORD		m_nAccessPeriod;
	DWORD		m_nUpdatePeriod;
	SubType		m_nSubType;
	IN_ADDR		m_pAddress;
	WORD		m_nPort;

	void		Remove(BOOL bCheck = TRUE);
	void		OnSuccess();
	void		OnFailure();

protected:
	void		OnAccess();
	void		OnHostAdd(int nCount = 1);
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
	HINTERNET			m_hInternet;
	HINTERNET			m_hRequest;
	CDiscoveryService*	m_pWebCache;
	Mode				m_nWebCache;
	CDiscoveryService*	m_pSubmit;
	PROTOCOLID			m_nLastQueryProtocol;		// Protocol that was queried most recently
	DWORD				m_tUpdated;					// Time a webcache was last updated
	PROTOCOLID			m_nLastUpdateProtocol;		// Protocol that had a service update most recently
	BOOL				m_bFirstTime;
	DWORD				m_tExecute;					// Time the Execute() function was last run
	DWORD				m_tQueried;					// Time a webcache/MET was last queried
	DWORD				m_tMetQueried;				// Time a MET was last queried

public:
	POSITION			GetIterator() const;
	CDiscoveryService*	GetNext(POSITION& pos) const;
	BOOL				Check(CDiscoveryService* pService, CDiscoveryService::Type nType = CDiscoveryService::dsNull) const;
	CDiscoveryService*	Add(CDiscoveryService* pService);
	CDiscoveryService*	Add(LPCTSTR pszAddress, int nType, PROTOCOLID nProtocol = PROTOCOL_NULL);
	BOOL				CheckMinimumServices();
//	DWORD				MetQueried() const;
	DWORD				LastExecute() const;
	CDiscoveryService*	GetByAddress(LPCTSTR pszAddress) const;
	CDiscoveryService*	GetByAddress( IN_ADDR* pAddress, WORD nPort, CDiscoveryService::SubType nSubType );
	BOOL				Load();
	BOOL				Save();
	BOOL				Update();
	BOOL				Execute(BOOL bDiscovery, PROTOCOLID nProtocol, USHORT nForceDiscovery);
	void				Stop();
	void				OnGnutellaAdded(IN_ADDR* pAddress, int nCount);
	void				OnGnutellaFailed(IN_ADDR* pAddress);
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
	void				StopWebRequest();
	void				OnRun();
	BOOL 				WebCacheStore(CString Line, LPCTSTR ID, int SubmitType, PROTOCOLID AddrProtocol);
	BOOL				RunWebCacheGet(BOOL bCache);
	BOOL				RunWebCacheUpdate();
	BOOL				RunServerMet();
	BOOL				SendWebCacheRequest(CString strURL, CString& strOutput);
	BOOL				EnoughServices() const;
	void				AddDefaults();
	void				MergeURLs();

	friend class CDiscoveryService;
};

extern CDiscoveryServices DiscoveryServices;
