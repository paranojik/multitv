rem rebuild all VBLib_7_0 for VC2005

rem add VC to path
path = %path%;%programfiles%"\Microsoft Visual Studio 8\Common7\IDE"

rem ///////////////////////////////////////////////////////////////////////
rem recompile VBLib dll release and debug
rem ///////////////////////////////////////////////////////////////////////
devenv "VBLib_dll\VBLib_dll.vcproj" /Rebuild release 
devenv "VBLib_dll\VBLib_dll.vcproj" /Rebuild debug

rem specific for VBLib_dll
del /Q "VBLib_dll\debug\*.*"
rmdir "VBLib_dll\debug"
del /Q "VBLib_dll\release\*.*"
rmdir "VBLib_dll\release

rem ///////////////////////////////////////////////////////////////////////
rem recompile VBLib static release and debug
rem ///////////////////////////////////////////////////////////////////////
devenv "VBLib_static\VBLib_static.vcproj" /Rebuild release 
devenv "VBLib_static\VBLib_static.vcproj" /Rebuild debug

rem specific for VBLib_static
del /Q "VBLib_static\debug\*.*"
rmdir "VBLib_static\debug"
del /Q "VBLib_static\release\*.*"
rmdir "VBLib_static\release

rem ///////////////////////////////////////////////////////////////////////
rem recompile VBLib test release
rem ///////////////////////////////////////////////////////////////////////
devenv "VBLib_test\VBLib_test.vcproj" /Rebuild release 
rem specific for VBLib_test
del /Q "VBLib_test\debug\*.*"
rmdir "VBLib_test\debug"
del /Q "VBLib_test\release\*.*"
rmdir "VBLib_test\release

rem ///////////////////////////////////////////////////////////////////////
rem generaly needed
rem ///////////////////////////////////////////////////////////////////////
rem del useless files
del "..\..\bin\*.ilk"
del "..\..\bin\*.pdb"
del "..\..\lib\VC2005_dll\*.exp"
rem copy dll's to path
copy "..\..\bin\*.*" "..\..\..\addMeToPath"

@echo off 
echo.
echo End of batch
pause

