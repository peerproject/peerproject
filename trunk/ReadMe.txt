PeerProject Compiling Notes:
___________________________


0. This sourcecode package is self-contained, no setup required.  Only Microsoft Visual Studio is supported.


1. Install Visual Studio 2012
	- http://www.microsoft.com/visualstudio/11/en-us/downloads  (90day Trial)
	- http://www.microsoft.com/en-us/download/details.aspx?id=38188  (Update 2)

   OR Install Visual Studio 2010
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=26bae65f-b0df-4081-ae6e-1d828993d4d0  (30/90day Trial)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=75568aa6-8107-475d-948a-ef22627e57a5  (Service Pack)

   OR Install Visual Studio 2008 SP1
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=83c3a1ec-ed72-4a79-8961-25635db0192b  (90day Trial)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=fbee1648-7106-44a7-9649-6d9f6d58056e  (Service Pack)
	- http://www.microsoft.com/downloads/details.aspx?familyid=C17BA869-9671-4330-A63E-1FD44E0E2505  (Win7 SDK)

2. Locate this complete PeerProject sourcecode set.
	- http://peerproject.svn.sourceforge.net/viewvc/peerproject/trunk.tar.gz
	- OR http://github.com/peerproject/peerproject/zipball/master
	- That's it, there are no external dependencies or setup.
	- Open \VS20XX\PeerProject.sln and click "Build Solution"




Note: Visual Studio Express alternative for Browse/Edit only:
	- No-Cost Visual C++ Express (Desktop) can be configured for MFC, but some projects STILL FAIL to build.

X. Install Visual C++ Express + Windows SDK + WDK
	- http://www.microsoft.com/visualstudio/eng/downloads#d-express-windows-desktop  (Visual Studio Express 2012 for Desktop)
	- OR http://www.microsoft.com/visualstudio/eng/downloads#d-2010-express  (Visual C++ 2010 Express)
	- http://www.microsoft.com/en-us/download/details.aspx?id=8279  (Win7 SDK)
	- http://www.microsoft.com/downloads/details.aspx?FamilyID=36a2630f-5d56-43b5-b996-7633f2ec14ff  (WDK 7.1)
	- Configure MFC from WDK mfc42 folder ("Build Environments" only install)






PeerProject License Notes:
_________________________


0. This sourcecode package contains files under many licenses.  READ THEM.


1. Code is GPL/AGPLv3, except where noted otherwise.
	- Any resulting build or reuse must be AGPL 3+ compliant, including section 7 "Additional Terms."
	- As this is network server software affecting others, Affero REQUIRES PUBLIC AVAILABILITY OF MODIFIED CODE.
	- Other original work not covered by a prior license is available as Persistent Public Domain license (PPD).

2. Original visual resources are Creative Commons BY-NC-SA or PPD.
	- Many Images = Attribution, Non-Commercial, Share-Alike.

3. "PeerProject" is a protected trademark.
	- Does not prevent cost-free AGPLv3/CC-compliant "mods" that both properly attribute and distinguish.

4. COMMERCIAL USE OF THIS SOURCEPACKAGE AS-IS IS NOT PERMITTED.
	- Some Non-GPL "aggregate" resources do not provide for normal GPL-compliant commercial usage.
	- Most Images and LibGFL.dll graphic library must first be removed! Etc.


PeerProject is copyright PeerProject Development Team. (peerproject.org)
Some portions previously copyright Shareaza Development Team, and others.

All good-faith use is encouraged, no scams or misrepresentation will be tolerated.  Proud to be Open Source.