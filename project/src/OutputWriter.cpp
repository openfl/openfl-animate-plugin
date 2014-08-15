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

#include "OutputWriter.h"

#include <cstring>
#include <fstream>
#include "FlashFCMPublicIDs.h"
#include "FCMPluginInterface.h"
#include "libjson.h"
#include "Utils.h"
#include "Service/Image/IBitmapExportService.h"

namespace OpenFL
{
    static const std::string moveTo = "M";
    static const std::string lineTo = "L";
    static const std::string bezierCurveTo = "Q";
    static const std::string space = " ";
    static const std::string comma = ",";
    static const std::string semiColon = ";";

    static const FCM::Float GRADIENT_VECTOR_CONSTANT = 16384.0;

    static const char* htmlOutput = 
        "<!DOCTYPE html>\r\n \
        <html>\r\n \
        <head> \r\n\
            <script src=\"cjs/createjs-2013.12.12.min.js\"></script> \r\n\
            <script src=\"cjs/movieclip-0.7.1.min.js\"></script> \r\n\
            <script src=\"cjs/easeljs-0.7.0.min.js\"></script> \r\n\
            <script src=\"cjs/tweenjs-0.5.1.min.js\"></script> \r\n\
            <script src=\"cjs/preloadjs-0.4.1.min.js\"></script> \r\n\
           \r\n\
            <script src=\"dist/jquery-1.10.2.min.js\"></script> \r\n\
            <script src=\"runtime/resourcemanager.js\"></script> \r\n \
            <script src=\"runtime/utils.js\"></script>     \r\n\
            <script src=\"runtime/timelineanimator.js\"></script>    \r\n\
            <script src=\"runtime/player.js\"></script>     \r\n\
            \r\n\
            <script type=\"text/javascript\"> \r\n\
            \r\n\
            var loader = new createjs.LoadQueue(false); \r\n\
            loader.addEventListener(\"complete\", handleComplete); \r\n\
            loader.loadManifest(\"./images/B.png\"); \r\n\
            function handleComplete() \r\n\
                { \r\n\
                $(document).ready(function() { \r\n\
                \r\n\
                \r\n\
                    var canvas = document.getElementById(\"canvas\"); \r\n\
                    var stage = new createjs.Stage(canvas);         \r\n\
                    //pass FPS and use that in the player \r\n\
                    init(stage);             \r\n\
                }); \r\n\
                } \r\n\
            </script> \r\n\
        </head> \r\n\
        \r\n\
        <body> \r\n\
            <canvas id=\"canvas\" width=\"%d\" height=\"%d\" style=\"background-color:#%06X\"> \r\n\
                alternate content \r\n\
            </canvas> \r\n\
        </body> \r\n\
        </html>";


    /* -------------------------------------------------- JSONOutputWriter */

    FCM::Result JSONOutputWriter::StartOutput(std::string& outputFileName)
    {
        std::string parent;

        Utils::GetParent(outputFileName, parent);

        m_outputHTMLFile = outputFileName;
        m_outputJSONFile = parent + JSON_OUTPUT_FILE_NAME;
        m_outputImageFolder = parent + IMAGE_FOLDER;

        return FCM_SUCCESS;
    }


    FCM::Result JSONOutputWriter::EndOutput()
    {

        return FCM_SUCCESS;
    }


    FCM::Result JSONOutputWriter::StartDocument(
        const DOM::Utils::COLOR& background,
        FCM::U_Int32 stageHeight, 
        FCM::U_Int32 stageWidth)
    {
        FCM::U_Int32 backColor;

        m_HTMLOutput = new char[strlen(htmlOutput) + 50];
        if (m_HTMLOutput == NULL)
        {
            return FCM_MEM_NOT_AVAILABLE;
        }

        backColor = (background.red << 16) | (background.green << 8) | (background.blue);
        sprintf(m_HTMLOutput, htmlOutput, stageWidth, stageHeight, backColor);

        return FCM_SUCCESS;
    }


    FCM::Result JSONOutputWriter::EndDocument()
    {
        std::ofstream file;

        m_pRootNode->push_back(*m_pShapeArray);
        m_pRootNode->push_back(*m_pBitmapArray);
        m_pRootNode->push_back(*m_pTimelineArray);

        // Write the json file
        remove(m_outputJSONFile.c_str());

        JSONNode firstNode(JSON_NODE);
        firstNode.push_back(*m_pRootNode);
        file.open(m_outputJSONFile.c_str());
        file << firstNode.write_formatted();
        file.close();

        // Write the HTML file (Remove file if it already exists)
        remove(m_outputHTMLFile.c_str());

        file.open(m_outputHTMLFile.c_str());
        file << m_HTMLOutput;
        file.close();

        delete [] m_HTMLOutput;

        return FCM_SUCCESS;
    }


    FCM::Result JSONOutputWriter::StartDefineTimeline()
    {
        return FCM_SUCCESS;
    }


    FCM::Result JSONOutputWriter::EndDefineTimeline(
        FCM::U_Int32 resId, 
        FCM::StringRep16 pName,
        ITimelineWriter* pTimelineWriter)
    {
        JSONTimelineWriter* pWriter = static_cast<JSONTimelineWriter*> (pTimelineWriter);

        pWriter->Finish(resId, pName);

        m_pTimelineArray->push_back(*(pWriter->GetRoot()));

        return FCM_SUCCESS;
    }


    FCM::Result JSONOutputWriter::StartDefineShape()
    {
        m_shapeElem = new JSONNode(JSON_NODE);
        ASSERT(m_shapeElem);

        m_pathArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_pathArray);
        m_pathArray->set_name("path");

        return FCM_SUCCESS;
    }


    // Marks the end of a shape
    FCM::Result JSONOutputWriter::EndDefineShape(FCM::U_Int32 resId)
    {
        m_shapeElem->push_back(JSONNode(("charid"), OpenFL::Utils::ToString(resId)));
        m_shapeElem->push_back(*m_pathArray);

        m_pShapeArray->push_back(*m_shapeElem);

        delete m_pathArray;
        delete m_shapeElem;

        return FCM_SUCCESS;
    }


    // Start of fill region definition
    FCM::Result JSONOutputWriter::StartDefineFill()
    {
        m_pathElem = new JSONNode(JSON_NODE);
        ASSERT(m_pathElem);

        m_pathCmdStr.clear();

        return FCM_SUCCESS;
    }


    // Solid fill style definition
    FCM::Result JSONOutputWriter::DefineSolidFillStyle(const DOM::Utils::COLOR& color)
    {
        std::string colorStr = Utils::ToString(color);
        std::string colorOpacityStr = OpenFL::Utils::ToString((double)(color.alpha / 255.0));

        m_pathElem->push_back(JSONNode("color", colorStr.c_str()));
        m_pathElem->push_back(JSONNode("colorOpacity", colorOpacityStr.c_str()));

        return FCM_SUCCESS;
    }


    // Bitmap fill style definition
    FCM::Result JSONOutputWriter::DefineBitmapFillStyle(
        FCM::Boolean clipped,
        const DOM::Utils::MATRIX2D& matrix,
        FCM::S_Int32 height, 
        FCM::S_Int32 width,
        FCM::StringRep16 pName,
        DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        FCM::Result res;
        JSONNode bitmapElem(JSON_NODE);
        std::string bitmapPath;
        std::string bitmapName;

        bitmapElem.set_name("image");
        
        bitmapElem.push_back(JSONNode(("height"), OpenFL::Utils::ToString(height)));
        bitmapElem.push_back(JSONNode(("width"), OpenFL::Utils::ToString(width)));

        // Form the image path
        bitmapName = Utils::ToString(pName, m_pCallback);
        std::size_t pos = bitmapName.find(".");

        if (pos != std::string::npos)
        {
            FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
            std::string bitmapRelPath;
            std::string bitmapExportPath = m_outputImageFolder + "/";
            
            if ((bitmapName.substr(pos + 1) != "jpg") ||
                (bitmapName.substr(pos + 1) != "png"))
            {
                std::string bitmapNameWithoutExt;

                bitmapNameWithoutExt = bitmapName.substr(0, pos);
                bitmapName = bitmapNameWithoutExt + ".png";
            }
            
            bitmapRelPath = "./";
            bitmapRelPath += IMAGE_FOLDER;
            bitmapRelPath += "/" + bitmapName;

            res = m_pCallback->GetService(DOM::FLA_BITMAP_SERVICE, pUnk.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            FCM::AutoPtr<DOM::Service::Image::IBitmapExportService> bitmapExportService = pUnk;
            if (bitmapExportService)
            {
                res = bitmapExportService->ExportToFile(pMediaItem, Utils::ToString16(bitmapExportPath, m_pCallback), 100);
                ASSERT(FCM_SUCCESS_CODE(res));
            }

            bitmapElem.push_back(JSONNode(("bitmapPath"), bitmapRelPath)); 
        }
        else
        {
            // Should not reach here
            ASSERT(0);
        }

        DOM::Utils::MATRIX2D matrix1 = matrix;
        matrix1.a /= 20.0;
        matrix1.b /= 20.0;
        matrix1.c /= 20.0;
        matrix1.d /= 20.0;

        bitmapElem.push_back(JSONNode(("patternUnits"), "userSpaceOnUse"));
        bitmapElem.push_back(JSONNode(("patternTransform"), Utils::ToString(matrix1).c_str()));

        m_pathElem->push_back(bitmapElem);

        return FCM_SUCCESS;
    }


    // Start Linear Gradient fill style definition
    FCM::Result JSONOutputWriter::StartDefineLinearGradientFillStyle(
        DOM::FillStyle::GradientSpread spread,
        const DOM::Utils::MATRIX2D& matrix)
    {
        DOM::Utils::POINT2D point;

        m_gradientColor = new JSONNode(JSON_NODE);
        ASSERT(m_gradientColor);
        m_gradientColor->set_name("linearGradient");

        point.x = -GRADIENT_VECTOR_CONSTANT / 20;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point);

        m_gradientColor->push_back(JSONNode("x1", Utils::ToString((double) (point.x))));
        m_gradientColor->push_back(JSONNode("y1", Utils::ToString((double) (point.y))));

        point.x = GRADIENT_VECTOR_CONSTANT / 20;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point);

        m_gradientColor->push_back(JSONNode("x2", Utils::ToString((double) (point.x))));
        m_gradientColor->push_back(JSONNode("y2", Utils::ToString((double) (point.y))));

        m_gradientColor->push_back(JSONNode("spreadMethod", Utils::ToString(spread)));

        m_stopPointArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_stopPointArray);
        m_stopPointArray->set_name("stop");

        return FCM_SUCCESS;
    }


    // Sets a specific key point in a color ramp (for both radial and linear gradient)
    FCM::Result JSONOutputWriter::SetKeyColorPoint(
        const DOM::Utils::GRADIENT_COLOR_POINT& colorPoint)
    {
        JSONNode stopEntry(JSON_NODE);
        FCM::Float offset;
        
        offset = (float)((colorPoint.pos * 100) / 255.0);

        stopEntry.push_back(JSONNode("offset", Utils::ToString((double) offset)));
        stopEntry.push_back(JSONNode("stopColor", Utils::ToString(colorPoint.color)));
        stopEntry.push_back(JSONNode("stopOpacity", Utils::ToString((double)(colorPoint.color.alpha / 255.0))));

        m_stopPointArray->push_back(stopEntry);

        return FCM_SUCCESS;
    }


    // End Linear Gradient fill style definition
    FCM::Result JSONOutputWriter::EndDefineLinearGradientFillStyle()
    {
        m_gradientColor->push_back(*m_stopPointArray);
        m_pathElem->push_back(*m_gradientColor);

        delete m_stopPointArray;
        delete m_gradientColor;

        return FCM_SUCCESS;
    }


    // Start Radial Gradient fill style definition
    FCM::Result JSONOutputWriter::StartDefineRadialGradientFillStyle(
        DOM::FillStyle::GradientSpread spread,
        const DOM::Utils::MATRIX2D& matrix,
        FCM::S_Int32 focalPoint)
    {
        DOM::Utils::POINT2D point;
        DOM::Utils::POINT2D point1;
        DOM::Utils::POINT2D point2;

        m_gradientColor = new JSONNode(JSON_NODE);
        ASSERT(m_gradientColor);
        m_gradientColor->set_name("radialGradient");

        point.x = 0;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point1);

        point.x = GRADIENT_VECTOR_CONSTANT / 20;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point2);

        FCM::Float xd = point1.x - point2.x;
        FCM::Float yd = point1.y - point2.y;
        FCM::Float r = sqrt(xd * xd + yd * yd);

        FCM::Float angle = atan2(yd, xd);
        double focusPointRatio = focalPoint / 255.0;
        double fx = -r * focusPointRatio * cos(angle);
        double fy = -r * focusPointRatio * sin(angle);

        m_gradientColor->push_back(JSONNode("cx", "0"));
        m_gradientColor->push_back(JSONNode("cy", "0"));
        m_gradientColor->push_back(JSONNode("r", Utils::ToString((double) r)));
        m_gradientColor->push_back(JSONNode("fx", Utils::ToString((double) fx)));
        m_gradientColor->push_back(JSONNode("fy", Utils::ToString((double) fy)));

        FCM::Float scaleFactor = (GRADIENT_VECTOR_CONSTANT / 20) / r;
        DOM::Utils::MATRIX2D matrix1 = {};
        matrix1.a = matrix.a * scaleFactor;
        matrix1.b = matrix.b * scaleFactor;
        matrix1.c = matrix.c * scaleFactor;
        matrix1.d = matrix.d * scaleFactor;
        matrix1.tx = matrix.tx;
        matrix1.ty = matrix.ty;

        m_gradientColor->push_back(JSONNode("gradientTransform", Utils::ToString(matrix1)));
        m_gradientColor->push_back(JSONNode("spreadMethod", Utils::ToString(spread)));

        m_stopPointArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_stopPointArray);
        m_stopPointArray->set_name("stop");

        return FCM_SUCCESS;
    }


    // End Radial Gradient fill style definition
    FCM::Result JSONOutputWriter::EndDefineRadialGradientFillStyle()
    {
        m_gradientColor->push_back(*m_stopPointArray);
        m_pathElem->push_back(*m_gradientColor);

        delete m_stopPointArray;
        delete m_gradientColor;

        return FCM_SUCCESS;
    }


    // Start of fill region boundary
    FCM::Result JSONOutputWriter::StartDefineBoundary()
    {
        return StartDefinePath();
    }


    // Sets a segment of a path (Used for boundary, holes)
    FCM::Result JSONOutputWriter::SetSegment(const DOM::Utils::SEGMENT& segment)
    {
        if (m_firstSegment)
        {
            if (segment.segmentType == DOM::Utils::LINE_SEGMENT)
            {
                m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.line.endPoint1.x)));
                m_pathCmdStr.append(space);
                m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.line.endPoint1.y)));
                m_pathCmdStr.append(space);
            }
            else
            {
                m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.quadBezierCurve.anchor1.x)));
                m_pathCmdStr.append(space);
                m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.quadBezierCurve.anchor1.y)));
                m_pathCmdStr.append(space);
            }
            m_firstSegment = false;
        }

        if (segment.segmentType == DOM::Utils::LINE_SEGMENT)
        {
            m_pathCmdStr.append(lineTo);
            m_pathCmdStr.append(space);
            m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.line.endPoint2.x)));
            m_pathCmdStr.append(space);
            m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.line.endPoint2.y)));
            m_pathCmdStr.append(space);
        }
        else
        {
            m_pathCmdStr.append(bezierCurveTo);
            m_pathCmdStr.append(space);
            m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.quadBezierCurve.control.x)));
            m_pathCmdStr.append(space);
            m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.quadBezierCurve.control.y)));
            m_pathCmdStr.append(space);
            m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.quadBezierCurve.anchor2.x)));
            m_pathCmdStr.append(space);
            m_pathCmdStr.append(OpenFL::Utils::ToString((double)(segment.quadBezierCurve.anchor2.y)));
            m_pathCmdStr.append(space);
        }

        return FCM_SUCCESS;
    }


    // End of fill region boundary
    FCM::Result JSONOutputWriter::EndDefineBoundary()
    {
        return EndDefinePath();
    }


    // Start of fill region hole
    FCM::Result JSONOutputWriter::StartDefineHole()
    {
        return StartDefinePath();
    }


    // End of fill region hole
    FCM::Result JSONOutputWriter::EndDefineHole()
    {
        return EndDefinePath();
    }


    // Start of stroke group
    FCM::Result JSONOutputWriter::StartDefineStrokeGroup()
    {
        // No need to do anything
        return FCM_SUCCESS;
    }


    // Start solid stroke style definition
    FCM::Result JSONOutputWriter::StartDefineSolidStrokeStyle(
        FCM::Double thickness,
        const DOM::StrokeStyle::JOIN_STYLE& joinStyle,
        const DOM::StrokeStyle::CAP_STYLE& capStyle,
        DOM::Utils::ScaleType scaleType,
        FCM::Boolean strokeHinting)
    {
        m_strokeStyle.type = SOLID_STROKE_STYLE_TYPE;
        m_strokeStyle.solidStrokeStyle.capStyle = capStyle;
        m_strokeStyle.solidStrokeStyle.joinStyle = joinStyle;
        m_strokeStyle.solidStrokeStyle.thickness = thickness;
        m_strokeStyle.solidStrokeStyle.scaleType = scaleType;
        m_strokeStyle.solidStrokeStyle.strokeHinting = strokeHinting;

        return FCM_SUCCESS;
    }


    // End of solid stroke style
    FCM::Result JSONOutputWriter::EndDefineSolidStrokeStyle()
    {
        // No need to do anything
        return FCM_SUCCESS;
    }


    // Start of stroke 
    FCM::Result JSONOutputWriter::StartDefineStroke()
    {
        m_pathElem = new JSONNode(JSON_NODE);
        ASSERT(m_pathElem);

        m_pathCmdStr.clear();
        StartDefinePath();

        return FCM_SUCCESS;
    }


    // End of a stroke 
    FCM::Result JSONOutputWriter::EndDefineStroke()
    {
        m_pathElem->push_back(JSONNode("d", m_pathCmdStr));

        if (m_strokeStyle.type == SOLID_STROKE_STYLE_TYPE)
        {
            m_pathElem->push_back(JSONNode("strokewidth", OpenFL::Utils::ToString((double)m_strokeStyle.solidStrokeStyle.thickness).c_str()));
            m_pathElem->push_back(JSONNode("fill", "none"));
            m_pathElem->push_back(JSONNode("strokeLinecap", Utils::ToString(m_strokeStyle.solidStrokeStyle.capStyle.type).c_str()));
            m_pathElem->push_back(JSONNode("strokeLinejoin", Utils::ToString(m_strokeStyle.solidStrokeStyle.joinStyle.type).c_str()));

            if (m_strokeStyle.solidStrokeStyle.joinStyle.type == DOM::Utils::MITER_JOIN)
            {
                m_pathElem->push_back(JSONNode(
                    "stroke-miterlimit", 
                    OpenFL::Utils::ToString((double)m_strokeStyle.solidStrokeStyle.joinStyle.miterJoinProp.miterLimit).c_str()));
            }
            m_pathElem->push_back(JSONNode("pathType", "Stroke"));
        }
        m_pathArray->push_back(*m_pathElem);

        delete m_pathElem;

        m_pathElem = NULL;

        return FCM_SUCCESS;
    }


    // End of stroke group
    FCM::Result JSONOutputWriter::EndDefineStrokeGroup()
    {
        // No need to do anything
        return FCM_SUCCESS;
    }


    // End of fill style definition
    FCM::Result JSONOutputWriter::EndDefineFill()
    {
        m_pathElem->push_back(JSONNode("d", m_pathCmdStr));
        m_pathElem->push_back(JSONNode("pathType", JSON_TEXT("Fill")));
        m_pathElem->push_back(JSONNode("stroke", JSON_TEXT("none")));

        m_pathArray->push_back(*m_pathElem);

        delete m_pathElem;

        m_pathElem = NULL;
        
        return FCM_SUCCESS;
    }


    // Define a bitmap
    FCM::Result JSONOutputWriter::DefineBitmap(
        FCM::U_Int32 resId,
        FCM::S_Int32 height, 
        FCM::S_Int32 width,
        FCM::StringRep16 pName,
        DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        FCM::Result res;
        JSONNode bitmapElem(JSON_NODE);
        std::string bitmapPath;
        std::string bitmapName;

        bitmapElem.set_name("image");
        
        bitmapElem.push_back(JSONNode(("charid"), OpenFL::Utils::ToString(resId)));
        bitmapElem.push_back(JSONNode(("height"), OpenFL::Utils::ToString(height)));
        bitmapElem.push_back(JSONNode(("width"), OpenFL::Utils::ToString(width)));

        // Form the image path
        bitmapName = Utils::ToString(pName, m_pCallback);
        std::size_t pos = bitmapName.find(".");

        if (pos != std::string::npos)
        {
            FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
            std::string bitmapRelPath;
            std::string bitmapExportPath = m_outputImageFolder + "/";
            
            if ((bitmapName.substr(pos + 1) != "jpg") ||
                (bitmapName.substr(pos + 1) != "png"))
            {
                std::string bitmapNameWithoutExt;

                bitmapNameWithoutExt = bitmapName.substr(0, pos);
                bitmapName = bitmapNameWithoutExt + ".png";
            }

            bitmapExportPath += bitmapName;
            
            bitmapRelPath = "./";
            bitmapRelPath += IMAGE_FOLDER;
            bitmapRelPath += "/" + bitmapName;

            res = m_pCallback->GetService(DOM::FLA_BITMAP_SERVICE, pUnk.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            FCM::AutoPtr<DOM::Service::Image::IBitmapExportService> bitmapExportService = pUnk;
            if (bitmapExportService)
            {
                res = bitmapExportService->ExportToFile(pMediaItem, Utils::ToString16(bitmapExportPath, m_pCallback), 100);
                ASSERT(FCM_SUCCESS_CODE(res));
            }

            bitmapElem.push_back(JSONNode(("bitmapPath"), bitmapRelPath)); 
        }
        else
        {
            // Should not reach here
            ASSERT(0);
        }

        m_pBitmapArray->push_back(bitmapElem);

        return FCM_SUCCESS;
    }


    JSONOutputWriter::JSONOutputWriter(FCM::PIFCMCallback pCallback)
        : m_pCallback(pCallback),
          m_shapeElem(NULL),
          m_pathArray(NULL),
          m_pathElem(NULL),
          m_firstSegment(false),
          m_HTMLOutput(NULL)
    {
        m_pRootNode = new JSONNode(JSON_NODE);
        ASSERT(m_pRootNode);
        m_pRootNode->set_name("DOMDocument");

        m_pShapeArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_pShapeArray);
        m_pShapeArray->set_name("Shape");

        m_pTimelineArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_pTimelineArray);
        m_pTimelineArray->set_name("Timeline");

        m_pBitmapArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_pBitmapArray);
        m_pBitmapArray->set_name("Bitmaps");

        m_strokeStyle.type = INVALID_STROKE_STYLE_TYPE;
    }


    JSONOutputWriter::~JSONOutputWriter()
    {
        delete m_pBitmapArray;

        delete m_pTimelineArray;

        delete m_pShapeArray;

        delete m_pRootNode;
    }


    FCM::Result JSONOutputWriter::StartDefinePath()
    {
        m_pathCmdStr.append(moveTo);
        m_pathCmdStr.append(space);
        m_firstSegment = true;

        return FCM_SUCCESS;
    }

    FCM::Result JSONOutputWriter::EndDefinePath()
    {
        // No need to do anything
        return FCM_SUCCESS;
    }

    /* -------------------------------------------------- JSONTimelineWriter */

    FCM::Result JSONTimelineWriter::PlaceObject(
        FCM::U_Int32 resId,
        FCM::U_Int32 objectId,
        FCM::U_Int32 placeAfterObjectId,
        const DOM::Utils::MATRIX2D* pMatrix,
        FCM::PIFCMUnknown pUnknown /* = NULL*/)
    {
        JSONNode commandElement(JSON_NODE);

        commandElement.push_back(JSONNode("cmdType", "Place"));
        commandElement.push_back(JSONNode("charid", OpenFL::Utils::ToString(resId)));
        commandElement.push_back(JSONNode("objectId", OpenFL::Utils::ToString(objectId)));
        commandElement.push_back(JSONNode("placeAfter", OpenFL::Utils::ToString(placeAfterObjectId)));

        if (pMatrix)
        {
            commandElement.push_back(JSONNode("transformMatrix", Utils::ToString(*pMatrix).c_str()));
        }

        m_pCommandArray->push_back(commandElement);

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::RemoveObject(
        FCM::U_Int32 objectId)
    {
        JSONNode commandElement(JSON_NODE);

        commandElement.push_back(JSONNode("cmdType", "Remove"));
        commandElement.push_back(JSONNode("objectId", OpenFL::Utils::ToString(objectId)));

        m_pCommandArray->push_back(commandElement);

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::UpdateZOrder(
        FCM::U_Int32 objectId,
        FCM::U_Int32 placeAfterObjectId)
    {
        JSONNode commandElement(JSON_NODE);

        // Goutam: Commenting out the code for demo as fix is necessary in the Exporter service for it work properly.
#if 0
        commandElement.push_back(JSONNode("cmdType", "UpdateZOrder"));
        commandElement.push_back(JSONNode("objectId", std::to_string((FCM::U_Int64)(objectId)).c_str()));
        commandElement.push_back(JSONNode("placeAfter", std::to_string((FCM::U_Int64)(placeAfterObjectId)).c_str()));

        m_pCommandArray->push_back(commandElement);
#endif

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::UpdateBlendMode(
        FCM::U_Int32 objectId,
        DOM::FrameElement::BlendMode blendMode)
    {
        // Not tested yet

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::UpdateVisibility(
        FCM::U_Int32 objectId,
        FCM::Boolean visible)
    {
        JSONNode commandElement(JSON_NODE);

        commandElement.push_back(JSONNode("cmdType", "UpdateVisibility"));
        commandElement.push_back(JSONNode("objectId", OpenFL::Utils::ToString(objectId)));

        if (visible)
        {
            commandElement.push_back(JSONNode("visibility", "true"));
        }
        else
        {
            commandElement.push_back(JSONNode("visibility", "false"));
        }

        m_pCommandArray->push_back(commandElement);

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::AddGraphicFilter(
        FCM::U_Int32 objectId,
        FCM::PIFCMUnknown pFilter)
    {
        // Not tested yet

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::UpdateDisplayTransform(
        FCM::U_Int32 objectId,
        const DOM::Utils::MATRIX2D& matrix)
    {
        JSONNode commandElement(JSON_NODE);
        std::string transformMat;

        commandElement.push_back(JSONNode("cmdType", "Move"));
        commandElement.push_back(JSONNode("objectId", OpenFL::Utils::ToString(objectId)));
        transformMat = OpenFL::Utils::ToString(matrix);
        commandElement.push_back(JSONNode("transformMatrix", transformMat.c_str()));

        m_pCommandArray->push_back(commandElement);

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::UpdateColorTransform(
        FCM::U_Int32 objectId,
        const DOM::Utils::COLOR_MATRIX& colorMatrix)
    {
        // Not tested yet
        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::ShowFrame(FCM::U_Int32 frameNum)
    {
        JSONNode frameElement(JSON_NODE);

        frameElement.push_back(JSONNode(("num"), OpenFL::Utils::ToString(frameNum)));
        frameElement.push_back(*m_pCommandArray);
        m_pFrameArray->push_back(frameElement);

        delete m_pCommandArray;

        m_pCommandArray = new JSONNode(JSON_ARRAY);
        m_pCommandArray->set_name("Command");

        return FCM_SUCCESS;
    }


    FCM::Result JSONTimelineWriter::AddFrameScript(FCM::StringRep16 pFrameScript)
    {
        // Not tested yet

        return FCM_SUCCESS;
    }

    JSONTimelineWriter::JSONTimelineWriter()
    {
        m_pCommandArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_pCommandArray);
        m_pCommandArray->set_name("Command");

        m_pFrameArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_pFrameArray);
        m_pFrameArray->set_name("Frame");

        m_pTimelineElement = new JSONNode(JSON_NODE);
        ASSERT(m_pTimelineElement);
        m_pTimelineElement->set_name("Timeline");
    }


    JSONTimelineWriter::~JSONTimelineWriter()
    {
        delete m_pCommandArray;

        delete m_pFrameArray;

        delete m_pTimelineElement;
    }


    const JSONNode* JSONTimelineWriter::GetRoot()
    {
        return m_pTimelineElement;
    }


    void JSONTimelineWriter::Finish(FCM::U_Int32 resId, FCM::StringRep16 pName)
    {
        if (resId != 0)
        {
            m_pTimelineElement->push_back(
                JSONNode(("charid"), 
                OpenFL::Utils::ToString(resId)));
        }

        m_pTimelineElement->push_back(*m_pFrameArray);
    }

};
