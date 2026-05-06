<h1 align="center">Recife Beat 🎶</h1>

## 📖 Sobre o Jogo

**Recife Beat** é um jogo de ritmo inspirado em *Friday Night Funkin’* e *Guitar Hero*, mas com a alma e a energia da cultura pernambucana! 

O jogador é desafiado a manter o ritmo e a precisão apertando as teclas direcionais em sincronia perfeita com grandes clássicos da música da cidade, acumulando combos e disputando o topo do ranking.

---

## 👥 Equipe de Desenvolvimento

<div align="center">

| Nome | Função | GitHub |
| :--- | :--- | :--- |
| **Eros Amancio** | Desenvolvedor | [@erosamancio](https://github.com/erosamancio) |
| **Lucas Bayma** | Desenvolvedor | [@lucascbayma](https://github.com/lucascbayma) |
| **Luiz Felipe** | Desenvolvedor | [@luiznocode](https://github.com/luiznocode) |
| **Pedro Moreira** | Desenvolvedor | [@Pedromoreira18](https://github.com/PedroMoreira18) |

</div>

---

## 🎯 Jogabilidade e Mecânicas

O jogo conta com um menu interativo completo (Jogar, Ver Ranking e Sair) e entrega uma experiência focada na sincronia:

* **Sincronização Perfeita:** Utilizando a função `GetMusicTimePlayed()` da Raylib, o motor do jogo amarra o tempo real da música com o surgimento das setas animadas na tela, garantindo precisão e feedback visual instantâneo de acerto ou erro.
* **Sistema de Pontuação:** Acerte no tempo exato, mantenha seus combos e tente bater o recorde!

---

## 🧠 Arquitetura e Estruturas de Dados

Este projeto foi desenvolvido aplicando conceitos fundamentais de Ciência da Computação:

* **Listas Encadeadas:** O coração da gameplay. Cada passo da música é um nó contendo `tempo + tecla + próximo passo`. A lista permite manipulação dinâmica (inserir/remover notas no Editor) e é percorrida em tempo real durante o jogo.
* **Algoritmo de Ordenação (QuickSort):** Utilizado no sistema de Ranking (Leaderboard). Escolhido por sua alta eficiência (`O(n log n)`), o QuickSort organiza rapidamente as pontuações dos jogadores para exibir os melhores resultados na tela.
* **Modularidade:** Código limpo e estruturado com funções bem definidas como `carregar_mapa()`, `salvar_mapa()`, `executar_jogo()`, entre outras.

---

## 🕹️ Controles

| Ação | Teclado |
| :--- | :--- |
| **Acertar Notas** | `Setas` (↑, ↓, ←, →) ou `W`, `A`, `S`, `D` |
| **Navegar no Menu** | `Setas` (↑, ↓) |
| **Selecionar Opção** | `Enter` |
| **Sair/Voltar** | `ESC` |

---

## 🛠️ Tecnologias Utilizadas

* **Linguagem:** C
* **Biblioteca Gráfica e Áudio:** [Raylib](https://www.raylib.com/)
* **Compilação:** GCC / Clang / Make

---

## 💻 Como Baixar e Compilar

### 1. Pré-requisitos

Certifique-se de ter instalado em sua máquina:
* **Git**
* **Compilador C** (GCC ou Clang)
* **Raylib** (Instalada via Homebrew no Mac ou `apt` no Linux) e **Make**.

### 2. Clonando o Repositório:

Abra seu terminal e execute:
```bash
git clone [https://github.com/SEU_USUARIO/Recife_Beat.git](https://github.com/SEU_USUARIO/Recife_Beat.git)
cd Recife_Beat
```

### 3. Compilando e Rodando Multiplataforma:

O projeto possui um Makefile que detecta se você está no macOS ou no Linux, compilar o jogo é extremamente simples:
```bash
make
```

### 4. Para limpar os arquivos gerados pela compilação:
```bash
make clean
```