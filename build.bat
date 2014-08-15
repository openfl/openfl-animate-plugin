@echo on

cd project
haxelib run hxcpp Build.xml -Dwindows -DHXCPP_M64

@echo off
if %errorlevel% neq 0 (
	cd ..
	exit /b %errorlevel%
} else ()
@echo on

cd ..
xcopy OpenFLExtension %APPDATA%\Adobe\CEP\extensions\OpenFLExtension /s /i /y