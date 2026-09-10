#include <gtest/gtest.h>
#include "zafra/xml/xml_document.hpp"

using namespace zafra::xml;

TEST(XmlDocumentTest, LoadValidXmlFromString) {
    XmlDocument doc;
    std::string xml = R"(<?xml version="1.0"?><NFe><infNFe Id="123"><emit><CNPJ>12345678000199</CNPJ></emit></infNFe></NFe>)";

    EXPECT_TRUE(doc.LoadFromString(xml));

    auto root = doc.GetRoot();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->GetName(), "NFe");
}

TEST(XmlDocumentTest, ParseChildrenAndValues) {
    XmlDocument doc;
    std::string xml = "<NFe><infNFe><emit><CNPJ>12345678000199</CNPJ></emit></infNFe></NFe>";
    ASSERT_TRUE(doc.LoadFromString(xml));

    auto root = doc.GetRoot();
    auto infNFe = root->GetChild("infNFe");
    ASSERT_NE(infNFe, nullptr);

    auto emit = infNFe->GetChild("emit");
    ASSERT_NE(emit, nullptr);

    auto cnpj = emit->GetChild("CNPJ");
    ASSERT_NE(cnpj, nullptr);
    EXPECT_EQ(cnpj->GetValue(), "12345678000199");
}

TEST(XmlDocumentTest, ReadAttribute) {
    XmlDocument doc;
    std::string xml = R"(<infNFe Id="NFe123456"></infNFe>)";
    ASSERT_TRUE(doc.LoadFromString(xml));

    auto root = doc.GetRoot();
    auto id_attr = root->GetAttribute("Id");

    ASSERT_TRUE(id_attr.has_value());
    EXPECT_EQ(id_attr.value(), "NFe123456");
}
