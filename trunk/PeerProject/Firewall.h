//
// Firewall.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

// CFirewall wraps Windows COM components to change Windows Firewall settings, and talk UPnP to a NAT router
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CFirewall
//
// Include headers from the Windows SDK

#pragma once

#include <upnp.h>


// Control the Windows Firewall, and talk UPnP to the NAT router to setup port forwarding
class CFirewall
{
public:
	CFirewall();
	~CFirewall();

public:

	// Windows Firewall COM interfaces accessed with the object
	CComPtr< INetFwMgr >					FwManager;
	CComPtr< INetFwPolicy >					Policy;
	CComPtr< INetFwProfile >				Profile;
	CComPtr< INetFwServices >				ServiceList;
	CComPtr< INetFwAuthorizedApplications >	ProgramList;
	CComPtr< INetFwOpenPorts >				PortList;

	// Windows Firewall COM interfaces accessed in methods
	CComPtr< INetFwService >				Service;
	CComPtr< INetFwAuthorizedApplication >	Program;
	CComPtr< INetFwOpenPort >				Port;

	// UPnP COM interfaces (Obsolete)
	//CComPtr< IUPnPNAT >					Nat;
	//CComPtr< IStaticPortMappingCollection > Collection;
	//CComPtr< IStaticPortMapping >			Mapping;

	// Examples controlling Windows Firewall
	//
	//	// Let a program listen on a socket without Windows Firewall poping up a warning
	//	CFirewall firewall;
	//	firewall.SetupProgram( "C:\\Program Files\\Outlook Express\\msimn.exe", "Outlook Express" );
	//
	//	// Enable the UPnP service in Windows Firewall so we can talk UPnP through it
	//	firewall.SetupService( NET_FW_SERVICE_UPNP );

	// Windows Firewall Methods
	BOOL Init();															// Initialization, access the Windows Firewall COM objects
	BOOL AddProgram( const CString& path, const CString& name );			// Add a program to the list with a checked box
	BOOL RemoveProgram( const CString& path );								// Add a program to the list with a checked box
	BOOL SetupService( NET_FW_SERVICE_TYPE service );						// Check a box for a service on the Windows Firewall exceptions list
	BOOL SetupProgram( const CString& path, const CString& name, BOOL bRemove = FALSE);	// List a program and check its box
	BOOL EnableService( NET_FW_SERVICE_TYPE service );						// Check the box for a service
	BOOL EnableProgram( const CString& path );								// Check the box for a program
	BOOL IsServiceEnabled( NET_FW_SERVICE_TYPE service, BOOL* enabled );	// Determine if a service is checked
	BOOL IsProgramEnabled( const CString& path, BOOL* enabled );			// Determine if a listed program is checked
	BOOL IsProgramListed( const CString& path, BOOL* listed );				// Determine if a program is on the exceptions list
	BOOL AreExceptionsAllowed() const;										// Find out if the system is in no-exceptions mode
};
