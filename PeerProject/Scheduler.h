//
// Scheduler.h
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

class CScheduler
{
// Construction
public:
	CScheduler();
	~CScheduler();

// Attributes
public:
	BYTE			m_pSchedule[7][24];
protected:
	BYTE			m_nCurrentHour;
	DWORD			m_tLastCheck;

// Operations
public:
	BOOL			Load();
	void			Save();
	void			Update();
protected:
	void			SetVariables(BYTE nCurrentSettings);
	void			Serialize(CArchive& ar);
};

enum
{
	SCHEDULE_OFF, SCHEDULE_LIMITED_SPEED, SCHEDULE_FULL_SPEED
};

extern CScheduler Schedule;
