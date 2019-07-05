echo off
if "%PATH%"=="%PATH:Visual=%" (
call "D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)
echo on
msbuild Nelson\Nelson.vcxproj -target:Clean -property:Configuration=PGO
set /p Build=<buildnumber.txt
set /a Build=%Build%+1
echo %Build%>buildnumber.txt
echo #pragma once>Nelson\version.h
echo #define BUILD_NUMBER %Build%>>Nelson\version.h
MSBuild.exe Nelson\Nelson.vcxproj -property:Configuration=PGO;WholeProgramOptimization=PGInstrument;LinkTimeCodeGeneration=PGInstrument;PostBuildEventUseInBuild=false
.\Nelson\x64\PGO\nemorino bench 10
.\Nelson\x64\PGO\nemorino bench4 10
MSBuild.exe Nelson\Nelson.vcxproj -property:Configuration=PGO;WholeProgramOptimization=PGOptimize;LinkTimeCodeGeneration=PGOptimization;PostBuildEventUseInBuild=false
copy /Y Nelson\x64\PGO\nemorino.exe \\DESKTOP1\Users\chrgu_000\Documents\EngineDev\Builds\
copy /Y Nelson\x64\PGO\nemorino.pdb \\DESKTOP1\Users\chrgu_000\Documents\EngineDev\Builds\