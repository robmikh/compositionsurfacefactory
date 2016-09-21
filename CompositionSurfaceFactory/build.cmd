@echo off
SETLOCAL

IF "%VisualStudioVersion%" LSS "12.0" (
    GOTO WRONG_COMMAND_PROMPT
)

IF "%VisualStudioVersion%" LSS "14.0" (
    ECHO Warning: Visual Studio 2015 or higher required to build with Universal Windows Platform support.
    ECHO          Building without Universal Windows Platform support.
    ECHO.
    SET NOUAP=1
)

WHERE /Q msbuild >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO Error: It appears that 'msbuild' is not available in this environment. 
    ECHO.
    GOTO WRONG_COMMAND_PROMPT
)

ECHO Building x86...
msbuild "%~dp0CompositionSurfaceFactory.vcxproj" /v:m /maxcpucount /nr:false /property:Configuration=Release /property:Platform=Win32

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO Building ARM...
msbuild "%~dp0CompositionSurfaceFactory.vcxproj" /v:m /maxcpucount /nr:false /property:Configuration=Release /property:Platform=ARM

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO Building x64...
msbuild "%~dp0CompositionSurfaceFactory.vcxproj" /v:m /maxcpucount /nr:false /property:Configuration=Release /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO.

ECHO Starting nuget packaging...
CALL "%~dp0build-nupkg.cmd" %* local
GOTO END

:WRONG_COMMAND_PROMPT

ECHO Please run this script from the appropriate command prompt:
ECHO.
ECHO For Visual Studio 2015 and Universal Windows Platform:
ECHO - MSBuild Command Prompt for VS2015
ECHO.
PAUSE
GOTO END

:END