#ifndef ScriptOutputWriter_h__
#define ScriptOutputWriter_h__



#include "IOutputWriter.h"
#include <string>
#include "Publisher.h"
#include "Utils.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"

using namespace std;

XERCES_CPP_NAMESPACE_USE

namespace OpenFL {

	class OpenFLOutputWriter : public IOutputWriter
	{
	public:

		static DOMElement* currSymbolNode;

		static DOMElement* currScriptNode;

		static DOMElement* currFrameNode;

		static DOMImplementation *implementation;

		static XERCES_CPP_NAMESPACE::DOMDocument*        document;

		static bool theNextFrameContainsScripts;

		// Marks the begining of the output
		FCM::Result StartOutput(std::string& outputFileName) {
			string exportDir;
			Utils::GetParent(outputFileName, exportDir);

			XMLPlatformUtils::Initialize();
			implementation = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));
			document = implementation->createDocument(0, L"symbols", 0);
			currSymbolNode = document->createElement(L"symbol");
			return FCM_SUCCESS;
		}

		// Marks the end of the output
		FCM::Result EndOutput() {
			return FCM_SUCCESS;
		}

		// Marks the begining of the Document
		FCM::Result StartDocument(
			const DOM::Utils::COLOR& background,
			FCM::U_Int32 stageHeight,
			FCM::U_Int32 stageWidth,
			FCM::U_Int32 fps) {
			return FCM_SUCCESS;
		}

		// Marks the end of the Document
		FCM::Result EndDocument() {
			XMLFormatTarget *target = new LocalFileFormatTarget("C:/Users/Marko/Desktop/ExportTest/scripts.xml");
			DOMLSSerializer *writer = implementation->createLSSerializer();
			DOMConfiguration *config = writer->getDomConfig();
			config->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
			DOMLSOutput *output = implementation->createLSOutput();

			output->setByteStream(target);
			writer->write(document, output);
			output->release();
			output = nullptr;
			writer->release();
			writer = nullptr;
			document->release();
			delete target;
			implementation = nullptr;
			return FCM_SUCCESS;
		}

		// Marks the start of a timeline
		FCM::Result StartDefineTimeline() {
			return FCM_SUCCESS;
		}

		// Marks the end of a timeline
		FCM::Result EndDefineTimeline(
			FCM::U_Int32 resId,
			FCM::StringRep16 pName,
			ITimelineWriter* pTimelineWriter) {
			string idString = Utils::ToString(resId);
			currSymbolNode->setAttribute(L"id", XMLString::transcode(idString.c_str()));
			currSymbolNode = OpenFLOutputWriter::document->createElement(L"symbol");
			return FCM_SUCCESS;
		}

		// Marks the start of a shape
		FCM::Result StartDefineShape() {
			return FCM_SUCCESS;
		}

		// Start of fill region definition
		FCM::Result StartDefineFill() {
			return FCM_SUCCESS;
		}

		// Solid fill style definition
		FCM::Result DefineSolidFillStyle(const DOM::Utils::COLOR& color) {
			return FCM_SUCCESS;
		}

		// Bitmap fill style definition
		FCM::Result DefineBitmapFillStyle(
			FCM::Boolean clipped,
			const DOM::Utils::MATRIX2D& matrix,
			FCM::S_Int32 height,
			FCM::S_Int32 width,
			std::string& name,
			DOM::LibraryItem::PIMediaItem pMediaItem) {
			return FCM_SUCCESS;
		}

		// Start Linear Gradient fill style definition
		FCM::Result StartDefineLinearGradientFillStyle(
			DOM::FillStyle::GradientSpread spread,
			const DOM::Utils::MATRIX2D& matrix) {
			return FCM_SUCCESS;
		}

		// Sets a specific key point in a color ramp (for both radial and linear gradient)
		FCM::Result SetKeyColorPoint(
			const DOM::Utils::GRADIENT_COLOR_POINT& colorPoint) {
			return FCM_SUCCESS;
		}

		// End Linear Gradient fill style definition
		FCM::Result EndDefineLinearGradientFillStyle() {
			return FCM_SUCCESS;
		}

		// Start Radial Gradient fill style definition
		FCM::Result StartDefineRadialGradientFillStyle(
			DOM::FillStyle::GradientSpread spread,
			const DOM::Utils::MATRIX2D& matrix,
			FCM::S_Int32 focalPoint) {
			return FCM_SUCCESS;
		}

		// End Radial Gradient fill style definition
		FCM::Result EndDefineRadialGradientFillStyle() {
			return FCM_SUCCESS;
		}

		// Start of fill region boundary
		FCM::Result StartDefineBoundary() {
			return FCM_SUCCESS;
		}

		// Sets a segment of a path (Used for boundary, holes)
		FCM::Result SetSegment(const DOM::Utils::SEGMENT& segment) {
			return FCM_SUCCESS;
		}

		// End of fill region boundary
		FCM::Result EndDefineBoundary() {
			return FCM_SUCCESS;
		}

		// Start of fill region hole
		FCM::Result StartDefineHole() {
			return FCM_SUCCESS;
		}

		// End of fill region hole
		FCM::Result EndDefineHole() {
			return FCM_SUCCESS;
		}

		// Start of stroke group
		FCM::Result StartDefineStrokeGroup() {
			return FCM_SUCCESS;
		}

		// Start solid stroke style definition
		FCM::Result StartDefineSolidStrokeStyle(
			FCM::Double thickness,
			const DOM::StrokeStyle::JOIN_STYLE& joinStyle,
			const DOM::StrokeStyle::CAP_STYLE& capStyle,
			DOM::Utils::ScaleType scaleType,
			FCM::Boolean strokeHinting) {
			return FCM_SUCCESS;
		}

		// End of solid stroke style
		FCM::Result EndDefineSolidStrokeStyle() {
			return FCM_SUCCESS;
		}

		// Start of stroke 
		FCM::Result StartDefineStroke() {
			return FCM_SUCCESS;
		}

		// End of a stroke 
		FCM::Result EndDefineStroke() {
			return FCM_SUCCESS;
		}

		// End of stroke group
		FCM::Result EndDefineStrokeGroup() {
			return FCM_SUCCESS;
		}

		// End of fill style definition
		FCM::Result EndDefineFill() {
			return FCM_SUCCESS;
		}

		// Marks the end of a shape
		FCM::Result EndDefineShape(FCM::U_Int32 resId) {
			return FCM_SUCCESS;
		}

		// Define a bitmap
		FCM::Result DefineBitmap(
			FCM::U_Int32 resId,
			FCM::S_Int32 height,
			FCM::S_Int32 width,
			const std::string& name,
			DOM::LibraryItem::PIMediaItem pMediaItem) {
			return FCM_SUCCESS;
		}

		// Define text
		FCM::Result DefineText(
			FCM::U_Int32 resId,
			const std::string& name,
			const DOM::Utils::COLOR& color,
			const std::string& displayText,
			DOM::FrameElement::PIClassicText pTextItem) {
			return FCM_SUCCESS;
		}

		FCM::Result DefineSound(
			FCM::U_Int32 resId,
			const std::string& name,
			DOM::LibraryItem::PIMediaItem pMediaItem) {
			return FCM_SUCCESS;
		}
		OpenFLOutputWriter(FCM::PIFCMCallback pCallback) : m_pCallback(pCallback) {

		}

		~OpenFLOutputWriter() {

		}

		// Start of a path
		FCM::Result StartDefinePath() {
			return FCM_SUCCESS;
		}

		// End of a path 
		FCM::Result EndDefinePath() {
			return FCM_SUCCESS;
		}

	private:

		FCM::PIFCMCallback m_pCallback;
		
	};


	class OpenFLTimelineWriter : public ITimelineWriter
	{
	public:

		FCM::Result PlaceObject(
			FCM::U_Int32 resId,
			FCM::U_Int32 objectId,
			FCM::U_Int32 placeAfterObjectId,
			const DOM::Utils::MATRIX2D* pMatrix,
			FCM::PIFCMUnknown pUnknown = NULL) {

			return FCM_SUCCESS;
		}
		FCM::Result PlaceObject(
			FCM::U_Int32 resId,
			FCM::U_Int32 objectId,
			FCM::PIFCMUnknown pUnknown = NULL) {

			return FCM_SUCCESS;
		}

		FCM::Result RemoveObject(
			FCM::U_Int32 objectId) {

			return FCM_SUCCESS;
		}

		FCM::Result UpdateZOrder(
			FCM::U_Int32 objectId,
			FCM::U_Int32 placeAfterObjectId) {

			return FCM_SUCCESS;
		}

		FCM::Result UpdateBlendMode(
			FCM::U_Int32 objectId,
			DOM::FrameElement::BlendMode blendMode) {

			return FCM_SUCCESS;
		}

		FCM::Result UpdateVisibility(
			FCM::U_Int32 objectId,
			FCM::Boolean visible) {

			return FCM_SUCCESS;
		}

		FCM::Result AddGraphicFilter(
			FCM::U_Int32 objectId,
			FCM::PIFCMUnknown pFilter) {

			return FCM_SUCCESS;
		}

		FCM::Result UpdateDisplayTransform(
			FCM::U_Int32 objectId,
			const DOM::Utils::MATRIX2D& matrix) {

			return FCM_SUCCESS;
		}

		FCM::Result UpdateColorTransform(
			FCM::U_Int32 objectId,
			const DOM::Utils::COLOR_MATRIX& colorMatrix) {

			return FCM_SUCCESS;
		}

		FCM::Result ShowFrame(FCM::U_Int32 frameNum) {
			if (OpenFLOutputWriter::theNextFrameContainsScripts == true) {
				OpenFLOutputWriter::currFrameNode->setAttribute(L"num",
					XMLString::transcode(to_string(frameNum).c_str()));
				OpenFLOutputWriter::currSymbolNode->appendChild(OpenFLOutputWriter::currFrameNode);
				OpenFLOutputWriter::theNextFrameContainsScripts = false;
			}
			return FCM_SUCCESS;
		}

		FCM::Result AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum) {
			std::string script = Utils::ToString(pScript, m_pCallback);

			if (OpenFLOutputWriter::theNextFrameContainsScripts == false) {
				OpenFLOutputWriter::currFrameNode = OpenFLOutputWriter::document->createElement(L"frame");
				OpenFLOutputWriter::theNextFrameContainsScripts = true;
			}

			OpenFLOutputWriter::document->getDocumentElement()->appendChild(OpenFLOutputWriter::currSymbolNode);

			DOMElement *scriptElem = OpenFLOutputWriter::document->createElement(L"script");
			scriptElem->setAttribute(L"layer", XMLString::transcode(to_string(layerNum).c_str()));
			DOMCDATASection *cdataNode = OpenFLOutputWriter::document->createCDATASection(
				XMLString::transcode(script.c_str()));

			scriptElem->appendChild(cdataNode);
			OpenFLOutputWriter::currFrameNode->appendChild(scriptElem);

			return FCM_SUCCESS;
		}

		FCM::Result RemoveFrameScript(FCM::U_Int32 layerNum) {

			return FCM_SUCCESS;
		}

		FCM::Result SetFrameLabel(FCM::StringRep16 pLabel, DOM::KeyFrameLabelType labelType) {

			return FCM_SUCCESS;
		}

		OpenFLTimelineWriter(FCM::PIFCMCallback pCallback) : m_pCallback(pCallback) {


		}

		~OpenFLTimelineWriter() {


		}

		void Finish(FCM::U_Int32 resId, FCM::StringRep16 pName) {

		}

	private:

		FCM::PIFCMCallback m_pCallback;

	};

};
#endif // ScriptOutputWriter_h__