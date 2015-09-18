/*
 * PLUGIN DEVELOPERS MUST CHANGE VALUES OF ALL THE MACROS AND CONSTANTS IN THIS FILE 
 * IN ORDER TO AVOID ANY CLASH WITH OTHER PLUGINS.
 */


#ifndef _PLUGIN_CONFIGURATION_H_
#define _PLUGIN_CONFIGURATION_H_

#define PUBLISHER_NAME						"OpenFL"
#define PUBLISHER_UNIVERSAL_NAME			"org.openfl.OpenFLPluginPublisher"

/* The value of the PUBLISH_SETTINGS_UI_ID has to be the HTML extension ID used for Publish settings dialog*/
#define PUBLISH_SETTINGS_UI_ID				"org.openfl.OpenFLPlugin.PublishSettings"

#define DOCTYPE_NAME						"OpenFL"
#define DOCTYPE_UNIVERSAL_NAME				"org.openfl.OpenFLPlugin"
#define DOCTYPE_DESCRIPTION					"Create a new FLA file (*.fla) in the Flash Document window for use with OpenFL, enabling support for Windows, Mac, Linux, iOS, Android, BlackBerry, Firefox OS, Flash, HTML5 and other targets from a single codebase."

/* The value of RUNTIME_FOLDER_NAME must be the name of the runtime folder present in EclipseProject/ExtensionContent. */
#define RUNTIME_FOLDER_NAME                 "OpenFLPlugin"

namespace OpenFL
{
    // {5EC3136C-63BF-4D2D-80E2-8CCA6E54DF28}
    const FCM::FCMCLSID CLSID_DocType =
        {0x5EC3136C, 0x63BF, 0x4D2D, {0x80, 0xE2, 0x8C, 0xCA, 0x6E, 0x54, 0xDF, 0x28}};

    // {50705258-F3B8-4CFF-929B-E10EC7DA8816}
    // {B0FC020D-85D8-47A3-9912-CA64F6B9EAF4}
    const FCM::FCMCLSID CLSID_FeatureMatrix =
        {0xB0FC020D, 0x85D8, 0x47A3, {0x99, 0x12, 0xCA, 0x64, 0xF6, 0xB9, 0xEA, 0xF4}};

    // {52354A97-C07F-419D-B232-4B76AF20310B}
    const FCM::FCMCLSID CLSID_Publisher =
        {0x52354A97, 0xC07F, 0x419D, {0xB2, 0x32, 0x4B, 0x76, 0xAF, 0x20, 0x31, 0x0B}};

    // {CE7E3F55-00B7-4914-AA6D-1A64F54D292C}
    const FCM::FCMCLSID CLSID_ResourcePalette =
        {0xCE7E3F55, 0x00B7, 0x4914, {0xAA, 0x6D, 0x1A, 0x64, 0xF5, 0x4D, 0x29, 0x2C}};

    // {75AE78DA-BA36-4953-8F29-2273D916E8F5}
    const FCM::FCMCLSID CLSID_TimelineBuilder =
        {0x75AE78DA, 0xBA36, 0x4953, {0x8F, 0x29, 0x22, 0x73, 0xD9, 0x16, 0xE8, 0xF5}};

    // {E0BC7DD7-6FB0-4311-A9E4-D30FE38D3387}
    const FCM::FCMCLSID CLSID_TimelineBuilderFactory =
        {0xE0BC7DD7, 0x6FB0, 0x4311, {0xA9, 0xE4, 0xD3, 0x0F, 0xE3, 0x8D, 0x33, 0x87}};
}


#endif // _PLUGIN_CONFIGURATION_H_