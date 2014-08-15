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

/*
 * @file  DocType.h
 *
 * @brief This file contains declarations for a DocType plugin.
 */


#ifndef DOC_TYPE_H_
#define DOC_TYPE_H_

#include <map>

#include "FCMTypes.h"
#include "FCMPluginInterface.h"
#include "FlashFCMPublicIDs.h"
#include "DocType/IDocType.h"
#include "DocType/IFeatureMatrix.h"
#include <string>

/* -------------------------------------------------- Forward Decl */

using namespace FCM;
using namespace DocType;

namespace OpenFL
{
    class CDocType;
    class FeatureMatrix;
    class Value;
    class Property;
    class Feature;
    class FeatureDocumentHandler;
}

 
/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */

namespace OpenFL
{
    // {835B2A74-9646-43AD-AA86-A35F4E0ECD1B}
    const FCM::FCMCLSID CLSID_DocType =
        {0x835b2a74, 0x9646, 0x43ad, {0xaa, 0x86, 0xa3, 0x5f, 0x4e, 0xe, 0xcd, 0x1b}};

    // {50705258-F3B8-4CFF-929B-E10EC7DA8816}
    const FCM::FCMCLSID CLSID_FeatureMatrix =
        {0x50705258, 0xf3b8, 0x4cff, {0x92, 0x9b, 0xe1, 0xe, 0xc7, 0xda, 0x88, 0x16}};
}


/* -------------------------------------------------- Structs / Unions */

namespace OpenFL
{
    typedef std::map<std::string, Value*> StrValueMap;
    typedef std::map<std::string, Property*> StrPropertyMap;
    typedef std::map<std::string, Feature*> StrFeatureMap;
};


/* -------------------------------------------------- Class Decl */

namespace OpenFL
{
	class CDocType : public DocType::IDocType, public FCM::FCMObjectBase
    {
        BEGIN_INTERFACE_MAP(CDocType, FCM_PLUGIN_VERSION)
            INTERFACE_ENTRY(IDocType)    
        END_INTERFACE_MAP
            
    public:

        virtual FCM::Result _FCMCALL GetFeatureMatrix(DocType::PIFeatureMatrix& pFeatureMatrix);

        CDocType();

        ~CDocType();

    private:

        DocType::PIFeatureMatrix m_fm;
    };

    
    class FeatureMatrix : public DocType::IFeatureMatrix, public FCM::FCMObjectBase
    {
        BEGIN_MULTI_INTERFACE_MAP(FeatureMatrix, FCM_PLUGIN_VERSION)
            INTERFACE_ENTRY(IFeatureMatrix)
        END_INTERFACE_MAP

    public:

        virtual FCM::Result _FCMCALL IsSupported(
            CStringRep16 inFeatureName, 
            FCM::Boolean& isSupported);

        virtual FCM::Result _FCMCALL IsSupported(
            CStringRep16 inFeatureName, 
            CStringRep16 inPropName, 
            FCM::Boolean& isSupported);

        virtual FCM::Result _FCMCALL IsSupported(
            CStringRep16 inFeatureName, 
            CStringRep16 inPropName, 
            CStringRep16 inValName, 
            FCM::Boolean& isSupported);

        virtual FCM::Result _FCMCALL GetDefaultValue(
            CStringRep16 inFeatureName, 
            CStringRep16 inPropName,
            FCM::VARIANT& outDefVal);
        
        FeatureMatrix();

        ~FeatureMatrix();

        void Init(FCM::PIFCMCallback pCallback);

    private:

        FCM::Result StartElement(
            const std::string name,
            const std::map<std::string, std::string>& attrs);
        
        FCM::Result EndElement(const std::string name);            

        Feature* FindFeature(const std::string& inFeatureName);

        Feature* UpdateFeature(const std::map<std::string, std::string>& inAttrs);

        Property* UpdateProperty(Feature* inFeature, const std::map<std::string,std::string>& inAttrs);

        Value* UpdateValue(Property* inProperty, const std::map<std::string, std::string>& inAttrs);
            
    private:

        StrFeatureMap mFeatures;

        Feature* mCurrentFeature;

        Property* mCurrentProperty;

        bool m_bInited;
        
        friend class FeatureDocumentHandler;
    };


    class Value
    {
    public:

        Value(bool supported);

        ~Value();

        bool IsSupported();

    private:
        bool mbSupported;
    };


    class Property
    {
    public:
        Property(const std::string& def, bool supported);

        ~Property();

        Value* FindValue(const std::string& inValueName);

        bool AddValue(const std::string& valueName, Value* pValue);

        bool IsSupported();

        std::string GetDefault();
        
    private:
        std::string mDefault;
        bool mbSupported;
        StrValueMap mValues;
    };


    class Feature
    {

    public:

        Feature(bool supported);

        ~Feature();

        Property* FindProperty(const std::string& inPropertyName);

        bool AddProperty(const std::string& name, Property* pProperty);

        bool IsSupported();

    private:

        bool mbSupported;

        StrPropertyMap mProperties;
    };

    FCM::Result RegisterDocType(FCM::PIFCMDictionary pPlugins);
};


#endif // DOC_TYPE_H_

