#pragma once

#include "zafra/core/document_interfaces.hpp"

namespace zafra {
namespace document {

class NFeDocument : public core::IDocument {
public:
    NFeDocument() = default;

    core::DocumentType GetType() const override { return core::DocumentType::NFe; }
    std::string GetVersion() const override { return m_version; }
    std::string GetId() const override { return m_id; }

    bool Parse(const core::IXmlNode* root_node) override;

private:
    std::string m_version;
    std::string m_id;
};

} // namespace document
} // namespace zafra
