/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright [2013] Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and are protected by all applicable intellectual 
* property laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
**************************************************************************/

#include "FCMPluginInterface.h"
#include "FCMUtils.h"

#include "DocType.h"
#include "Publisher.h"

namespace OpenFL
{
    BEGIN_CLASS_MAP(OpenFLModule)
        CLASS_ENTRY(CLSID_DocType, CDocType)
        CLASS_ENTRY(CLSID_FeatureMatrix, FeatureMatrix)
        CLASS_ENTRY(CLSID_Publisher, CPublisher)
        CLASS_ENTRY(CLSID_ResourcePalette, ResourcePalette)
        CLASS_ENTRY(CLSID_TimelineBuilder, TimelineBuilder)
        CLASS_ENTRY(CLSID_TimelineBuilderFactory, TimelineBuilderFactory)
    END_CLASS_MAP
    
    OpenFLModule g_openFLModule;

    extern "C" FCMPLUGIN_IMP_EXP FCM::Result PluginGetClassInfo(
        FCM::PIFCMCalloc pCalloc, 
        FCM::PFCMClassInterfaceInfo* ppClassInfo)
    {
        return g_openFLModule.getClassInfo(pCalloc, ppClassInfo);
    }

    extern "C" FCMPLUGIN_IMP_EXP FCM::Result PluginGetClassObject(
        FCM::PIFCMUnknown pUnkOuter, 
        FCM::ConstRefFCMCLSID clsid, 
        FCM::ConstRefFCMIID iid, 
        FCM::PPVoid pAny)
    {
        return g_openFLModule.getClassObject(pUnkOuter, clsid, iid, pAny);
    }

    extern "C" FCMPLUGIN_IMP_EXP FCM::Result PluginBoot(FCM::PIFCMCallback pCallback)
    {
        return g_openFLModule.init(pCallback);
    }

    // Register the plugin - Register plugin as both DocType and Publisher
    extern "C" FCMPLUGIN_IMP_EXP FCM::Result PluginRegister(FCM::PIFCMPluginDictionary pPluginDict)
    {
        FCM::Result res = FCM_SUCCESS;

        AutoPtr<IFCMDictionary> pDictionary = pPluginDict;

	    AutoPtr<IFCMDictionary> pPlugins;
	    pDictionary->AddLevel((const FCM::StringRep8)kFCMComponent, pPlugins.m_Ptr);
	
        res = RegisterDocType(pPlugins);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }
        
        res = RegisterPublisher(pPlugins, CLSID_DocType);

        return res;
    }

    extern "C" FCMPLUGIN_IMP_EXP FCM::U_Int32 PluginCanUnloadNow(void)
    {
        return g_openFLModule.canUnloadNow();
    }

    extern "C" FCMPLUGIN_IMP_EXP FCM::Result PluginShutdown()
    {
        g_openFLModule.finalize();

        return FCM_SUCCESS;
    }

};
