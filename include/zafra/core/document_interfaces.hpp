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

enum class TipoOperacao {
    Unknown,
    Saida,
    Entrada
};

inline std::string to_string(TipoOperacao tipo) {
    switch (tipo) {
        case TipoOperacao::Saida:   return "1";
        case TipoOperacao::Entrada: return "0";
        default:                    return "";
    }
}

inline TipoOperacao ParseTipoOperacao(std::string str) {
    if (str == "1") return TipoOperacao::Saida;
    if (str == "0") return TipoOperacao::Entrada;
    if (str == "") return TipoOperacao::Unknown;
    throw std::invalid_argument("Valor inválido para TipoOperacao: " + str);
}

enum class RegimeTributario {
    Unknown,
    SimplesNacional,
    SimplesNacionalExcessoReceitaBruta,
    RegimeNormal,
    MEI
};

inline std::string to_string(RegimeTributario tipo) {
    switch (tipo) {
        case RegimeTributario::SimplesNacional: return "1";
        case RegimeTributario::SimplesNacionalExcessoReceitaBruta: return "2";
        case RegimeTributario::RegimeNormal: return "3";
        case RegimeTributario::MEI: return "4";
        default: return "";
    }
}

inline RegimeTributario ParseRegimeTributaro(std::string str) {
    if (str == "1") return RegimeTributario::SimplesNacional;
    if (str == "2") return RegimeTributario::SimplesNacionalExcessoReceitaBruta;
    if (str == "3") return RegimeTributario::RegimeNormal;
    if (str == "4") return RegimeTributario::MEI;
    if (str == "") return RegimeTributario::Unknown;
    throw std::invalid_argument("Valor inválido para RegimeTributario: " + str);
}

class IDocument {
public:
    virtual ~IDocument() = default;

    virtual DocumentType GetType() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetId() const = 0;

    virtual std::string getChaveAcesso() const = 0;
    virtual TipoOperacao getTipoOperacao() const = 0;

    // Inicializa o documento a partir de um parser XML genérico
    virtual bool Parse(const IXmlNode* root_node) = 0;
};

} // namespace core
} // namespace zafra
