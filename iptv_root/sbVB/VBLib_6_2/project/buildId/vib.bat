@echo off
VBBuildId.exe -dtStr:changeStrBuild_str_DateTime  "%sbvb_home%\template\buildId_template.h" ..\..\include\VBLib_buildid.h VBLib_build.txt changeStrBuildId changeStrBuildDateTime changeStrUserVersion VBLib_version.txt

