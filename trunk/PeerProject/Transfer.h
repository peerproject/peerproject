//
// Transfer.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "Connection.h"

class CBuffer;


class CTransfer abstract : public CConnection
{
// Construction
public:
	CTransfer(PROTOCOLID nProtocol = PROTOCOL_ANY);
	virtual ~CTransfer();

// Attributes
public:
	SOCKADDR_IN			m_pServer;			// Reference server (ED2K, DC++)
	CString				m_sRemoteNick;		// Remote user nick
	DWORD				m_nRunCookie;
	CList< CString >	m_pSourcesSent;
	CArray< CString >	m_pHeaderName;
	CArray< CString >	m_pHeaderValue;

	int					m_nState;			// Common state code
	QWORD				m_nLength;			// Fragment length
	QWORD				m_nOffset;			// Fragment offset
	QWORD				m_nPosition;		// Fragment position
	DWORD				m_nBandwidth;		// Bandwidth allocated
	DWORD				m_tRequest;			// The time a request was sent

// Operations
public:
	virtual BOOL	ConnectTo(const IN_ADDR* pAddress, WORD nPort);
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close(UINT nError = 0);

protected:
	void			ClearHeaders();
	virtual BOOL	OnHeaderLine(CString& strHeader, CString& strValue);
};
