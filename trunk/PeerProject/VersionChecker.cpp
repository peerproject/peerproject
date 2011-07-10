//
// VersionChecker.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "VersionChecker.h"
#include "Library.h"
#include "SharedFile.h"
#include "Transfer.h"
#include "Network.h"
#include "GProfile.h"
#include "DiscoveryServices.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CVersionChecker VersionChecker;

#define VERSIONCHECKER_FREQUENCY	7


//////////////////////////////////////////////////////////////////////
// CVersionChecker construction

CVersionChecker::CVersionChecker() :
	m_bVerbose( false )
{
}

CVersionChecker::~CVersionChecker()
{
	Stop();
}

void CVersionChecker::ForceCheck()
{
	m_bVerbose = true;
	Settings.VersionCheck.UpdateCheck = true;
	Settings.VersionCheck.NextCheck = 0;
	Start();
}

void CVersionChecker::ClearVersionCheck()
{
	Settings.VersionCheck.UpgradePrompt.Empty();
	Settings.VersionCheck.UpgradeFile.Empty();
	Settings.VersionCheck.UpgradeSHA1.Empty();
	Settings.VersionCheck.UpgradeTiger.Empty();
	Settings.VersionCheck.UpgradeSize.Empty();
	Settings.VersionCheck.UpgradeSources.Empty();
	Settings.VersionCheck.UpgradeVersion.Empty();
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker time check

BOOL CVersionChecker::NeedToCheck()
{
	if ( theApp.m_sVersion.Compare( Settings.VersionCheck.UpgradeVersion ) >= 0 ) // user manually upgraded
		ClearVersionCheck();

	if ( ! Settings.VersionCheck.UpdateCheck ) return FALSE;
	if ( ! Settings.VersionCheck.NextCheck ) return TRUE;
	if ( Settings.VersionCheck.NextCheck == 0xFFFFFFFF ) return FALSE;

	return (DWORD)CTime::GetCurrentTime().GetTime() >= Settings.VersionCheck.NextCheck;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker start

BOOL CVersionChecker::Start()
{
	if ( IsThreadAlive() )
		return TRUE;

	m_pRequest.Clear();

	return BeginThread( "VersionChecker" );
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker stop

void CVersionChecker::Stop()
{
	if ( IsThreadAlive() )
	{
		m_pRequest.Cancel();
		CloseThread();
	}
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker thread run

void CVersionChecker::OnRun()
{
	if ( NeedToCheck() )
	{
		if ( ExecuteRequest() )
			ProcessResponse();
		else
			SetNextCheck( VERSIONCHECKER_FREQUENCY );

		if ( IsThreadEnabled() )
			PostMainWndMessage( WM_VERSIONCHECK, VC_MESSAGE_AND_CONFIRM );
	}
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker undertake request

BOOL CVersionChecker::ExecuteRequest()
{
	const CString strUpdateURL( UPDATE_URL );

	m_pRequest.SetURL( strUpdateURL + _T("?Version=") + theApp.m_sVersion
#ifdef WIN64
		+ _T("&Platform=x64")
#else
		+ _T("&Platform=Win32")
#endif	//WIN64
		+ _T("&Language=") + Settings.General.Language.Left(2)
	);

	if ( ! m_pRequest.Execute( FALSE ) ) return FALSE;

	int nStatusCode = m_pRequest.GetStatusCode();
	if ( nStatusCode < 200 || nStatusCode > 299 ) return FALSE;

	CString strResponse = m_pRequest.GetResponseString();
	for ( strResponse += '&' ; strResponse.GetLength() ; )
	{
		CString strItem	= strResponse.SpanExcluding( _T("&") );
		strResponse		= strResponse.Mid( strItem.GetLength() + 1 );

		CString strKey = strItem.SpanExcluding( _T("=") );
		if ( strKey.GetLength() == strItem.GetLength() ) continue;

		strItem = URLDecode( strItem.Mid( strKey.GetLength() + 1 ) );
		strItem.Trim();

		m_pResponse.SetAt( strKey, strItem );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker process response

void CVersionChecker::ProcessResponse()
{
	int nDays = VERSIONCHECKER_FREQUENCY;
	CString strValue;

	if ( m_pResponse.Lookup( _T("Message"), strValue ) ||
		 m_pResponse.Lookup( _T("MessageBox"), strValue ) )
	{
		m_sMessage = strValue;
	}

	if ( m_pResponse.Lookup( _T("Quote"), strValue ) )
	{
		Settings.VersionCheck.Quote = strValue;
	}

	if ( m_pResponse.Lookup( _T("SystemMsg"), strValue ) )
	{
		for ( strValue += '\n' ; strValue.GetLength() ; )
		{
			CString strLine	= strValue.SpanExcluding( _T("\r\n") );
			strValue		= strValue.Mid( strLine.GetLength() + 1 );
			if ( ! strLine.IsEmpty() ) theApp.Message( MSG_NOTICE, strLine );
		}
	}

	if ( m_pResponse.Lookup( _T("UpgradePrompt"), strValue ) )
	{
		Settings.VersionCheck.UpgradePrompt = strValue;

		m_pResponse.Lookup( _T("UpgradeFile"), Settings.VersionCheck.UpgradeFile );
		m_pResponse.Lookup( _T("UpgradeSHA1"), Settings.VersionCheck.UpgradeSHA1 );
		m_pResponse.Lookup( _T("UpgradeTiger"), Settings.VersionCheck.UpgradeTiger );
		m_pResponse.Lookup( _T("UpgradeSize"), Settings.VersionCheck.UpgradeSize );
		m_pResponse.Lookup( _T("UpgradeSources"), Settings.VersionCheck.UpgradeSources );
		m_pResponse.Lookup( _T("UpgradeVersion"), Settings.VersionCheck.UpgradeVersion );

		// Old name
		if ( Settings.VersionCheck.UpgradeSHA1.IsEmpty() )
			m_pResponse.Lookup( _T("UpgradeHash"), Settings.VersionCheck.UpgradeSHA1 );
	}
	else
	{
		ClearVersionCheck();
	}

	if ( m_pResponse.Lookup( _T("AddDiscovery"), strValue ) )
	{
		strValue.Trim();
		DiscoveryServices.Add( strValue, CDiscoveryService::dsWebCache );
	}

	if ( m_pResponse.Lookup( _T("AddDiscoveryUHC"), strValue ) )
	{
		strValue.Trim();
		DiscoveryServices.Add( strValue, CDiscoveryService::dsGnutella, PROTOCOL_G1 );
	}

	if ( m_pResponse.Lookup( _T("AddDiscoveryKHL"), strValue ) )
	{
		strValue.Trim();
		DiscoveryServices.Add( strValue, CDiscoveryService::dsGnutella, PROTOCOL_G2 );
	}

	if ( m_pResponse.Lookup( _T("NextCheck"), strValue ) )
	{
		_stscanf( strValue, _T("%lu"), &nDays );
	}

	SetNextCheck( nDays );

	m_pResponse.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker NextCheck update

void CVersionChecker::SetNextCheck(int nDays)
{
	CTimeSpan tPeriod( nDays, 0, 0, 0 );
	CTime tNextCheck = CTime::GetCurrentTime() + tPeriod;
	Settings.VersionCheck.NextCheck = (DWORD)tNextCheck.GetTime();
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker check if a download is an upgrade

BOOL CVersionChecker::CheckUpgradeHash(const CLibraryFile* pFile)
{
	if ( ! IsUpgradeAvailable() )
		return FALSE;

	Hashes::Sha1Hash oSHA1;
	if ( oSHA1.fromString( Settings.VersionCheck.UpgradeSHA1 ) )
	{
		CQuickLock oLock( Library.m_pSection );
		if ( ! pFile )
			pFile = LibraryMaps.LookupFileBySHA1( oSHA1 );

		if ( pFile && validAndEqual( pFile->m_oSHA1, oSHA1 ) &&
			_tcsicmp( PathFindExtension( pFile->GetPath() ), _T(".exe") ) )
		{
			m_sUpgradePath = pFile->GetPath();
			PostMainWndMessage( WM_VERSIONCHECK, VC_UPGRADE );
			return TRUE;
		}
	}

	return FALSE;
}
