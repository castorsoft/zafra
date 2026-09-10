#include "zafra/document/document_factory.hpp"

namespace zafra {
namespace document {

std::unique_ptr<core::IDocument> DocumentFactory::DetectAndCreate(const core::IXmlNode* root_node) {
    if (!root_node) return nullptr;

    std::string name = root_node->GetName();

    if (name == "NFe" || name == "nfeProc") {
        auto doc = std::make_unique<NFeDocument>();
        if (doc->Parse(root_node)) {
            return doc;
        }
    }

    return nullptr;
}

} // namespace document
} // namespace zafra
