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

#include "FCMTypes.h"
#include "FCMPluginInterface.h"
#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"
#include "Publisher/IPublisher.h"
#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"
#include "Exporter/Service/IFrameCommandGenerator.h"

#include "OutputWriter.h"

/* -------------------------------------------------- Forward Decl */

using namespace FCM;
using namespace Publisher;
using namespace Exporter::Service;

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

namespace OpenFL
{
    // {835B2A74-9646-43AD-BA86-A35F4E0ECD1B}
    const FCMCLSID CLSID_Publisher =
        {0x835b2a74, 0x9646, 0x43ad, {0xba, 0x86, 0xa3, 0x5f, 0x4e, 0xe, 0xcd, 0x1b}};
    
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
        BEGIN_INTERFACE_MAP(CPublisher,FCM_PLUGIN_VERSION)
            INTERFACE_ENTRY(IPublisher)
        END_INTERFACE_MAP
        
    public:

        virtual FCM::Result _FCMCALL Publish(
            DOM::PIFLADocument pFlaDocument, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);

        virtual FCM::Result _FCMCALL Publish(
            DOM::PITimeline pITimeline, 
            const Exporter::Service::RANGE &frameRange, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);
        
        virtual FCM::Result _FCMCALL ClearCache();

        CPublisher();

        ~CPublisher();

    private:

        bool ReadString(
            FCM::PIFCMDictionary pDict, 
            FCM::StringRep8 key, 
            std::string& retString);

        FCM::Result Init();

        void LaunchBrowser(const std::string& outputFileName);

    private:

        AutoPtr<IFrameCommandGenerator> m_frameCmdGeneratorService;
        AutoPtr<IResourcePalette> m_pResourcePalette;
    };


    class ResourcePalette : public IResourcePalette, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(ResourcePalette, FCM_PLUGIN_VERSION)    
            INTERFACE_ENTRY(IResourcePalette)            
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL AddMovieClip(
            FCM::U_Int32 resourceId, 
            FCM::StringRep16 pName, 
            Exporter::Service::PITimelineBuilder pTimelineBuilder);

        virtual FCM::Result _FCMCALL AddGraphic(
            FCM::U_Int32 resourceId, 
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

    private:

        IOutputWriter* m_pOutputWriter;

        std::vector<FCM::U_Int32> m_resourceList;
    };


    class TimelineBuilder : public ITimelineBuilder, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(TimelineBuilder, FCM_PLUGIN_VERSION)    
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
            PIFCMList& pFilterable);
            
        virtual FCM::Result _FCMCALL UpdateDisplayTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::MATRIX2D& matrix);
            
        virtual FCM::Result _FCMCALL UpdateColorTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::COLOR_MATRIX& colorMatrix);

        virtual FCM::Result _FCMCALL ShowFrame();        

        virtual FCM::Result _FCMCALL AddFrameScript(FCM::StringRep16* ppScript);

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

        BEGIN_INTERFACE_MAP(TimelineBuilderFactory, FCM_PLUGIN_VERSION)    
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

