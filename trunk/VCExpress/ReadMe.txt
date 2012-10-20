PeerProject Compiling Notes:
___________________________


0. This sourcecode package is self-contained, no setup required.  Only Microsoft Visual Studio is supported.


1. Install Visual Studio 2010
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=26bae65f-b0df-4081-ae6e-1d828993d4d0  (30/90day Trial)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=75568aa6-8107-475d-948a-ef22627e57a5  (Service Pack)

   OR Install Visual Studio 2008 SP1
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=83c3a1ec-ed72-4a79-8961-25635db0192b  (90day Trial)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=fbee1648-7106-44a7-9649-6d9f6d58056e  (Service Pack)
	- http://www.microsoft.com/downloads/details.aspx?familyid=C17BA869-9671-4330-A63E-1FD44E0E2505  (Win7 SDK)

   OR Install Visual Studio 2012
	- http://www.microsoft.com/visualstudio/11/en-us/downloads  (90day Trial)
	- Note: No Windows XP support

2. Locate this complete PeerProject sourcecode set.
	- http://peerproject.svn.sourceforge.net/viewvc/peerproject/trunk.tar.gz
	- OR http://github.com/peerproject/peerproject/zipball/master
	- That's it, there are no external dependencies or setup.
	- Open \VS20XX\PeerProject.sln and click "Build Solution"




Note. Alternative for Browse/Edit only:
	- Free Visual C++ Express can be configured, but some projects STILL FAIL to build.
	- Note legacy Visual Studio 2005 files also still available but outdated.

X. Install Visual C++ Express + Windows SDK + WDK
	- http://www.microsoft.com/express/downloads  (Visual C++ 2010 Express, and/or 2008)
	- http://www.microsoft.com/downloads/details.aspx?familyid=C17BA869-9671-4330-A63E-1FD44E0E2505  (Win7 SDK)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=36a2630f-5d56-43b5-b996-7633f2ec14ff  (WDK 7.1)
	- http://download.microsoft.com/download/9/0/f/90f019ac-8243-48d3-91cf-81fc4093ecfd/1830_usa_ddk.iso  (DDK)

 	- See http://www.codeproject.com/KB/MFC/MFCinVisualStudioExpress.aspx
	- For Windows SDK web installer, "Documentation" and "Samples" are not needed: uncheck them.
	- For WDK (Windows Driver Kit) installer, only "Build Environments" is needed: leave others unchecked.
	- To add MFC in VC2008 go to:  Tools > Options > Projects and Solutions > VC++ Directories > (See dropdown at upper-right)
	--  In "Include Directories" add to top:  c:\<WDK>\inc\atl71  and  c:\<WDK>\inc\mfc42  and  c:\<Win7 SDK>\Include
	--  In "Library Directores"  add to end:  c:\<WDK>\lib\ATL\i386  and  c:\<WDK>\lib\MFC\i386  (Use actual paths)
	- Uncomment "//#define VCEXPRESS" early in file StdAfx.h	-Note MFC file afxocc.h is still missing
