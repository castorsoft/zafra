#pragma once

#include "zafra/core/document_interfaces.hpp"
#include <set>
#include <chrono>


namespace zafra {
namespace document {

class NFeDocumentIde {
public:
    NFeDocumentIde() = default;

    int GetCUf() const { return m_cUf; }
    std::string GetCNf() const { return m_cNf; }
    std::string GetNatOp() const { return m_natOp; }
    std::string GetMod() const { return m_mod; }
    int GetSerie() const { return m_serie; }
    int GetNNf() const { return m_nNf; }
    time_t GetDhEmissao() const { return m_dhEmissao; }
    time_t GetDhSaiEnt() const { return m_dhSaiEnt; }
    std::string GetTpNf() const { return m_tpNf; }
    std::string GetIdDest() const { return m_idDest; }

    bool Parse(const core::IXmlNode* ideNode);

private:
    int m_cUf;
    std::string m_cNf;
    std::string m_natOp;
    std::string m_mod;
    int m_serie;
    int m_nNf;
    time_t m_dhEmissao;
    time_t m_dhSaiEnt;
    std::string m_tpNf;
    std::string m_idDest;
};

class NFeDocumentEndereco {
public:
    NFeDocumentEndereco() = default;

    std::string GetLogradouro() const { return m_logradouro; }
    std::string GetNumero() const { return m_numero; }
    std::string GetComplemento() const { return m_complemento; }
    std::string GetBairro() const  { return m_bairro; }
    int GetCodMunicipio() const { return m_codMunicipio; }
    std::string GetMunicipio() const { return m_municipio; }
    std::string GetUf() const { return m_uf; }
    std::string GetCEP() const { return m_cep; }
    int GetCodPais() const { return m_codPais; }
    std::string GetPais() const { return m_pais; }
    std::string GetFone() const { return m_fone; }

    bool Parse(const core::IXmlNode* enderEmitNode);

private:
    std::string m_logradouro;
    std::string m_numero;
    std::string m_complemento;
    std::string m_bairro;
    int m_codMunicipio;
    std::string m_municipio;
    std::string m_uf;
    std::string m_cep;
    int m_codPais;
    std::string m_pais;
    std::string m_fone;
};

class NFeDocumentDest {
public:
    NFeDocumentDest() = default;
    ~NFeDocumentDest();

    std::string GetCnpjCpf() const { return m_cnpjCpf; }
    std::string GetNome() const { return m_nome; }
    std::string GetInscricaoEstadual() const { return m_inscricaoEstadual; }
    std::string GetInscricaoMunicipal() const { return m_inscricaoMunicipal; }
    std::string GetEmail() const { return m_email; }

    NFeDocumentEndereco* GetEnderDest() const { return m_enderDest; }

    bool Parse(const core::IXmlNode* destNode);
private:
    std::string m_cnpjCpf;
    std::string m_nome;
    std::string m_inscricaoEstadual;
    std::string m_inscricaoMunicipal;
    std::string m_email;

    NFeDocumentEndereco* m_enderDest = nullptr;
};

class NFeDocumentEmit {
public:
    NFeDocumentEmit() = default;
    ~NFeDocumentEmit();

    std::string GetCnpjCpf() const { return m_cnpjCpf; };
    std::string GetRazaoSocial() const { return m_razaoSocial; };
    std::string GetNomeFantasia() const { return m_nomeFantasia; };
    std::string GetInscricaoEstadual() const { return m_inscricaoEstadual; };
    std::string GetInscricaoMunicipal() const { return m_inscricaoMunicipal; };

    NFeDocumentEndereco* GetEnderEmit() const { return m_enderEmit; };

    bool Parse(const core::IXmlNode* emitNode);
private:

    std::string m_cnpjCpf;
    std::string m_razaoSocial;
    std::string m_nomeFantasia;
    std::string m_inscricaoEstadual;
    std::string m_inscricaoMunicipal;

    NFeDocumentEndereco* m_enderEmit = nullptr;
};

class NFeDocumentDetProd {
public:
    NFeDocumentDetProd() = default;

    std::string GetCProd() const { return m_cProd; }
    std::string GetCEan() const { return m_cEan; }
    std::string GetXProd() const { return m_xProd; }
    std::string GetNcm() const { return m_ncm; }
    std::string GetCest() const { return m_cest; }
    std::string GetCfop() const { return m_cfop; }
    std::string GetUCom() const { return m_uCom; }
    double GetQCom() const { return m_qCom; }
    double GetVUnCom() const { return m_vUnCom; }
    double GetVProd() const { return m_vProd; }
    std::string GetCEanTrib() const { return m_cEanTrib; }
    std::string GetUTrib() const { return m_uTrib; }
    double GetQTrib() const { return m_qTrib; }
    double GetVUnTrib() const { return m_vUnTrib; }

    bool Parse(const core::IXmlNode* prodNode);

private:
    std::string m_cProd;
    std::string m_cEan;
    std::string m_xProd;
    std::string m_ncm;
    std::string m_cest;
    std::string m_cfop;
    std::string m_uCom;
    double m_qCom;
    double m_vUnCom;
    double m_vProd;
    std::string m_cEanTrib;
    std::string m_uTrib;
    double m_qTrib;
    double m_vUnTrib;
};

class NFeDocumentDetImpostoICMS {
public:
    NFeDocumentDetImpostoICMS() = default;

    std::optional<std::string> GetCSOSN() const { return m_CSOSN; }
    std::optional<std::string> GetCST() const { return m_CST; }
    std::string GetOrig() const { return m_orig; }
    std::optional<double> GetVBC() const { return m_vBC; }
    std::optional<double> GetPICMS() const { return m_pICMS; }
    std::optional<double> GetVICMS() const { return m_vICMS; }

    bool Parse(const core::IXmlNode* icmsNode);

private:
    std::optional<std::string> m_CSOSN;
    std::optional<std::string> m_CST;
    std::string m_orig;
    std::optional<double> m_vBC;
    std::optional<double> m_pICMS;
    std::optional<double> m_vICMS;

    // TODO: demais csosns/cst
};

class NFeDocumentDetImpostoIPI {
public:
    NFeDocumentDetImpostoIPI() = default;
    ~NFeDocumentDetImpostoIPI();

    bool Parse(const core::IXmlNode* ipiNode);
private:
    std::string m_CST;
    std::optional<double> m_vBC;
    std::optional<double> m_pIPI;
    std::optional<double> m_qUnid;
    std::optional<double> m_vUnid;
    double m_vIPI;
};

class NFeDocumentDetImpostoII {
public:
    NFeDocumentDetImpostoII() = default;

    double GetVBC() const { return m_vBC; }
    double GetVDespAdu() const { return m_vDespAdu; }
    double GetVII() const { return m_vII; }
    double GetVIOF() const { return m_vIOF; }

    bool Parse(const core::IXmlNode* iiNode);

private:
    double m_vBC;
    double m_vDespAdu;
    double m_vII;
    double m_vIOF;
};

class NFeDocumentDetImpostoPIS {
public:
    NFeDocumentDetImpostoPIS() = default;

    std::string GetCST() const { return m_CST; }
    std::optional<double> GetVBC() const { return m_vBC; }
    std::optional<double> GetPPis() const { return m_pPIS; }
    std::optional<double> GetQBcProd() const { return m_qBCProd; }
    std::optional<double> GetVAliqProd() const { return m_vAliqProd; }
    double GetVPis() const { return m_vPIS; }

    bool Parse(const core::IXmlNode* pisNode);

private:
    std::string m_CST;
    std::optional<double> m_vBC;
    std::optional<double> m_pPIS;
    std::optional<double> m_qBCProd;
    std::optional<double> m_vAliqProd;
    double m_vPIS;
};

class NFeDocumentDetImpostoPISST {
public:
    NFeDocumentDetImpostoPISST() = default;

    std::optional<double> GetVBC() const { return m_vBC; }
    std::optional<double> GetPPIS() const { return m_pPis; }
    std::optional<double> GetQBCProd() const { return m_qBCProd; }
    std::optional<double> GetVAliqProd() const { return m_vAliqProd; }
    double GetVPIS() const { return m_vPIS; }

    bool Parse(const core::IXmlNode* pisstNode);

private:
    std::optional<double> m_vBC;
    std::optional<double> m_pPis;
    std::optional<double> m_qBCProd;
    std::optional<double> m_vAliqProd;
    double m_vPIS;
};
class NFeDocumentDetImpostoCOFINS {
public:
    NFeDocumentDetImpostoCOFINS() = default;

    std::string GetCST() const { return m_CST; }
    std::optional<double> GetVBC() const { return m_vBC; }
    std::optional<double> GetPCofins() const { return m_pCOFINS; }
    std::optional<double> GetQBcProd() const { return m_qBCProd; }
    std::optional<double> GetVAliqProd() const { return m_vAliqProd; }
    double GetVCofins() const { return m_vCOFINS; }

    bool Parse(const core::IXmlNode* cofinsNode);

private:
    std::string m_CST;
    std::optional<double> m_vBC;
    std::optional<double> m_pCOFINS;
    std::optional<double> m_qBCProd;
    std::optional<double> m_vAliqProd;
    double m_vCOFINS;
};

class NFeDocumentDetImpostoCOFINSST {
public:
    NFeDocumentDetImpostoCOFINSST() = default;

    std::optional<double> GetVBC() const { return m_vBC; }
    std::optional<double> GetPCOFINS() const { return m_pCOFINS; }
    std::optional<double> GetQBCProd() const { return m_qBCProd; }
    std::optional<double> GetVAliqProd() const { return m_vAliqProd; }
    double GetVCOFINS() const { return m_vCOFINS; }

    bool Parse(const core::IXmlNode* cofinsstNode);

private:
    std::optional<double> m_vBC;
    std::optional<double> m_pCOFINS;
    std::optional<double> m_qBCProd;
    std::optional<double> m_vAliqProd;
    double m_vCOFINS;
};

class NFeDocumentDetImpostoICMSUFDest {
public:
    NFeDocumentDetImpostoICMSUFDest() = default;

    double GetVBCUFDest() const { return m_vBCUFDest; }
    double GetVBCFCPUFDest() const { return m_vBCFCPUFDest; }
    double GetPFCPUFDest() const { return m_pFCPUFDest; }
    double GetPICMSUFDest() const { return m_pICMSUFDest; }
    double GetPICMSInter() const { return m_pICMSInter; }
    double GetPICMSInterPart() const { return m_pICMSInterPart; }
    double GetVFCPUFDest() const { return m_vFCPUFDest; }
    double GetVICMSUFDest() const { return m_vICMSUFDest; }
    double GetVICMSUFRemet() const { return m_vICMSUFRemet; }

    bool Parse(const core::IXmlNode* icmsufdestNode);

private:
    double m_vBCUFDest;
    double m_vBCFCPUFDest;
    double m_pFCPUFDest;
    double m_pICMSUFDest;
    double m_pICMSInter;
    double m_pICMSInterPart;
    double m_vFCPUFDest;
    double m_vICMSUFDest;
    double m_vICMSUFRemet;
};

class NFeDocumentDetImpostoIS {
public:
    NFeDocumentDetImpostoIS() = default;

    std::string GetCST() const { return m_CST; }
    std::string GetCClassTribIS() const { return m_cClassTribIS; }
    std::optional<double> GetVBCIS() const { return m_vBCIS; }
    std::optional<double> GetPIS() const { return m_pIS; }
    std::optional<double> GetAdRemIS() const { return m_adRemIS; }
    std::optional<std::string> GetUTrib() const { return m_uTrib; }
    std::optional<double> GetQTrib() const { return m_qTrib; }
    std::optional<double> GetVIS() const { return m_vIS; }

    bool Parse(const core::IXmlNode* isNode);

private:
    std::string m_CST;
    std::string m_cClassTribIS;
    std::optional<double> m_vBCIS;
    std::optional<double> m_pIS;
    std::optional<double> m_adRemIS;
    std::optional<std::string> m_uTrib;
    std::optional<double> m_qTrib;
    std::optional<double> m_vIS;
};

class NFeDocumentDetImposto {
public:
    NFeDocumentDetImposto() = default;
    ~NFeDocumentDetImposto();

    std::optional<NFeDocumentDetImpostoICMS*> GetICMS() const { return m_ICMS; }
    std::optional<NFeDocumentDetImpostoIPI*> GetIPI() const { return m_IPI; }
    std::optional<NFeDocumentDetImpostoII*> GetII() const { return m_II; }
    std::optional<NFeDocumentDetImpostoPIS*> GetPIS() const { return m_PIS; }
    std::optional<NFeDocumentDetImpostoPISST*> GetPISST() const { return m_PISST; }
    std::optional<NFeDocumentDetImpostoCOFINS*> GetCOFINS() const { return m_COFINS; }
    std::optional<NFeDocumentDetImpostoCOFINSST*> GetCOFINSST() const { return m_COFINSST; }
    std::optional<NFeDocumentDetImpostoICMSUFDest*> GetICMSUFDest() const { return m_ICMSUFDEST; }
    std::optional<NFeDocumentDetImpostoIS*> GetIS() const { return m_IS; }
    // std::optional<NFeDocumentDetImpostoIBSCBS*> GetIBSCBS() const { return m_IBSCBS; }

    bool Parse(const core::IXmlNode* impostoNode);

private:
    std::optional<NFeDocumentDetImpostoICMS*> m_ICMS;
    std::optional<NFeDocumentDetImpostoIPI*> m_IPI;
    std::optional<NFeDocumentDetImpostoII*> m_II;
    std::optional<NFeDocumentDetImpostoPIS*> m_PIS;
    std::optional<NFeDocumentDetImpostoPISST*> m_PISST;
    std::optional<NFeDocumentDetImpostoCOFINS*> m_COFINS;
    std::optional<NFeDocumentDetImpostoCOFINSST*> m_COFINSST;
    std::optional<NFeDocumentDetImpostoICMSUFDest*> m_ICMSUFDEST;
    std::optional<NFeDocumentDetImpostoIS*> m_IS;
    // TODO: IBSCBS
    // std::optional<NFeDocumentDetImpostoIBSCBS*> m_IBSCBS;
};

class NFeDocumentDet {
public:
    NFeDocumentDet() = default;
    ~NFeDocumentDet();

    int GetNItem() const { return m_nItem; }
    NFeDocumentDetProd* GetProd() const { return m_prod; }
    NFeDocumentDetImposto* GetImposto() const { return m_imposto; }

    bool Parse(const core::IXmlNode* detNode);

private:
    int m_nItem;

    NFeDocumentDetProd* m_prod = nullptr;
    NFeDocumentDetImposto* m_imposto = nullptr;
};

class NFeDocument : public core::IDocument {
public:
    NFeDocument() = default;
    ~NFeDocument();

    core::DocumentType GetType() const override { return core::DocumentType::NFe; }
    std::string GetVersion() const override { return m_version; }
    std::string GetId() const override { return m_id; }
    std::string getChaveAcesso() const override { return m_chaveAcesso; }
    core::TipoOperacao getTipoOperacao() const override { return m_tipoOperacao; }

    NFeDocumentIde* GetIde() const { return m_ide; }
    NFeDocumentEmit* GetEmit() const { return m_emit; }
    NFeDocumentDest* GetDest() const { return m_dest; }
    std::set<NFeDocumentDet*> GetDet() const { return m_det; }

    bool Parse(const core::IXmlNode* root_node) override;

private:
    std::string m_version;
    std::string m_id;
    std::string m_chaveAcesso;
    core::TipoOperacao m_tipoOperacao;

    NFeDocumentIde* m_ide = nullptr;
    NFeDocumentEmit* m_emit = nullptr;
    NFeDocumentDest* m_dest = nullptr;
    std::set<NFeDocumentDet*> m_det;
};

} // namespace document
} // namespace zafra
