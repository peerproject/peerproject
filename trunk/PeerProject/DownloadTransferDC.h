//
// DownloadTransferDC.h
//
// This file is part of PeerProject (peerproject.org) © 2011
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

class CDCClient;
class CDownloadTransfer;


class CDownloadTransferDC : public CDownloadTransfer
{
public:
	CDownloadTransferDC(CDownloadSource* pSource, CDCClient* pClient);
	virtual ~CDownloadTransferDC();

	CDCClient*	m_pClient;		// Download owner

	virtual void	AttachTo(CConnection* pConnection);
	virtual BOOL	Initiate();
	virtual void	Close(TRISTATE bKeepSource, DWORD nRetryAfter = 0);
	virtual DWORD	GetMeasuredSpeed();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();

	// Got $ADCSND command
	BOOL			OnDownload(const std::string& strType, const std::string& strFilename, QWORD nOffset, QWORD nLength, const std::string& strOptions);
	BOOL			OnQueue(int nQueue);	// Got $MaxedOut command with queue position
	BOOL			OnBusy();				// Got $MaxedOut command without queue position
	BOOL			OnError();				// Got $Error command

protected:
	QWORD			m_nTigerLength;

	virtual BOOL	SubtractRequested(Fragments::List& ppFragments);
	BOOL			StartNextFragment();
	BOOL			ReadContent();
	BOOL			ReadTiger();
};
