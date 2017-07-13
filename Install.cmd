REM register MTZext.dll to display infotips when hovering mouse over mtz files
REM in the Windows Explorer.
REM Supply a valid file path of MTZext.dll as command line argument 


:: BatchGotAdmin
:-------------------------------------
REM  --> Check if we got admin privileges by running a particular command that will fail if we haven't
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    REM Create vbscript to run this cmd script again but with administrator privileges
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params = %1:"=""
    echo UAC.ShellExecute "%~s0", "%1", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
    pushd "%CD%"
    CD /D "%~dp0"
:--------------------------------------

@echo Enabling infotips for mtz files
@echo registering %1
regsvr32 %1

