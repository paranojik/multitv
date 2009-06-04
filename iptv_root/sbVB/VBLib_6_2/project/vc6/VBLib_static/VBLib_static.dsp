# Microsoft Developer Studio Project File - Name="VBLib_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=VBLib_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VBLib_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VBLib_static.mak" CFG="VBLib_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VBLib_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "VBLib_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VBLib_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VBLIB_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x416 /d "NDEBUG"
# ADD RSC /l 0x416 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib/VC6_static/VBLib_static.lib"

!ELSEIF  "$(CFG)" == "VBLib_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VBLIB_STATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x416 /d "_DEBUG"
# ADD RSC /l 0x416 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib/VC6_static/VBLib_static_d.lib"

!ENDIF 

# Begin Target

# Name "VBLib_static - Win32 Release"
# Name "VBLib_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBBinaryData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBClock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBCrossDLL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBDate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBFileDirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBLib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBMD5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBProcessArgs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\VBLib\VBString.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBBinaryData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBClock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBCrossDLL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBDate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBFileDirectory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBGenMath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBLib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBLib_buildid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBLib_raw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBMD5.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBMemCheck.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBProcessArgs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBSerial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBShowSTLContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\VBLib\VBString.h
# End Source File
# End Group
# End Target
# End Project
