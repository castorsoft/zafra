#include "zafra/webservice/distribuicao_dfe.hpp"
#include <sstream>

namespace zafra {
namespace webservice {

void DistribuicaoDFeClient::SetHttpClient(core::IHttpClient* client) {
    m_http_client = client;
}

std::string DistribuicaoDFeClient::GenerateSoapRequest(const std::string& uf, const std::string& cnpj, const std::string& nsu) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    ss << "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ";
    ss << "xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">\n";
    ss << "  <soap12:Body>\n";
    ss << "    <nfeDistDFeInteresse xmlns=\"http://www.portalfiscal.inf.br/nfe/wsdl/NFeDistribuicaoDFe\">\n";
    ss << "      <nfeDadosMsg>\n";
    ss << "        <distDFeInt versao=\"1.01\" xmlns=\"http://www.portalfiscal.inf.br/nfe\">\n";
    ss << "          <tpAmb>1</tpAmb>\n";
    ss << "          <cUFAutor>" << uf << "</cUFAutor>\n";
    ss << "          <CNPJ>" << cnpj << "</CNPJ>\n";
    ss << "          <consNSU><NSU>" << nsu << "</NSU></consNSU>\n";
    ss << "        </distDFeInt>\n";
    ss << "      </nfeDadosMsg>\n";
    ss << "    </nfeDistDFeInteresse>\n";
    ss << "  </soap12:Body>\n";
    ss << "</soap12:Envelope>";
    return ss.str();
}

} // namespace webservice
} // namespace zafra
