#pragma once

#include "zafra/core/xml_interfaces.hpp"
#include <string>
#include <string_view>
#include <stdexcept>

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

enum class OperationType {
    Unknown,
    Saida,
    Entrada
};

// Converte OperationType para std::string (constexpr para maior performance)
inline std::string to_string(OperationType type) {
    switch (type) {
        case OperationType::Saida:   return "1";
        case OperationType::Entrada: return "0";
        default:                     return "";
    }
}

// Converte de std::string (ou string_view) para OperationType
inline OperationType ParseOperationType(std::string_view str) {
    if (str == "1") return OperationType::Saida;
    if (str == "0") return OperationType::Entrada;
    if (str == "") return OperationType::Unknown;
    throw std::invalid_argument("Valor inválido para OperationType: " + std::string(str));
}

class IDocument {
public:
    virtual ~IDocument() = default;

    virtual DocumentType GetType() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetId() const = 0;
    virtual std::string getChaveAcesso() const = 0;
    virtual OperationType getTipoOperacao() const = 0;

    // Inicializa o documento a partir de um parser XML genérico
    virtual bool Parse(const IXmlNode* root_node) = 0;
};

class IDocumentParticipant {
public:
    virtual ~IDocumentParticipant() = default;

}

} // namespace core
} // namespace zafra
