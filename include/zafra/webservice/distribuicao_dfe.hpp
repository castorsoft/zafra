#pragma once

#include "zafra/core/webservice_interfaces.hpp"
#include <string>

namespace zafra {
namespace webservice {

class DistribuicaoDFeClient : public core::IWebServiceClient {
public:
    void SetHttpClient(core::IHttpClient* client) override;

    // Gera o envelope SOAP para a requisição consNSU
    std::string GenerateSoapRequest(const std::string& uf, const std::string& cnpj, const std::string& nsu);

    // Método que enviaria o request e trataria a resposta (mockado para este teste)
    // core::HttpResponse Consultar(const std::string& uf, const std::string& cnpj, const std::string& nsu);

private:
    core::IHttpClient* m_http_client = nullptr;
};

} // namespace webservice
} // namespace zafra
