@echo off
rem -------------------
rem Rebuild iptv_client
rem Author: Guilherme Lello
rem Operating System: Windows XP Professional
rem -------------------
rem Changing background and font color
rem -------------------
color 0A
rem -------------------
rem Setting environment variables
rem -------------------
set _clientSolutionPath="%iptv_shared%\shared" "%iptv_chat%\chat" "%iptv_irm_stub%\stub" "%iptv_media2%" "%iptv_kernel%\build\VC2005" "%iptv_client%\build\VC2005" "%skin_lite%\build\VC2005"
set _buildTool="%multitv_tools%\VS2005BuildTool"
rem -------------------
rem Welcome messages
rem -------------------
rem cls
@echo ===================
@echo Rebuild iptv_client
@echo ===================
rem -------------------
:MENU
rem -------------------
rem Menu
rem -------------------
@echo.
@echo Please choose the build configuration
@echo.
@echo 1 - Release
@echo 2 - Debug
@echo 3 - Release and debug
@echo 4 - Clean
@echo 5 - Exit
rem -------------------
rem User input
rem -------------------
@echo.
set /p _configuration="Enter your selection: "
set _configuration=%_configuration:~0,1%
rem -------------------
rem Input handling
rem -------------------
if "%_configuration%" == "1" goto BUILD
if "%_configuration%" == "2" goto BUILD
if "%_configuration%" == "3" goto BUILD
if "%_configuration%" == "4" goto BUILD
if "%_configuration%" == "5" goto END
goto INPUT_ERROR
rem -------------------
:INPUT_ERROR
rem -------------------
@echo.
@echo Invalid option.
goto MENU
rem -------------------
:BUILD
rem -------------------
@echo.
@CALL %_buildTool% %_configuration% %_clientSolutionPath%
rem -------------------
:END
rem -------------------
pause
