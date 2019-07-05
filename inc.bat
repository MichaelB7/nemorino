cd /d %~dp0
set /p Build=<buildnumber.txt
set /a Build=%Build%+1
echo #pragma once>buildnumber.txt
echo #define BUILD_NUMBER %Build%>> buildnumber.txt
