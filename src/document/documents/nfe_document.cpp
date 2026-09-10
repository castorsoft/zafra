#include "zafra/document/documents/nfe_document.hpp"

namespace zafra {
namespace document {

bool NFeDocument::Parse(const core::IXmlNode* root_node) {
    if (!root_node) return false;

    std::unique_ptr<core::IXmlNode> child_owner;
    const core::IXmlNode* nfe_node = nullptr;

    if (root_node->GetName() == "nfeProc") {
        child_owner = root_node->GetChild("NFe");
        if (child_owner) {
            nfe_node = child_owner.get();
        }
    } else if (root_node->GetName() == "NFe") {
        nfe_node = root_node;
    }

    if (!nfe_node) return false;

    auto infNFe = nfe_node->GetChild("infNFe");
    if (infNFe) {
        auto versao = infNFe->GetAttribute("versao");
        if (versao) {
            m_version = versao.value();
        }

        auto id = infNFe->GetAttribute("Id");
        if (id) {
            m_id = id.value();
        }
        return true;
    }

    return false;
}
}
}