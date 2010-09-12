//
// Scheduler.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CXMLElement;


#define SCHEDULER_SER_VERSION	1000	// 1


// Add new tasks here: (In order)
enum ScheduleTask
{
	BANDWIDTH_FULL		= 0x1,
	BANDWIDTH_LIMITED	= 0x2,
	BANDWIDTH_STOP		= 0x4,
	SYSTEM_EXIT 		= 0x8,
	SYSTEM_SHUTDOWN 	= 0x10,
	SYSTEM_DISCONNECT	= 0x20,
	SYSTEM_NOTICE		= 0x40
};

enum DayOfWeek
{
	SUNDAY		= 0x1,
	MONDAY		= 0x2,
	TUESDAY 	= 0x4,
	WEDNESDAY	= 0x8,
	THURSDAY	= 0x10,
	FRIDAY		= 0x20,
	SATURDAY	= 0x40
};


//////////////////////////////////////////////////////////////////////
// CScheduleTask class: Represents a scheduled task
//

class CScheduleTask
{
// Construction
public:
	CScheduleTask(BOOL bCreate = TRUE);
	CScheduleTask(const CScheduleTask& pItem);
	virtual ~CScheduleTask();

// Attributes
public:
	unsigned int	m_nDays;			// Will have a combination of DayOfWeek
	unsigned int	m_nAction;			// Will have one of ScheduleTask values plus 0 as invalid state indicator
	bool			m_bSpecificDays;	// Task is scheduled for everyday or just today
	CString			m_sDescription;		// Optional task description
	CTime			m_tScheduleDateTime;// Time the task is scheduled for
	bool			m_bActive;			// Task should be executed or not
	bool			m_bExecuted;		// Task is executed or not
	bool			m_bLimitedNetworks;	// Network is limited to G2 or not (in SCHEDULE_LIMITED_SPEED)
	int				m_nLimit;			// Obsolete Bandwidth limit (ToDo: Remove)
	int				m_nLimitDown;		// Downstream bandwidth limit
	int				m_nLimitUp;			// Upstream bandwidth limit
	GUID			m_pGUID;			// GUID for each scheduled item

// Operations
public:
	CXMLElement*	ToXML();
	BOOL			FromXML(CXMLElement* pXML);
	void			Serialize(CArchive& ar, int nVersion);
};


//////////////////////////////////////////////////////////////////////
// CScheduler class: Controls scheduler operations
//

class CScheduler
{
// Construction
public:
	CScheduler();
	virtual ~CScheduler();

// Attributes
public:
	static LPCTSTR					xmlns;

	// Lock used when objects read/write to/from m_pScheduleItems
	mutable CCriticalSection		m_pSection;

protected:
	CList< CScheduleTask* >			m_pScheduleTasks;

// Operations
public:

	// To iterate through m_pScheduleItems
	inline POSITION	GetIterator() const
	{
		return m_pScheduleTasks.GetHeadPosition();
	}

	inline CScheduleTask*	GetNext(POSITION& pos) const
	{
		return m_pScheduleTasks.GetNext( pos );
	}

	inline int		GetCount() const
	{
		return m_pScheduleTasks.GetCount();
	}

	// Checks to see pItem exists in m_pScheduleItems or not, by comparing GUID values
	bool			Check(CScheduleTask* pSchTask) const
	{
		return pSchTask != NULL && GetGUID( pSchTask->m_pGUID ) != NULL;
	}

	// It is called regularly by timers to see if any scheduled item should be executed
	// Also sets Settings.Scheduler.Enable to indiate globally if any item is going to be executed
	void			CheckSchedule();		// Called regularly by timers to check if any scheduled item should be executed, also sets Settings.Scheduler.Enable
	void			HangUpConnection(); 	// Used to disconnect dial up connection
	bool			ShutDownComputer(); 	// Used to shut down computer 
	bool			SetShutdownRights();	// Called by Load() to get shutdown privilege for the process
	bool			IsScheduledTimePassed(CScheduleTask* pSchTask) const;	// Called by CheckSchedule() to see if Now is grater than Then or not.
	int				ScheduleFromToday(CScheduleTask* pSchTask) const;		// Checks to see if task should be executed today 0, in the past -1, or later 1.

	void			Add(CScheduleTask* pSchTask);		// Adds a new task to m_pScheduleItems after giving it a GUID
	void			Remove(CScheduleTask* pSchTask);	// Removes a task from m_pScheduleItems if it exists
	void			Clear();							// Clears all m_pScheduleItems items
	BOOL			Save();
	BOOL			Load();
	BOOL			Import(LPCTSTR pszFile);

	// Calculates hours remaining to execution of a combination of scheduled tasks
	// Example: nEventCombination = BANDWIDTH_FULL | SYSTEM_DISCONNECT
	LONGLONG		GetHoursTo(unsigned int nTaskCombination);

protected:
	CXMLElement*	ToXML(BOOL bTasks);
	BOOL			FromXML(CXMLElement* pXML);
	void			Serialize(CArchive& ar);

	void			ExecuteScheduledTask(CScheduleTask* pItem);	// Called by CheckSchedule() to execute a task

	CScheduleTask*	GetGUID(const GUID& pGUID) const;
};

extern CScheduler Scheduler;
