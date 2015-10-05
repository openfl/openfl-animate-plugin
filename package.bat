@echo off
SetLocal EnableDelayedExpansion
@echo on

if not exist ".staged-extension" mkdir .staged-extension
if exist ".staged-extension\OpenFLPlugin" rmdir /s /q .staged-extension\OpenFLPlugin

xcopy OpenFLPlugin .staged-extension\OpenFLPlugin /s /i /y

copy /y icon.png .staged-extension\icon.png
copy /y org.openfl.OpenFLPlugin.mxi .staged-extension\org.openfl.OpenFLPlugin.mxi

if exist "org.openfl.OpenFLPlugin.zxp" del /f org.openfl.OpenFLPlugin.zxp

ZXPSignCmd -sign .staged-extension org.openfl.OpenFLPlugin.zxp certificate.p12 "%1"