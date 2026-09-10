#pragma once

#include "zafra/core/document_interfaces.hpp"

namespace zafra {
namespace document {

class NFSeNacionalDocument: public core::IDocument {
public:
    NFSeNacionalDocument() = default;

    core::DocumentType GetType() const override { return core::DocumentType::NFSeNacional; }
    std::string GetVersion() const override { return m_version; }
    std::string GetId() const override { return m_id; }

private:
    std::string m_version;
    std::string m_id;
};

} // namespace document
} // namespace zafra