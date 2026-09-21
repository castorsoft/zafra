#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace zafra {
namespace core {

class IXmlNode {
public:
    virtual ~IXmlNode() = default;

    virtual std::string GetName() const = 0;
    virtual std::string GetValue() const = 0;
    virtual std::unique_ptr<IXmlNode> GetChild(const std::string& name) const = 0;
    virtual std::vector<std::unique_ptr<IXmlNode>> GetChildren(const std::string& name) const = 0;
    virtual std::unique_ptr<IXmlNode> SelectSingleNode(const std::string& xpath) const = 0;
    virtual std::optional<std::string> GetAttribute(const std::string& name) const = 0;
};

class IXmlDocument {
public:
    virtual ~IXmlDocument() = default;

    virtual bool LoadFromString(const std::string& xml_content) = 0;
    virtual bool LoadFromFile(const std::string& file_path) = 0;
    virtual std::unique_ptr<IXmlNode> GetRoot() const = 0;
    virtual std::string ToString() const = 0;
};

} // namespace core
} // namespace zafra
