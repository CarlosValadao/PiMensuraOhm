#ifndef RGB_H
#define RGB_H

#define RED         0             /**< Define a cor vermelha para os LEDs */
#define GREEN       1             /**< Define a cor verde para os LEDs */
#define BLUE        2             /**< Define a cor azul para os LEDs */
#define YELLOW      3             /**< Define a cor amarela para os LEDs */
#define PURPLE      4             /**< Define a cor roxa para os LEDs */
#define WHITE       5             /**< Define a cor branca para os LEDs */
#define BLUE_MARINE 6             /**< Define a cor azul-marinho para os LEDs */

#include "pico/stdlib.h"

/** 
 * @file rgb.h
 * @brief Este arquivo contém declarações de funções e definições relacionadas a um
 *        LED RGB conectado aos pinos GPIO.
 * 
 * @note A intensidade dos LEDs é dada em porcentagem, variando de 0 a 100, onde
 *       0 significa que o LED estará apagado, e 100 representa a intensidade máxima.
 *       A função `rgb_turn_on_*` pode ser utilizada para controlar a intensidade 
 *       de cada cor individualmente, enquanto as funções `rgb_turn_off_*` 
 *       desligam as cores correspondentes.
 *
 * @author Carlos Valadao
 * @date 17/01/2025
 */


typedef struct
{
    uint red;    /**< Pino do LED vermelho */
    uint green;  /**< Pino do LED verde */
    uint blue;   /**< Pino do LED azul */
} rgb_t;


/**
 * @brief Inicializa os pinos do LED RGB com base nas configurações passadas.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 */
void rgb_init_all(rgb_t *rgb, uint8_t red, uint8_t green, uint8_t blue, float clkdiv, uint16_t wrap);


/**
 * @brief Acende o LED vermelho com a intensidade especificada.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 * @param intensity Intensidade do LED vermelho, variando de 0 a 100.
 */
void rgb_turn_on_red(const rgb_t *pins, uint8_t intensity);


/**
 * @brief Desliga o LED vermelho.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 */
void rgb_turn_off_red(const rgb_t *pins);


/**
 * @brief Acende o LED verde com a intensidade especificada.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 * @param intensity Intensidade do LED verde, variando de 0 a 100.
 */
void rgb_turn_on_green(const rgb_t *pins, uint8_t intensity);


/**
 * @brief Desliga o LED verde.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 */
void rgb_turn_off_green(const rgb_t *pins);


/**
 * @brief Acende o LED azul com a intensidade especificada.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 * @param intensity Intensidade do LED azul, variando de 0 a 100.
 */
void rgb_turn_on_blue(const rgb_t *pins, uint8_t intensity);


/**
 * @brief Desliga o LED azul.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 */
void rgb_turn_off_blue(const rgb_t *pins);


/**
 * @brief Acende o LED branco com a intensidade especificada.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 * @param intensity Intensidade do LED branco, variando de 0 a 100.
 */
void rgb_turn_on_white(const rgb_t *pins, uint8_t intensity);


/**
 * @brief Desliga o LED branco.
 * 
 * @param pins Estrutura contendo os pinos dos LEDs RGB.
 */
void rgb_turn_off_white(const rgb_t *pins);

void turn_off_led_by_gpio(uint8_t pin);

void rgb_turn_on_by_gpio(uint8_t pin, uint8_t intensity);

#endif // RGB_H
