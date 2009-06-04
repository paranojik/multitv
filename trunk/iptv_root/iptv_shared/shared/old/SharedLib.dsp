# Microsoft Developer Studio Project File - Name="SharedLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SharedLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SharedLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SharedLib.mak" CFG="SharedLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SharedLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SharedLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SharedLib - Win32 Release"

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
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/Release/sharedlib.lib"

!ELSEIF  "$(CFG)" == "SharedLib - Win32 Debug"

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
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MM_" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/Debug/sharedlib.lib"

!ENDIF 

# Begin Target

# Name "SharedLib - Win32 Release"
# Name "SharedLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=clbuffer.cpp
# End Source File
# Begin Source File

SOURCE=clsbuffer.cpp
# End Source File
# Begin Source File

SOURCE=cmd_base.cpp
# End Source File
# Begin Source File

SOURCE=common.cpp
# End Source File
# Begin Source File

SOURCE=compat.cpp
# End Source File
# Begin Source File

SOURCE=.\date.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\expr.cpp
# End Source File
# Begin Source File

SOURCE=..\socket\fd_set.cpp
# End Source File
# Begin Source File

SOURCE=.\file_ct.cpp
# End Source File
# Begin Source File

SOURCE=.\file_ot.cpp
# End Source File
# Begin Source File

SOURCE=.\file_x.cpp
# End Source File
# Begin Source File

SOURCE=.\hash_cmd.cpp
# End Source File
# Begin Source File

SOURCE=jitterbuffer.cpp
# End Source File
# Begin Source File

SOURCE=locked_inc.cpp
# End Source File
# Begin Source File

SOURCE=mem_x.cpp
# End Source File
# Begin Source File

SOURCE=memmgr.cpp
# End Source File
# Begin Source File

SOURCE="new-del.cpp"
# End Source File
# Begin Source File

SOURCE=..\socket\proxy.cpp
# End Source File
# Begin Source File

SOURCE=semaphore.cpp
# End Source File
# Begin Source File

SOURCE=..\socket\sockbuffs.cpp
# End Source File
# Begin Source File

SOURCE=..\socket\SocketCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\socket\sockmwbuffs.cpp
# End Source File
# Begin Source File

SOURCE=str_ap.cpp
# End Source File
# Begin Source File

SOURCE=txtbufvect.cpp
# End Source File
# Begin Source File

SOURCE=winthread.cpp
# End Source File
# Begin Source File

SOURCE=XJitterCtrl.cpp
# End Source File
# Begin Source File

SOURCE=XStr.cpp
# End Source File
# Begin Source File

SOURCE=.\xversion.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BufVectorT.h
# End Source File
# Begin Source File

SOURCE=clbuffer.h
# End Source File
# Begin Source File

SOURCE=clsbuffer.h
# End Source File
# Begin Source File

SOURCE=cmd_base.h
# End Source File
# Begin Source File

SOURCE=common.h
# End Source File
# Begin Source File

SOURCE=compat.h
# End Source File
# Begin Source File

SOURCE=.\date.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=def_ap.h
# End Source File
# Begin Source File

SOURCE=Expr.h
# End Source File
# Begin Source File

SOURCE=..\socket\fd_set.h
# End Source File
# Begin Source File

SOURCE=File_ct.h
# End Source File
# Begin Source File

SOURCE=File_ot.h
# End Source File
# Begin Source File

SOURCE=File_x.h
# End Source File
# Begin Source File

SOURCE=Hash_cmd.h
# End Source File
# Begin Source File

SOURCE=HashT.h
# End Source File
# Begin Source File

SOURCE=..\socket\isockbuff.h
# End Source File
# Begin Source File

SOURCE=Ithread.h
# End Source File
# Begin Source File

SOURCE=jitterbuffer.h
# End Source File
# Begin Source File

SOURCE=lingua.h
# End Source File
# Begin Source File

SOURCE=ListT.h
# End Source File
# Begin Source File

SOURCE=locked_inc.h
# End Source File
# Begin Source File

SOURCE=mem_x.h
# End Source File
# Begin Source File

SOURCE=memmgr.h
# End Source File
# Begin Source File

SOURCE=..\socket\proxy.h
# End Source File
# Begin Source File

SOURCE=QueueT.h
# End Source File
# Begin Source File

SOURCE=semaphore.h
# End Source File
# Begin Source File

SOURCE=..\socket\sockbuffs.h
# End Source File
# Begin Source File

SOURCE=..\socket\SocketCtrl.h
# End Source File
# Begin Source File

SOURCE=..\socket\sockmwbuffs.h
# End Source File
# Begin Source File

SOURCE=StackT.h
# End Source File
# Begin Source File

SOURCE=str_ap.h
# End Source File
# Begin Source File

SOURCE=SVectorT.h
# End Source File
# Begin Source File

SOURCE=txtbufvect.h
# End Source File
# Begin Source File

SOURCE=VectorT.h
# End Source File
# Begin Source File

SOURCE=XException.h
# End Source File
# Begin Source File

SOURCE=XJitterCtrl.h
# End Source File
# Begin Source File

SOURCE=XObjFactory.h
# End Source File
# Begin Source File

SOURCE=XResult.h
# End Source File
# Begin Source File

SOURCE=XStr.h
# End Source File
# Begin Source File

SOURCE=.\xversion.h
# End Source File
# End Group
# End Target
# End Project
