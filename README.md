OpenFL Flash Plugin
===================

Adds OpenFL custom platform support to Flash Professional CC.


Development Builds
==================

Clone the OpenFL Flash Plugin repository:

    git clone https://github.com/openfl/openfl-flash-plugin --recursive


Windows
-------

In order to see the "Publish Settings" dialog, you must enable "PlayerDebugMode".

Open `regedit` and browse to HKEY_CURRENT_USER\SOFTWARE\Adobe\CSXS.6. Create a String key called "PlayerDebugMode" and give it a value 1.

To compile the plugin, you need Haxe, HXCPP and Visual Studio C++ installed. You can use "build.bat" (included in the project) to build and install the plugin locally:

    cd openfl-flash-plugin
    build.bat

You can use `build.bat clean` as well, if you want a clean build.

Making changes to the "Publish Settings" dialog will be available the next time the dialog is opened. Making other changes usually requires closing and re-opening Flash Professional.


Mac
---

In order to use a development version, you need to enable "PlayerDebugMode".

Edit "~/Library/Preferences/com.adobe.CSXS.6.plist" then add a key called "PlayerDebugMode" with a string value of "1"

To compile the plugin, you need Haxe, HXCPP and Xcode installed. You can use "build.sh" (included in the project) to build and install the plugin locally:

    cd openfl-flash-plugin
    ./build.sh

You can use `./build.sh clean` as well, if you want a clean build.

Making changes to the "Publish Settings" dialog will be available the next time the dialog is opened. Making other changes usually requires closing and re-opening Flash Professional.
