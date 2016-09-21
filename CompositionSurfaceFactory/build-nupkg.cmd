@ECHO OFF
REM
REM Version is read from the VERSION file.
REM

SETLOCAL

PUSHD "%~dp0"

WHERE /Q nuget >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO nuget not found.
    ECHO.
    ECHO Run "%~pd0download-nuget.cmd" to download the latest version, or update PATH as appropriate.
    GOTO END
)

SET /p VERSION=<VERSION

SET NUGET_ARGS=^
    -nopackageanalysis ^
    -version %VERSION% ^

ECHO Time for some nugety goodness...
nuget pack CompositionSurfaceFactory.nuspec %NUGET_ARGS%
IF %ERRORLEVEL% NEQ 0 GOTO END

:END

POPD
EXIT /B %ERRORLEVEL%