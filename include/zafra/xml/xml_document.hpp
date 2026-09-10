#pragma once

#include "zafra/core/xml_interfaces.hpp"
#include <tinyxml2.h>
#include <memory>

namespace zafra {
namespace xml {

class XmlNode : public core::IXmlNode {
public:
    explicit XmlNode(const tinyxml2::XMLElement* node);

    std::string GetName() const override;
    std::string GetValue() const override;
    std::unique_ptr<core::IXmlNode> GetChild(const std::string& name) const override;
    std::unique_ptr<core::IXmlNode> SelectSingleNode(const std::string& xpath) const override;
    std::optional<std::string> GetAttribute(const std::string& name) const override;

private:
    const tinyxml2::XMLElement* m_node;
};

class XmlDocument : public core::IXmlDocument {
public:
    XmlDocument() = default;
    ~XmlDocument() override = default;

    bool LoadFromString(const std::string& xml_content) override;
    bool LoadFromFile(const std::string& file_path) override;
    std::unique_ptr<core::IXmlNode> GetRoot() const override;
    std::string ToString() const override;

private:
    tinyxml2::XMLDocument m_doc;
};

} // namespace xml
} // namespace zafra
