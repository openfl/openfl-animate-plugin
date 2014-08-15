cd project
haxelib run hxcpp Build.xml -Dwindows -DHXCPP_M64
cd ..
xcopy OpenFLExtension %APPDATA%\Adobe\CEP\extensions\OpenFLExtension /s /i /y