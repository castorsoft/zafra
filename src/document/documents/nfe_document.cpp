#include "zafra/document/documents/nfe_document.hpp"
#include <sstream>
#include <locale>
#include <ctime>
#include <cstdio>

namespace {
    double ParseDouble(const std::string& str) {
        if (str.empty()) return 0.0;
        std::istringstream iss(str);
        iss.imbue(std::locale::classic());
        double val = 0.0;
        iss >> val;
        return val;
    }

    time_t ParseIso8601(const std::string& str) {
        if (str.empty()) return 0;

        int year = 0, month = 0, day = 0;
        int hour = 0, min = 0, sec = 0;
        char tz_sign = '\0';
        int tz_hour = 0, tz_min = 0;

        // Formato padrão SEFAZ: "AAAA-MM-DDThh:mm:ssTZD" (ex: "2026-07-30T17:42:23-03:00")
        int matched = std::sscanf(str.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d%c%2d:%2d",
                                  &year, &month, &day, &hour, &min, &sec,
                                  &tz_sign, &tz_hour, &tz_min);

        if (matched < 3) {
            // Suporta formatos mais antigos de NF-e apenas com data: "AAAA-MM-DD"
            matched = std::sscanf(str.c_str(), "%4d-%2d-%2d", &year, &month, &day);
            if (matched < 3) return 0;
        }

        struct tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = min;
        tm.tm_sec = sec;
        tm.tm_isdst = 0;

#if defined(_WIN32)
        time_t t = _mkgmtime(&tm);
#else
        time_t t = timegm(&tm);
#endif

        if (t == static_cast<time_t>(-1)) return 0;

        // Ajusta o offset para UTC (Local = UTC + Offset => UTC = Local - Offset)
        if (tz_sign == '-') {
            t += (tz_hour * 3600) + (tz_min * 60);
        } else if (tz_sign == '+') {
            t -= (tz_hour * 3600) + (tz_min * 60);
        }

        return t;
    }

    std::string RemoveLineBreaks(const std::string& str, bool replaceWithSpace = true) {
        std::string result;
        result.reserve(str.size());
        for (char c : str) {
            if (c == '\r') continue;
            if (c == '\n') {
                if (replaceWithSpace && !result.empty() && result.back() != ' ') {
                    result += ' ';
                }
            } else {
                result += c;
            }
        }
        while (!result.empty() && result.back() == ' ') {
            result.pop_back();
        }
        return result;
    }
}

namespace zafra {
namespace document {

NFeDocument::~NFeDocument() {
    if (m_ide) delete m_ide;
    if (m_emit) delete m_emit;
    if (m_dest) delete m_dest;
    for (auto d : m_det) delete d;
}

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

    if (m_xml.empty()) {
        m_xml = root_node->ToString();
    }

    auto infNFe = nfe_node->GetChild("infNFe");
    if (infNFe) {
        auto versao = infNFe->GetAttribute("versao");
        if (versao) {
            m_version = versao.value();
        }

        auto id = infNFe->GetAttribute("Id");
        if (id) {
            m_id = id.value();
            m_chaveAcesso = m_id;
            m_chaveAcesso = m_chaveAcesso.replace(m_id.find("NFe"), 3, "");
        }

        auto ide = infNFe->GetChild("ide");
        if (ide) {
            m_tipoOperacao = zafra::core::ParseTipoOperacao(ide->GetChild("tpNF")->GetValue());
            if (!m_ide) m_ide = new NFeDocumentIde();
            m_ide->Parse(ide.get());
        }

        auto emit = infNFe->GetChild("emit");
        if (emit) {
            if (!m_emit) m_emit = new NFeDocumentEmit();
            m_emit->Parse(emit.get());
        }

        auto dest = infNFe->GetChild("dest");
        if (dest) {
            if (!m_dest) m_dest = new NFeDocumentDest();
            m_dest->Parse(dest.get());
        }

        auto det = infNFe->GetChildren("det");
        for (const auto &detItem: det) {
            auto item = new NFeDocumentDet();
            if (item->Parse(detItem.get())) {
                m_det.insert(item);
            } else {
                delete item;
            }
        }

        return true;
    }

    return false;
}

NFeDocumentEmit::~NFeDocumentEmit()
{
    if (m_enderEmit) delete m_enderEmit;
}

bool NFeDocumentEmit::Parse(const core::IXmlNode *emitNode) {
    if (!emitNode) return false;

    if (emitNode->GetChild("CNPJ")) {
        m_cnpjCpf = emitNode->GetChild("CNPJ")->GetValue();
    } else {
        m_cnpjCpf = emitNode->GetChild("CPF")->GetValue();
    }
    m_razaoSocial = emitNode->GetChild("xNome")->GetValue();
    m_nomeFantasia = emitNode->GetChild("xFant") ? emitNode->GetChild("xFant")->GetValue() : m_razaoSocial;
    m_inscricaoEstadual = emitNode->GetChild("IE")->GetValue();
    if (emitNode->GetChild("IM")) {
        m_inscricaoMunicipal = emitNode->GetChild("IM")->GetValue();
    } else {
        m_inscricaoMunicipal = "";
    }

    if (auto enderEmit = emitNode->GetChild("enderEmit")) {
        if (!m_enderEmit) m_enderEmit = new NFeDocumentEndereco();
        m_enderEmit->Parse(enderEmit.get());
    }

    return true;
}

bool NFeDocumentEndereco::Parse(const core::IXmlNode *enderEmitNode) {
    if (!enderEmitNode) return false;

    m_logradouro = enderEmitNode->GetChild("xLgr")->GetValue();
    m_numero = enderEmitNode->GetChild("nro")->GetValue();
    m_complemento = enderEmitNode->GetChild("xCpl") ? enderEmitNode->GetChild("xCpl")->GetValue() : "";
    m_bairro = enderEmitNode->GetChild("xBairro")->GetValue();
    m_codMunicipio = std::stoi(enderEmitNode->GetChild("cMun")->GetValue());
    m_municipio = enderEmitNode->GetChild("xMun")->GetValue();
    m_uf = enderEmitNode->GetChild("UF")->GetValue();
    m_cep = enderEmitNode->GetChild("CEP")->GetValue();

    if (enderEmitNode->GetChild("cPais")) {
        m_codPais = std::stoi(enderEmitNode->GetChild("cPais")->GetValue());
    }
    if (enderEmitNode->GetChild("xPais")) {
        m_pais = enderEmitNode->GetChild("xPais")->GetValue();
    }
    if (enderEmitNode->GetChild("fone")) {
        m_fone = enderEmitNode->GetChild("fone")->GetValue();
    }

    return true;
}

NFeDocumentDest::~NFeDocumentDest() {
    if (m_enderDest) delete m_enderDest;
}

bool NFeDocumentDest::Parse(const core::IXmlNode *destNode) {
    m_cnpjCpf = destNode->GetChild("CNPJ") ? destNode->GetChild("CNPJ")->GetValue() : destNode->GetChild("CPF")->GetValue();
    m_nome = destNode->GetChild("xNome")->GetValue();
    if (destNode->GetChild("IE")) {
        m_inscricaoEstadual = destNode->GetChild("IE")->GetValue();
    }

    if (destNode->GetChild("IM")) {
        m_inscricaoMunicipal = destNode->GetChild("IM")->GetValue();
    }

    if (auto enderDest = destNode->GetChild("enderDest")) {
        if (!m_enderDest) m_enderDest = new NFeDocumentEndereco();
        m_enderDest->Parse(enderDest.get());
    }

    return true;
}

NFeDocumentDet::~NFeDocumentDet() {
    if (m_prod) delete m_prod;
    if (m_imposto) delete m_imposto;
}

bool NFeDocumentDet::Parse(const core::IXmlNode *detNode) {
    m_nItem = std::stoi(detNode->GetAttribute("nItem").value());

    auto prodNode = detNode->GetChild("prod");
    if (prodNode) {
        if (!m_prod) m_prod = new NFeDocumentDetProd();
        m_prod->Parse(prodNode.get());
    }

    auto impostoNode = detNode->GetChild("imposto");
    if (impostoNode) {
        if (!m_imposto) m_imposto = new NFeDocumentDetImposto();
        m_imposto->Parse(impostoNode.get());
    }

    return true;
}

bool NFeDocumentIde::Parse(const core::IXmlNode *ideNode) {
    if (!ideNode) return false;

    if (auto node = ideNode->GetChild("cUF")) m_cUf = std::stoi(node->GetValue());
    if (auto node = ideNode->GetChild("cNF")) m_cNf = node->GetValue();
    if (auto node = ideNode->GetChild("natOp")) m_natOp = node->GetValue();
    if (auto node = ideNode->GetChild("mod")) m_mod = node->GetValue();
    if (auto node = ideNode->GetChild("serie")) m_serie = std::stoi(node->GetValue());
    if (auto node = ideNode->GetChild("nNF")) m_nNf = std::stoi(node->GetValue());
    if (auto node = ideNode->GetChild("dhEmi")) m_dhEmissao = ParseIso8601(node->GetValue());
    if (auto node = ideNode->GetChild("dhSaiEnt")) m_dhSaiEnt = ParseIso8601(node->GetValue());

    if (auto node = ideNode->GetChild("tpNF")) m_tpNf = node->GetValue();
    if (auto node = ideNode->GetChild("idDest")) m_idDest = node->GetValue();

    return true;
}

bool NFeDocumentDetProd::Parse(const core::IXmlNode *prodNode)
{
    if (!prodNode) return false;

    if (auto node = prodNode->GetChild("cProd")) m_cProd = node->GetValue();
    if (auto node = prodNode->GetChild("cEAN")) m_cEan = node->GetValue();
    if (auto node = prodNode->GetChild("xProd")) m_xProd = RemoveLineBreaks(node->GetValue());
    if (auto node = prodNode->GetChild("NCM")) m_ncm = node->GetValue();
    if (auto node = prodNode->GetChild("CEST")) m_cest = node->GetValue();
    if (auto node = prodNode->GetChild("CFOP")) m_cfop = node->GetValue();
    if (auto node = prodNode->GetChild("uCom")) m_uCom = node->GetValue();
    if (auto node = prodNode->GetChild("qCom")) m_qCom = ParseDouble(node->GetValue());
    if (auto node = prodNode->GetChild("vUnCom")) m_vUnCom = ParseDouble(node->GetValue());
    if (auto node = prodNode->GetChild("vProd")) m_vProd = ParseDouble(node->GetValue());
    if (auto node = prodNode->GetChild("vDesc")) m_vDesc = ParseDouble(node->GetValue());
    if (auto node = prodNode->GetChild("cEANTrib")) m_cEanTrib = node->GetValue();
    if (auto node = prodNode->GetChild("uTrib")) m_uTrib = node->GetValue();
    if (auto node = prodNode->GetChild("qTrib")) m_qTrib = ParseDouble(node->GetValue());
    if (auto node = prodNode->GetChild("vUnTrib")) m_vUnTrib = ParseDouble(node->GetValue());

    return true;
}

NFeDocumentDetImposto::~NFeDocumentDetImposto() {
    if (m_ICMS) delete m_ICMS.value();
    if (m_IPI) delete m_IPI.value();
    if (m_II) delete m_II.value();
    if (m_PIS) delete m_PIS.value();
    if (m_PISST) delete m_PISST.value();
    if (m_COFINS) delete m_COFINS.value();
    if (m_COFINSST) delete m_COFINSST.value();
    if (m_ICMSUFDEST) delete m_ICMSUFDEST.value();
    if (m_IS) delete m_IS.value();
}

bool NFeDocumentDetImposto::Parse(const core::IXmlNode *impostoNode)
{
    if (!impostoNode) return false;

    if (auto icmsNode = impostoNode->GetChild("ICMS")) {
        auto icms = new NFeDocumentDetImpostoICMS();
        if (icms->Parse(icmsNode.get())) m_ICMS = icms;
        else delete icms;
    }

    if (auto ipiNode = impostoNode->GetChild("IPI")) {
        auto ipi = new NFeDocumentDetImpostoIPI();
        if (ipi->Parse(ipiNode.get())) m_IPI = ipi;
        else delete ipi;
    }

    if (auto iiNode = impostoNode->GetChild("II")) {
        auto ii = new NFeDocumentDetImpostoII();
        if (ii->Parse(iiNode.get())) m_II = ii;
        else delete ii;
    }

    if (auto pisNode = impostoNode->GetChild("PIS")) {
        auto pis = new NFeDocumentDetImpostoPIS();
        if (pis->Parse(pisNode.get())) m_PIS = pis;
        else delete pis;
    }

    if (auto pisstNode = impostoNode->GetChild("PISST")) {
        auto pisst = new NFeDocumentDetImpostoPISST();
        if (pisst->Parse(pisstNode.get())) m_PISST = pisst;
        else delete pisst;
    }

    if (auto cofinsNode = impostoNode->GetChild("COFINS")) {
        auto cofins = new NFeDocumentDetImpostoCOFINS();
        if (cofins->Parse(cofinsNode.get())) m_COFINS = cofins;
        else delete cofins;
    }

    if (auto cofinsstNode = impostoNode->GetChild("COFINSST")) {
        auto cofinsst = new NFeDocumentDetImpostoCOFINSST();
        if (cofinsst->Parse(cofinsstNode.get())) m_COFINSST = cofinsst;
        else delete cofinsst;
    }

    if (auto icmsufdestNode = impostoNode->GetChild("ICMSUFDest")) {
        auto icmsufdest = new NFeDocumentDetImpostoICMSUFDest();
        if (icmsufdest->Parse(icmsufdestNode.get())) m_ICMSUFDEST = icmsufdest;
        else delete icmsufdest;
    }

    if (auto isNode = impostoNode->GetChild("IS")) {
        auto is = new NFeDocumentDetImpostoIS();
        if (is->Parse(isNode.get())) m_IS = is;
        else delete is;
    }

    return true;
}

NFeDocumentDetImpostoIPI::~NFeDocumentDetImpostoIPI() {}

bool NFeDocumentDetImpostoIPI::Parse(const core::IXmlNode *ipiNode) {
    if (!ipiNode) return false;

    std::unique_ptr<core::IXmlNode> innerNode;
    const char* ipiGroups[] = { "IPITrib", "IPINT" };
    for (const auto& group : ipiGroups) {
        innerNode = ipiNode->GetChild(group);
        if (innerNode) break;
    }

    if (!innerNode) return false;

    if (auto node = innerNode->GetChild("CST")) m_CST = node->GetValue();
    if (auto node = innerNode->GetChild("vBC")) m_vBC = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("pIPI")) m_pIPI = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("qUnid")) m_qUnid = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("vUnid")) m_vUnid = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("vIPI")) m_vIPI = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoII::Parse(const core::IXmlNode *iiNode) {
    if (!iiNode) return false;

    if (auto node = iiNode->GetChild("vBC")) m_vBC = ParseDouble(node->GetValue());
    if (auto node = iiNode->GetChild("vDespAdu")) m_vDespAdu = ParseDouble(node->GetValue());
    if (auto node = iiNode->GetChild("vII")) m_vII = ParseDouble(node->GetValue());
    if (auto node = iiNode->GetChild("vIOF")) m_vIOF = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoPIS::Parse(const core::IXmlNode *pisNode) {
    if (!pisNode) return false;

    std::unique_ptr<core::IXmlNode> innerNode;
    const char* pisGroups[] = { "PISAliq", "PISQtde", "PISNT", "PISOutr" };
    for (const auto& group : pisGroups) {
        innerNode = pisNode->GetChild(group);
        if (innerNode) break;
    }

    if (!innerNode) return false;

    if (auto node = innerNode->GetChild("CST")) m_CST = node->GetValue();
    if (auto node = innerNode->GetChild("vBC")) m_vBC = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("pPIS")) m_pPIS = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("qBCProd")) m_qBCProd = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("vAliqProd")) m_vAliqProd = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("vPIS")) m_vPIS = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoPISST::Parse(const core::IXmlNode *pisstNode) {
    if (!pisstNode) return false;

    if (auto node = pisstNode->GetChild("vBC")) m_vBC = ParseDouble(node->GetValue());
    if (auto node = pisstNode->GetChild("pPIS")) m_pPIS = ParseDouble(node->GetValue());
    if (auto node = pisstNode->GetChild("qBCProd")) m_qBCProd = ParseDouble(node->GetValue());
    if (auto node = pisstNode->GetChild("vAliqProd")) m_vAliqProd = ParseDouble(node->GetValue());
    if (auto node = pisstNode->GetChild("vPIS")) m_vPIS = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoCOFINS::Parse(const core::IXmlNode *cofinsNode) {
    if (!cofinsNode) return false;

    std::unique_ptr<core::IXmlNode> innerNode;
    const char* cofinsGroups[] = { "COFINSAliq", "COFINSQtde", "COFINSNT", "COFINSOutr" };
    for (const auto& group : cofinsGroups) {
        innerNode = cofinsNode->GetChild(group);
        if (innerNode) break;
    }

    if (!innerNode) return false;

    if (auto node = innerNode->GetChild("CST")) m_CST = node->GetValue();
    if (auto node = innerNode->GetChild("vBC")) m_vBC = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("pCOFINS")) m_pCOFINS = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("qBCProd")) m_qBCProd = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("vAliqProd")) m_vAliqProd = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("vCOFINS")) m_vCOFINS = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoCOFINSST::Parse(const core::IXmlNode *cofinsstNode) {
    if (!cofinsstNode) return false;

    if (auto node = cofinsstNode->GetChild("vBC")) m_vBC = ParseDouble(node->GetValue());
    if (auto node = cofinsstNode->GetChild("pCOFINS")) m_pCOFINS = ParseDouble(node->GetValue());
    if (auto node = cofinsstNode->GetChild("qBCProd")) m_qBCProd = ParseDouble(node->GetValue());
    if (auto node = cofinsstNode->GetChild("vAliqProd")) m_vAliqProd = ParseDouble(node->GetValue());
    if (auto node = cofinsstNode->GetChild("vCOFINS")) m_vCOFINS = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoICMSUFDest::Parse(const core::IXmlNode *icmsufdestNode) {
    if (!icmsufdestNode) return false;

    if (auto node = icmsufdestNode->GetChild("vBCUFDest")) m_vBCUFDest = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("vBCFCPUFDest")) m_vBCFCPUFDest = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("pFCPUFDest")) m_pFCPUFDest = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("pICMSUFDest")) m_pICMSUFDest = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("pICMSInter")) m_pICMSInter = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("pICMSInterPart")) m_pICMSInterPart = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("vFCPUFDest")) m_vFCPUFDest = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("vICMSUFDest")) m_vICMSUFDest = ParseDouble(node->GetValue());
    if (auto node = icmsufdestNode->GetChild("vICMSUFRemet")) m_vICMSUFRemet = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoIS::Parse(const core::IXmlNode *isNode) {
    if (!isNode) return false;

    if (auto node = isNode->GetChild("CST")) m_CST = node->GetValue();
    if (auto node = isNode->GetChild("cClassTribIS")) m_cClassTribIS = node->GetValue();
    if (auto node = isNode->GetChild("vBCIS")) m_vBCIS = ParseDouble(node->GetValue());
    if (auto node = isNode->GetChild("pIS")) m_pIS = ParseDouble(node->GetValue());
    if (auto node = isNode->GetChild("adRemIS")) m_adRemIS = ParseDouble(node->GetValue());
    if (auto node = isNode->GetChild("uTrib")) m_uTrib = node->GetValue();
    if (auto node = isNode->GetChild("qTrib")) m_qTrib = ParseDouble(node->GetValue());
    if (auto node = isNode->GetChild("vIS")) m_vIS = ParseDouble(node->GetValue());

    return true;
}

bool NFeDocumentDetImpostoICMS::Parse(const core::IXmlNode *icmsNode)
{
    if (!icmsNode) return false;

    std::unique_ptr<core::IXmlNode> innerNode;
    const char* icmsGroups[] = {
        "ICMS00", "ICMS10", "ICMS20", "ICMS30", "ICMS40", "ICMS51", "ICMS60", "ICMS70", "ICMS90",
        "ICMSPart", "ICMSST", "ICMSSN101", "ICMSSN102", "ICMSSN201", "ICMSSN202", "ICMSSN500", "ICMSSN900"
    };

    for (const auto& group : icmsGroups) {
        innerNode = icmsNode->GetChild(group);
        if (innerNode) break;
    }

    if (!innerNode) return false;

    if (auto node = innerNode->GetChild("orig")) m_orig = node->GetValue();
    if (auto node = innerNode->GetChild("CSOSN")) m_CSOSN = node->GetValue();
    if (auto node = innerNode->GetChild("CST")) m_CST = node->GetValue();
    if (auto node = innerNode->GetChild("vBC")) m_vBC = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("pICMS")) m_pICMS = ParseDouble(node->GetValue());
    if (auto node = innerNode->GetChild("vICMS")) m_vICMS = ParseDouble(node->GetValue());

    return true;
}

} // namespace document
} // namespace zafra