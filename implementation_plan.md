# Especificação Arquitetural: Biblioteca de Documentos Fiscais Brasileiros (Zafra Lib)

Este documento descreve a arquitetura proposta para uma biblioteca C++ focada no gerenciamento de documentos fiscais brasileiros (NF-e, NFC-e, NFS-e), projetada para ser extensível e desenvolvida sob a metodologia TDD (Test-Driven Development).

## 1. Stack Tecnológico e Dependências

Para garantir um código aberto e permissivo (licenças MIT/BSD/Apache), as seguintes ferramentas são sugeridas:
- **Linguagem:** C++17 ou superior (para uso de features modernas como `std::optional`, `std::variant`, `std::filesystem`).
- **Build System:** CMake (padrão de mercado, facilita integração de dependências e cross-compiling).
- **Manipulação de XML:** [pugixml](https://pugixml.org/) (Licença MIT). É leve, extremamente rápido e possui uma API DOM muito amigável para C++.
- **Comunicação HTTP/SOAP:** [libcurl](https://curl.se/libcurl/) (Licença MIT/X derivate). A comunicação com a RFB usa SOAP sobre HTTPS com certificados digitais clientes (mTLS). Montaremos o envelope SOAP via `pugixml` e o enviaremos usando `libcurl`.
- **Framework de Testes (TDD):** [Catch2](https://github.com/catchorg/Catch2) (Licença BSL-1.0) ou [Google Test](https://github.com/google/googletest) (Licença BSD). 

## 2. Padrões de Projeto (Design Patterns)

A extensibilidade é o requisito principal, pois novos modelos de NFS-e (existem centenas de provedores/prefeituras) e novas versões das Notas Técnicas da NF-e surgirão frequentemente.

- **Factory Method / Abstract Factory:** Usado para instanciar as classes de representação de documento corretas (`NFe`, `NFSe_Abrasf`, etc.) baseando-se na inspeção inicial do XML.
- **Strategy:** Cada webservice da RFB ou prefeitura terá sua própria "estratégia" de comunicação e extração de resposta, implementando uma interface de serviço comum.
- **Facade:** Para fornecer uma API pública simples ao usuário final da biblioteca, escondendo as complexidades de manipulação de nós XML, cabeçalhos SOAP e requisições HTTP.
- **Dependency Injection (Inversão de Controle):** O cliente HTTP e o parser XML devem ser passados como interfaces. Isso permite que, durante os testes (TDD), usemos um cliente HTTP "mockado" que retorna XMLs fixos ao invés de bater nos servidores da Sefaz.

## 3. Estrutura de Diretórios Proposta

```text
zafra_lib/
├── CMakeLists.txt
├── include/
│   └── zafra/
│       ├── core/          # Interfaces base (IDocument, IWebService, IHttpClient)
│       ├── xml/           # Wrapper para pugixml
│       ├── document/      # NF-e, NFC-e, NFS-e (Padrão Nacional, ABRASF, IPM)
│       └── webservice/    # Clientes de comunicação (DistribuicaoDFe)
├── src/
│   ├── xml/
│   ├── document/
│   └── webservice/
├── tests/                 # Espelho da estrutura src/ para TDD
│   ├── CMakeLists.txt
│   ├── xml/
│   ├── document/
│   └── webservice/
└── third_party/           # Dependências baixadas via CMake (FetchContent)
```

## 4. Detalhamento dos Requisitos Iniciais

### 4.1. Leitura e Escrita de Arquivos XML
Criaremos uma abstração sobre o `pugixml` (ou outro escolhido) para não expor a dependência a terceiros nos headers públicos da biblioteca.
- **Interface:** `IXmlDocument` / `IXmlNode`
- **Responsabilidade:** Carregar XML a partir do disco, de uma `std::string` em memória, iterar por nós, buscar valores via XPath ou navegação direta, e exportar novamente para arquivo/string.

### 4.2. Identificação de Modelo pelo Cabeçalho (Parser Inteligente)
Implementaremos uma Factory ("Detector de Documento").
- **Classe:** `DocumentFactory`
- **Lógica:** A classe recebe um XML. O método `DetectAndCreate(xml)` inspeciona o `namespace` e a *root tag*:
  - `<nfeProc>` ou `<NFe>` -> Instancia o modelo `NFe`.
  - `<NFSe>` sob namespace do Padrão Nacional -> Instancia `NFSeNacional`.
  - Diferentes tags/namespaces para ABRASF e IPM.
  - O retorno será um ponteiro inteligente genérico `std::unique_ptr<IDocument>`, permitindo polimorfismo.

### 4.3. Implementação de Webservices (DistribuicaoDFe)
O webservice `NFeDistribuicaoDFe` requer criação de um envelope SOAP com os dados da consulta, o envio por HTTPS com Certificado Digital A1/A3, e o tratamento de retorno (frequentemente documentos em formato `.zip` e em Base64).
- **Interface base:** `IWebServiceClient`
- **Fluxo:** 
  1. Recebe os parâmetros de busca (CNPJ, ultNSU).
  2. Gera o XML de `<distDFeInt>` e coloca no `<soapenv:Body>`.
  3. Envia o POST usando nossa interface `IHttpClient`.
  4. Valida a resposta SOAP.
  5. Descompacta (`zlib`) os documentos retornados na tag `<docZip>` (Base64 + GZip).

## 5. Fluxo de TDD Aplicado

O ciclo **Red -> Green -> Refactor** será aplicado rigorosamente.

**Exemplo do fluxo para a Detecção de NF-e:**
1. **Red:** Criamos `tests/document/document_factory_test.cpp`. Escrevemos um caso de teste passando uma string com `<NFe>...</NFe>` esperando que o método retorne um tipo de documento válido do tipo NFe. O teste não vai compilar (falta a classe).
2. **Green:** Criamos a classe `DocumentFactory` com uma lógica extremamente simples (ex: usando apenas a verificação de substring ou checando a primeira tag rigidamente) para que o teste passe.
3. **Refactor:** Ajustamos o código para usar o parser de XML de verdade, com namespaces corretos, criando um mapa registrando os tipos para deixar o `DocumentFactory` limpo e fechado para modificação (Open/Closed Principle).

## 6. Plano de Execução

1. Criar repositório e configurar estrutura básica do `CMakeLists.txt` incluindo `GTest` (ou `Catch2`) via FetchContent.
2. Definir as interfaces de abstração básicas em `include/zafra/core`.
3. Escrever o primeiro teste para carregamento de XML.
4. Implementar a classe concreta de XML integrando `pugixml`.
5. ... avançar sucessivamente com TDD para os outros módulos.

---

## User Review Required

Por favor, verifique os pontos abaixo para alinhar a implementação:

> [!IMPORTANT]
> 1. **Bibliotecas Base:** Você concorda com o uso do **pugixml** (para tratamento de XML) e **libcurl** (para requisições HTTP)? Ambas suportam a licença MIT.
> 2. **Framework de Testes:** Existe alguma preferência entre **Google Test (GTest)** ou **Catch2** para o TDD? Ambas são open-source (BSD / BSL).
> 3. **Extração GZip (DistribuicaoDFe):** Como a Sefaz retorna arquivos compactados (Base64 + GZip) na DistribuicaoDFe, adicionarei também uma dependência amigável à licença (como a `zlib`) ou usarei a nativa caso o sistema disponha. Alguma objeção?

Aguardando sua aprovação para iniciarmos a estruturação (criação das pastas e CMake do projeto)!
