@echo off
rem
rem mruby/c compile script
rem

set MRBC=.\mrbc.exe


setlocal EnableDelayedExpansion
cd /d %~dp0

for %%r in (*.rb) do (
  echo %MRBC% -E -B %%~nr %%r
  %MRBC% -E -B %%~nr %%r
  if errorlevel 1 exit /b 1
)
