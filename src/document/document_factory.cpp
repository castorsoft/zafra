#include "zafra/document/document_factory.hpp"
#include "zafra/xml/xml_document.hpp"

namespace zafra {
namespace document {

std::unique_ptr<core::IDocument> DocumentFactory::DetectAndCreate(const std::string& xml_content) {
    xml::XmlDocument doc;
    if (!doc.LoadFromString(xml_content)) {
        return nullptr;
    }

    auto root = doc.GetRoot();
    if (!root) return nullptr;

    std::string name = root->GetName();

    if (name == "NFe" || name == "nfeProc") {
        auto document = std::make_unique<NFeDocument>();
        if (document->Parse(root.get())) {
            document->SetXML(xml_content);
            return document;
        }
    }

    return nullptr;
}

} // namespace document
} // namespace zafra
