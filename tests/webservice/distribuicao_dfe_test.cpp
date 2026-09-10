#include <gtest/gtest.h>
#include "zafra/webservice/distribuicao_dfe.hpp"

using namespace zafra::webservice;

TEST(DistribuicaoDFeTest, GenerateSoapRequest) {
    DistribuicaoDFeClient client;
    std::string soap = client.GenerateSoapRequest("35", "12345678000199", "000000000000123");

    // Verifica elementos chave do SOAP
    EXPECT_TRUE(soap.find("<soap12:Envelope") != std::string::npos);
    EXPECT_TRUE(soap.find("<soap12:Body>") != std::string::npos);

    // Verifica os dados injetados
    EXPECT_TRUE(soap.find("<cUFAutor>35</cUFAutor>") != std::string::npos);
    EXPECT_TRUE(soap.find("<CNPJ>12345678000199</CNPJ>") != std::string::npos);
    EXPECT_TRUE(soap.find("<NSU>000000000000123</NSU>") != std::string::npos);
}
