//
// Firewall.h
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

// CFirewall wraps Windows COM components to change Windows Firewall settings, and talk UPnP to a NAT router
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CFirewall

// Include headers from the Windows XP SP2 version of Platform SDK or later

#ifndef FIREWALL_H_INCLUDED
#define FIREWALL_H_INCLUDED

#include <upnp.h>

// Control the Windows Firewall, and talk UPnP to the NAT router to setup port forwarding
class CFirewall
{
public:

	// Windows Firewall COM interfaces accessed with the object
    CComPtr< INetFwMgr >					Manager;
    CComPtr< INetFwPolicy >					Policy;
	CComPtr< INetFwProfile >				Profile;
	CComPtr< INetFwServices >				ServiceList;
	CComPtr< INetFwAuthorizedApplications >	ProgramList;
    CComPtr< INetFwOpenPorts >				PortList;

	// Windows Firewall COM interfaces accessed in methods
	CComPtr< INetFwService >				Service;
	CComPtr< INetFwAuthorizedApplication >	Program;
    CComPtr< INetFwOpenPort >				Port;

	// UPnP COM interfaces
	CComPtr< IUPnPNAT >						Nat;
	CComPtr< IStaticPortMappingCollection >	Collection;
	CComPtr< IStaticPortMapping >			Mapping;
	BOOL									m_bInitialized;

	// Constructor and destructor
	CFirewall();
	~CFirewall();

	// Examples controlling Windows Firewall
	//
	//	// Let a program listen on a socket without Windows Firewall poping up a warning
	//	CFirewall firewall;
	//	firewall.SetupProgram( "C:\\Program Files\\Outlook Express\\msimn.exe", "Outlook Express" );
	//
	//	// Enable the UPnP service in Windows Firewall so we can talk UPnP through it
	//	firewall.SetupService( NET_FW_SERVICE_UPNP );

	// Windows Firewall Methods
	BOOL SetupService( NET_FW_SERVICE_TYPE service );                    // Check a box for a service on the Windows Firewall exceptions list
	BOOL SetupProgram( const CString& path, const CString& name, BOOL bRemove = FALSE);// List a program and check its box
	BOOL AccessWindowsFirewall();                                        // Access the Windows Firewall COM objects, call before calling the methods below
	BOOL IsProgramListed( const CString& path, BOOL* listed );           // Determine if a program is on the exceptions list
	BOOL IsServiceEnabled( NET_FW_SERVICE_TYPE service, BOOL* enabled ); // Determine if a service is checked
	BOOL IsProgramEnabled( const CString& path, BOOL* enabled );         // Determine if a listed program is checked
	BOOL AreExceptionsAllowed();										 // Find out if the system is in no-exceptions mode
	BOOL AddProgram( const CString& path, const CString& name );         // Add a program to the list with a checked box
	BOOL RemoveProgram( const CString& path );							 // Add a program to the list with a checked box
	BOOL EnableService( NET_FW_SERVICE_TYPE service );                   // Check the box for a service
	BOOL EnableProgram( const CString& path );                           // Check the box for a program
};

#endif //#ifndef FIREWALL_H_INCLUDED
