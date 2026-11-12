# Zafra (`libzafra`)

**Zafra** é uma biblioteca C++17 multiplataforma (Linux e Windows) projetada para leitura, detecção automática, validação estrutural e parsing resiliente de documentos fiscais eletrônicos brasileiros (**NF-e**, **NFC-e** e **NFS-e**), incluindo suporte aos novos grupos tributários da **Reforma Tributária (IS, IBS e CBS)**.

---

## Principais Recursos

- **Detecção Automática (`DocumentFactory`):** Identifica automaticamente o modelo do documento fiscal a partir do conteúdo XML (`<NFe>`, `<nfeProc>`, `<NFSe>`, etc.) e retorna uma instância polimórfica (`IDocument`).
- **Parsing de NF-e (Modelo 55) e NFC-e (Modelo 65):**
  - Identificação (`ide`), Emitente (`emit`), Destinatário (`dest`) e Endereços (`enderEmit`, `enderDest`).
  - Detalhamento de Produtos e Serviços (`det` / `prod`) com sanitização automática de quebras de linha em descrições (`xProd`).
  - Tributos do Item (`imposto`): `ICMS` (Regime Normal e Simples Nacional), `IPI`, `II`, `PIS`, `PISST`, `COFINS`, `COFINSST`, `ICMSUFDest`.
  - **Reforma Tributária:** Imposto Seletivo (`IS`) e estrutura enxuta em dois níveis para `IBSCBS` (`IBSUF`, `IBSMun` e `CBS` com alíquota, valor, diferimento, devolução de tributos e redução de alíquota).
- **Abstração XML Desacoplada:** Interfaces `IXmlDocument` e `IXmlNode` que isolam as dependências externas (`tinyxml2` e `libxml2`) da API pública.
- **Gerenciamento Automático de Dependências:** Todas as dependências (`libxml2`, `tinyxml2` e `GoogleTest`) são baixadas e compiladas automaticamente via `FetchContent` do CMake, sem necessidade de configurar submódulos Git manualmente.

---

## Estrutura do Repositório

```text
zafra/
├── CMakeLists.txt              # Configuração principal de build e FetchContent
├── LICENSE                     # Licença MIT e avisos de software de terceiros
├── docs/                       # Documentação e Especificação de Requisitos
├── include/
│   └── zafra/
│       ├── core/               # Interfaces base (IDocument, IXmlDocument, enums)
│       ├── xml/                # Implementação do parser XML
│       ├── document/           # DocumentFactory e classes de documentos (NFeDocument, etc.)
│       └── webservice/         # Interfaces e clientes de comunicação
├── schemas/                    # Schemas XSD oficiais (NF-e v4.00, NFS-e Nacional)
├── src/                        # Implementações (.cpp)
└── tests/                      # Testes unitários e de integração parametrizados (GTest)
```

---

## Pré-requisitos

- **Compilador C++17:**
  - **Linux:** GCC 8+ ou Clang 7+
  - **Windows:** MSVC (Visual Studio 2019/2022) ou MinGW-w64
- **CMake:** versão **3.14** ou superior
- **Git:** necessário para o CMake baixar as dependências via `FetchContent` na primeira configuração

---

## Como Compilar

### 1. Clonar o repositório

```bash
git clone <url-do-repositorio> zafra
cd zafra
```

### 2. Configurar e compilar (Linux / macOS / Windows)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j
```

Os artefatos compilados serão gerados em:
- `build/lib/` — Biblioteca compartilhada (`libzafra.so` no Linux, `zafra.lib` no Windows) ou estática.
- `build/bin/` — Executáveis de teste e DLLs no Windows (`zafra.dll`).

### Opções do CMake

Você pode personalizar a compilação passando flags `-D` ao CMake:

| Opção | Padrão | Descrição |
| :--- | :---: | :--- |
| `BUILD_SHARED_LIBS` | `ON` | Compila a Zafra como biblioteca compartilhada (`.so` / `.dll`). Use `OFF` para biblioteca estática (`.a` / `.lib`). |
| `ZAFRA_BUILD_TESTS` | `ON` *(top-level)* | Compila a suíte de testes com GoogleTest. Desativada automaticamente quando a Zafra é incluída como subprojeto. |

Exemplo compilando como biblioteca estática e sem testes:
```bash
cmake -B build -DBUILD_SHARED_LIBS=OFF -DZAFRA_BUILD_TESTS=OFF
cmake --build build
```

---

## Executando os Testes

Com a opção `ZAFRA_BUILD_TESTS=ON` (padrão), execute a suíte de testes unitários e de integração usando o **CTest**:

```bash
ctest --test-dir build --output-on-failure
```

> **Dica (Testes de Integração com XMLs reais):**
> Qualquer arquivo `.xml` colocado na pasta `tests/data/` será automaticamente descoberto e validado pela suíte parametrizada `NFeIntegrationTest`.

---

## Instalação no Sistema

Para instalar os headers, bibliotecas e arquivos de configuração CMake (`zafraConfig.cmake`):

```bash
cmake --install build --prefix /usr/local
```

*(Ou defina um diretório local com `--prefix ./install`)*

---

## Como Integrar em Outro Projeto CMake

### Opção A: Via `FetchContent` (Recomendado)

No `CMakeLists.txt` do seu projeto:

```cmake
include(FetchContent)

FetchContent_Declare(
    zafra
    GIT_REPOSITORY https://github.com/castorsoft/zafra.git
    GIT_TAG master
)
FetchContent_MakeAvailable(zafra)

target_link_libraries(meu_app PRIVATE zafra)
```

### Opção B: Via `find_package` (Após instalar a biblioteca)

```cmake
find_package(zafra REQUIRED)
target_link_libraries(meu_app PRIVATE zafra::zafra)
```

---

## Exemplo Básico de Uso em C++

```cpp
#include <iostream>
#include "zafra/xml/xml_document.hpp"
#include "zafra/document/document_factory.hpp"
#include "zafra/document/documents/nfe_document.hpp"

int main() {
    zafra::xml::XmlDocument xmlDoc;
    if (!xmlDoc.LoadFromFile("nota_fiscal.xml")) {
        std::cerr << "Falha ao carregar o arquivo XML.\n";
        return 1;
    }

    auto doc = zafra::document::DocumentFactory::DetectAndCreate(xmlDoc.ToString());
    if (!doc || doc->GetType() != zafra::core::DocumentType::NFe) {
        std::cerr << "Documento não é uma NF-e válida.\n";
        return 1;
    }

    auto* nfe = dynamic_cast<zafra::document::NFeDocument*>(doc.get());
    std::cout << "Chave de Acesso: " << nfe->getChaveAcesso() << "\n";
    std::cout << "Versão: " << nfe->GetVersion() << "\n";

    if (auto* emit = nfe->GetEmit()) {
        std::cout << "Emitente: " << emit->GetRazaoSocial()
                  << " (CNPJ/CPF: " << emit->GetCnpjCpf() << ")\n";
    }

    for (const auto* item : nfe->GetDet()) {
        if (const auto* prod = item->GetProd()) {
            std::cout << "Item " << item->GetNItem() << ": "
                      << prod->GetXProd() << " - R$ " << prod->GetVProd() << "\n";
        }

        if (const auto* imp = item->GetImposto()) {
            if (imp->GetIBSCBS().has_value()) {
                auto* ibscbs = imp->GetIBSCBS().value();
                std::cout << "  IBS/CBS CST: " << ibscbs->GetCST()
                          << " | vIBS: " << ibscbs->GetVIBS().value_or(0.0) << "\n";
            }
        }
    }

    return 0;
}
```

---

## Licença

Este projeto é distribuído sob a **Licença MIT** — veja o arquivo LICENSE para mais detalhes.

### Licenças das Dependências
Todas as dependências utilizadas possuem licenças permissivas e compatíveis com a licença MIT:
- **[libxml2](https://gitlab.gnome.org/GNOME/libxml2):** MIT License
- **[tinyxml2](https://github.com/leethomason/tinyxml2):** Zlib License
- **[GoogleTest](https://github.com/google/googletest):** BSD 3-Clause License *(utilizado apenas nos testes)*
