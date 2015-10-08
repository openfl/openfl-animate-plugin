@echo off
SetLocal EnableDelayedExpansion
@echo on

if exist "OpenFLPlugin.zxp" del /f OpenFLPlugin.zxp

ZXPSignCmd -sign OpenFLPlugin OpenFLPlugin.zxp certificate.p12 "%1" -tsa https://timestamp.geotrust.com/tsa