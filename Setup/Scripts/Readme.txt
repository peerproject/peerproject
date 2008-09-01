You can download Inno Setup 5 here:
http://files.jrsoftware.org/is/5/
You'll also need to install the PreProcessor:
http://files.jrsoftware.org/ispack/

Minimum version needed is 5.1.7 Beta.
Do not deploy IS installers of an old version ontop of newer versions!
Have the user do a clean install if you cannot avoid this scenario.

To compile a debug build without any files uncomment "#define debug" in main.iss.

Legacy compile procedure for PeerProject:
1)	Place PeerProject.exe in \Setup\Builds\
2)	Place SkinInstaller.exe in \Setup\Builds\
3)	Place PeerProject.pdb in \Setup\Builds\ only for the debug builds of PeerProject
4)	Compile repair.iss
5)	Compile main.iss

Standalone compile procedure for TorrentWizard:
1)	Place TorrentWizard.exe in \Setup\Builds\
2)	Compile torrent.iss 

The output file should be in \Setup\Builds\