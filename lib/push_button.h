#ifndef PUSH_BUTTON_C
#define PUSH_BUTTON_C

#include <stdint.h>
#include "pico/stdlib.h"

// Definição dos pinos de conexão dos botões
#define BUTTON_A_PIN 5      /**< Define o pino GPIO do botão A */
#define BUTTON_B_PIN 6      /**< Define o pino GPIO do botão B */
#define BUTTON_A 5          /**< Define o valor lógico do botão A */
#define BUTTON_B 6          /**< Define o valor lógico do botão B */

// Macros para configuração dos botões com resistores pull-up
#define pb_config_btn_a() pb_config(BUTTON_A_PIN, true)  /**< Configura o botão A com pull-up */
#define pb_config_btn_b() pb_config(BUTTON_B_PIN, true)  /**< Configura o botão B com pull-up */
#define pb_is_button_a_pressed() pb_is_button_pressed(BUTTON_A)

/**
 * @file push_button.c
 * @brief Este arquivo contém funções e definições que permitem
 *        operar e gerenciar push buttons, configurando os 
 *        botões conectados ao GPIO e manipulando-os com interrupção
 * 
 * @author Carlos Valadao
 * @date 31/01/2025
 */

/** Variáveis externas */
extern volatile bool FIRST_IRQ_USE; /**< Variável de controle para a primeira utilização da interrupção */
extern volatile gpio_irq_callback_t PB_IRQ_CALLBACK; /**< Função callback para a interrupção do botão */
extern volatile uint32_t PB_DEBOUNCE_LAST_TIME;

/**
 * @brief Configura o pino do botão para o modo desejado (com ou sem pull-up).
 * 
 * A função configura o pino do botão, e habilita um resistor pull-up ou pull-down
 * dependendo do valor de "pullup".
 * 
 * @param button_pin Pino GPIO do botão.
 * @param pullup Ativa o resistor pull-up se verdadeiro, ou pull-down se falso.
 */
void pb_config(uint8_t button_pin, bool pullup);

/**
 * @brief Configura a função de callback que será chamada quando qualquer botão for pressionado.
 * 
 * A função "pb_set_irq_callback" define a função de callback que será associada à interrupção
 * dos botões. Essa função é chamada quando um botão é pressionado, e permite a execução de
 * ações quando isso ocorre.
 * 
 * @param callback Função callback a ser chamada durante a interrupção do botão.
 */
void pb_set_irq_callback(gpio_irq_callback_t callback);


/**
 * @brief Habilita a interrupção para o botão configurado.
 * 
 * A função ativa a interrupção para o pino do botão, permitindo que a interrupção seja
 * disparada quando o botão for pressionado ou liberado.
 * 
 * @param button_pin Pino GPIO do botão que terá a interrupção habilitada.
 */
void pb_enable_irq(uint button_pin);

bool pb_is_button_pressed(uint8_t button_pin);

bool pb_is_debounce_delay_over();

#endif //PUSH_BUTTON_H
