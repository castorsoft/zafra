#pragma once

#include "zafra/core/xml_interfaces.hpp"
#include <string>

namespace zafra {
namespace core {

enum class DocumentType {
    Unknown,
    NFe,
    NFCe,
    NFSeNacional,
    NFSeAbrasf,
    NFSeIpm
};

class IDocument {
public:
    virtual ~IDocument() = default;

    virtual DocumentType GetType() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetId() const = 0;

    // Inicializa o documento a partir de um parser XML genérico
    virtual bool Parse(const IXmlNode* root_node) = 0;
};

} // namespace core
} // namespace zafra
