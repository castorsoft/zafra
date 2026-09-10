#include "zafra/xml/xml_document.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

namespace zafra {
namespace xml {

XmlNode::XmlNode(const tinyxml2::XMLElement* node) : m_node(node) {}

std::string XmlNode::GetName() const {
    if (!m_node) return "";
    const char* name = m_node->Name();
    return name ? name : "";
}

std::string XmlNode::GetValue() const {
    if (!m_node) return "";
    const char* text = m_node->GetText();
    return text ? text : "";
}

std::unique_ptr<core::IXmlNode> XmlNode::GetChild(const std::string& name) const {
    if (!m_node) return nullptr;
    const tinyxml2::XMLElement* child = m_node->FirstChildElement(name.c_str());
    if (!child) return nullptr;
    return std::make_unique<XmlNode>(child);
}

std::unique_ptr<core::IXmlNode> XmlNode::SelectSingleNode(const std::string& xpath) const {
    // tinyxml2 não suporta XPath nativamente.
    // Para buscas avançadas, será necessário integrar libxml2 ou percorrer manualmente.
    return nullptr;
}

std::optional<std::string> XmlNode::GetAttribute(const std::string& name) const {
    if (!m_node) return std::nullopt;
    const char* attr = m_node->Attribute(name.c_str());
    if (!attr) return std::nullopt;
    return std::string(attr);
}

bool XmlDocument::LoadFromString(const std::string& xml_content) {
    return m_doc.Parse(xml_content.c_str()) == tinyxml2::XML_SUCCESS;
}

bool XmlDocument::LoadFromFile(const std::string &file_path)
{
    std::ifstream arq(file_path);
    std::string xml_content = "";
    std::string line = "";

    while (std::getline(arq, line)) {
        xml_content.append(line);
    }

    arq.close();

    return m_doc.Parse(xml_content.c_str()) == tinyxml2::XML_SUCCESS;
}

std::unique_ptr<core::IXmlNode> XmlDocument::GetRoot() const
{
    const tinyxml2::XMLElement* root = m_doc.RootElement();
    if (!root) return nullptr;
    return std::make_unique<XmlNode>(root);
}

std::string XmlDocument::ToString() const {
    tinyxml2::XMLPrinter printer;
    m_doc.Print(&printer);
    return printer.CStr();
}

} // namespace xml
} // namespace zafra
