/*
 * @file  MyXMLParser.h
 *
 * @brief This file contains declarations for a sample .
 */


#ifndef MY_XML_PARSER_H_
#define MY_XML_PARSER_H_

#include "xercesc/parsers/SAXParser.hpp"
#include "xercesc/sax/DocumentHandler.hpp"

/* -------------------------------------------------- Forward Decl */

using namespace xercesc;

namespace OpenFL
{
    class FeatureMatrix;
}


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace OpenFL
{
    class FeatureDocumentHandler : public DocumentHandler 
    {
    public:
        FeatureDocumentHandler(OpenFL::FeatureMatrix *pFeatureMat);
        
        virtual void characters(const XMLCh* const chars, const XMLSize_t length) {}
        virtual void startDocument() {}
        virtual void resetDocument() {}
        virtual void endDocument() {}
        virtual void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {}
        virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
        virtual void setDocumentLocator(const Locator* const locator) {}
        virtual void startElement(const XMLCh* const name, AttributeList& attrs);
        virtual void endElement(const XMLCh* const name);

    private:
        OpenFL::FeatureMatrix *m_pFeatureMat;
    };
}

#endif // MY_XML_PARSER_H_

