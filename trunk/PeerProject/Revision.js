//
// Revision.js
//
// This file is part of PeerProject (peerproject.org) © 2009
// Portions Copyright Shareaza Development Team, 2009.
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

// This script compares current directory revision and prior revision
// saved in Revision.h file using .SVN hidden folder data (for About dialog).


var fso = WScript.CreateObject( "Scripting.FileSystemObject" );
var fsvn = fso.GetAbsolutePathName( ".svn\\all-wcprops" );
var fname = fso.GetAbsolutePathName( "Revision.h" );

var revision;
try
{
	var svnfile = fso.OpenTextFile( fsvn, 1, false );
	revision = svnfile.Read( 68 ).substr( 66 );		// Parsing:  "!svn/ver/XXX/"
	WScript.Echo( "Current SVN Revision is " + revision );
	svnfile.Close();
}
catch(e)
{
	WScript.Echo( "SVN Revision Data Missing." );
	WScript.Quit( 0 );
}

var modified;
try
{
	var tsr = fso.OpenTextFile( fname, 1, false );
	modified = tsr.ReadLine().substr( 4 );			// Parsing:  "// rXXX"
	tsr.Close();
}
catch(e)
{
	WScript.Echo( "Revision.h does not exist." );
}

if ( revision != modified )
{
	WScript.Echo( "Updating Revision from \"" + modified + "\" to \"" + revision + "\"...");
	try
	{
		var tsw = fso.OpenTextFile( fname, 2, true );
		tsw.WriteLine( "// r" + revision );
		tsw.WriteLine( "// Auto-generated" );
		tsw.WriteLine( "" );
		tsw.WriteLine( "#pragma once" );
		tsw.WriteLine( "" );
		tsw.WriteLine( "#define __REVISION__ \t\"" + revision + "\"" );
	//	tsw.WriteLine( "#define __MODAUTHOR__\t\"YOUR NAME HERE\"" );
		tsw.Close();
	}
	catch(e)
	{
		WScript.Echo( "Revision Update Failed: \"" + fname + "\"" );
		WScript.Quit( 1 );
	}
}
else
	WScript.Echo( "Revision is up to date." );

WScript.Quit( 0 );
