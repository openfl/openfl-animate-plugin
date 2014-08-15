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

#include "DocType.h"
#include "Utils.h"
#include "MyXmlParser.h"
#include "xercesc/framework/MemBufInputSource.hpp"
#include <fstream>
#include <sstream>

#include "Application/Service/IOutputConsoleService.h"

namespace OpenFL
{

    /* -------------------------------------------------- Constants */

    static const std::string kElement_Features("Features");
    static const std::string kElement_Feature("Feature");
    static const std::string kElement_Property("Property");
    static const std::string kElement_Value("Value");
    static const std::string kAttribute_name("name");
    static const std::string kAttribute_supported("supported");
    static const std::string kAttribute_default("default");
    static const std::string kValue_true("true");
    static const std::string kValue_false("false");


    /* -------------------------------------------------- Static Functions */


    /* -------------------------------------------------- DocType */

    CDocType::CDocType()
    {
        m_fm = NULL;
    }

    CDocType::~CDocType()
    {
        FCM_RELEASE(m_fm);
    }

    FCM::Result CDocType::GetFeatureMatrix(PIFeatureMatrix& pFeatureMatrix)
    {
        FCM::Result res = FCM_SUCCESS;

        if (m_fm == NULL)
        {
            res = GetCallback()->CreateInstance(
                NULL, 
                CLSID_FeatureMatrix, 
                ID_IFEATURE_MATRIX, 
                (FCM::PPVoid)&m_fm);

            ((FeatureMatrix*)m_fm)->Init(GetCallback());
        }

        pFeatureMatrix = m_fm;
        return res;
    }


    /* -------------------------------------------------- FeatureMatrix */

    FeatureMatrix::FeatureMatrix()
    {
        m_bInited = false;
    }

    FeatureMatrix::~FeatureMatrix()
    {
    }

    void FeatureMatrix::Init(FCM::PIFCMCallback pCallback)
    {
        std::string featureXMLPath;

        if (m_bInited)
        {
            return;
        }

        Utils::GetModuleFilePath(featureXMLPath, pCallback);

#ifdef __APPLE__
        featureXMLPath += "Common/Configuration/Extensions/Features.xml";
#else
        featureXMLPath += "Common\\Configuration\\Extensions\\Features.xml";
#endif

        // trace
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
        FCM::Result res = pCallback->GetService(FlashApplication::Service::FLASHAPP_OUTPUT_CONSOLE_SERVICE, pUnk.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
        
        FCM::AutoPtr<FlashApplication::Service::IOutputConsoleService> outputConsoleService = pUnk;
        FCM::StringRep16 path = Utils::ToString16(featureXMLPath,pCallback);
        FCM::StringRep16 outputString = Utils::ToString16(std::string("\nThe feature settings for the OpenFLExtension document type is read from "),GetCallback());
        outputConsoleService->Trace(outputString);
        outputConsoleService->Trace(path);

        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc = OpenFL::Utils::GetCallocService(pCallback);
        ASSERT(pCalloc.m_Ptr != NULL);
  
        pCalloc->Free(outputString);
        pCalloc->Free(path);
        path = outputString = NULL;
        
        std::ifstream xmlFile(featureXMLPath.c_str(), std::ifstream::in | std::ifstream::binary);

        char *buffer = NULL;
        long length = 0;

        if (xmlFile) 
        {
            xmlFile.seekg (0, xmlFile.end);
            length = (long)xmlFile.tellg();
            xmlFile.seekg (0, xmlFile.beg);
            buffer = new char [length + 1];
            xmlFile.read (buffer, length);
            buffer[length] = 0;
        }
        xmlFile.close();        
        
        FeatureDocumentHandler featureXmlDocHandler(this);
        SAXParser parser;
        parser.setDocumentHandler(&featureXmlDocHandler);

        XERCES_CPP_NAMESPACE::MemBufInputSource	memInput((const XMLByte*) buffer, length, "dummy", false);
        m_bInited = true;
        parser.parse(memInput);

        delete[] buffer;
    }

    FCM::Result FeatureMatrix::IsSupported(CStringRep16 inFeatureName, FCM::Boolean& isSupported)
    {
        std::string featureLC = Utils::ToString(inFeatureName, GetCallback());
        
        Feature* pFeature = FindFeature(featureLC);
        if (pFeature == NULL)
        {
            /* If a feature is not found, it is supported */
            isSupported = true;
        }
        else
        {
            isSupported =  pFeature->IsSupported();
        }
        return FCM_SUCCESS;
    }

    FCM::Result FeatureMatrix::IsSupported(
        CStringRep16 inFeatureName, 
        CStringRep16 inPropName, 
        FCM::Boolean& isSupported)
    {     
        std::string featureLC = Utils::ToString(inFeatureName, GetCallback());

        Feature* pFeature = FindFeature(featureLC);
        if (pFeature == NULL)
        {
            /* If a feature is not found, it is supported */
            isSupported =  true;
        }
        else
        {
            if (!pFeature->IsSupported())
            {
                /* If a feature is not supported, sub-features are not supported */
                isSupported = false;
            }
            else
            {
                // Look if sub-features are supported.
                std::string propertyLC = Utils::ToString(inPropName, GetCallback());
                
                Property* pProperty = pFeature->FindProperty(propertyLC);
                if (pProperty == NULL)
                {
                    /* If a property is not found, it is supported */
                    isSupported =  true;
                }
                else
                {
                    isSupported = pProperty->IsSupported();
                }
            }
        }
        return FCM_SUCCESS;
    }


    FCM::Result FeatureMatrix::IsSupported(
        CStringRep16 inFeatureName, 
        CStringRep16 inPropName, 
        CStringRep16 inValName, 
        FCM::Boolean& isSupported)
    {
        std::string featureLC = Utils::ToString(inFeatureName, GetCallback());

        Feature* pFeature = FindFeature(featureLC);
        if (pFeature == NULL)
        {
            /* If a feature is not found, it is supported */
            isSupported = true;
        }
        else
        {
            if (!pFeature->IsSupported())
            {
                /* If a feature is not supported, sub-features are not supported */
                isSupported = false;
            }
            else
            {
                std::string propertyLC(Utils::ToString(inPropName, GetCallback()));

                Property* pProperty = pFeature->FindProperty(propertyLC);
                if (pProperty == NULL)
                {
                    /* If a property is not found, it is supported */
                    isSupported = true;
                }
                else
                {
                    if (!pProperty->IsSupported())
                    {
                        /* If a property is not supported, all values are not supported */
                        isSupported = false;
                    }
                    else
                    {
                        std::string valueLC(Utils::ToString(inValName, GetCallback()));

                        Value* pValue = pProperty->FindValue(valueLC);
                        if (pValue == NULL)
                        {
                            /* If a value is not found, it is supported */
                            isSupported = true;
                        }
                        else
                        {
                            isSupported = pValue->IsSupported();
                        }
                    }

                }
            }

        }
        return FCM_SUCCESS;
    }


    FCM::Result FeatureMatrix::GetDefaultValue(CStringRep16 inFeatureName, 
            CStringRep16 inPropName,
            FCM::VARIANT& outDefVal)
    {
        /*
        FCM::Result res = FCM_INVALID_PARAM;
        std::string featureName = Utils::ToString(inFeatureName, GetCallback());
        std::string propName = Utils::ToString(inPropName, GetCallback());
        
        Property* pProperty = NULL;
        Feature* pFeature = FindFeature(featureName);
        if (pFeature != NULL && pFeature->IsSupported())
        {
            pProperty = pFeature->FindProperty(propName);
            if (pProperty != NULL && pProperty->IsSupported())
            {
                std::string strVal = pProperty->GetDefault();
                std::istringstream iss(strVal);
                switch (outDefVal.m_type) {
                    case kFCMVarype_UInt32: iss>>outDefVal.m_value.uVal;break;
                    case kFCMVarype_Float: iss>>outDefVal.m_value.fVal;break;
                    case kFCMVarype_Bool: outDefVal.m_value.bVal = (kValue_true == strVal); break;
                    case kFCMVarype_CString: outDefVal.m_value.strVal = Utils::ToString16(strVal, GetCallback()); break;
                    case kFCMVarype_Double: iss>>outDefVal.m_value.dVal;break;
                    default: break;
                }
            }
        }

        isSupported = (pFeature == NULL) || (pFeature->IsSupported() && ((pProperty == NULL) || pFeature->IsSupported()));
        */
        return FCM_SUCCESS;
    }


    FCM::Result FeatureMatrix::StartElement(
        const std::string name,
        const std::map<std::string, std::string>& attrs)
    {
        std::string name8(name);

        if (kElement_Feature.compare(name8) == 0)
        {
            // Start of a feature tag
            mCurrentFeature = UpdateFeature(attrs);
            mCurrentProperty = NULL;
        }
        else if (kElement_Property.compare(name8) == 0)
        {
            // Start of a property tag
            mCurrentProperty = UpdateProperty(mCurrentFeature, attrs);
        }
        else if (kElement_Value.compare(name8) == 0)
        {
            // Start of a value tag
            UpdateValue(mCurrentProperty, attrs);
        }

        return FCM_SUCCESS;
    }

    FCM::Result FeatureMatrix::EndElement(const std::string name)
    {
        std::string name8(name);

        if (kElement_Feature.compare(name8) == 0)
        {
            // End of a feature tag
            mCurrentFeature = NULL;
            mCurrentProperty = NULL;
        }
        else if (kElement_Property.compare(name8) == 0)
        {
            // End of a property tag
            mCurrentProperty = NULL;
        }
        return FCM_SUCCESS;
    }

    Feature* FeatureMatrix::FindFeature(const std::string& inFeatureName)
    {
        StrFeatureMap::iterator itr = mFeatures.find(inFeatureName);
        if (itr != mFeatures.end())
        {
            return itr->second;
        }
        return NULL;
    }

    Feature* FeatureMatrix::UpdateFeature(const std::map<std::string, std::string>& inAttrs)
    {
        // name: mandatory attribute
        std::string name;
        std::map<std::string, std::string>::const_iterator itr = inAttrs.find(kAttribute_name);
        if ((itr == inAttrs.end()) || (itr->second.empty()))
        {
            return NULL;
        }
        else
        {
            name = itr->second;
        }

        // supported: optional attribute
        bool supported = true;
        itr = inAttrs.find(kAttribute_supported);
        if (itr != inAttrs.end())
        {
            supported = (itr->second == kValue_true);
        }
        
        // Find or Create new Feature

        Feature * pFeature = FindFeature(name);   
        if (pFeature == NULL)
        {
            pFeature = new Feature(supported);
            mFeatures.insert(std::pair<std::string, Feature*>(name, pFeature));
        }

        return pFeature;
    }


    Property* FeatureMatrix::UpdateProperty(
        Feature* inFeature, 
        const std::map<std::string,std::string>& inAttrs)
    {
        if (inFeature == NULL)
        {
            return NULL;
        }
        
        std::string name;
        
        // name: mandatory attribute
        std::map<std::string, std::string>::const_iterator itr = inAttrs.find(kAttribute_name);
        if ((itr == inAttrs.end()) || (itr->second.empty()))
        {
            return NULL;
        }
        else
        {
            name = itr->second;
        }

        // supported: optional attribute
        bool supported = true;
        itr = inAttrs.find(kAttribute_supported);
        if (itr != inAttrs.end())
        {
            supported = itr->second == kValue_true;
        }

        // default: optional attribute
        std::string def;
        itr = inAttrs.find(kAttribute_default);
        if ((itr != inAttrs.end()) && (itr->second.empty() == false))
        {
            def = itr->second;
        }

        // Find or Create new Property
        Property* pProperty = NULL;
        pProperty = inFeature->FindProperty(name);
        if (pProperty == NULL)
        {
            pProperty = new Property(def, supported);
            if (pProperty != NULL)
            {
                inFeature->AddProperty(name, pProperty);
            }
        }

        return pProperty;
    }


    Value* FeatureMatrix::UpdateValue(Property* inProperty, const std::map<std::string, std::string>& inAttrs)
    {
        if (inProperty == NULL)
        {
            return NULL;
        }

        // name: mandatory attribute
        std::string name;
        std::map<std::string, std::string>::const_iterator itr = inAttrs.find(kAttribute_name);
        if ((itr == inAttrs.end()) || (itr->second.empty()))
        {
            return NULL;
        }else
        {
            name = itr->second;
        }

        // supported: optional attribute
        bool supported = true;
        itr = inAttrs.find(kAttribute_supported);
        if (itr != inAttrs.end())
        {
            supported = (itr->second == kValue_true);
        }

        // Find or Create new Value
        Value * pValue = inProperty->FindValue(name);
        if (pValue == NULL)
        {
            pValue = new Value(supported);
            if (pValue != NULL)
            {
                inProperty->AddValue(name, pValue);
            }
        }

        return pValue;
    }


    /* -------------------------------------------------- Value */

    Value::Value(bool supported) 
    { 
        mbSupported = supported;
    }

    Value::~Value()
    {
    }

    bool Value::IsSupported()
    {
        return mbSupported;
    }


    /* -------------------------------------------------- Property */

    Property::Property(const std::string& def, bool supported)
    {
        mbSupported = supported;
        mDefault = def;
    }

    Property::~Property()
    {
        StrValueMap::iterator itr = mValues.begin();
        for(; itr != mValues.end(); itr++)
        {
            if (itr->second) delete itr->second;
        }
        mValues.clear();        
    }

    Value* Property::FindValue(const std::string& inValueName)
    {
        StrValueMap::iterator itr = mValues.find(inValueName);
        if (itr != mValues.end())
            return itr->second;
        return NULL;
    }

    bool Property::AddValue(const std::string& valueName, Value* pValue)
    {
        mValues.insert(std::pair<std::string, Value*>(valueName, pValue));

        return true;
    }

    bool Property::IsSupported()
    {
        return mbSupported;
    }


    std::string Property::GetDefault()
    {
        return mDefault;
    }


    /* -------------------------------------------------- Feature */

    Feature::Feature(bool supported)
    {
        mbSupported = supported;
    }

    Feature::~Feature()
    {
        StrPropertyMap::iterator itr = mProperties.begin();
        for(; itr != mProperties.end(); itr++)
        {
            if (itr->second) delete itr->second;
        }
        mProperties.clear();
    }

    Property* Feature::FindProperty(const std::string& inPropertyName)
    {
        StrPropertyMap::iterator itr = mProperties.find(inPropertyName);
        if (itr != mProperties.end())
        {
            return itr->second;
        }
        return NULL;
    }

    bool Feature::AddProperty(const std::string& name, Property* pProperty)
    {
        mProperties.insert(std::pair<std::string, Property*>(name, pProperty));

        return true;
    }

    bool Feature::IsSupported()
    {
        return mbSupported;
    }

    /* -------------------------------------------------- Public Global Functions */

    FCM::Result RegisterDocType(FCM::PIFCMDictionary pPlugins)
    {
        FCM::Result res = FCM_SUCCESS;

        /*
         * Dictionary structure for a DocType plugin is as follows:
         *
         *  Level 0 :    
         *              -----------------------
         *             | Flash.Plugin |  ----- | --------------------------------
         *              -----------------------                                  |
         *                                                                       |
         *  Level 1:                                   <-------------------------                          
         *              -----------------------------  
         *             | CLSID_DocType_GUID |  ----- | --------------------------
         *              -----------------------------                            |
         *                                                                       |
         *  Level 2:                                      <---------------------- 
         *              ---------------------------------
         *             | Flash.Category.DocType |  ----- |-----------------------
         *              ---------------------------------                        |
         *                                                                       |
         *  Level 3:                                                     <-------
         *              ------------------------------------------------
         *             | Flash.Plugin.Name          | "SamplePlugin"    |
         *              ------------------------------------------------
         *             | Flash.Plugin.UniversalName | "SamplePlugin"    |
         *              ------------------------------------------------
         *             | Flash.Plugin.Desc          | "The SamplePlugin"|
         *              ------------------------------------------------
         *
         *  Note that before calling this function the level 0 dictionary has already
         *  been added. Here, the 1st, 2nd and 3rd level dictionaries are being added.
         */ 

        {
            // Level 1 Dictionary
            AutoPtr<IFCMDictionary> pPlugin;
            res= pPlugins->AddLevel(
                (const FCM::StringRep8)Utils::ToString(CLSID_DocType).c_str(), 
                pPlugin.m_Ptr);

            {
                // Level 2 Dictionary
                AutoPtr<IFCMDictionary> pCategory;
                res = pPlugin->AddLevel((const FCM::StringRep8)kFlashCategoryKey_DocType, pCategory.m_Ptr);

                {
                    // Level 3 Dictionary

                    // Add plugin name - Used in the "New Document Dialog" / "Start Page".
                    std::string str_name = "OpenFL";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_Name, 
                        kFCMDictType_CString, 
                        (FCM::PVoid)str_name.c_str(),
                        (FCM::U_Int32)str_name.length() + 1);

                    // Add universal name - Used to refer to it from JSFL
                    std::string str_name_uni = "OpenFL";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_UniversalName, 
                        kFCMDictType_CString, 
                        (FCM::PVoid)str_name_uni.c_str(),
                        (FCM::U_Int32)str_name_uni.length() + 1);

                    // Add plugin description - Appears in the "New Document Dialog"
                    std::string str_desc = "Create a new FLA file (*.fla) in the Flash Document window for use with OpenFL, enabling support for Windows, Mac, Linux, iOS, Android, BlackBerry, Tizen, Firefox OS, Flash, HTML5 and other targets from a single codebase.";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashDocTypeKey_Desc, 
                        kFCMDictType_CString, 
                        (FCM::PVoid)str_desc.c_str(),
                        (FCM::U_Int32)str_desc.length() + 1);
                }
            }
        }

        return res;
    }
};
