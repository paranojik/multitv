rem rebuild all VBLib_6_2 for VC6

rem add VC to path
path = %programfiles%\Microsoft Visual Studio\VC98\Bin;%programfiles%\Microsoft Visual Studio\Common\MSDev98\Bin;C:\Documents and Settings\sbvb.SBVB-LAP\My Documents\_work\MultiTV\sbVB\AddMeToPath;%path%


rem ///////////////////////////////////////////////////////////////////////
rem recompile VBLib dynamic release and debug
rem ///////////////////////////////////////////////////////////////////////
msdev "VBLib_dll\VBLib_dll.dsp" /make "VBLib_dll - release" "VBLib_dll - debug" /rebuild

rem specific for VBLib_dll
del /Q "VBLib_dll\debug\*.*"
rmdir "VBLib_dll\debug"
del /Q "VBLib_dll\release\*.*"
rmdir "VBLib_dll\release


rem ///////////////////////////////////////////////////////////////////////
rem recompile VBLib static release and debug
rem ///////////////////////////////////////////////////////////////////////
msdev "VBLib_static\VBLib_static.dsp" /make "VBLib_static - release" "VBLib_static - debug" /rebuild

rem specific for VBLib_static
del /Q "VBLib_static\debug\*.*"
rmdir "VBLib_static\debug"
del /Q "VBLib_static\release\*.*"
rmdir "VBLib_static\release


rem ///////////////////////////////////////////////////////////////////////
rem recompile VBLib test release and debug
rem ///////////////////////////////////////////////////////////////////////
msdev "VBLibTest\VBLibTest.dsp" /make "VBLibTest - release" "VBLibTest - debug" /rebuild

rem specific for VBLibTest
del /Q "VBLibTest\debug\*.*"
rmdir "VBLibTest\debug
del /Q "VBLibTest\release\*.*"
rmdir "VBLibTest\release


rem ///////////////////////////////////////////////////////////////////////
rem generaly needed
rem ///////////////////////////////////////////////////////////////////////
rem del useless files
del "..\..\bin\*.ilk"
del "..\..\bin\*.pdb"
del "..\..\lib\VC6_dll\*.exp"
rem copy dll's to path
copy "..\..\bin\*.*" "..\..\..\addMeToPath"

@echo off 
echo.
echo End of batch
pause
