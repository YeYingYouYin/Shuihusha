@echo off
set str=%1
if exist "F:\QT\qt\bin\rcc.exe" goto branch
E:\QT\Desktop\Qt\4.7.4\mingw\bin\rcc.exe -binary %1 -o %str:~0,-4%.rcc
:branch
F:\QT\qt\bin\rcc.exe -binary %1 -o %str:~0,-4%.rcc