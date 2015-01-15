OpenFL Flash Plugin
===================

This is a work in-progress custom plugin for Flash CC, which provides an OpenFL project type.


Planned Features
================

 - [ ] Fix for OS X absolute paths to resolve errors when using CPublisher::Export()
 - [ ] Export both an "include.xml" and SWF content to a *.bundle file (see [PiratePig.bundle]( https://github.com/jgranick/PiratePig/tree/master/Assets/PiratePig.bundle))
 - [ ] Expose additional options (such as the "generate" option) in publish settings
 - [ ] Sweep through Flash editor features, disabling ones that are incompatible
 - [ ] Consider support for Ctrl+Enter testing of OpenFL projects
 - [ ] Polish styles, output and workflow to feel as seamless/professional as possible


Development Builds
==================

Clone the OpenFL Flash Plugin repository:

    git clone https://github.com/openfl/openfl-flash-plugin


Mac
---

Edit “~/Library/Preferences/com.adobe.CSXS.5.plist” and add a key called "PlayerDebugMode" with a string value of "1"

Make sure that you have Haxe, HXCPP and Xcode installed

Use the "build.sh" script file that is included in the project:

    cd openfl-flash-plugin
    ./build.sh

You can use `./build.sh clean` for a clean build, as well

Close Flash CC and open it again to see changes


Windows
-------

Use the "build.bat" file that is included in the project:

    cd openfl-flash-plugin
    build.bat

You can use `build.bat clean` for a clean build, as well

Close Flash CC and open it again to see changes
