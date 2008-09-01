cd ..
cd ..
cd ..
"%ProgramFiles%\Microsoft SDKs\Windows\v6.0A\bin\tlbimp.exe" "..\..\PeerProject\Win32 %1\PeerProject.tlb" /out:PeerProject.Interop.dll /namespace:PeerProject /asmversion:1.0.0.0 /keyfile:PeerProject.snk /nologo /primary /sysarray /machine:x86 /transform:DispRet