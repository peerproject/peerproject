//
// Scheduler.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007, 2010.
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
// Explanation:
// Scheduler module consists of four objects that work together to
// setup and execute user defined events. These objects are:
// ScheduleTask: Keeps type and time of execution for a particular task.
// Scheduler: Keeps list of ScheduleTasks. Iterates through tasks to see if a time has come, and sets global Settings.Scheduler.Enable if not.
// WndScheduler: Window that shows list of ScheduleTasks and enables user to add, edit, remove, import or export tasks.
// DlgScheduleTask: The dialog to create or edit a ScheduleTask.

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "DlgHelp.h"
#include "Scheduler.h"
#include "Network.h"
#include "XML.h"

#include "atltime.h"
#include "Reason.h"
#include "Ras.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CScheduler Scheduler;

//////////////////////////////////////////////////////////////////////
// CScheduler load and save

BOOL CScheduler::Load()
{
	CQuickLock oLock( Scheduler.m_pSection );
	CFile pFile;
	CString strFile = Settings.General.UserPath + _T("\\Data\\Scheduler.dat");

	if ( ! pFile.Open( strFile, CFile::modeRead ) )
	{
		theApp.Message( MSG_ERROR, _T("Failed to open Scheduler.dat") );
		return FALSE;
	}

	try
	{
		CArchive ar( &pFile, CArchive::load );	// 4 KB buffer
		Serialize( ar );
		ar.Close();
	}
	catch ( CException* pException )
	{
		pException->Delete();
	}

	pFile.Close();

	return TRUE;
}

BOOL CScheduler::Save()
{
	CQuickLock oLock( Scheduler.m_pSection );

	CString strFile = Settings.General.UserPath + _T("\\Data\\Scheduler.dat");

	CFile pFile;
	if ( ! pFile.Open( strFile, CFile::modeWrite|CFile::modeCreate ) )
		return FALSE;

	try
	{
		CArchive ar( &pFile, CArchive::store );	// 4 KB buffer
		try
		{
			Serialize( ar );
			ar.Close();
		}
		catch ( CException* pException )
		{
			ar.Abort();
			pFile.Abort();
			pException->Delete();
			return FALSE;
		}
		pFile.Close();
	}
	catch ( CException* pException )
	{
		pFile.Abort();
		pException->Delete();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CScheduler serialize

void CScheduler::Serialize(CArchive& ar)
{
	int nVersion = SCHEDULER_SER_VERSION;

	if ( ar.IsStoring() )
	{
		ar << nVersion;
		ar.WriteCount( GetCount() );	// Write the number of scheduled tasks

		for ( POSITION pos = GetIterator() ; pos ; )
		{
			CScheduleTask *pSchTask = GetNext( pos );	// Get a pointer to each task
			pSchTask->Serialize( ar, nVersion );		// Store each task's data
		}
	}
	else // Loading
	{
		Clear();	// First clear any existing tasks

		ar >> nVersion;

		for ( int nNumTasks = ar.ReadCount(); nNumTasks > 0; nNumTasks-- )	// Read the number of tasks to load
		{
			CScheduleTask *pSchTask = new CScheduleTask();	// Create a new instance of each task
			pSchTask->Serialize( ar,nVersion );				// Read each task's data
			m_pScheduleTasks.AddTail( pSchTask );			// Add the task to the task list
		}
	}
}


//////////////////////////////////////////////////////////////////////
// CScheduleTask class construction

CScheduleTask::CScheduleTask(BOOL bCreate)
{
	m_bSpecificDays = false;
	m_nDays = 0x7F; 	// All days of week
	m_nAction = 0;		// Invalid value
	m_sDescription = "";
	m_tScheduleDateTime = 0;

	m_bActive = false;
	m_bExecuted = false;

	if ( bCreate )
		CoCreateGuid( &m_pGUID );
}

CScheduleTask::CScheduleTask(const CScheduleTask& pSchTask)
{
	*this = pSchTask;
}

CScheduleTask::~CScheduleTask()
{
}

//////////////////////////////////////////////////////////////////////
// CScheduleTask serialize

void CScheduleTask::Serialize(CArchive& ar, int nVersion)
{
	if ( ar.IsStoring() )
	{
		// Store all task variables
		ar << m_bSpecificDays;
		ar << m_nAction;
		ar << m_sDescription;
		ar << m_tScheduleDateTime;
		ar << m_bActive;
		ar << m_bExecuted;
		ar << m_nLimitDown;
		ar << m_nLimitUp;
		ar << m_bLimitedNetworks;
		ar << m_nDays;
		ar.Write( &m_pGUID, sizeof( GUID ) );
	}
	else // Loading
	{
		if ( nVersion == 1000 ) // Initial release
		{
			// Load all task variables
			ar >> m_bSpecificDays;
			ar >> m_nAction;
			ar >> m_sDescription;
			ar >> m_tScheduleDateTime;
			ar >> m_bActive;
			ar >> m_bExecuted;
			ar >> m_nLimitDown;
			ar >> m_nLimitUp;
			ar >> m_bLimitedNetworks;
			ar >> m_nDays;
			ReadArchive( ar, &m_pGUID, sizeof( GUID ) );
		}
		else if ( nVersion < 100 ) // Shareaza import ?
		{
			// Load all task variables
			ar >> m_bSpecificDays;
			ar >> m_nAction;
			ar >> m_sDescription;
			ar >> m_tScheduleDateTime;
			ar >> m_bActive;
			ar >> m_bExecuted;
			ar >> m_nLimitDown;
			m_nLimitUp = m_nLimitDown;
			ar >> m_bLimitedNetworks;	//ar >> m_bToggleBandwidth;
			ar >> m_bLimitedNetworks;
			ar >> m_nDays;
			ReadArchive( ar, &m_pGUID, sizeof( GUID ) );
		}
		// else unknown serialize version
	}
}
/////////////////////////////////////////////////////////////////////
// CSchedulerTask XML

// Task type XML standards:
#define BANDWIDTH_FULL_TEXT			_T("Bandwidth: Full")
#define BANDWIDTH_LIMITED_TEXT		_T("Bandwidth: Limited")
#define BANDWIDTH_STOP_TEXT 		_T("Bandwidth: Stop")
#define SYSTEM_EXIT_TEXT			_T("System: Exit")
#define SYSTEM_SHUTDOWN_TEXT		_T("System: Shutdown")
#define SYSTEM_DISCONNECT_TEXT		_T("System: Disconnect Dial-Up")
#define SYSTEM_NOTICE_TEXT			_T("System: Notice")
// Legacy Shareaza import:
#define BANDWIDTH_FULL_TEXT_ALT 	_T("Bandwidth - Full Speed")
#define BANDWIDTH_LIMITED_TEXT_ALT	_T("Bandwidth - Reduced Speed")
#define BANDWIDTH_STOP_TEXT_ALT 	_T("Bandwidth - Stop")
#define SYSTEM_EXIT_TEXT_ALT		_T("System - Exit Shareaza")
#define SYSTEM_SHUTDOWN_TEXT_ALT	_T("System - Shutdown")
#define SYSTEM_DISCONNECT_TEXT_ALT	_T("System - Dial-Up Disconnect")

CXMLElement* CScheduleTask::ToXML()
{
	CXMLElement* pXML = new CXMLElement( NULL, _T("task") );
	CString strValue;

	if ( ! m_sDescription.IsEmpty() )
		pXML->AddAttribute( _T("description"), m_sDescription );

	switch ( m_nAction )
	{
	case BANDWIDTH_FULL:
		pXML->AddAttribute( _T("action"), BANDWIDTH_FULL_TEXT );
		break;
	case BANDWIDTH_LIMITED:
		pXML->AddAttribute( _T("action"), BANDWIDTH_LIMITED_TEXT );
		break;
	case BANDWIDTH_STOP:
		pXML->AddAttribute( _T("action"), BANDWIDTH_STOP_TEXT );
		break;
	case SYSTEM_EXIT:
		pXML->AddAttribute( _T("action"), SYSTEM_EXIT_TEXT );
		break;
	case SYSTEM_SHUTDOWN:
		pXML->AddAttribute( _T("action"), SYSTEM_SHUTDOWN_TEXT );
		break;
	case SYSTEM_DISCONNECT:
		pXML->AddAttribute( _T("action"), SYSTEM_DISCONNECT_TEXT );
		break;
	case SYSTEM_NOTICE:
		pXML->AddAttribute( _T("action"), SYSTEM_NOTICE_TEXT );
		break;
	}

	strValue.Format( _T("%I64i"), m_tScheduleDateTime.GetTime() );
	pXML->AddAttribute( _T("time"), strValue );

	pXML->AddAttribute( _T("active"), m_bActive ? _T("Yes") : _T("No") );
	pXML->AddAttribute( _T("executed"), m_bExecuted ? _T("Yes") : _T("No") );
	pXML->AddAttribute( _T("specificdays"), m_bSpecificDays ? _T("Yes") : _T("No") );
	pXML->AddAttribute( _T("limitednet"), m_bLimitedNetworks ? _T("Yes") : _T("No") );

	strValue.Format( _T("%i") , m_nLimitDown );
	pXML->AddAttribute( _T("limitdown"), strValue );

	strValue.Format( _T("%i") , m_nLimitUp );
	pXML->AddAttribute( _T("limitup"), strValue );

	strValue.Format( _T("%i|%i|%i|%i|%i|%i|%i") ,
		(m_nDays & SUNDAY) != 0,
		(m_nDays & MONDAY) != 0,
		(m_nDays & TUESDAY) != 0,
		(m_nDays & WEDNESDAY) != 0,
		(m_nDays & THURSDAY) != 0,
		(m_nDays & FRIDAY) != 0,
		(m_nDays & SATURDAY) != 0 );

	pXML->AddAttribute( _T("days"), strValue );

	wchar_t szGUID[39];
	szGUID[ StringFromGUID2( *(GUID*)&m_pGUID, szGUID, 39 ) - 2 ] = 0;
	pXML->AddAttribute( _T("guid"), (CString)&szGUID[1] );

	return pXML;
}

BOOL CScheduleTask::FromXML(CXMLElement* pXML)
{
	CString strValue;

	m_sDescription = pXML->GetAttributeValue( _T("description") );

	strValue = pXML->GetAttributeValue( _T("action") );

	if ( strValue == BANDWIDTH_FULL_TEXT )
		m_nAction = BANDWIDTH_FULL;
	else if ( strValue == BANDWIDTH_LIMITED_TEXT )
		m_nAction = BANDWIDTH_LIMITED;
	else if ( strValue == BANDWIDTH_STOP_TEXT )
		m_nAction = BANDWIDTH_STOP;
	else if ( strValue == SYSTEM_EXIT_TEXT )
		m_nAction = SYSTEM_EXIT;
	else if ( strValue == SYSTEM_SHUTDOWN_TEXT )
		m_nAction = SYSTEM_SHUTDOWN;
	else if ( strValue == SYSTEM_DISCONNECT_TEXT )
		m_nAction = SYSTEM_DISCONNECT;
	else if ( strValue == SYSTEM_NOTICE_TEXT )
		m_nAction = SYSTEM_NOTICE;
	else if ( strValue == BANDWIDTH_FULL_TEXT_ALT )
		m_nAction = BANDWIDTH_FULL;
	else if ( strValue == BANDWIDTH_LIMITED_TEXT_ALT )
		m_nAction = BANDWIDTH_LIMITED;
	else if ( strValue == BANDWIDTH_STOP_TEXT_ALT )
		m_nAction = BANDWIDTH_STOP;
	else if ( strValue == SYSTEM_EXIT_TEXT_ALT )
		m_nAction = SYSTEM_EXIT;
	else if ( strValue == SYSTEM_SHUTDOWN_TEXT_ALT )
		m_nAction = SYSTEM_SHUTDOWN;
	else if ( strValue == SYSTEM_DISCONNECT_TEXT_ALT )
		m_nAction = SYSTEM_DISCONNECT;
	else
		return FALSE;

	strValue = pXML->GetAttributeValue( _T("time") );
	__time64_t tTemp;
	if ( _stscanf( strValue, _T("%I64i"), &tTemp ) == EOF )
		return FALSE;
	if ( tTemp > 0 )
		m_tScheduleDateTime = tTemp;
	else
		return FALSE;

	strValue = pXML->GetAttributeValue( _T("active") );
	if ( strValue == _T("Yes") )
		m_bActive = TRUE;
	else if( strValue == _T("No") )
		m_bActive = FALSE;
	else
		return FALSE;

	strValue = pXML->GetAttributeValue( _T("specificdays") );
	if ( strValue == _T("Yes") )
		m_bSpecificDays = TRUE;
	else if ( strValue == _T("No") )
		m_bSpecificDays = FALSE;
	else
		return FALSE;

	strValue = pXML->GetAttributeValue( _T("executed") );
	if ( strValue == _T("Yes") )
		m_bExecuted = TRUE;
	else if ( strValue == _T("No") )
		m_bExecuted = FALSE;
	else
		return FALSE;

	strValue = pXML->GetAttributeValue( _T("limitednet") );
	if ( strValue == _T("Yes") )
		m_bLimitedNetworks = TRUE;
	else if ( strValue == _T("No") )
		m_bLimitedNetworks = FALSE;
	else
		return FALSE;

	strValue = pXML->GetAttributeValue( _T("limitdown") );
	if ( _stscanf( strValue, _T("%i"), &m_nLimitDown) == EOF ) return FALSE;

	strValue = pXML->GetAttributeValue( _T("limitup") );
	if ( _stscanf( strValue, _T("%i"), &m_nLimitUp) == EOF ) return FALSE;

	strValue = pXML->GetAttributeValue( _T("days") );

	// _wtoi returns 0 on failure so bad data won't crash the application
	m_nDays = 0;
	wchar_t wcTmp;
	wcTmp = strValue[0];
	if ( _wtoi( &wcTmp ) ) m_nDays |= SUNDAY;
	wcTmp =strValue[2];
	if ( _wtoi( &wcTmp ) ) m_nDays |= MONDAY;
	wcTmp =strValue[4];
	if ( _wtoi( &wcTmp ) ) m_nDays |= TUESDAY;
	wcTmp =	strValue[6];
	if ( _wtoi( &wcTmp ) ) m_nDays |= WEDNESDAY;
	wcTmp =	strValue[8];
	if ( _wtoi( &wcTmp ) ) m_nDays |= THURSDAY;
	wcTmp =strValue[10];
	if ( _wtoi( &wcTmp ) ) m_nDays |= FRIDAY;
	wcTmp =strValue[12];
	if ( _wtoi( &wcTmp ) ) m_nDays |= SATURDAY;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CScheduler class construction

CScheduler::CScheduler()
{
}

CScheduler::~CScheduler()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
//GUID

CScheduleTask* CScheduler::GetGUID(const GUID& pGUID) const
{
	CQuickLock oLock( m_pSection );

	for ( POSITION pos = m_pScheduleTasks.GetHeadPosition() ; pos ; )
	{
		CScheduleTask* pSchTask = m_pScheduleTasks.GetNext( pos );
		if ( pSchTask->m_pGUID == pGUID )
			return pSchTask;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CScheduler item modification

void CScheduler::Add(CScheduleTask* pSchTask)
{
	CQuickLock oLock( m_pSection );

	CScheduleTask* pExistingTask = GetGUID( pSchTask->m_pGUID );
	if ( pExistingTask == NULL )
	{
		m_pScheduleTasks.AddHead( pSchTask );
	}
	else if ( pExistingTask != pSchTask )
	{
		*pExistingTask = *pSchTask;
		delete pSchTask;
	}
}

void CScheduler::Remove(CScheduleTask* pSchTask)
{
	CQuickLock oLock( m_pSection );

	POSITION pos = m_pScheduleTasks.Find( pSchTask );
	if ( pos )
		m_pScheduleTasks.RemoveAt( pos );
	delete pSchTask;
}

void CScheduler::Clear()
{
	CQuickLock oLock( m_pSection );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		delete GetNext( pos );
	}

	m_pScheduleTasks.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
// CScheduler schedule checking

void CScheduler::CheckSchedule()
{
	//m_tLastCheckTicks = 0;
	bool bSchedulerIsEnabled = false;
	CTime tNow = CTime::GetCurrentTime();

	// Enable it to test GetHoursTo()
	//int nHoursToDisconnect = Scheduler.GetHoursTo(BANDWIDTH_STOP|SYSTEM_DISCONNECT|SYSTEM_EXIT|SYSTEM_SHUTDOWN );
	//theApp.Message( MSG_DEBUG, _T("Calculated time to disconnect is %i hours."), nHoursToDisconnect );

	CQuickLock oLock(m_pSection);
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CScheduleTask *pSchTask = GetNext( pos );

		// Check if a task should be executed
		if ( pSchTask->m_bActive )
		{
			bSchedulerIsEnabled = true;	// We always ignore deactivated tasks

			// Shorter if structure
			if ( pSchTask->m_bExecuted )
			{
				// Task is executed and active. The task is either "Only Once" or "Specific Days of Week"
				// In the first case if the date is for the days passed, its a task not executed and expired
				// In the second case it should mark as not executed so in the next CheckSchedule() call it will enter else block.
				if( ! pSchTask->m_bSpecificDays || ( ScheduleFromToday( pSchTask ) < 0 ) )
					pSchTask->m_bExecuted = false;
			}
			else if( IsScheduledTimePassed( pSchTask ) )
			{
				// Time is passed so task should be executed if one of two conditions is met:
				// It is scheduled for a specific date and time ("Only Once"). Checking for date.
				// Or, it is scheduled for specific days of week. Checking for day.

				if( ( ! pSchTask->m_bSpecificDays && ScheduleFromToday( pSchTask ) == 0 )  ||
					( pSchTask->m_bSpecificDays && ( ( 1 << ( tNow.GetDayOfWeek() - 1 ) ) & pSchTask->m_nDays ) ) )
				{
					//static_cast<int>(pow(2.0f, tNow.GetDayOfWeek() - 1)
					// It will also mark it as executed
					ExecuteScheduledTask( pSchTask );

					// If active but not executed, scheduler will remain enabled
					bSchedulerIsEnabled = false;

					// Smart way for deactivating task if it is "Only Once"
					pSchTask->m_bActive = pSchTask->m_bSpecificDays;

					// Setting the date of task to last execution for further checks
					pSchTask->m_tScheduleDateTime = CTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(),
						pSchTask->m_tScheduleDateTime.GetHour(), pSchTask->m_tScheduleDateTime.GetMinute(), pSchTask->m_tScheduleDateTime.GetSecond() );
				}
			}

			Scheduler.Save();
		}
	}

	// Scheduler is enabled when an active task is scheduled
	Settings.Scheduler.Enable = bSchedulerIsEnabled;
}

//////////////////////////////////////////////////////////////////////
// CScheduler execute task (Add new tasks here)

void CScheduler::ExecuteScheduledTask(CScheduleTask *pSchTask)
{
	// Execute the selected scheduled task
	pSchTask->m_bExecuted = true;

	switch ( pSchTask->m_nAction )
	{
	case BANDWIDTH_FULL:			// Set the bandwidth to full speed
		theApp.Message( MSG_NOTICE, _T("Scheduler| Bandwidth: Full") );
		Settings.Live.BandwidthScaleIn	= 101;
		Settings.Live.BandwidthScaleOut	= 101;
		Settings.Bandwidth.Downloads	= 0;
		Settings.Bandwidth.Uploads		= ( ( ( Settings.Connection.OutSpeed * ( 100 - Settings.Uploads.FreeBandwidthFactor ) ) / 100 ) / 8 ) * 1024;
		Settings.Gnutella2.EnableToday	= TRUE;
		Settings.Gnutella1.EnableToday	= Settings.Gnutella1.EnableAlways;
		Settings.eDonkey.EnableToday	= Settings.eDonkey.EnableAlways;
		if ( ! Network.IsConnected() )
			Network.Connect( TRUE );
		break;

	case BANDWIDTH_LIMITED:			// Set the bandwidth to limited speeds
		theApp.Message( MSG_NOTICE, _T("Scheduler| Bandwidth: Limited") );
		Settings.Live.BandwidthScaleIn	= pSchTask->m_nLimitDown;
		Settings.Live.BandwidthScaleOut	= pSchTask->m_nLimitUp;
		Settings.Bandwidth.Downloads	= ( Settings.Connection.InSpeed * 1024) / 8;
		Settings.Bandwidth.Uploads		= ( ( ( Settings.Connection.OutSpeed * ( 100 - Settings.Uploads.FreeBandwidthFactor ) ) / 100 ) / 8 ) * 1024;

		Settings.Gnutella2.EnableToday	= TRUE;
		Settings.Gnutella1.EnableToday	= pSchTask->m_bLimitedNetworks ? FALSE : Settings.Gnutella1.EnableAlways;
		Settings.eDonkey.EnableToday	= pSchTask->m_bLimitedNetworks ? FALSE : Settings.eDonkey.EnableAlways;
		if ( ! Network.IsConnected() )
			Network.Connect( TRUE );
		break;

	case BANDWIDTH_STOP:			// Set the bandwidth to 0 and disconnect all networks
		theApp.Message( MSG_NOTICE, _T("Scheduler| Bandwidth: Stop") );
		Settings.Live.BandwidthScaleIn	= 0;
		Settings.Live.BandwidthScaleOut	= 0;
		Settings.Gnutella2.EnableToday	= FALSE;
		Settings.Gnutella1.EnableToday	= FALSE;
		Settings.eDonkey.EnableToday	= FALSE;
		if ( Network.IsConnected() )
			Network.Disconnect();
		break;

	case SYSTEM_EXIT:				// Exit PeerProject
		theApp.Message( MSG_DEBUG, _T("Scheduler| System: Exit PeerProject") );
		if ( ! PostMainWndMessage( WM_CLOSE ) )
			theApp.Message( MSG_ERROR, _T("Scheduler failed to send CLOSE message") );
		break;

	case SYSTEM_SHUTDOWN:			// Shut down the computer
		theApp.Message( MSG_NOTICE, _T("Scheduler| System: Shut Down Computer") );

		if ( ! SetShutdownRights() ) // If we dont have shutdown rights
		{
			theApp.Message( MSG_DEBUG, _T("Insufficient rights to shut down the system") );
			return;
		}
		if ( ShutDownComputer() )
		{
			// Close PeerProject if shutdown successfully started
			if( ! PostMainWndMessage( WM_CLOSE ) )
				theApp.Message( MSG_ERROR, _T("Scheduler failed to send CLOSE message") );
			else
				theApp.Message( MSG_DEBUG, _T("System shutdown failed!") );
		}
		break;

	case SYSTEM_DISCONNECT:			// Dial-Up Connection
		theApp.Message( MSG_NOTICE, _T("Scheduler| System: Disconnect Dial-Up") );
		Settings.Live.BandwidthScaleIn	= 0;
		Settings.Live.BandwidthScaleOut	= 0;
		Settings.Gnutella2.EnableToday	= FALSE;
		Settings.Gnutella1.EnableToday	= FALSE;
		Settings.eDonkey.EnableToday	= FALSE;
		if ( Network.IsConnected() )
			Network.Disconnect();
		HangUpConnection();			// Close
		break;

	case SYSTEM_NOTICE:				// Reminder Notes
		LoadString( IDS_SCHEDULER_REMINDER_NOTICE );
		theApp.Message( MSG_NOTICE, _T("Scheduler| System: Reminder Notice") );
		theApp.Message( MSG_TRAY, LoadString( IDS_SCHEDULER_REMINDER_NOTICE ) );

		PostMainWndMessage( WM_COMMAND, ID_TRAY_OPEN );

		pSchTask->m_bActive = false; // Repeat AfxMsgBox workaround
		AfxMessageBox( LoadString( IDS_SCHEDULER_REMINDER_NOTICE ) + _T("\n\n") + pSchTask->m_sDescription, MB_OK );
		pSchTask->m_bActive = true;
		break;

	default: // Error
		pSchTask->m_bExecuted = false;
		theApp.Message( MSG_ERROR, _T("Scheduler| Invalid task scheduled") );
	}
}

// Disconnect a dial-up connection
void CScheduler::HangUpConnection()
{
	DWORD dwCb = sizeof( RASCONN );
	DWORD dwConnections = 0;
	RASCONN* lpRasConn = NULL;
	LPRASCONNSTATUS RasConStatus = 0;

	// Allocate the size needed for the RAS structure.
	lpRasConn = (RASCONN*)HeapAlloc( GetProcessHeap(), 0, dwCb );

	// Set the structure size for version checking purposes.
	lpRasConn->dwSize = sizeof( RASCONN );

	// Call the RAS API
	RasEnumConnections( lpRasConn, &dwCb, &dwConnections );

	DWORD i;
	int loop = 0;

	for ( i = 0; i < dwConnections; i++ ) // Loop through all current connections
	{
		RasHangUp( lpRasConn[i].hrasconn ); // Hang up the connection
		while( ( RasGetConnectStatus( lpRasConn[i].hrasconn,RasConStatus ) || ( loop > 10 ) ) )
		{
			// Loop until the connection handle is invalid, or 3 seconds have passed
			Sleep(300);
			loop++;
		}
	}

	// Free the memory if necessary.
	if ( lpRasConn != NULL )
	{
		HeapFree( GetProcessHeap(), 0, lpRasConn );
		lpRasConn = NULL;
	}
}

bool  CScheduler::ShutDownComputer()
{
	int ShutdownSuccess = 0;

	// Try 2000/XP way first
	ShutdownSuccess = InitiateSystemShutdownEx( NULL,_T("PeerProject Scheduled Shutdown\n\nA system shutdown was scheduled using PeerProject. The system will now shut down."), 30, Settings.Scheduler.ForceShutdown, FALSE, SHTDN_REASON_FLAG_USER_DEFINED );

	// Fall back to 9x way if this does not work
	if ( ! ShutdownSuccess && GetLastError() != ERROR_SHUTDOWN_IN_PROGRESS )
	{
		UINT ShutdownFlags = Settings.Scheduler.ForceShutdown ? ( EWX_POWEROFF | EWX_FORCE ) : ( EWX_POWEROFF | EWX_FORCEIFHUNG );
		DWORD dReason = ( SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED );
		ShutdownSuccess = ExitWindowsEx( ShutdownFlags, dReason );
	}
	return ( ShutdownSuccess != 0 );
}

// Give the process shutdown rights
bool  CScheduler::SetShutdownRights()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process.
	if ( ! OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
		return( FALSE );

	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // One privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 );

	if ( GetLastError() != ERROR_SUCCESS )
		return FALSE;

	return TRUE;
}

bool  CScheduler::IsScheduledTimePassed(CScheduleTask* pSchTask) const
{
	CTime tNow = CTime::GetCurrentTime();

	if ( tNow.GetHour() < pSchTask->m_tScheduleDateTime.GetHour() )
		return false;

	if ( tNow.GetHour() == pSchTask->m_tScheduleDateTime.GetHour() )
	{
		if ( tNow.GetMinute() < pSchTask->m_tScheduleDateTime.GetMinute() )
			return false;

		if ( tNow.GetMinute() == pSchTask->m_tScheduleDateTime.GetMinute() )
		{
			if ( tNow.GetSecond() < pSchTask->m_tScheduleDateTime.GetSecond() )
				return false;

			//if ( tNow.GetSecond() == pSchTask->m_tScheduleDateTime.GetSecond() )
			//	return true;
		}
	}

	return true;
}

int  CScheduler::ScheduleFromToday(CScheduleTask* pSchTask) const
{
	CTime tNow = CTime::GetCurrentTime();

	// Task should be executed: today 0, in the past -1, or later 1.

	if ( tNow.GetYear() > pSchTask->m_tScheduleDateTime.GetYear() )
		return -1;

	if ( tNow.GetYear() < pSchTask->m_tScheduleDateTime.GetYear() )
		return 1;

	if ( tNow.GetMonth() > pSchTask->m_tScheduleDateTime.GetMonth() )
		return  -1;

	if ( tNow.GetMonth() < pSchTask->m_tScheduleDateTime.GetMonth() )
		return 1;

	if ( tNow.GetDay() > pSchTask->m_tScheduleDateTime.GetDay() )
		return -1;

	if ( tNow.GetDay() < pSchTask->m_tScheduleDateTime.GetDay() )
		return 1;

	//if ( tNow.GetDay() == pSchTask->m_tScheduleDateTime.GetDay() )
		return 0;
}

//Calculates the different between current hour and shutdown hour
//Caller must first check to see if scheduler is enabled or not
LONGLONG CScheduler::GetHoursTo(unsigned int nTaskCombination)
{
	int nHoursToTasks = 0xFFFF;
	POSITION pos = GetIterator();
	CTime tNow = CTime::GetCurrentTime();

	CQuickLock oLock( m_pSection );
	while ( pos )
	{
		CScheduleTask *pSchTask = GetNext( pos );
		if ( pSchTask->m_bActive && ( pSchTask->m_nAction & nTaskCombination ) )
		{
			CTimeSpan tToTasks(1, 0, 0, 0);
			if ( pSchTask->m_bSpecificDays )
			{
				for ( int i = -1 ; i < 6 ; ++i )
				{
					if ( ( ( 1 << ( ( tNow.GetDayOfWeek() + i ) % 7 ) ) & pSchTask->m_nDays ) && ( i != -1 || ! pSchTask->m_bExecuted ) )
					{
						tToTasks = CTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), pSchTask->m_tScheduleDateTime.GetHour(), pSchTask->m_tScheduleDateTime.GetMinute(), pSchTask->m_tScheduleDateTime.GetSecond() ) + CTimeSpan( i + 1, 0, 0, 0 ) - tNow;
						break;
					};
				}
			}
			else
			{
				tToTasks = pSchTask->m_tScheduleDateTime - tNow;
			}

			if ( tToTasks.GetTotalHours() < nHoursToTasks )
				nHoursToTasks = tToTasks.GetTotalHours();
		}
	}

	return nHoursToTasks;
}

//////////////////////////////////////////////////////////////////////
// CScheduler XML

LPCTSTR CScheduler::xmlns = _T("http://schemas.peerproject.org/Scheduler.xsd");

CXMLElement* CScheduler::ToXML(BOOL bTasks)
{
	CXMLElement* pXML = new CXMLElement( NULL, _T("scheduler") );
	pXML->AddAttribute( _T("xmlns"), CScheduler::xmlns );

	if ( bTasks)
	{
		for ( POSITION pos = GetIterator() ; pos ; )
		{
			pXML->AddElement( GetNext( pos )->ToXML() );
		}
	}

	return pXML;
}

BOOL CScheduler::FromXML(CXMLElement* pXML)
{
	if ( ! pXML->IsNamed( _T("scheduler") ) )
		return FALSE;

	int nCount = 0;

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = pXML->GetNextElement( pos );

		if ( pElement->IsNamed( _T("task") ) )
		{
			CQuickLock oLock( m_pSection );
			CScheduleTask* pSchTask	= NULL;
			CString strGUID = pElement->GetAttributeValue( _T("guid") );
			BOOL bExisting = FALSE;
			GUID pGUID;

			if ( Hashes::fromGuid( strGUID, &pGUID ) )
			{
				if ( ( pSchTask = GetGUID( pGUID ) ) != NULL )
					bExisting = TRUE;

				if ( pSchTask == NULL )
				{
					pSchTask = new CScheduleTask( FALSE );
					pSchTask->m_pGUID = pGUID;
				}
			}
			else
			{
				pSchTask = new CScheduleTask();
			}

			if ( pSchTask->FromXML( pElement ) )
			{
				if ( ! bExisting )
					m_pScheduleTasks.AddTail( pSchTask );

				nCount++;
			}
			else	// Unsuccessful read from XML
			{
				if ( ! bExisting )
					delete pSchTask;
			}
		}
	}

	return nCount > 0;
}

//////////////////////////////////////////////////////////////////////
// CScheduler import

BOOL CScheduler::Import(LPCTSTR pszFile)
{
	CString strText;
	CBuffer pBuffer;
	CFile pFile;

	if ( ! pFile.Open( pszFile, CFile::modeRead ) ) return FALSE;
	pBuffer.EnsureBuffer( (DWORD)pFile.GetLength() );
	pBuffer.m_nLength = (DWORD)pFile.GetLength();
	pFile.Read( pBuffer.m_pBuffer, pBuffer.m_nLength );
	pFile.Close();

	BOOL bResult = FALSE;

	CXMLElement* pXML = CXMLElement::FromBytes( pBuffer.m_pBuffer, pBuffer.m_nLength, TRUE );
	if ( pXML )
	{
		bResult = FromXML( pXML );
		delete pXML;
	}

	return bResult;
}
