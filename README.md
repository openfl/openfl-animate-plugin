OpenFL Animate Plugin
===================

Adds OpenFL custom platform support to Adobe Animate CC.


Development Builds
==================

Uninstall the release version of the OpenFL Animate plugin from Creative Cloud.

Clone the OpenFL Animate Plugin repository:

    git clone https://github.com/openfl/openfl-animate-plugin --recursive


Windows
-------

In order to see the "Publish Settings" dialog, you must enable "PlayerDebugMode".

Open `regedit` and browse to HKEY_CURRENT_USER\SOFTWARE\Adobe\CSXS.7. Create a String key called "PlayerDebugMode" and give it a value 1.

To compile the plugin, you need Haxe, HXCPP and Visual Studio C++ installed. You can use "build.bat" (included in the project) to build and install the plugin locally:

    cd openfl-animate-plugin
    build.bat

You can use `build.bat clean` as well, if you want a clean build.

Making changes to the "Publish Settings" dialog will be available the next time the dialog is opened. Making other changes usually requires closing and re-opening Animate.


Mac
---

In order to use a development copy of the plugin, you need to enable "PlayerDebugMode".

Run the following command:

    defaults write com.adobe.CSXS.7 PlayerDebugMode 1

To compile the plugin, you need Haxe, HXCPP and Xcode installed. You can use "build.sh" (included in the project) to build and install the plugin locally:

    cd openfl-animate-plugin
    ./build.sh

You can use `./build.sh clean` as well, if you want a clean build.

Making changes to the "Publish Settings" dialog will be available the next time the dialog is opened. Making other changes usually requires closing and re-opening Animate.
