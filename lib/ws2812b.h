#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include "hardware/pio.h"
#include "ws2812b_definitions.h"

#define WS2812B_PIN 7             /**< Pino GPIO utilizado para controlar o WS2812B */
#define RED         0             /**< Define a cor vermelha para os LEDs */
#define GREEN       1             /**< Define a cor verde para os LEDs */
#define BLUE        2             /**< Define a cor azul para os LEDs */
#define YELLOW      3             /**< Define a cor amarela para os LEDs */
#define PURPLE      4             /**< Define a cor roxa para os LEDs */
#define WHITE       5             /**< Define a cor branca para os LEDs */
#define BLUE_MARINE 6             /**< Define a cor azul-marinho para os LEDs */

#define init_ws2812b_default(pio) init_ws2812b(pio, WS2812B_PIN)

/** 
 * @file ws2812b.h
 * @brief Este arquivo contém declarações de funções e definições relacionadas
 *        ao dispositivo WS2812B conectado ao pino GPIO do Raspberry Pi Pico W,
 *        controlando uma matriz de LEDs 5x5 (25 LEDs).
 *       
 *        ***************
 *        *** ATENÇÃO ***
 *        ***************
 *       Todos os plots nesta matriz são feitos em um quadrado 3x3 de LEDs
 *       dispostos onde se encontra o #
 * 
 *           . . . . .
 *           . # # # .
 *           . # # # .
 *           . # # # .
 *           . . . . .   
 *
 * @author Carlos Valadao
 * @date 23/01/2025
 */

typedef struct {
    PIO pio;                 /**< Ponteiro para o controlador PIO utilizado para comunicação com os LEDs */
    uint state_machine_id;   /**< ID da máquina de estado (state machine) que controla o envio dos dados para os LEDs */
    uint8_t out_pin;         /**< Pino GPIO ao qual o WS2812B está conectado */
} ws2812b_t;

/**
 * @brief Inicializa o WS2812B configurando o PIO e a máquina de estado.
 * 
 * Esta função configura o controlador PIO e a máquina de estado (state machine) 
 * para controlar a comunicação com a matriz de LEDs WS2812B. O pino de controle é configurado 
 * para enviar os sinais aos LEDs, e a máquina de estado é configurada para esse propósito.
 * 
 * @param pio O controlador PIO que será utilizado para enviar os dados aos LEDs WS2812B.
 * @param pin O pino GPIO utilizado para comunicação com o WS2812B.
 * 
 * @return ws2812b_t* Retorna um ponteiro para a estrutura `ws2812b_t` contendo as configurações do WS2812B.
 */
ws2812b_t *init_ws2812b(PIO pio, uint8_t pin);

/**
 * @brief Desenha uma imagem (glyph) na matriz de LEDs WS2812B.
 * 
 * Esta função envia os dados da imagem (glyph) para o WS2812B e exibe a imagem na matriz de LEDs 5x5,
 * considerando a cor e a intensidade fornecidas. A imagem é uma matriz de 25 elementos (5x5),
 * onde cada elemento representa um LED individual na matriz.
 * 
 * @param ws Ponteiro para a estrutura `ws2812b_t` contendo as configurações do WS2812B.
 * @param glyph A matriz de 25 elementos representando a imagem a ser desenhada nos LEDs.
 * @param color A cor dos LEDs, definida pelas constantes `RED`, `GREEN`, `BLUE`, etc.
 * @param intensity A intensidade dos LEDs, em valor de 0 a 100.
 */
void ws2812b_draw(const ws2812b_t *ws, const uint8_t *glyph, const uint8_t color, const uint8_t intensity);

void ws2812b_draw_b(const uint8_t *glyph, const uint8_t color, const uint8_t intensity);

/**
 * @brief Desliga todos os LEDs da matriz WS2812B.
 * 
 * Esta função envia um comando para desligar todos os LEDs da matriz WS2812B,
 * apagando todos os LEDs conectados ao controlador.
 * 
 * @param ws Ponteiro para a estrutura `ws2812b_t` contendo as configurações do WS2812B.
 */
void ws2812b_turn_off_all(const ws2812b_t *ws);

/**
 * @brief Envia dados para o WS2812B via PIO.
 * 
 * Esta função é responsável por enviar os dados para os LEDs WS2812B através do controlador PIO.
 * Ela utiliza a máquina de estado para enviar um valor de 24 bits, que corresponde à cor e à intensidade dos LEDs.
 * 
 * @param pio O controlador PIO utilizado para enviar os dados.
 * @param sm O identificador da máquina de estado (state machine) que controla a transmissão de dados.
 * @param data O valor de 24 bits a ser enviado, representando a cor e intensidade dos LEDs.
 */
void send_ws2812b_data(PIO pio, uint sm, uint32_t data);

/**
 * @brief Prepara a imagem (glyph) para exibição.
 * 
 * Esta função prepara a imagem a ser exibida nos LEDs, realizando operações de transformação, como
 * a inversão ou qualquer outro ajuste necessário para a correta exibição no WS2812B.
 * 
 * @param glyph A matriz de 25 elementos representando a imagem a ser exibida nos LEDs.
 */
void prepare_glyph(uint8_t *glyph);

#endif // WS2812B_H
