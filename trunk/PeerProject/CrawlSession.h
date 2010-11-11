//
// CrawlSession.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CG2Packet;
class CCrawlNode;


class CCrawlSession
{
// Construction
public:
	CCrawlSession();
	virtual ~CCrawlSession();

// Attributes
public:
	BOOL		m_bActive;
	CList< CCrawlNode*, CCrawlNode* > m_pNodes;

// Operations
public:
	void		Clear();
	void		Bootstrap();
	void		SendCrawl(SOCKADDR_IN* pHost);
	int			GetHubCount();
	int			GetLeafCount();

	void		OnRun();
	void		OnCrawl(const SOCKADDR_IN* pHost, CG2Packet* pPacket);

protected:
	CCrawlNode*	Find(const IN_ADDR* pAddress, BOOL bCreate);

	friend class CCrawlNode;
};


class CCrawlNode
{
// Construction
public:
	CCrawlNode();
	virtual ~CCrawlNode();

// Attributes
public:
	SOCKADDR_IN		m_pHost;
	int				m_nType;
	int				m_nLeaves;
	CString			m_sNick;
	float			m_nLatitude;
	float			m_nLongitude;

	POSITION		m_nUnique;
	DWORD			m_tDiscovered;
	DWORD			m_tCrawled;
	DWORD			m_tResponse;

	CList< CCrawlNode* > m_pNeighbours;

	enum { ntUnknown, ntHub, ntLeaf };

// Operations
public:
	void	OnCrawl(CCrawlSession* pSession, CG2Packet* pPacket);
protected:
	void	OnNode(CCrawlSession* pSession, CG2Packet* pPacket, DWORD nPacket, int nType);

	enum { parseSelf, parseHub, parseLeaf };
};

extern CCrawlSession CrawlSession;
