#!/bin/bash
set -e

cd project

if [ "$1" == "clean" ]; then
	haxelib run hxcpp Build.xml clean -Dmac -DHXCPP_M64
fi

haxelib run hxcpp Build.xml -Dmac -DHXCPP_M64

cd ..
mkdir -p ~/Library/Application\ Support/Adobe/CEP/extensions/
mv OpenFLPlugin/plugin/lib/mac/OpenFLPlugin.fcm.plugin/Contents/MacOS/OpenFLPlugin OpenFLPlugin/plugin/lib/mac/OpenFLPlugin.fcm.plugin/Contents/MacOS/OpenFLPlugin
rm -rf ~/Library/Application\ Support/Adobe/CEP/extensions/OpenFLPlugin
cp -rf OpenFLPlugin ~/Library/Application\ Support/Adobe/CEP/extensions/OpenFLPlugin
