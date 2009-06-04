# Microsoft Developer Studio Project File - Name="VBLib_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=VBLib_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VBLib_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VBLib_dll.mak" CFG="VBLib_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VBLib_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VBLib_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VBLib_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VBLIB_DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VBLIB_DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x416 /d "NDEBUG"
# ADD RSC /l 0x416 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../bin/VBLib_6_2_VC6.dll" /implib:"../../../lib/VC6_dll/VBLib_6_2.lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Documents and Settings\Administrator\My Documents\_work\sbvb\VBLib_6_2\bin\VBLib_6_2_VC6.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "$(TargetPath)" "$(SBVB_HOME)\addMeToPath"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VBLib_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VBLIB_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VBLIB_DLL_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x416 /d "_DEBUG"
# ADD RSC /l 0x416 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"Debug/VBLib_6_2.pdb" /debug /machine:I386 /out:"../../../bin/VBLib_6_2_VC6_d.dll" /implib:"../../../lib/VC6_dll/VBLib_6_2_d.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Documents and Settings\Administrator\My Documents\_work\sbvb\VBLib_6_2\bin\VBLib_6_2_VC6_d.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "$(TargetPath)" "$(SBVB_HOME)\addMeToPath"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "VBLib_dll - Win32 Release"
# Name "VBLib_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBBinaryData_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBClock_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBCrossDLL_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBDate_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBException_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBFileDirectory_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBLib.def
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBLib_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBMD5_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBProcessArgs_dll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib_dll\VBString_dll.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBBinaryData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBClock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBCrossDLL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBDate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBFileDirectory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBGenMath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib_buildid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib_raw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBMD5.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBMemCheck.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBProcessArgs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBShowSTLContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBString.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\vblib_posBuild.bat
# End Source File
# End Group
# Begin Source File

SOURCE=.\deaft.txt
# End Source File
# Begin Source File

SOURCE=.\memory.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
