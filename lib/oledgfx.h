#ifndef OLEDGFX_H
#define OLEDGFX_H

#include "ssd1306.h"
#include <stdint.h>

/**
 * @file oledgfx.h
 * @brief Interface para renderização gráfica em displays OLED SSD1306.
 *
 * Este cabeçalho define funções para inicialização, desenho e manipulação 
 * de gráficos em um display OLED controlado pelo driver SSD1306.
 *
 * @author Carlos Valadão
 * @date 2025-02-16
 * @version 1.0
 * @license GNU General Public License v3.0 (GPLv3)
 * 
 * @copyright
 * Copyright (C) 2025 Carlos Valadão
 *
 * Este programa é software livre: você pode redistribuí-lo e/ou modificá-lo 
 * sob os termos da Licença Pública Geral GNU, conforme publicada pela 
 * Free Software Foundation, na versão 3 da licença, ou (a seu critério) 
 * qualquer versão posterior.
 *
 * Este programa é distribuído na esperança de que seja útil, 
 * mas SEM QUALQUER GARANTIA; sem mesmo a garantia implícita de 
 * COMERCIALIZAÇÃO ou ADEQUAÇÃO A UM DETERMINADO FIM. 
 * Consulte a Licença Pública Geral GNU para mais detalhes.
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral GNU 
 * junto com este programa. Se não, veja <https://www.gnu.org/licenses/>.
 */

/**
 * @defgroup OLED_Graphics Módulo de Gráficos OLED
 * @brief Conjunto de funções para manipulação de gráficos no display OLED SSD1306.
 * @{
 */

/** 
 * @brief Valor inválido para a posição do cursor.
 */
#define INVALID_CURSOR ((int8_t) (-1))

#define CURSOR_SIDE 8
#define BORDER_THICK 3
#define BORDER_LIGHT 1

/** 
 * @brief Última posição X do cursor no display OLED.
 */
extern volatile int8_t last_cursor_x;

/** 
 * @brief Última posição Y do cursor no display OLED.
 */
extern volatile int8_t last_cursor_y;

/**
 * @brief Inicializa o display OLED SSD1306.
 *
 * Configura o barramento I2C e inicializa o driver SSD1306 para exibição gráfica.
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] i2c Ponteiro para a instância do barramento I2C.
 * @param[in] baudrate Taxa de comunicação I2C.
 * @param[in] sda Pino GPIO utilizado para SDA.
 * @param[in] scl Pino GPIO utilizado para SCL.
 * @param[in] address Endereço I2C do display OLED.
 */
void oledgfx_init_all(ssd1306_t *ssd, i2c_inst_t *i2c, uint baudrate, uint8_t sda, uint8_t scl, uint8_t address);

/**
 * @brief Limpa a tela do display OLED.
 *
 * Preenche o display inteiro com cor preta (desligando todos os pixels).
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 */
void oledgfx_clear_screen(ssd1306_t *ssd);

/**
 * @brief Desenha o cursor na posição especificada.
 *
 * O cursor é representado por um quadrado na coordenada (x, y).
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] x Posição X do cursor.
 * @param[in] y Posição Y do cursor.
 */
void oledgfx_draw_cursor(ssd1306_t *ssd, uint8_t x, uint8_t y);

/**
 * @brief Atualiza a posição do cursor no display.
 *
 * Apaga a posição anterior do cursor e desenha a nova posição.
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] x Nova posição X do cursor.
 * @param[in] y Nova posição Y do cursor.
 */
void oledgfx_update_cursor(ssd1306_t *ssd, uint8_t x, uint8_t y);

/**
 * @brief Desenha uma linha vertical na tela.
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] x Posição X inicial da linha.
 * @param[in] thickness Espessura da linha.
 */
void oledgfx_draw_vline(ssd1306_t *ssd, uint8_t x, uint8_t thickness);

/**
 * @brief Desenha uma linha horizontal na tela.
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] y Posição Y inicial da linha.
 * @param[in] thickness Espessura da linha.
 */
void oledgfx_draw_hline(ssd1306_t *ssd, uint8_t y, uint8_t thickness);

/**
 * @brief Apaga uma linha vertical previamente desenhada.
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] y Posição Y da linha a ser apagada.
 * @param[in] thickness Espessura da linha.
 */
void oledgfx_clear_vline(ssd1306_t *ssd, uint8_t y, uint8_t thickness);

/**
 * @brief Apaga uma linha horizontal previamente desenhada.
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] x Posição X da linha a ser apagada.
 * @param[in] thickness Espessura da linha.
 */
void oledgfx_clear_hline(ssd1306_t *ssd, uint8_t x, uint8_t thickness);

/**
 * @brief Atualiza a renderização do display OLED.
 *
 * Envia os dados em buffer para o display físico, aplicando todas as alterações gráficas feitas anteriormente.
 *
 * @param[out] ssd Ponteiro para a estrutura do display SSD1306.
 */
void oledgfx_render(ssd1306_t *ssd);

/**
 * @brief Desenha uma borda com espessura ajustável no display OLED SSD1306.
 *
 * Esta função desenha uma borda ao redor da tela OLED, onde o usuário pode definir 
 * apenas a espessura da borda, mas não suas dimensões. A borda se estende para dentro 
 * da tela conforme o valor de `thickness`.
 *
 * @details A função desenha quatro segmentos de linha para formar a borda da tela: 
 * uma linha superior, uma inferior, uma esquerda e uma direita. A espessura da borda 
 * é aplicada uniformemente em todas as direções, começando das extremidades e avançando 
 * para o centro da tela.
 *
 * @note A espessura máxima recomendada depende da resolução do display (128x64 pixels) 
 * e deve ser escolhida de forma que não comprometa a área útil da tela.
 *
 * @warning Se `thickness` for muito alto, pode cobrir completamente a tela, tornando 
 * o conteúdo ilegível.
 *
 * @param[in,out] ssd Ponteiro para a estrutura do display SSD1306.
 * @param[in] thickness Espessura da borda em pixels.
 */
void oledgfx_draw_border(ssd1306_t *ssd, uint8_t thickness);

void oledgfx_draw_string(ssd1306_t *ssd, const char* str, uint8_t x, uint8_t y);

void oledgfx_draw_resistor(ssd1306_t *ssd, uint8_t x, uint8_t y);

void oledgfx_draw_ohm_symbol(ssd1306_t *ssd, uint8_t x, uint8_t y);

/** @} */ // Fim do grupo "OLED_Graphics"

#endif // OLEDGFX_H
