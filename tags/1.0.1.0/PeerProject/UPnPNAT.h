//
// UPnPNAT.h
//
// This file is part of PeerProject (peerproject.org) © 2014
// Portions copyright Shareaza Development Team, 2014.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "UPnP.h"

class CUPnPNAT : public CUPnP, public CThreadImpl
{
public:
	CUPnPNAT();
	virtual ~CUPnPNAT();

public:
	virtual void DeletePorts();
	virtual void StartDiscovery();
	virtual void StopAsyncFind();
	virtual bool IsAsyncFindRunning();

protected:
	WORD	m_nExternalTCPPort;
	WORD	m_nExternalUDPPort;
	CString	m_sExternalAddress;

	void OnRun();

	// Create port mapping
	WORD MapPort(IStaticPortMappingCollection* pCollection, LPCWSTR szLocalIP, WORD nPort, LPCWSTR szProtocol, LPCWSTR szDescription);
};
