#include "MyXmlParser.h"
#include "IFCMStringUtils.h"
#include "Utils.h"
#include "DocType.h"
#include <map>

namespace OpenFL
{
    typedef std::pair <std::string, std::string> Pair_Attrib;

    FeatureDocumentHandler::FeatureDocumentHandler(OpenFL::FeatureMatrix *pFeatureMat) : 
        DocumentHandler(), 
        m_pFeatureMat(pFeatureMat)
    {
    }


    void FeatureDocumentHandler::startElement(const XMLCh* const name, AttributeList& attrs)
    {
        char elementName[1024];

        XMLString::transcode(name, elementName, 1024 - 1);
        std::string elemName(elementName);
        std::map<std::string, std::string> attributes;
        
        for (unsigned int c = 0; c < attrs.getLength() ; c++) 
        {
            char attributeName[1024];
            char attributeValue [1024];
            XMLString::transcode(attrs.getName(c), attributeName, 1024 - 1);
            XMLString::transcode(attrs.getValue(c), attributeValue, 1024 - 1);
            std::string attribName(attributeName);
            std::string attribVal(attributeValue);

            attributes.insert(Pair_Attrib(attribName, attribVal));
        }
        m_pFeatureMat->StartElement(elemName, attributes);
    }
    
    void FeatureDocumentHandler::endElement(const XMLCh* const name)
    {
        char elementName[1024];
        XMLString::transcode(name, elementName, 1024 - 1);
        std::string elemName(elementName);
        m_pFeatureMat->EndElement(elemName);
    }

};
