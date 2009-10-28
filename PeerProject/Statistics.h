//
// Statistics.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

class CStatistics
{
// Construction
public:
	CStatistics();
	virtual ~CStatistics();

// Attributes
public:
	struct
	{
		struct
		{
			QWORD	Connected;
			QWORD	Hub;
			QWORD	Ultrapeer;
		} Timer;

		struct
		{
			QWORD	Outgoing;
			QWORD	Incoming;
			QWORD	Errors;
		} Connections;

		struct
		{
			QWORD	Outgoing;
			QWORD	Incoming;
		} Bandwidth;

		struct
		{
			QWORD	Files;
			QWORD	Volume;
		} Uploads;

		struct
		{
			QWORD	Files;
			QWORD	Volume;
		} Downloads;

		struct
		{
			QWORD	Outgoing;
			QWORD	Incoming;
			QWORD	Routed;
			QWORD	Dropped;
			QWORD	Lost;
			QWORD	Queries;
			QWORD	PingsSent;
			QWORD	PingsReceived;
			QWORD	PongsSent;
			QWORD	PongsReceived;
		} Gnutella1, Gnutella2;

		struct
		{
			QWORD	Outgoing;
			QWORD	Incoming;
			QWORD	Dropped;
		} eDonkey;
	}
	Ever, Today, Last, Current;

	DWORD	m_tUpdate;
	DWORD	m_tSeconds;

// Operations
public:
	void	Update();
protected:
	static void Add(LPVOID pTarget, LPCVOID pSource, int nCount);
};

extern CStatistics Statistics;
