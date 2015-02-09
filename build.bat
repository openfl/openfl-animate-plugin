@echo off
SetLocal EnableDelayedExpansion
@echo on

cd project

@echo off
if "%1" == "clean" (
	@echo on
	haxelib run hxcpp Build.xml clean -Dwindows -DHXCPP_M64
)
@echo on

haxelib run hxcpp Build.xml -Dwindows -Ddebug -DHXCPP_M64

@echo off
if %errorlevel% neq 0 (
	cd ..
	exit /b %errorlevel%
)
@echo on

cd ..
xcopy OpenFLPlugin %APPDATA%\Adobe\CEP\extensions\OpenFLPlugin /s /i /y

