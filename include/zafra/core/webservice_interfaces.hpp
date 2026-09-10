#pragma once

#include <string>
#include <map>

namespace zafra {
namespace core {

struct HttpResponse {
    int status_code;
    std::string body;
};

class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual HttpResponse Post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers) = 0;
};

class IWebServiceClient {
public:
    virtual ~IWebServiceClient() = default;

    // Configura o cliente HTTP que será usado (Dependency Injection)
    virtual void SetHttpClient(IHttpClient* client) = 0;
};

} // namespace core
} // namespace zafra
