#pragma once

#include "zafra/core/document_interfaces.hpp"

namespace zafra {
namespace document {

class NFSeNacionalDocument: public core::IDocument {
public:
    NFSeNacionalDocument() = default;

    std::string GetXML() const override { return m_xml; }
    void SetXML(const std::string& xml) override { m_xml = xml; }

    core::DocumentType GetType() const override { return core::DocumentType::NFSeNacional; }
    std::string GetVersion() const override { return m_version; }
    std::string GetId() const override { return m_id; }

    std::string getChaveAcesso() const override { return ""; }
    core::TipoOperacao getTipoOperacao() const override { return core::TipoOperacao::Entrada; }
    bool Parse(const core::IXmlNode* root_node) override { return false; }

private:
    std::string m_xml;
    std::string m_version;
    std::string m_id;
};

} // namespace document
} // namespace zafra