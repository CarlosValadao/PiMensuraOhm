#include "ws2812b.h"
#include "hardware/clocks.h"
#include <stdlib.h>
#include "../generated/ws2812b.pio.h"

/**
 * @brief Inverte horizontalmente a matriz 5x5 de LEDs.
 * 
 * @param matrix Ponteiro para a matriz de LEDs representada como um array de 25 elementos.
 * 
 * @note Esta operação é necessária para ajustar a orientação do padrão de LEDs
 *       quando exibido na matriz. A inversão ocorre em pares de elementos em posições
 *       simétricas em relação ao eixo central vertical.
 */
static void fliplr(uint8_t *matrix) {
    uint8_t temp;

    // Troca os elementos da primeira linha
    temp = matrix[5];
    matrix[5] = matrix[9]; 
    matrix[9] = temp;

    temp = matrix[6];
    matrix[6] = matrix[8]; 
    matrix[8] = temp;

    // Troca os elementos da terceira linha
    temp = matrix[15];
    matrix[15] = matrix[19]; 
    matrix[19] = temp;

    temp = matrix[16];
    matrix[16] = matrix[18]; 
    matrix[18] = temp;
}


/**
 * @brief Compoe o valor do LED com base na cor e intensidade fornecida.
 * 
 * Esta função calcula o valor do LED para ser enviado ao controlador WS2812B. A intensidade é convertida para um valor entre 0 e 255 com base na porcentagem fornecida.
 * O valor final depende da cor especificada (vermelho, verde, azul, etc.) e é retornado como um valor composto.
 * 
 * @param color Cor do LED (vermelho, verde, azul, amarelo, roxo, branco ou azul-marinho).
 * @param intensity Intensidade do LED em porcentagem (0-100).
 * @return uint32_t Valor composto do LED.
 */
static uint32_t ws2812b_compose_led_value(uint8_t color, uint8_t intensity)
{
    uint32_t composite_value;
    uint8_t intensity_value = (intensity*255)/100; // Mapeia a intensidade para um valor entre 0 e 255

    // Dependendo da cor fornecida, o valor do LED é composto alterando as posições dos bits
    switch (color)
    {
    case RED:
        composite_value = intensity_value << 16; // Coloca o valor da intensidade na posição do vermelho
        break;
    case GREEN:
        composite_value = intensity_value << 24; // Coloca o valor da intensidade na posição do verde
        break;
    case BLUE:
        composite_value = intensity_value << 8;  // Coloca o valor da intensidade na posição do azul
        break;
    case YELLOW:
        intensity_value /= 2u; 
        composite_value = ((intensity_value << 16) | (intensity_value << 24)); // Mistura vermelho e verde
        break;
    case PURPLE:
        intensity_value /= 2u;
        composite_value = ((intensity_value << 8) | (intensity_value << 16)); // Mistura vermelho e azul
        break;
    case WHITE:
        intensity_value /= 3u;
        composite_value = ((intensity_value << 24) | (intensity_value << 16) | (intensity_value << 8)); // Mistura as três cores
        break;
    case BLUE_MARINE:
        intensity_value /= 2u;
        composite_value = ((intensity_value << 24) | (intensity_value << 8)); // Mistura verde e azul
        break;
    default:
        composite_value = 0; // Caso a cor não seja reconhecida, retorna 0
        break;
    }
    return composite_value; // Retorna o valor composto do LED
}

/**
 * @brief Desenha a matriz de LEDs (glyph) com base nas cores e intensidade fornecidas.
 * 
 * Esta função percorre a matriz de LEDs (glyph) e envia os dados para o controlador WS2812B. 
 * O valor de cada LED é calculado usando a cor e intensidade fornecidas. A matriz é percorrida de trás para frente.
 * 
 * @param ws Ponteiro para o controlador WS2812B.
 * @param glyph Matriz de 25 elementos (5x5) que representa o padrão de LEDs a ser exibido.
 * @param color Cor do LED (vermelho, verde, azul, etc.).
 * @param intensity Intensidade do LED (0-100%).
 */
void ws2812b_draw(const ws2812b_t *ws, const uint8_t *glyph, const uint8_t color, const uint8_t intensity)
{
    uint8_t i;
    uint32_t composite_value;
    
    // Percorre cada posição do "glyph" (matriz 5x5) e acende o LED correspondente
    for(i = 0; i < 25; i++) {
        // Se o valor da posição for 1, acende o LED com o valor calculado
        if(glyph[24-i] == 1) {
            composite_value = ws2812b_compose_led_value(color, intensity); // Calcula o valor para a cor e intensidade
            send_ws2812b_data(ws->pio, ws->state_machine_id, composite_value); // Envia o valor do LED via PIO
        }
        else send_ws2812b_data(ws->pio, ws->state_machine_id, 0); // Se o LED estiver apagado, envia 0
    }
}

/**
 * @brief Apaga todos os LEDs da matriz (configura todos os LEDs como 0).
 * 
 * Esta função desliga todos os LEDs da matriz WS2812B enviando o valor 0 para cada um.
 * 
 * @param ws Ponteiro para o controlador WS2812B.
 */
void ws2812b_turn_off_all(const ws2812b_t *ws)
{
    uint8_t i;
    // Envia o valor 0 para todos os LEDs, apagando-os
    for(i = 0; i < 25; i++) send_ws2812b_data(ws->pio, ws->state_machine_id, 0);
}

/**
 * @brief Envia os dados para a máquina de estado (state machine) do PIO, acionando os LEDs.
 * 
 * Esta função envia o valor composto de dados para a PIO do Raspberry Pi Pico, controlando a saída dos LEDs.
 * 
 * @param pio Ponteiro para o PIO.
 * @param sm Número da máquina de estado (state machine).
 * @param data Dados a serem enviados para os LEDs.
 */
void send_ws2812b_data(PIO pio, uint sm, uint32_t data)
{
    pio_sm_put_blocking(pio, sm, data); // Envia o dado para o PIO, bloqueando até o envio ser completado
}

/**
 * @brief Inicializa o controlador WS2812B e configura o PIO (Programmable Input/Output).
 * 
 * Esta função inicializa a configuração do PIO para controlar o WS2812B, incluindo a definição de pinos, configuração de clock e a máquina de estado do PIO.
 * 
 * @param pio Ponteiro para o PIO a ser utilizado.
 * @param pin Pino GPIO conectado ao LED WS2812B.
 * @return Ponteiro para o controlador WS2812B inicializado.
 */
ws2812b_t *init_ws2812b(PIO pio, uint8_t pin)
{
    ws2812b_t *ws = malloc(sizeof(ws2812b_t)); // Aloca memória para a estrutura que representará o controlador WS2812B
    uint offset = pio_add_program(pio, &ws2812_program); // Adiciona o programa WS2812 ao PIO
    uint sm = pio_claim_unused_sm(pio, true); // Requisita uma máquina de estado livre no PIO

    pio_sm_config c = ws2812_program_get_default_config(offset); // Obtém a configuração padrão do programa WS2812

    // Configura o pino para ser utilizado como saída de controle para o WS2812
    sm_config_set_set_pins(&c, pin, 1);
    
    pio_gpio_init(pio, pin); // Inicializa o pino no PIO

    // Configura a direção do pino para saída
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    // Configura a frequência do clock do PIO para 8 MHz (8.000.000 Hz)
    float div = clock_get_hz(clk_sys) / 8000000.0;
    sm_config_set_clkdiv(&c, div); // Define o divisor do clock do PIO

    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX); // Configura o PIO para usar a FIFO de transmissão

    sm_config_set_out_shift(&c, false, true, 24); // Configura o shift de 24 bits na saída

    sm_config_set_out_special(&c, true, false, false); // Define a configuração especial de saída

    // Inicializa a máquina de estado com a configuração definida
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true); // Habilita a máquina de estado

    // Inicializa o controlador WS2812B
    ws->out_pin = pin;
    ws->state_machine_id = sm;
    ws->pio = pio;

    return ws; // Retorna o controlador WS2812B configurado
}

/**
 * @brief Prepara o "glyph" (matriz de LEDs) invertendo horizontalmente.
 * 
 * Esta função chama a função `fliplr` para inverter a matriz de LEDs horizontalmente.
 * 
 * @param glyph Matriz de LEDs a ser invertida.
 */
void prepare_glyph(uint8_t *glyph)
{
    fliplr(glyph); // Inverte a matriz horizontalmente
}

/**
 * @brief Desenha a matriz de LEDs no pino 0 do PIO0 com a cor e intensidade fornecidas.
 * 
 * Esta função percorre a matriz de LEDs (glyph) e envia os dados para o controlador WS2812B.
 * 
 * @param glyph Matriz de 25 elementos (5x5) que representa o padrão de LEDs a ser exibido.
 * @param color Cor do LED (vermelho, verde, azul, etc.).
 * @param intensity Intensidade do LED (0-100%).
 */
void ws2812b_draw_b(const uint8_t *glyph, const uint8_t color, const uint8_t intensity)
{
    uint8_t i;
    uint32_t composite_value;
    // Percorre a matriz de LEDs e envia os dados para acender os LEDs
    for(i = 0; i < 25; i++) {
        if(glyph[24-i] == 1) {
            composite_value = ws2812b_compose_led_value(color, intensity); // Calcula o valor do LED
            send_ws2812b_data(pio0, 0, composite_value); // Envia o dado para o pino 0 do PIO0
        }
        else send_ws2812b_data(pio0, 0, 0); // Envia 0 para apagar o LED
    }
}
