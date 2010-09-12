PeerProject Compiling Notes:
___________________________


0. This sourcecode package is self-contained.  Only Microsoft Visual Studio is supported.


1. Install Visual Studio 2008 + SP1
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=83c3a1ec-ed72-4a79-8961-25635db0192b  (90day Trial)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=fbee1648-7106-44a7-9649-6d9f6d58056e  (Service Pack)

   OR Install Visual Studio 2010
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=26bae65f-b0df-4081-ae6e-1d828993d4d0  (30/90day Trial)
	- Note Visual Studio 2005 files also available but likely outdated

2. Locate this complete PeerProject sourcecode set.
	- http://peerproject.svn.sourceforge.net/viewvc/peerproject/trunk.tar.gz
	- That's it, there are no external dependencies or setup.
	- Open \VS2008\PeerProject.sln and click "Build Solution"




X. Cost-Free alternative for Browse/Edit only:
	- Visual C++ Express can be configured, but some projects STILL FAIL to build.

   Install Visual C++ Express (2008) + Windows SDK + WDK
	- http://www.microsoft.com/express/downloads  ( Visual C++ 2008 Express, and/or 2010 )
	- http://www.microsoft.com/downloads/details.aspx?familyid=C17BA869-9671-4330-A63E-1FD44E0E2505  (Win7 SDK)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=36a2630f-5d56-43b5-b996-7633f2ec14ff  (WDK 7.1)

	- For Windows SDK web installer, "Documentation" and "Samples" are not needed: uncheck them.
	- For WDK (Windows Driver Kit) installer, only "Build Environments" is needed: leave others unchecked.
	- To add MFC in VC2008 go to:  Tools > Options > Projects and Solutions > VC++ Directories > (See dropdown at upper-right)
	--  In "Include Directories" add to top:  C:\<WDK>\inc\atl71  and  C:\<WDK>\inc\mfc42  and  C:\<Win7 SDK>\Include
	--  In "Library Directores"  add to end:  C:\<WDK>\lib\ATL\i386  and  C:\<WDK>\lib\MFC\i386  (Use actual paths)





PeerProject License Notes:
_________________________


0. This sourcecode package contains files under many licenses.  READ THEM.


1. Code is GPL 2/3, except where noted otherwise.
	- Any resulting build or reuse must be GPL 3+ compliant, including section 7 "Additional Terms."
	- To the extent reuse is a server affecting others, AGPL applies (Affero requires public posting of modified code.)
	- Other original work not covered by a prior license is Persistent Public Domain license (PPD).

2. Original visual resources are Creative Commons BY-NC-SA or PPD.
	- Many Images = Attribution, Non-Commercial, Share-Alike.

3. "PeerProject" is a protected trademark.
	- Does not prevent GPLv3/CC-compliant "mods" that both properly attribute and distinguish.

4. COMMERCIAL USE OF THIS SOURCEPACKAGE AS-IS IS NOT PERMITTED.
	- Some Non-GPL "aggregate" resources do not provide for normal GPL-compliant commercial usage.
	- Most Images and LibGFL.dll graphic library must first be removed! Etc.


PeerProject is copyright PeerProject Development Team. (peerproject.org)
Some portions previously copyright Shareaza Development Team, and others.

Any good-faith use is encouraged, no scams or misrepresentation will be tolerated.  Proud to be Open Source.
