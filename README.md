# PicoMensuraOhm ![Demonstração do PicoMensuraOhm](https://youtu.be/kOCLvDyadcU)
  
## Sistema de medição de resistores com Raspberry Pi Pico W
  
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Pico SDK: 2.1.0](https://img.shields.io/badge/Pico%20SDK-2.1.0-blue)](https://github.com/raspberrypi/pico-sdk)
[![Status: Em Desenvolvimento](https://img.shields.io/badge/Status-Em%20Desenvolvimento-green)](https://github.com/seu-usuario/PicoMensuraOhm)

## 📋 Sobre o Projeto

O **PicoMensuraOhm** é um sistema avançado para medição de resistência elétrica, desenvolvido com o microcontrolador RP2040 (Raspberry Pi Pico W). Através de um divisor de tensão e um resistor de referência de 10kΩ, o sistema mede com precisão resistores conectados e apresenta os resultados visualmente em um display OLED e em uma matriz de LEDs coloridos.

Este projeto foi criado com o objetivo de facilitar a identificação e medição de resistores em ambiente educacional e de laboratório, proporcionando uma solução visual e interativa que vai além dos multímetros tradicionais.


## 🌟 Funcionalidades

- **Medição precisa** utilizando o ADC de 12 bits do RP2040
- **Visualização da resistência medida** em display OLED SSD1306
- **Identificação automática** do valor nominal mais próximo (tabela E24)
- **Representação visual** dos anéis coloridos do resistor na matriz de LEDs WS2812b
- **Interface interativa** com botões para navegação nos menus
- **Otimizado para desempenho** em microcontroladores sem FPU
- **Algoritmo de mínima diferença** para determinar o valor comercial mais próximo na série E24
- **Visualização das 12 cores padrão** de resistores na matriz de LEDs

## 🛠️ Componentes Utilizados

- **Raspberry Pi Pico W** (microcontrolador RP2040)
- **Display OLED SSD1306** (conexão I2C)
- **Matriz de LEDs WS2812b 5x5**
- **Resistor de referência de 10kΩ** para o divisor de tensão
- **BitDogLab** (placa base com botões de controle)
- **Conectores** para resistores de diferentes formatos

### Especificações Técnicas

| Componente         | Especificação                                         |
|--------------------|-------------------------------------------------------|
| Microcontrolador    | RP2040 (Dual-core ARM Cortex M0+ @ 133MHz)           |
| Memória            | 264KB RAM                                             |
| Display            | OLED SSD1306 128x64 pixels                            |
| LEDs               | WS2812b RGB endereçáveis                              |
| ADC                | 12-bit (4096 níveis)                                  |
| Alimentação        | USB ou bateria externa (3.3V)                         |
| Linguagem          | C com Pico SDK 2.1.0                                  |

## 🔍 Princípio de Funcionamento

O PicoMensuraOhm funciona com base no princípio do divisor de tensão. Quando um resistor desconhecido é conectado em série com um resistor de referência conhecido (10kΩ), a tensão no ponto intermediário varia de acordo com a resistência desconhecida.

### Processo de Medição

1. O resistor a ser medido é conectado ao sistema
2. O ADC do RP2040 mede a tensão no divisor
3. O firmware calcula a resistência usando a fórmula do divisor de tensão
4. O valor calculado é comparado com a tabela E24 para determinar o valor nominal mais próximo usando o algoritmo de mínima diferença
5. Os dados são exibidos no display OLED e a representação visual dos anéis coloridos é mostrada na matriz de LEDs

### Faixa de Medição

O sistema foi projetado para medir com precisão resistores na faixa de:

- **Mínimo:** 560Ω
- **Máximo:** 100kΩ
- **Observação:** Há uma perda de precisão devido à sobrecarga que os periféricos geram sobre a placa

## 🎨 Representação de Cores dos Resistores

O sistema representa visualmente as 12 cores padrão dos anéis de resistores na matriz de LEDs WS2812b:

1. **Preto** (0)
2. **Marrom** (1)
3. **Vermelho** (2)
4. **Laranja** (3)
5. **Amarelo** (4)
6. **Verde** (5)
7. **Azul** (6)
8. **Violeta** (7)
9. **Cinza** (8)
10. **Branco** (9)
11. **Dourado** (multiplicador: ×0.1, tolerância: ±5%)
12. **Prata** (multiplicador: ×0.01, tolerância: ±10%)

Estas cores são exibidas nos LEDs RGB endereçáveis, proporcionando uma representação visual precisa do código de cores do resistor medido.

## 🚀 Como Instalar

### 1. Pré-requisitos

- [Pico SDK](https://github.com/raspberrypi/pico-sdk) v2.1.0 ou superior
- [CMake](https://cmake.org/download/) (3.12 ou superior)
- Compilador C/C++ (GCC ARM)
- Ferramentas de build (Make, Ninja)
- [Python 3](https://www.python.org/downloads/) (para scripts auxiliares)

### 2. Download e Configuração do Ambiente

```bash
# Instale as dependências (Ubuntu/Debian)
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

# Clone o Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init
cd ..

# Configure o caminho do SDK (adicione ao seu .bashrc ou .zshrc)
export PICO_SDK_PATH=/caminho/para/pico-sdk

# Clone o repositório
git clone https://github.com/seu-usuario/PicoMensuraOhm.git
cd PicoMensuraOhm

# Configure o projeto com CMake
mkdir build
cd build
cmake ..

# Compile
make -j4
```

### O arquivo PicoMensuraOhm.uf2 será gerado na pasta build
