#include <gtest/gtest.h>
#include "zafra/xml/xml_document.hpp"
#include "zafra/document/document_factory.hpp"

using namespace zafra::core;
using namespace zafra::xml;
using namespace zafra::document;

TEST(DocumentFactoryTest, DetectNFeFromRootTag) {
    std::string xml = R"(<?xml version="1.0"?><NFe xmlns="http://www.portalfiscal.inf.br/nfe"><infNFe Id="NFe12345678901234567890123456789012345678901234" versao="4.00"></infNFe></NFe>)";
    auto nfe_doc = DocumentFactory::DetectAndCreate(xml);

    ASSERT_NE(nfe_doc, nullptr);
    EXPECT_EQ(nfe_doc->GetType(), DocumentType::NFe);
    EXPECT_EQ(nfe_doc->GetVersion(), "4.00");
    EXPECT_EQ(nfe_doc->GetId(), "NFe12345678901234567890123456789012345678901234");
    EXPECT_EQ(nfe_doc->GetXML(), xml);
}

TEST(DocumentFactoryTest, DetectNFeFromProcTag) {
    // Quando autorizado, vem envelopado em <nfeProc>
    std::string xml = R"(<?xml version="1.0"?><nfeProc versao="4.00" xmlns="http://www.portalfiscal.inf.br/nfe"><NFe><infNFe Id="NFe1234" versao="4.00"></infNFe></NFe></nfeProc>)";
    auto nfe_doc = DocumentFactory::DetectAndCreate(xml);

    ASSERT_NE(nfe_doc, nullptr);
    EXPECT_EQ(nfe_doc->GetType(), DocumentType::NFe);
    EXPECT_EQ(nfe_doc->GetId(), "NFe1234");
    EXPECT_EQ(nfe_doc->GetXML(), xml);
}

TEST(DocumentFactoryTest, UnknownDocumentReturnsNull) {
    std::string xml = "<UnknownDoc></UnknownDoc>";
    auto unknown_doc = DocumentFactory::DetectAndCreate(xml);

    EXPECT_EQ(unknown_doc, nullptr);
}

TEST(DocumentFactoryTest, DetectAndCreateFromStringAndGetXML) {
    std::string xml = R"(<?xml version="1.0"?><nfeProc versao="4.00" xmlns="http://www.portalfiscal.inf.br/nfe"><NFe><infNFe Id="NFe9999" versao="4.00"></infNFe></NFe></nfeProc>)";
    auto nfe_doc = DocumentFactory::DetectAndCreate(xml);

    ASSERT_NE(nfe_doc, nullptr);
    EXPECT_EQ(nfe_doc->GetType(), DocumentType::NFe);
    EXPECT_EQ(nfe_doc->GetId(), "NFe9999");
    EXPECT_EQ(nfe_doc->GetXML(), xml);
    EXPECT_EQ(nfe_doc->getXML(), xml);

    // Test SetXML
    std::string new_xml = "<modified></modified>";
    nfe_doc->SetXML(new_xml);
    EXPECT_EQ(nfe_doc->GetXML(), new_xml);
}
