#pragma once

#include "documents/nfe_document.hpp"
#include "zafra/core/document_interfaces.hpp"
#include "zafra/core/xml_interfaces.hpp"
#include <memory>
#include <string>

namespace zafra {
namespace document {

class DocumentFactory {
public:
    // Detecta o tipo de documento a partir de uma string XML e retorna a instância apropriada
    static std::unique_ptr<core::IDocument> DetectAndCreate(const std::string& xml_content);
};

} // namespace document
} // namespace zafra
