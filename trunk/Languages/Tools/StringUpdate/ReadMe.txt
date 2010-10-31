StringUpdate 1.0

Test resources for validity.  Extracts string table from .RC file and saves result as .XML file.

Usage:
	StringUpdate.exe input.h input.rc output.xml

Example for PeerProject resources:
	StringUpdate.exe ..\..\..\PeerProject\Resource.h "$(VCInstallDir)\atlmfc\include\afxres.h" ..\..\..\PeerProject\PeerProject.rc strings.xml
