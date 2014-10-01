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

/**
 * @file  Publisher.h
 *
 * @brief This file contains declarations for a Publisher plugin.
 */

#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <vector>

#include "Version.h"
#include "FCMTypes.h"
#include "FCMPluginInterface.h"
#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"
#include "Publisher/IPublisher.h"
#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"
#include "FrameElement/IClassicText.h"
#include "FrameElement/ITextStyle.h"
#include "Exporter/Service/IFrameCommandGenerator.h"
#include "OutputWriter.h"


/* -------------------------------------------------- Forward Decl */

using namespace FCM;
using namespace Publisher;
using namespace Exporter::Service;

namespace Application
{
    namespace Service
    {
        class IOutputConsoleService;
    }
}

namespace OpenFL
{
    class CPublisher;
    class ResourcePalette;
    class TimelineBuilder;
    class TimelineBuilderFactory;
    class IOutputWriter;
    class ITimelineWriter;
}

namespace DOM
{
    namespace Service
    {
        namespace Shape
        {
            FORWARD_DECLARE_INTERFACE(IPath);
        }
    };
};


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */

#ifdef USE_SWF_EXPORTER_SERVICE
    #define OUTPUT_FILE_EXTENSION       "swf"
#else
    #define OUTPUT_FILE_EXTENSION       "html"
#endif


namespace OpenFL
{
    // {1635256D-2F63-4715-BC70-6B2948CC84D5}
    const FCMCLSID CLSID_Publisher =
        {0x1635256d, 0x2f63, 0x4715, {0xbc, 0x70, 0x6b, 0x29, 0x48, 0xcc, 0x84, 0xd5}};


    // {D5830903-02D6-4133-A1F1-F272D29A1802}
    const FCM::FCMCLSID CLSID_ResourcePalette =
        {0xd5830903, 0x2d6, 0x4133, {0xa1, 0xf1, 0xf2, 0x72, 0xd2, 0x9a, 0x18, 0x2}};

    // {70E17C74-34DF-4C46-8F9D-900D2EC045E3}
    const FCMCLSID CLSID_TimelineBuilder =
        {0x70e17c74, 0x34df, 0x4c46, {0x8f, 0x9d, 0x90, 0xd, 0x2e, 0xc0, 0x45, 0xe3}};

    // {442305C9-F02F-43A4-B259-D1CC39666A94}
    const FCMCLSID CLSID_TimelineBuilderFactory =
        {0x442305c9, 0xf02f, 0x43a4, {0xb2, 0x59, 0xd1, 0xcc, 0x39, 0x66, 0x6a, 0x94}};

}


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace OpenFL
{

    class CPublisher : public IPublisher, public FCMObjectBase
    {
        BEGIN_INTERFACE_MAP(CPublisher, SAMPLE_PLUGIN_VERSION)
            INTERFACE_ENTRY(IPublisher)
        END_INTERFACE_MAP
        
    public:

        virtual FCM::Result _FCMCALL Publish(
            DOM::PIFLADocument pFlaDocument, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);

        virtual FCM::Result _FCMCALL Publish(
            DOM::PIFLADocument pFlaDocument, 
            DOM::PITimeline pTimeline, 
            const Exporter::Service::RANGE &frameRange, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);
        
        virtual FCM::Result _FCMCALL ClearCache();

        CPublisher();

        ~CPublisher();

    private:

        bool ReadString(
            const FCM::PIFCMDictionary pDict, 
            FCM::StringRep8 key, 
            std::string& retString);

        FCM::Result GetOutputFileName(        
            DOM::PIFLADocument pFlaDocument, 
            DOM::PITimeline pITimeline, 
            const PIFCMDictionary pDictPublishSettings,
            std::string& outFile);

        FCM::Result Export(
            DOM::PIFLADocument pFlaDocument, 
            DOM::PITimeline pTimeline, 
            const Exporter::Service::RANGE* pFrameRange, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);

        FCM::Boolean IsPreviewNeeded(const PIFCMDictionary pDictConfig);

        FCM::Result Init();

        void LaunchBrowser(const std::string& outputFileName);

        FCM::Result ExportLibraryItems(FCM::FCMListPtr pLibraryItemList);

    private:

        AutoPtr<IFrameCommandGenerator> m_frameCmdGeneratorService;
        AutoPtr<IResourcePalette> m_pResourcePalette;
    };


    class ResourcePalette : public IResourcePalette, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(ResourcePalette, SAMPLE_PLUGIN_VERSION)    
            INTERFACE_ENTRY(IResourcePalette)            
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL AddSymbol(
            FCM::U_Int32 resourceId, 
            FCM::StringRep16 pName, 
            Exporter::Service::PITimelineBuilder pTimelineBuilder);

        virtual FCM::Result _FCMCALL AddShape(
            FCM::U_Int32 resourceId, 
            DOM::FrameElement::PIShape pShape);

        virtual FCM::Result _FCMCALL AddSound(
            FCM::U_Int32 resourceId, 
            DOM::LibraryItem::PIMediaItem pLibItem);

        virtual FCM::Result _FCMCALL AddBitmap(
            FCM::U_Int32 resourceId, 
            DOM::LibraryItem::PIMediaItem pLibItem);

        virtual FCM::Result _FCMCALL AddClassicText(
            FCM::U_Int32 resourceId, 
            DOM::FrameElement::PIClassicText pClassicText);

        virtual FCM::Result _FCMCALL HasResource(
            FCM::U_Int32 resourceId, 
            FCM::Boolean& hasResource);

        ResourcePalette();

        ~ResourcePalette();

        void Init(IOutputWriter* pOutputWriter);

        void Clear();

        FCM::Result HasResource(
            const std::string& name, 
            FCM::Boolean& hasResource);

    private:

        FCM::Result ExportFill(DOM::FrameElement::PIShape pIShape);

        FCM::Result ExportStroke(DOM::FrameElement::PIShape pIShape);

        FCM::Result ExportStrokeStyle(FCM::PIFCMUnknown pStrokeStyle);

        FCM::Result ExportSolidStrokeStyle(DOM::StrokeStyle::ISolidStrokeStyle* pSolidStrokeStyle);

        FCM::Result ExportFillStyle(FCM::PIFCMUnknown pFillStyle);

        FCM::Result ExportFillBoundary(DOM::Service::Shape::PIPath pPath);

        FCM::Result ExportHole(DOM::Service::Shape::PIPath pPath);

        FCM::Result ExportPath(DOM::Service::Shape::PIPath pPath);

        FCM::Result ExportSolidFillStyle(
            DOM::FillStyle::ISolidFillStyle* pSolidFillStyle);

        FCM::Result ExportRadialGradientFillStyle(
            DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportLinearGradientFillStyle(
            DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportBitmapFillStyle(
            DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle);

        FCM::Result CreateImageFileName(DOM::ILibraryItem* pLibItem, std::string& name);

        FCM::Result CreateSoundFileName(DOM::ILibraryItem* pLibItem, std::string& name);

		FCM::Result GetFontInfo(DOM::FrameElement::ITextStyle* pTextStyleItem, std::string& name,FCM::U_Int16 fontSize);

        FCM::Result HasFancyStrokes(DOM::FrameElement::PIShape pShape, FCM::Boolean& hasFancy); 

        FCM::Result ConvertStrokeToFill(
            DOM::FrameElement::PIShape pShape,
            DOM::FrameElement::PIShape& pNewShape);

    private:

        IOutputWriter* m_pOutputWriter;

        std::vector<FCM::U_Int32> m_resourceList;

        FCM::U_Int32 m_imageFileNameLabel;

        FCM::U_Int32 m_soundFileNameLabel;

        std::vector<std::string> m_resourceNames;
    };


    class TimelineBuilder : public ITimelineBuilder, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(TimelineBuilder, SAMPLE_PLUGIN_VERSION)    
            INTERFACE_ENTRY(ITimelineBuilder)            
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL AddShape(
            FCM::U_Int32 objectId, 
            SHAPE_INFO* pShapeInfo);

        virtual FCM::Result _FCMCALL AddClassicText(
            FCM::U_Int32 objectId, 
            CLASSIC_TEXT_INFO* pClassicTextInfo);
            
        virtual FCM::Result _FCMCALL AddBitmap(
            FCM::U_Int32 objectId, 
            BITMAP_INFO* pBitmapInfo);
            
        virtual FCM::Result _FCMCALL AddMovieClip(
            FCM::U_Int32 objectId, 
            MOVIE_CLIP_INFO* pMovieClipInfo, 
            DOM::FrameElement::PIMovieClip pMovieClip);
            
        virtual FCM::Result _FCMCALL AddGraphic(
            FCM::U_Int32 objectId, 
            GRAPHIC_INFO* pGraphicInfo);

        virtual FCM::Result _FCMCALL AddSound(
            FCM::U_Int32 objectId, 
            SOUND_INFO* pSoundInfo, 
            DOM::FrameElement::PISound pSound);

        virtual FCM::Result _FCMCALL UpdateZOrder(
            FCM::U_Int32 objectId, 
            FCM::U_Int32 placeAfterObjectId);

        virtual FCM::Result _FCMCALL Remove(FCM::U_Int32 objectId);
            
        virtual FCM::Result _FCMCALL UpdateBlendMode(
            FCM::U_Int32 objectId, 
            DOM::FrameElement::BlendMode blendMode);

        virtual FCM::Result _FCMCALL UpdateVisibility(
            FCM::U_Int32 objectId, 
            FCM::Boolean visible);

        virtual FCM::Result _FCMCALL UpdateGraphicFilter(
            FCM::U_Int32 objectId, 
            PIFCMList pFilterable);
            
        virtual FCM::Result _FCMCALL UpdateDisplayTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::MATRIX2D& matrix);
            
        virtual FCM::Result _FCMCALL UpdateColorTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::COLOR_MATRIX& colorMatrix);

        virtual FCM::Result _FCMCALL ShowFrame();        

        virtual FCM::Result _FCMCALL AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum);

        virtual FCM::Result _FCMCALL RemoveFrameScript(FCM::U_Int32 layerNum);

        virtual FCM::Result _FCMCALL SetFrameLabel(FCM::StringRep16 pLabel, DOM::KeyFrameLabelType labelType);

        TimelineBuilder();

        ~TimelineBuilder();

        virtual FCM::Result Build(
            FCM::U_Int32 resourceId, 
            FCM::StringRep16 pName,
            ITimelineWriter** ppTimelineWriter);

        void Init(IOutputWriter* pOutputWriter);

    private:

        IOutputWriter* m_pOutputWriter;

        ITimelineWriter* m_pTimelineWriter;

        FCM::U_Int32 m_frameIndex;
    };


    class TimelineBuilderFactory : public ITimelineBuilderFactory, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(TimelineBuilderFactory, SAMPLE_PLUGIN_VERSION)    
            INTERFACE_ENTRY(ITimelineBuilderFactory)            
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL CreateTimelineBuilder(PITimelineBuilder& pTimelineBuilder);

        TimelineBuilderFactory();

        ~TimelineBuilderFactory();

        void Init(IOutputWriter* pOutputWriter);

    private:

        IOutputWriter* m_pOutputWriter;
    };

    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId);
};

#endif // PUBLISHER_H_

