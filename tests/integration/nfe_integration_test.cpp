#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include "zafra/document/document_factory.hpp"
#include "zafra/document/documents/nfe_document.hpp"

using namespace zafra::core;
using namespace zafra::document;
namespace fs = std::filesystem;

class NFeIntegrationTest : public ::testing::TestWithParam<std::string> {
};

// Este teste roda para cada arquivo de teste que for encontrado
TEST_P(NFeIntegrationTest, LeXMLComSucessoSemSegfault) {
    std::string filePath = GetParam();

    std::ifstream file(filePath);
    ASSERT_TRUE(file.is_open()) << "Falha ao abrir o arquivo: " << filePath;
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlContent = buffer.str();

    auto nfe_doc = DocumentFactory::DetectAndCreate(xmlContent);

    // Como a factory só deve falhar se o tipo do documento não for NFe/nfeProc ou faltarem tags muito vitais,
    // garantimos que conseguimos criar a instância.
    ASSERT_NE(nfe_doc, nullptr) << "Falha ao processar ou identificar NFe em: " << filePath;
    EXPECT_EQ(nfe_doc->GetType(), DocumentType::NFe);

    // Podemos fazer cast para acessar atributos específicos e ter certeza de que parsearam
    auto nfe = dynamic_cast<NFeDocument*>(nfe_doc.get());
    ASSERT_NE(nfe, nullptr);

    // Validar se dados chave estão presentes, de forma genérica
    EXPECT_FALSE(nfe->GetId().empty()) << "A NFe não extraiu o Id corretamente.";
    EXPECT_FALSE(nfe->getChaveAcesso().empty()) << "A NFe não extraiu a Chave de Acesso corretamente.";
    EXPECT_FALSE(nfe->GetVersion().empty()) << "A NFe não extraiu a versão corretamente.";
    EXPECT_FALSE(nfe->GetXML().empty()) << "A NFe não retornou o XML vinculado.";
    EXPECT_EQ(nfe->GetXML(), nfe->getXML());

    // Verifica dados do Emitente (opcional, pode depender se todos os XMLs tem emit)
    auto emit = nfe->GetEmit();
    if (emit) {
        EXPECT_FALSE(emit->GetCnpjCpf().empty()) << "CNPJ/CPF do Emitente está vazio.";
    }

    // Verifica dados de Identificação da NF-e (ide)
    auto ide = nfe->GetIde();
    if (ide) {
        EXPECT_GT(ide->GetDhEmissao(), 0) << "Data de emissão (dhEmi) não foi parseada corretamente.";
    }
}

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "tests/data"
#endif

// Uma função para listar todos os arquivos .xml numa pasta de testes
std::vector<std::string> GetXmlTestFiles() {
    std::vector<std::string> files;
    // Pasta onde os XMLs devem ser colocados
    std::string test_data_dir = TEST_DATA_DIR;

    if (fs::exists(test_data_dir) && fs::is_directory(test_data_dir)) {
        for (const auto& entry : fs::directory_iterator(test_data_dir)) {
            if (entry.path().extension() == ".xml") {
                files.push_back(entry.path().string());
            }
        }
    }

    return files;
}

// Permite que o GTest não falhe caso a pasta esteja vazia
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(NFeIntegrationTest);

// Registra os testes de acordo com a pasta
INSTANTIATE_TEST_SUITE_P(
    NFeFiles,
    NFeIntegrationTest,
    ::testing::ValuesIn(GetXmlTestFiles())
);
