#include "ScriptOutputWriter.h"

#include <cstring>
#include <cmath>
#include <fstream>
#include "FlashFCMPublicIDs.h"
#include "FCMPluginInterface.h"
#include "libjson.h"
#include "Utils.h"
#include "FrameElement/ISound.h"
#include "Service/Image/IBitmapExportService.h"
#include "Service/TextLayout/ITextLinesGeneratorService.h"
#include "Service/TextLayout/ITextLine.h"
#include "Service/Sound/ISoundExportService.h"
#include "GraphicFilter/IDropShadowFilter.h"
#include "GraphicFilter/IAdjustColorFilter.h"
#include "GraphicFilter/IBevelFilter.h"
#include "GraphicFilter/IBlurFilter.h"
#include "GraphicFilter/IGlowFilter.h"
#include "GraphicFilter/IGradientBevelFilter.h"
#include "GraphicFilter/IGradientGlowFilter.h"
#include "Utils/ILinearColorGradient.h"
//#include "xercesc/dom/DOMDocument.hpp"

/*

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
XERCES_CPP_NAMESPACE_USE
*/

namespace OpenFL
{

	DOMElement* OpenFLOutputWriter::currScriptNode;

	DOMElement* OpenFLOutputWriter::currFrameNode;

	DOMImplementation *OpenFLOutputWriter::implementation;

	XERCES_CPP_NAMESPACE::DOMDocument*        OpenFLOutputWriter::document;

	DOMElement* OpenFLOutputWriter::currSymbolNode;

	bool OpenFLOutputWriter::theNextFrameContainsScripts = false;

};