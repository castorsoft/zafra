#include <gtest/gtest.h>
#include "zafra/xml/xml_document.hpp"
#include "zafra/document/document_factory.hpp"

using namespace zafra::core;
using namespace zafra::xml;
using namespace zafra::document;

TEST(DocumentFactoryTest, DetectNFeFromRootTag) {
    XmlDocument doc;
    std::string xml = R"(<?xml version="1.0"?><NFe xmlns="http://www.portalfiscal.inf.br/nfe"><infNFe Id="NFe12345678901234567890123456789012345678901234" versao="4.00"></infNFe></NFe>)";
    ASSERT_TRUE(doc.LoadFromString(xml));

    auto root = doc.GetRoot();
    auto nfe_doc = DocumentFactory::DetectAndCreate(root.get());

    ASSERT_NE(nfe_doc, nullptr);
    EXPECT_EQ(nfe_doc->GetType(), DocumentType::NFe);
    EXPECT_EQ(nfe_doc->GetVersion(), "4.00");
    EXPECT_EQ(nfe_doc->GetId(), "NFe12345678901234567890123456789012345678901234");
}

TEST(DocumentFactoryTest, DetectNFeFromProcTag) {
    XmlDocument doc;
    // Quando autorizado, vem envelopado em <nfeProc>
    std::string xml = R"(<?xml version="1.0"?><nfeProc versao="4.00" xmlns="http://www.portalfiscal.inf.br/nfe"><NFe><infNFe Id="NFe1234" versao="4.00"></infNFe></NFe></nfeProc>)";
    ASSERT_TRUE(doc.LoadFromString(xml));

    auto root = doc.GetRoot();
    auto nfe_doc = DocumentFactory::DetectAndCreate(root.get());

    ASSERT_NE(nfe_doc, nullptr);
    EXPECT_EQ(nfe_doc->GetType(), DocumentType::NFe);
    EXPECT_EQ(nfe_doc->GetId(), "NFe1234");
}

TEST(DocumentFactoryTest, UnknownDocumentReturnsNull) {
    XmlDocument doc;
    std::string xml = "<UnknownDoc></UnknownDoc>";
    ASSERT_TRUE(doc.LoadFromString(xml));

    auto root = doc.GetRoot();
    auto unknown_doc = DocumentFactory::DetectAndCreate(root.get());

    EXPECT_EQ(unknown_doc, nullptr);
}
