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

#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#endif

#include "Publisher.h"
#include "Utils.h"

#include "FlashFCMPublicIDs.h"

#include "FrameElement/IShape.h"

#include "StrokeStyle/IDashedStrokeStyle.h"
#include "StrokeStyle/IDottedStrokeStyle.h"
#include "StrokeStyle/IHatchedStrokeStyle.h"
#include "StrokeStyle/IRaggedStrokeStyle.h"
#include "StrokeStyle/ISolidStrokeStyle.h"
#include "StrokeStyle/IStippleStrokeStyle.h"

#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"


#include "MediaInfo/IBitmapInfo.h"
#include "FrameElement/IBitmapInstance.h"
#include "FrameElement/ISound.h"
#include "LibraryItem/IMediaItem.h"
#include "ILibraryItem.h"

#include "Service/Shape/IRegionGeneratorService.h"
#include "Service/Shape/IFilledRegion.h"
#include "Service/Shape/IStrokeGroup.h"
#include "Service/Shape/IPath.h"
#include "Service/Shape/IEdge.h"
#include "Service/Image/IBitmapExportService.h"

#include "Utils/DOMTypes.h"
#include "Utils/ILinearColorGradient.h"
#include "Utils/IRadialColorGradient.h"

#include "OutputWriter.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"


namespace OpenFL
{

    /* ----------------------------------------------------- CPublisher */
	
    CPublisher::CPublisher()
    {
    }

    CPublisher::~CPublisher()
    {

    }

    FCM::Result CPublisher::Publish(
        DOM::PIFLADocument pFlaDocument, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        std::string outFile;
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;

        Init();

        FCM::Result tempRes = GetCallback()->GetService(FlashApplication::Service::FLASHAPP_OUTPUT_CONSOLE_SERVICE, pUnk.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(tempRes));
        
        FCM::AutoPtr<FlashApplication::Service::IOutputConsoleService> outputConsoleService = pUnk;
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc = OpenFL::Utils::GetCallocService(GetCallback());
        ASSERT(pCalloc.m_Ptr != NULL);

        //trace
        FCM::StringRep16 outputString = Utils::ToString16(std::string("\nPublishing begins...."),GetCallback());
        outputConsoleService->Trace(outputString);
        pCalloc->Free(outputString);
        
#ifndef USE_EXPORTER_SERVICE

        // Use the SWF Exporter Service to export to a SWF
		
        res = GetCallback()->GetService(Exporter::Service::EXPORTER_SWF_SERVICE, pUnk.m_Ptr);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }
		
        // Read the output file name from the publish settings
        ReadString(pDictPublishSettings, (FCM::StringRep8)"out_file", outFile);
        FCM::StringRep16 outputFilePath = Utils::ToString16(outFile, GetCallback());
		
        //trace
        outputString = Utils::ToString16(std::string("\nCreating the SWF file "),GetCallback());
        outputConsoleService->Trace(outputString);
        pCalloc->Free(outputString);
        outputConsoleService->Trace(outputFilePath);
        
        FCM::AutoPtr<Exporter::Service::ISWFExportService> pSWfExportService = pUnk;

        res = pSWfExportService->ExportToFile(pFlaDocument, pDictPublishSettings, outputFilePath);
        pCalloc->Free(outputFilePath);
        if (FCM_SUCCESS_CODE(res))
        {
            return res;
        }
		
		// Post-process the SWF

#else
        DOM::Utils::COLOR color;
        FCM::U_Int32 stageHeight;
        FCM::U_Int32 stageWidth;
        AutoPtr<ITimelineBuilderFactory> pTimelineBuilderFactory;

        // Create a output writer
        std::auto_ptr<IOutputWriter> pOutputWriter(new JSONOutputWriter(GetCallback()));
        if (pOutputWriter.get() == NULL)
        {
            return FCM_MEM_NOT_AVAILABLE;
        }

        // Read the output file name from the publish settings
        ReadString(pDictPublishSettings, (FCM::StringRep8)"out_file", outFile);
        
        //trace
        FCM::StringRep16 outputFilePath = Utils::ToString16(outFile, GetCallback());
        outputString = Utils::ToString16(std::string("\nCreating "),GetCallback());
        outputConsoleService->Trace(outputString);
        pCalloc->Free(outputString);
        outputConsoleService->Trace(outputFilePath);
        pCalloc->Free(outputFilePath);

        
        // Start output
        pOutputWriter->StartOutput(outFile);

        // Create a Timeline Builder Factory for the root timeline of the document
        res = GetCallback()->CreateInstance(
            NULL, 
            CLSID_TimelineBuilderFactory, 
            IID_ITimelineBuilderFactory, 
            (void**)&pTimelineBuilderFactory);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }

        (static_cast<TimelineBuilderFactory*>(pTimelineBuilderFactory.m_Ptr))->Init(
            pOutputWriter.get());

        ResourcePalette* pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);
        pResPalette->Clear();
        pResPalette->Init(pOutputWriter.get());

        // Get all the timelines for the document
        FCM::FCMListPtr pTimelineList;
        res = pFlaDocument->GetTimelines(pTimelineList.m_Ptr);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }

        res = pFlaDocument->GetBackgroundColor(color);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pFlaDocument->GetStageHeight(stageHeight);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pFlaDocument->GetStageWidth(stageWidth);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pOutputWriter->StartDocument(color, stageHeight, stageWidth);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::U_Int32 timelineCount;
        res = pTimelineList->Count(timelineCount);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }

        // Generate frame commands for each timeline
        for (FCM::U_Int32 i = 0; i < timelineCount; i++)
        {
            Exporter::Service::RANGE range;
            AutoPtr<ITimelineBuilder> pTimelineBuilder;
            ITimelineWriter* pTimelineWriter;

            AutoPtr<DOM::ITimeline> timeline = pTimelineList[i];

            range.min = 0;
            res = timeline->GetMaxFrameCount(range.max);
            if (FCM_FAILURE_CODE(res))
            {
                return res;
            }

            range.max--;

            // Generate frame commands
            res = m_frameCmdGeneratorService->GenerateFrameCommands(
                timeline, 
                range, 
                m_pResourcePalette, 
                pTimelineBuilderFactory, 
                pTimelineBuilder.m_Ptr);

            if (FCM_FAILURE_CODE(res))
            {
                return res;
            }

            ((TimelineBuilder*)pTimelineBuilder.m_Ptr)->Build(0, NULL, &pTimelineWriter);
        }


        res = pOutputWriter->EndDocument();
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pOutputWriter->EndOutput();
        ASSERT(FCM_SUCCESS_CODE(res));

        // Launch the browser
        std::string fileName;
        Utils::GetFileName(outFile, fileName);
        LaunchBrowser(fileName);
#endif
        return FCM_SUCCESS;
    }


    // Not tested yet.
    FCM::Result CPublisher::Publish(
        DOM::PITimeline pITimeline, 
        const Exporter::Service::RANGE &frameRange, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        return FCM_SUCCESS;
    }


    // Not tested yet.
    FCM::Result CPublisher::ClearCache()
    {
        ResourcePalette* pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);

        pResPalette->Clear();

        return FCM_SUCCESS;
    }


    bool CPublisher::ReadString(
        FCM::PIFCMDictionary pDict,
        FCM::StringRep8 key, 
        std::string &retString)
    {
        FCM::U_Int32 valueLen;
        FCM::FCMDictRecTypeID type;

        FCM::Result res = pDict->GetInfo(key, type, valueLen);
        if (FCM_FAILURE_CODE(res))
        {
            return false;
        }

        FCM::StringRep8 strValue = new char[valueLen];
        res = pDict->Get(key, type, (FCM::PVoid)strValue, valueLen);
        if (FCM_FAILURE_CODE(res))
        {
            delete [] strValue;
            return false;
        }

        retString = strValue;

        delete [] strValue;
        return true;
    }


    FCM::Result CPublisher::Init()
    {
        FCM::Result res = FCM_SUCCESS;;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;

        if (!m_frameCmdGeneratorService)
        {
            // Get the frame command generator service
            res = GetCallback()->GetService(Exporter::Service::EXPORTER_FRAME_CMD_GENERATOR_SERVICE, pUnk.m_Ptr);
            m_frameCmdGeneratorService = pUnk;
        }

        if (!m_pResourcePalette)
        {
            // Create a Resource Palette
            res = GetCallback()->CreateInstance(NULL, CLSID_ResourcePalette, IID_IResourcePalette, (void**)&m_pResourcePalette);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        return res;
    }


    void CPublisher::LaunchBrowser(const std::string& outputFileName)
    {

#ifdef _WINDOWS

        std::wstring output = L"http://localhost:8080/";
        std::wstring tail;
        tail.assign(outputFileName.begin(), outputFileName.end());
        output += tail;
        ShellExecute(NULL, L"open", output.c_str(), NULL, NULL, SW_SHOWNORMAL);

#else

        std::string output = "http://localhost:8080/";
        output += outputFileName;
        std::string str = "/usr/bin/open " + output;
        system(str.c_str());

#endif // _WINDOWS

    }

    /* ----------------------------------------------------- Resource Palette */

    FCM::Result ResourcePalette::AddMovieClip(
        FCM::U_Int32 resourceId, 
        FCM::StringRep16 pName, 
        Exporter::Service::PITimelineBuilder pTimelineBuilder)
    {
        FCM::Result res;
        ITimelineWriter* pTimelineWriter;

        m_resourceList.push_back(resourceId);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);

        res = pTimeline->Build(resourceId, pName, &pTimelineWriter);

        return res;
    }

    FCM::Result ResourcePalette::AddGraphic(
        FCM::U_Int32 resourceId, 
        Exporter::Service::PITimelineBuilder pTimelineBuilder)
    {
        FCM::Result res;
        ITimelineWriter* pTimelineWriter;

        m_resourceList.push_back(resourceId);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);

        res = pTimeline->Build(resourceId, NULL, &pTimelineWriter);

        return res;
    }


    FCM::Result ResourcePalette::AddShape(
        FCM::U_Int32 resourceId, 
        DOM::FrameElement::PIShape pShape)
    {
        m_resourceList.push_back(resourceId);
        m_pOutputWriter->StartDefineShape();

        if (pShape)
        {
            ExportFill(pShape);
            ExportStroke(pShape);
        }

        m_pOutputWriter->EndDefineShape(resourceId);

        return FCM_SUCCESS;
    }


    FCM::Result ResourcePalette::AddSound(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        // Sound is not tested yet.
        m_resourceList.push_back(resourceId);
        return FCM_SUCCESS;
    }


    FCM::Result ResourcePalette::AddBitmap(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        DOM::AutoPtr<DOM::ILibraryItem> pLibItem;
        FCM::StringRep16 pName;
        FCM::Result res;

        m_resourceList.push_back(resourceId);

        // Get name
        pLibItem = pMediaItem;
        pLibItem->GetName(&pName);

        AutoPtr<FCM::IFCMUnknown> medInfo;
        pMediaItem->GetMediaInfo(medInfo.m_Ptr);

        AutoPtr<DOM::MediaInfo::IBitmapInfo> bitsInfo = medInfo;
        ASSERT(bitsInfo);

        // Get image height
        FCM::S_Int32 height;
        res = bitsInfo->GetHeight(height);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Get image width
        FCM::S_Int32 width;
        res = bitsInfo->GetWidth(width);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Dump the definition of a bitmap
        res = m_pOutputWriter->DefineBitmap(resourceId, height, width, pName, pMediaItem);

        // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pName);

        return res;
    }


    FCM::Result ResourcePalette::AddClassicText(FCM::U_Int32 resourceId, DOM::FrameElement::PIClassicText pClassicText)
    {
        // Text is not tested yet
        m_resourceList.push_back(resourceId);
        return FCM_SUCCESS;
    }


    FCM::Result ResourcePalette::HasResource(FCM::U_Int32 resourceId, FCM::Boolean& hasResource)
    {
        hasResource = false;

        for (std::vector<FCM::U_Int32>::iterator listIter = m_resourceList.begin();
                listIter != m_resourceList.end(); listIter++)
        {
            if (*listIter == resourceId)
            {
                hasResource = true;
                break;
            }
        }

        return FCM_SUCCESS;
    }


    ResourcePalette::ResourcePalette()
    {
        m_pOutputWriter = NULL;
    }


    ResourcePalette::~ResourcePalette()
    {
    }


    void ResourcePalette::Init(IOutputWriter* pOutputWriter)
    {
        m_pOutputWriter = pOutputWriter;
    }

    void ResourcePalette::Clear()
    {
        m_resourceList.clear();
    }

    FCM::Result ResourcePalette::ExportFill(DOM::FrameElement::PIShape pIShape)
    {
        FCM::Result res;
        FCM::FCMListPtr pFilledRegionList;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        FCM::U_Int32 regionCount;

        GetCallback()->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
        AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService(pUnkSRVReg);
        ASSERT(pIRegionGeneratorService);

        res = pIRegionGeneratorService->GetFilledRegions(pIShape, pFilledRegionList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        pFilledRegionList->Count(regionCount);

        for (FCM::U_Int32 j = 0; j < regionCount; j++)
        {
            FCM::AutoPtr<DOM::Service::Shape::IFilledRegion> pFilledRegion = pFilledRegionList[j];
            FCM::AutoPtr<DOM::Service::Shape::IPath> pPath;

            m_pOutputWriter->StartDefineFill();

            // Fill Style
            FCM::AutoPtr<DOM::IFCMUnknown> fillStyle;

            res = pFilledRegion->GetFillStyle(fillStyle.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            ExportFillStyle(fillStyle);

            // Boundary
            res = pFilledRegion->GetBoundary(pPath.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = ExportFillBoundary(pPath);
            ASSERT(FCM_SUCCESS_CODE(res));

            // Hole List
            FCMListPtr pHoleList;
            FCM::U_Int32 holeCount;

            res = pFilledRegion->GetHoles(pHoleList.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = pHoleList->Count(holeCount);
            ASSERT(FCM_SUCCESS_CODE(res));

            for (FCM::U_Int32 k = 0; k < holeCount; k++)
            {
                FCM::FCMListPtr pEdgeList;
                FCM::AutoPtr<DOM::Service::Shape::IPath> pPath = pHoleList[k];

                res = ExportHole(pPath);
            }

            m_pOutputWriter->EndDefineFill();
        }

        return res;
    }


    FCM::Result ResourcePalette::ExportFillBoundary(DOM::Service::Shape::PIPath pPath)
    {
        FCM::Result res;

        m_pOutputWriter->StartDefineBoundary();

        res = ExportPath(pPath);
        ASSERT(FCM_SUCCESS_CODE(res));

        m_pOutputWriter->EndDefineBoundary();

        return res;
    }


    FCM::Result ResourcePalette::ExportHole(DOM::Service::Shape::PIPath pPath)
    {
        FCM::Result res;

        m_pOutputWriter->StartDefineHole();

        res = ExportPath(pPath);
        ASSERT(FCM_SUCCESS_CODE(res));

        m_pOutputWriter->EndDefineHole();

        return res;
    }


    FCM::Result ResourcePalette::ExportPath(DOM::Service::Shape::PIPath pPath)
    {
        FCM::Result res;
        FCM::U_Int32 edgeCount;
        FCM::FCMListPtr pEdgeList;

        res = pPath->GetEdges(pEdgeList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pEdgeList->Count(edgeCount);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int32 l = 0; l < edgeCount; l++)
        {
            DOM::Utils::SEGMENT segment;

            segment.structSize = sizeof(DOM::Utils::SEGMENT);

            FCM::AutoPtr<DOM::Service::Shape::IEdge> pEdge = pEdgeList[l];

            res = pEdge->GetSegment(segment);

            m_pOutputWriter->SetSegment(segment);
        }

        return res;
    }

    FCM::Result ResourcePalette::ExportFillStyle(FCM::PIFCMUnknown pFillStyle)
    {
        FCM::Result res = FCM_SUCCESS;

        AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle;
        AutoPtr<DOM::FillStyle::IGradientFillStyle> pGradientFillStyle;
        AutoPtr<DOM::FillStyle::IBitmapFillStyle> pBitmapFillStyle;

        // Check for solid fill color
        pSolidFillStyle = pFillStyle;
        if (pSolidFillStyle)
        {
            res = ExportSolidFillStyle(pSolidFillStyle);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        // Check for Gradient Fill
        pGradientFillStyle = pFillStyle;
        AutoPtr<FCM::IFCMUnknown> pGrad;

        if (pGradientFillStyle)
        {
            pGradientFillStyle->GetColorGradient(pGrad.m_Ptr);

            if (AutoPtr<DOM::Utils::IRadialColorGradient>(pGrad))
            {
                res = ExportRadialGradientFillStyle(pGradientFillStyle);
                ASSERT(FCM_SUCCESS_CODE(res));
            }
            else if (AutoPtr<DOM::Utils::ILinearColorGradient>(pGrad))
            {
                res = ExportLinearGradientFillStyle(pGradientFillStyle);
                ASSERT(FCM_SUCCESS_CODE(res));
            }
        }

        pBitmapFillStyle = pFillStyle;
        if (pBitmapFillStyle)
        {
            res = ExportBitmapFillStyle(pBitmapFillStyle);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        return res;
    }


    FCM::Result ResourcePalette::ExportStroke(DOM::FrameElement::PIShape pIShape)
    {
        FCM::FCMListPtr pStrokeGroupList;
        FCM::U_Int32 strokeStyleCount;
        FCM::Result res;

        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        GetCallback()->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
        AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService(pUnkSRVReg);
        ASSERT(pIRegionGeneratorService);

        res = pIRegionGeneratorService->GetStrokeGroups(pIShape, pStrokeGroupList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pStrokeGroupList->Count(strokeStyleCount);
        ASSERT(FCM_SUCCESS_CODE(res));

        AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle = NULL;
        AutoPtr<DOM::FillStyle::IGradientFillStyle> pGradientFillStyle = NULL;
        AutoPtr<DOM::FillStyle::IBitmapFillStyle> pBitmapFillStyle = NULL;

        AutoPtr<FCM::IFCMUnknown> pGrad;

        for (FCM::U_Int32 j = 0; j < strokeStyleCount; j++)
        {
            AutoPtr<DOM::Service::Shape::IStrokeGroup> pStrokeGroup = pStrokeGroupList[j];
            ASSERT(pStrokeGroup);

            res = m_pOutputWriter->StartDefineStrokeGroup();
            ASSERT(FCM_SUCCESS_CODE(res));

            AutoPtr<FCM::IFCMUnknown> pStrokeStyle;
            pStrokeGroup->GetStrokeStyle(pStrokeStyle.m_Ptr);

            DOM::Utils::COLOR color = {};

            FCMListPtr pPathList;
            FCM::U_Int32 pathCount;

            res = pStrokeGroup->GetPaths(pPathList.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = pPathList->Count(pathCount);
            ASSERT(FCM_SUCCESS_CODE(res));

            for (FCM::U_Int32 k = 0; k < pathCount; k++)
            {
                FCM::AutoPtr<DOM::Service::Shape::IPath> pPath;

                pPath = pPathList[k];
                ASSERT(pPath);

                res = m_pOutputWriter->StartDefineStroke();
                ASSERT(FCM_SUCCESS_CODE(res));

                res = ExportStrokeStyle(pStrokeStyle);
                ASSERT(FCM_SUCCESS_CODE(res));

                res = ExportPath(pPath);
                ASSERT(FCM_SUCCESS_CODE(res));

                res = m_pOutputWriter->EndDefineStroke();
                ASSERT(FCM_SUCCESS_CODE(res));
            }

            res = m_pOutputWriter->EndDefineStrokeGroup();
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        return res;
    }


    FCM::Result ResourcePalette::ExportStrokeStyle(FCM::PIFCMUnknown pStrokeStyle)
    {
        FCM::Result res = FCM_SUCCESS;
        AutoPtr<DOM::StrokeStyle::ISolidStrokeStyle> pSolidStrokeStyle;

        pSolidStrokeStyle = pStrokeStyle;

        if (pSolidStrokeStyle)
        {
            res = ExportSolidStrokeStyle(pSolidStrokeStyle);
        }
        else
        {
            // Other stroke styles are not tested yet.
        }

        return res;
    }


    FCM::Result ResourcePalette::ExportSolidStrokeStyle(DOM::StrokeStyle::ISolidStrokeStyle* pSolidStrokeStyle)
    {
        FCM::Result res;
        FCM::Double thickness;
        AutoPtr<DOM::IFCMUnknown> pFillStyle;
        DOM::StrokeStyle::CAP_STYLE capStyle;
        DOM::StrokeStyle::JOIN_STYLE joinStyle;
        DOM::Utils::ScaleType scaleType;
        FCM::Boolean strokeHinting;

        res = pSolidStrokeStyle->GetCapStyle(capStyle);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pSolidStrokeStyle->GetJoinStyle(joinStyle);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pSolidStrokeStyle->GetThickness(thickness);
        ASSERT(FCM_SUCCESS_CODE(res));

        if (thickness < 0.1)
        {
            thickness = 0.1;
        }

        res = pSolidStrokeStyle->GetScaleType(scaleType);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pSolidStrokeStyle->GetStrokeHinting(strokeHinting);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = m_pOutputWriter->StartDefineSolidStrokeStyle(
            thickness, 
            joinStyle, 
            capStyle, 
            scaleType, 
            strokeHinting);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Stroke fill styles
        res = pSolidStrokeStyle->GetFillStyle(pFillStyle.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = ExportFillStyle(pFillStyle);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = m_pOutputWriter->EndDefineSolidStrokeStyle();
        ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ResourcePalette::ExportSolidFillStyle(DOM::FillStyle::ISolidFillStyle* pSolidFillStyle)
    {
        FCM::Result res;
        DOM::Utils::COLOR color;

        AutoPtr<DOM::FillStyle::ISolidFillStyle> solidFill = pSolidFillStyle;
        ASSERT(solidFill);

        res = solidFill->GetColor(color);
        ASSERT(FCM_SUCCESS_CODE(res));

        m_pOutputWriter->DefineSolidFillStyle(color);

        return res;
    }


    FCM::Result ResourcePalette::ExportRadialGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
    {
        DOM::FillStyle::GradientSpread spread;

        AutoPtr<FCM::IFCMUnknown> pGrad;

        AutoPtr<DOM::FillStyle::IGradientFillStyle> gradientFill = pGradientFillStyle;
        FCM::Result res = gradientFill->GetSpread(spread);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = gradientFill->GetColorGradient(pGrad.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        AutoPtr<DOM::Utils::IRadialColorGradient> radialColorGradient = pGrad;
        ASSERT(radialColorGradient);

        DOM::Utils::MATRIX2D matrix;
        res = gradientFill->GetMatrix(matrix);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::S_Int32 focalPoint = 0;
        res = radialColorGradient->GetFocalPoint(focalPoint);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = m_pOutputWriter->StartDefineRadialGradientFillStyle(spread, matrix, focalPoint);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::U_Int8 nColors;
        res = radialColorGradient->GetKeyColorCount(nColors);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int8 i = 0; i < nColors; i++)
        {
            DOM::Utils::GRADIENT_COLOR_POINT point;

            res = radialColorGradient->GetKeyColorAtIndex(i, point);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = m_pOutputWriter->SetKeyColorPoint(point);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        res = m_pOutputWriter->EndDefineRadialGradientFillStyle();
        ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ResourcePalette::ExportLinearGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
    {
        DOM::FillStyle::GradientSpread spread;
        AutoPtr<FCM::IFCMUnknown> pGrad;

        AutoPtr<DOM::FillStyle::IGradientFillStyle> gradientFill = pGradientFillStyle;
        FCM::Result res = gradientFill->GetSpread(spread);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = gradientFill->GetColorGradient(pGrad.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        AutoPtr<DOM::Utils::ILinearColorGradient> linearColorGradient = pGrad;
        ASSERT(linearColorGradient);

        DOM::Utils::MATRIX2D matrix;
        res = gradientFill->GetMatrix(matrix);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = m_pOutputWriter->StartDefineLinearGradientFillStyle(spread, matrix);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::U_Int8 nColors;
        res = linearColorGradient->GetKeyColorCount(nColors);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int8 i = 0; i < nColors; i++)
        {
            DOM::Utils::GRADIENT_COLOR_POINT point;

            res = linearColorGradient->GetKeyColorAtIndex(i, point);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = m_pOutputWriter->SetKeyColorPoint(point);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        res = m_pOutputWriter->EndDefineLinearGradientFillStyle();
        ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ResourcePalette::ExportBitmapFillStyle(DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle)
    {
        DOM::AutoPtr<DOM::ILibraryItem> pLibItem;
        DOM::AutoPtr<DOM::LibraryItem::IMediaItem> pMediaItem;
        FCM::StringRep16 pName;
        FCM::Result res;
        FCM::Boolean isClipped;
        DOM::Utils::MATRIX2D matrix;

        // IsClipped ?
        res = pBitmapFillStyle->IsClipped(isClipped);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Matrix
        res = pBitmapFillStyle->GetMatrix(matrix);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Get name
        res = pBitmapFillStyle->GetBitmap(pMediaItem.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        pLibItem = pMediaItem;
        pLibItem->GetName(&pName);

        AutoPtr<FCM::IFCMUnknown> medInfo;
        pMediaItem->GetMediaInfo(medInfo.m_Ptr);

        AutoPtr<DOM::MediaInfo::IBitmapInfo> bitsInfo = medInfo;
        ASSERT(bitsInfo);

        // Get image height
        FCM::S_Int32 height;
        res = bitsInfo->GetHeight(height);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Get image width
        FCM::S_Int32 width;
        res = bitsInfo->GetWidth(width);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Dump the definition of a bitmap fill style
        res = m_pOutputWriter->DefineBitmapFillStyle(
            isClipped, 
            matrix, 
            height, 
            width, 
            pName, 
            pMediaItem);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pName);

        return res;
    }


    /* ----------------------------------------------------- TimelineBuilder */

    FCM::Result TimelineBuilder::AddShape(FCM::U_Int32 objectId, SHAPE_INFO* pShapeInfo)
    {
        FCM::Result res;

        res = m_pTimelineWriter->PlaceObject(
            pShapeInfo->resourceId, 
            objectId, 
            pShapeInfo->placeAfterObjectId, 
            &pShapeInfo->matrix);

        return res;
    }

    FCM::Result TimelineBuilder::AddClassicText(FCM::U_Int32 objectId, CLASSIC_TEXT_INFO* pClassicTextInfo)
    {
        FCM::Result res;

        res = m_pTimelineWriter->PlaceObject(
            pClassicTextInfo->resourceId, 
            objectId, 
            pClassicTextInfo->placeAfterObjectId, 
            &pClassicTextInfo->matrix);

        return res;
    }

    FCM::Result TimelineBuilder::AddBitmap(FCM::U_Int32 objectId, BITMAP_INFO* pBitmapInfo)
    {
        FCM::Result res;

        res = m_pTimelineWriter->PlaceObject(
            pBitmapInfo->resourceId, 
            objectId, 
            pBitmapInfo->placeAfterObjectId, 
            &pBitmapInfo->matrix);

        return res;
    }

    FCM::Result TimelineBuilder::AddMovieClip(FCM::U_Int32 objectId, MOVIE_CLIP_INFO* pMovieClipInfo, DOM::FrameElement::PIMovieClip pMovieClip)
    {
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnknown = pMovieClip;

        res = m_pTimelineWriter->PlaceObject(
            pMovieClipInfo->resourceId, 
            objectId, 
            pMovieClipInfo->placeAfterObjectId, 
            &pMovieClipInfo->matrix,
            pUnknown);

        return res;
    }

    FCM::Result TimelineBuilder::AddGraphic(FCM::U_Int32 objectId, GRAPHIC_INFO* pGraphicInfo)
    {
        FCM::Result res;

        res = m_pTimelineWriter->PlaceObject(
            pGraphicInfo->resourceId, 
            objectId, 
            pGraphicInfo->placeAfterObjectId, 
            &pGraphicInfo->matrix);

        return res;
    }

    FCM::Result TimelineBuilder::AddSound(
        FCM::U_Int32 objectId, 
        SOUND_INFO* pSoundInfo, 
        DOM::FrameElement::PISound pSound)
    {
        FCM::AutoPtr<FCM::IFCMUnknown> pUnknown = pSound;
        FCM::Result res;

        res = m_pTimelineWriter->PlaceObject(
            pSoundInfo->resourceId, 
            objectId, 
            pSoundInfo->placeAfterObjectId, 
            NULL, 
            pUnknown);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateZOrder(FCM::U_Int32 objectId, FCM::U_Int32 placeAfterObjectId)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateZOrder(objectId, placeAfterObjectId);

        return res;
    }

    FCM::Result TimelineBuilder::Remove(FCM::U_Int32 objectId)
    {
        FCM::Result res;

        res = m_pTimelineWriter->RemoveObject(objectId);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateBlendMode(FCM::U_Int32 objectId, DOM::FrameElement::BlendMode blendMode)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateBlendMode(objectId, blendMode);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateVisibility(FCM::U_Int32 objectId, FCM::Boolean visible)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateVisibility(objectId, visible);

        return res;
    }


    FCM::Result TimelineBuilder::UpdateGraphicFilter(FCM::U_Int32 objectId, PIFCMList& pFilterable)
    {
        FCM::U_Int32 count;
        FCM::Result res;

        res = pFilterable->Count(count);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }
        
        return FCM_SUCCESS;
    }

    FCM::Result TimelineBuilder::UpdateDisplayTransform(FCM::U_Int32 objectId, const DOM::Utils::MATRIX2D& matrix)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateDisplayTransform(objectId, matrix);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateColorTransform(FCM::U_Int32 objectId, const DOM::Utils::COLOR_MATRIX& colorMatrix)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateColorTransform(objectId, colorMatrix);

        return res;
    }

    FCM::Result TimelineBuilder::ShowFrame()        
    {
        FCM::Result res;

        res = m_pTimelineWriter->ShowFrame(m_frameIndex);

        m_frameIndex++;

        return res;
    }

    FCM::Result TimelineBuilder::AddFrameScript(FCM::StringRep16* ppScript)
    {
        FCM::Result res = FCM_SUCCESS;

        if (ppScript != NULL)
        {
            res = m_pTimelineWriter->AddFrameScript(*ppScript);
        }

        return res;
    }

    FCM::Result TimelineBuilder::Build(
        FCM::U_Int32 resourceId, 
        FCM::StringRep16 pName,
        ITimelineWriter** ppTimelineWriter)
    {
        FCM::Result res;

        res = m_pOutputWriter->EndDefineTimeline(resourceId, pName, m_pTimelineWriter);

        *ppTimelineWriter = m_pTimelineWriter;

        return res;
    }


    TimelineBuilder::TimelineBuilder() :
        m_pOutputWriter(NULL),
        m_frameIndex(0)
    {
    }

    TimelineBuilder::~TimelineBuilder()
    {
    }

    void TimelineBuilder::Init(IOutputWriter* pOutputWriter)
    {
        m_pOutputWriter = pOutputWriter;

        m_pOutputWriter->StartDefineTimeline();

        m_pTimelineWriter = new JSONTimelineWriter();
        ASSERT(m_pTimelineWriter);
    }

    /* ----------------------------------------------------- TimelineBuilderFactory */

    TimelineBuilderFactory::TimelineBuilderFactory()
    {
    }

    TimelineBuilderFactory::~TimelineBuilderFactory()
    {
    }

    FCM::Result TimelineBuilderFactory::CreateTimelineBuilder(PITimelineBuilder& pTimelineBuilder)
    {
        FCM::Result res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilder, IID_ITimelineBuilder, (void**)&pTimelineBuilder);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);
        
        pTimeline->Init(m_pOutputWriter);

        return res;
    }

    void TimelineBuilderFactory::Init(IOutputWriter* pOutputWriter)
    {
        m_pOutputWriter = pOutputWriter;
    }

    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId)
    {
        FCM::Result res;

        /*
         * Dictionary structure for a Publisher plugin is as follows:
         *
         *  Level 0 :    
         *              -----------------------
         *             | Flash.Plugin |  ----- | --------------------------------
         *              -----------------------                                  |
         *                                                                       |
         *  Level 1:                                   <-------------------------                          
         *              ------------------------------  
         *             | CLSID_Publisher_GUID | ----- | -------------------------
         *              ------------------------------                           |
         *                                                                       |
         *  Level 2:                                      <---------------------- 
         *              -----------------------------------
         *             | Flash.Category.Publisher |  ----- |---------------------
         *              -----------------------------------                      |
         *                                                                       |
         *  Level 3:                                                           <-
         *              ---------------------------------------------------------
         *             | Flash.Plugin.Name                  | "SamplePlugin"     |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.UI                    | "com.example..."   |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetFormatName      | "xyz"              |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetFormatExtension | "abc"              |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.CanExportImage        | "true"             |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetDocs            |           ---------|-
         *              ---------------------------------------------------------  |
         *                                                                         |
         *  Level 4:                                                    <----------
         *              -----------------------------------------------
         *             | CLSID_DocType   |  Empty String               |
         *              -----------------------------------------------
         *
         *  Note that before calling this function the level 0 dictionary has already
         *  been added. Here, the 1st, 2nd and 3rd level dictionaries are being added.
         */ 

        {
            // Level 1 Dictionary
            AutoPtr<IFCMDictionary> pPlugin;
            res = pPlugins->AddLevel(
                (const FCM::StringRep8)Utils::ToString(CLSID_Publisher).c_str(), 
                pPlugin.m_Ptr);

            {
                // Level 2 Dictionary
                AutoPtr<IFCMDictionary> pCategory;
                res = pPlugin->AddLevel(
                    (const FCM::StringRep8)kFlashCategoryKey_Publisher, 
                    pCategory.m_Ptr);

                {
                    // Level 3 Dictionary
                    std::string str_name = "OpenFLExtension";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_Name,
                        kFCMDictType_CString, 
                        (FCM::PVoid)str_name.c_str(),
                        (FCM::U_Int32)str_name.length() + 1);

                    std::string str_uniname = "OpenFLExtension";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_UniversalName,
                        kFCMDictType_CString,
                        (FCM::PVoid)str_uniname.c_str(),
                        (FCM::U_Int32)str_uniname.length() + 1);

                    std::string str_ui = "org.openfl.OpenFLExtension.PublishSettings";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_UI, 
                        kFCMDictType_CString, 
                        (FCM::PVoid)str_ui.c_str(),
                        (FCM::U_Int32)str_ui.length() + 1);

                    std::string format = "xyz";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_TargetFormatName, 
                        kFCMDictType_CString, 
                        (FCM::PVoid)format.c_str(),
                        (FCM::U_Int32)format.length() + 1);

                    std::string exten = "abc";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_TargetFormatExtn, 
                        kFCMDictType_CString, 
                        (FCM::PVoid)exten.c_str(),
                        (FCM::U_Int32)exten.length() + 1);

                    FCM::Boolean canExport = true;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_CanExportImage,
                        kFCMDictType_Bool, 
                        (FCM::PVoid)&canExport,
                        sizeof(FCM::Boolean));

                    AutoPtr<IFCMDictionary> pDocs;
                    res = pCategory->AddLevel((const FCM::StringRep8)kFlashPublisherKey_TargetDocs, pDocs.m_Ptr);

                    {
                        // Level 4 Dictionary
                        std::string empytString = "";   
                        res = pDocs->Add(
                            (const FCM::StringRep8)Utils::ToString(docId).c_str(), 
                            kFCMDictType_CString, 
                            (FCM::PVoid)empytString.c_str(),
                            (FCM::U_Int32)empytString.length() + 1);
                    }
                }
            }
        }

        return res;
    }

};
