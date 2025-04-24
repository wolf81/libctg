echo Building LuaJIT ...
cd deps/luajit/src

setlocal ENABLEDELAYEDEXPANSION

echo Building LuaJIT...

:: Clear the variable
set "VSPATH="

:: Use vswhere to find VS installation
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "VSPATH=%%i"
)

:: Suppress echo if the condition fails
if "!VSPATH!" == "" (
    @echo [ERROR] Visual Studio installation not found!
    exit /b 1
)

echo Found Visual Studio at: !VSPATH!

set "VSCMD=!VSPATH!\Common7\Tools\VsDevCmd.bat"
call "!VSCMD!" -arch=amd64 || exit /b 1
call msvcbuild.bat

endlocal
