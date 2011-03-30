//
// DCNeighbour.h
//
// This file is part of PeerProject (peerproject.org) © 2010-2011
// Portions copyright Shareaza Development Team, 2010.
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

#include "Neighbour.h"


class CDCNeighbour : public CNeighbour
{
public:
	CDCNeighbour();
	virtual ~CDCNeighbour();

public:
	CString			m_sNick;		// User nick on this hub
	BOOL			m_bExtended;	// Using extended protocol
	CStringList		m_oFeatures;	// Remote client supported features

	BOOL			ConnectToMe(const CString& sNick);		// Send $ConnectToMe command

	virtual BOOL	ConnectTo(const IN_ADDR* pAddress, WORD nPort, BOOL bAutomatic);
	virtual BOOL	Send(CPacket* pPacket, BOOL bRelease = TRUE, BOOL bBuffered = FALSE);

protected:
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnRead();

	BOOL			ReadCommand(std::string& strLine);		// Read single command from input buffer
	BOOL			OnCommand(const std::string& strCommand, const std::string& strParams);
	BOOL			OnHello();								// Got $Hello command
	BOOL			OnLock(const std::string& strLock);		// Got $Lock command
	BOOL			OnChat(const std::string& strMessage);	// Got chat message
	BOOL			OnSearch(const IN_ADDR* pAddress, WORD nPort, std::string& strSearch);	// Got search request
};
