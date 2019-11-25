@echo off
pushd %~dp0
set PATH=C:\Program Files\DynamixelSDK\lib;%PATH%
if not defined RTM_VC_CONFIG set RTM_VC_CONFIG=Release
start "%~n0 %*" "build\src\%RTM_VC_CONFIG%\%~n0Comp.exe" %*
popd
