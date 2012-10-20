//
// Statistics.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
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


class CStatistics
{
public:
	CStatistics();
	~CStatistics();

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
			QWORD	Queries;	// Incoming
			QWORD	QueriesProcessed;
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
		} BitTorrent, eDonkey, DC;
	}
	Ever, Today, Last, Current;

	void	Update();

protected:
	QWORD	m_tSeconds;		// ms

	static void Add(LPVOID pTarget, LPCVOID pSource, int nCount);
};

extern CStatistics Statistics;
