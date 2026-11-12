#include <gtest/gtest.h>
#include "zafra/xml/xml_document.hpp"
#include "zafra/document/document_factory.hpp"
#include "zafra/document/documents/nfe_document.hpp"

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

TEST(DocumentFactoryTest, NFeProductDescriptionRemovesLineBreaks) {
    std::string xml = R"(<?xml version="1.0"?>
<nfeProc versao="4.00" xmlns="http://www.portalfiscal.inf.br/nfe">
    <NFe>
        <infNFe Id="NFe1234" versao="4.00">
            <det nItem="1">
                <prod>
                    <cProd>1001</cProd>
                    <xProd>PRODUTO LINHA 1
LINHA 2
LINHA 3</xProd>
                </prod>
            </det>
        </infNFe>
    </NFe>
</nfeProc>)";
    auto doc = DocumentFactory::DetectAndCreate(xml);
    ASSERT_NE(doc, nullptr);
    auto nfe = dynamic_cast<NFeDocument*>(doc.get());
    ASSERT_NE(nfe, nullptr);
    auto det = nfe->GetDet();
    ASSERT_EQ(det.size(), 1);
    auto item = *det.begin();
    ASSERT_NE(item->GetProd(), nullptr);
    EXPECT_EQ(item->GetProd()->GetXProd(), "PRODUTO LINHA 1 LINHA 2 LINHA 3");
}

TEST(DocumentFactoryTest, NFeParseIBSCBS) {
    std::string xml = R"(<?xml version="1.0"?>
<nfeProc versao="4.00" xmlns="http://www.portalfiscal.inf.br/nfe">
    <NFe>
        <infNFe Id="NFe1234" versao="4.00">
            <det nItem="1">
                <prod>
                    <cProd>1001</cProd>
                    <xProd>PRODUTO TESTE</xProd>
                </prod>
                <imposto>
                    <IBSCBS>
                        <CST>000</CST>
                        <cClassTrib>000001</cClassTrib>
                        <indDoacao>1</indDoacao>
                        <gIBSCBS>
                            <vBC>1000.00</vBC>
                            <gIBSUF>
                                <pIBSUF>0.1000</pIBSUF>
                                <gDif>
                                    <pDif>10.00</pDif>
                                    <vDif>0.10</vDif>
                                </gDif>
                                <gDevTrib>
                                    <pDevTrib>5.00</pDevTrib>
                                    <vDevTrib>0.05</vDevTrib>
                                </gDevTrib>
                                <gRed>
                                    <pRedAliq>20.00</pRedAliq>
                                    <pAliqEfet>0.0800</pAliqEfet>
                                </gRed>
                                <vIBSUF>1.00</vIBSUF>
                            </gIBSUF>
                            <gIBSMun>
                                <pIBSMun>0.0500</pIBSMun>
                                <vIBSMun>0.50</vIBSMun>
                            </gIBSMun>
                            <vIBS>1.50</vIBS>
                            <gCBS>
                                <pCBS>0.9000</pCBS>
                                <vCBS>9.00</vCBS>
                            </gCBS>
                        </gIBSCBS>
                    </IBSCBS>
                </imposto>
            </det>
        </infNFe>
    </NFe>
</nfeProc>)";
    auto doc = DocumentFactory::DetectAndCreate(xml);
    ASSERT_NE(doc, nullptr);
    auto nfe = dynamic_cast<NFeDocument*>(doc.get());
    ASSERT_NE(nfe, nullptr);
    auto det = nfe->GetDet();
    ASSERT_EQ(det.size(), 1);
    auto item = *det.begin();
    ASSERT_NE(item->GetImposto(), nullptr);
    ASSERT_TRUE(item->GetImposto()->GetIBSCBS().has_value());

    auto ibscbs = item->GetImposto()->GetIBSCBS().value();
    ASSERT_NE(ibscbs, nullptr);
    EXPECT_EQ(ibscbs->GetCST(), "000");
    EXPECT_EQ(ibscbs->GetCClassTrib(), "000001");
    ASSERT_TRUE(ibscbs->GetIndDoacao().has_value());
    EXPECT_EQ(ibscbs->GetIndDoacao().value(), "1");
    ASSERT_TRUE(ibscbs->GetVBC().has_value());
    EXPECT_DOUBLE_EQ(ibscbs->GetVBC().value(), 1000.00);
    ASSERT_TRUE(ibscbs->GetVIBS().has_value());
    EXPECT_DOUBLE_EQ(ibscbs->GetVIBS().value(), 1.50);

    ASSERT_TRUE(ibscbs->GetIBSUF().has_value());
    auto ibsuf = ibscbs->GetIBSUF().value();
    EXPECT_DOUBLE_EQ(ibsuf->GetPAliq(), 0.1);
    EXPECT_DOUBLE_EQ(ibsuf->GetVTrib(), 1.0);
    EXPECT_DOUBLE_EQ(ibsuf->GetPDif().value_or(0.0), 10.0);
    EXPECT_DOUBLE_EQ(ibsuf->GetVDif().value_or(0.0), 0.10);
    EXPECT_DOUBLE_EQ(ibsuf->GetPDevTrib().value_or(0.0), 5.0);
    EXPECT_DOUBLE_EQ(ibsuf->GetVDevTrib().value_or(0.0), 0.05);
    EXPECT_DOUBLE_EQ(ibsuf->GetPRedAliq().value_or(0.0), 20.0);
    EXPECT_DOUBLE_EQ(ibsuf->GetPAliqEfet().value_or(0.0), 0.08);

    ASSERT_TRUE(ibscbs->GetIBSMun().has_value());
    auto ibsmun = ibscbs->GetIBSMun().value();
    EXPECT_DOUBLE_EQ(ibsmun->GetPAliq(), 0.05);
    EXPECT_DOUBLE_EQ(ibsmun->GetVTrib(), 0.50);
    EXPECT_FALSE(ibsmun->GetPDif().has_value());

    ASSERT_TRUE(ibscbs->GetCBS().has_value());
    auto cbs = ibscbs->GetCBS().value();
    EXPECT_DOUBLE_EQ(cbs->GetPAliq(), 0.90);
    EXPECT_DOUBLE_EQ(cbs->GetVTrib(), 9.00);
}
