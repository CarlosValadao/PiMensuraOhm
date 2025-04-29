#include "rgb.h"
#include "hardware/pwm.h"

/** 
 * @brief Calcula o valor de intensidade para o LED com base na porcentagem fornecida.
 * 
 * A função converte a intensidade fornecida, que é um valor entre 1 e 100, para um valor correspondente 
 * que será passado para o PWM. A intensidade mínima é 1, para evitar que o valor de intensidade seja zero 
 * (o que poderia desligar o LED).
 *
 * @param intensity Intensidade desejada do LED, variando de 1 a 100.
 * @return O valor de intensidade calculado para o LED, adequado para ser usado com PWM.
 */
static uint16_t calculate_led_intensity_value(uint8_t intensity)
{
    // Evita que a intensidade seja zero, o que desligaria o LED
    intensity = (intensity == 0) ? 1 : intensity;
    
    // Define a intensidade máxima do LED (valor 0xFF00, um valor de 16 bits)
    uint16_t MAX_LED_INTENSITY = 0xFFFF;
    
    // Retorna o valor de intensidade proporcional
    return (uint16_t) ((MAX_LED_INTENSITY * intensity) / 100u);
}

/** 
 * @brief Inicializa os pinos GPIO para controlar os LEDs RGB com PWM.
 * 
 * A função configura os pinos GPIO fornecidos para usar a funcionalidade de PWM 
 * (Pulse Width Modulation) para controle da intensidade dos LEDs.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 */
void rgb_init_all(rgb_t *rgb, uint8_t red, uint8_t green, uint8_t blue, float clkdiv, uint16_t wrap)
{
    uint slice_r, slice_g, slice_b;
    
    // Configura os pinos dos LEDs como pinos PWM
    gpio_set_function(red, GPIO_FUNC_PWM);
    gpio_set_function(green, GPIO_FUNC_PWM);
    gpio_set_function(blue, GPIO_FUNC_PWM);

    // Obtém o número do "slice" para cada pino
    slice_r = pwm_gpio_to_slice_num(red);
    slice_g = pwm_gpio_to_slice_num(green);
    slice_b = pwm_gpio_to_slice_num(blue);

    pwm_set_clkdiv(slice_r, clkdiv);
    pwm_set_clkdiv(slice_g, clkdiv);
    pwm_set_clkdiv(slice_b, clkdiv);
    
    pwm_set_wrap(slice_r, wrap);
    pwm_set_wrap(slice_g, wrap);
    pwm_set_wrap(slice_b, wrap);

    pwm_set_gpio_level(red, 0);
    pwm_set_gpio_level(green, 0);
    pwm_set_gpio_level(blue, 0);

    pwm_set_enabled(slice_r, true);
    pwm_set_enabled(slice_g, true);
    pwm_set_enabled(slice_b, true);

    rgb->red = red;
    rgb->green = green;
    rgb->blue = blue;
}

/** 
 * @brief Acende o LED vermelho com a intensidade especificada.
 * 
 * A intensidade é convertida para um valor PWM adequado e então o LED vermelho é aceso 
 * na intensidade desejada.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 * @param intensity Intensidade do LED vermelho, variando de 0 a 100.
 */
void rgb_turn_on_red(const rgb_t *pins, uint8_t intensity)
{
    //uint16_t led_intensity = calculate_led_intensity_value(intensity); // Calcula a intensidade do LED
    pwm_set_gpio_level(pins->red, intensity); // Acende o LED vermelho com a intensidade calculada
}

/** 
 * @brief Desliga o LED vermelho.
 * 
 * Define o valor de PWM do LED vermelho como 0, apagando-o.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 */
void rgb_turn_off_red(const rgb_t *pins)
{
    pwm_set_gpio_level(pins->red, 0); // Desliga o LED vermelho
}

/** 
 * @brief Acende o LED verde com a intensidade especificada.
 * 
 * A intensidade é convertida para um valor PWM adequado e então o LED verde é aceso 
 * na intensidade desejada.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 * @param intensity Intensidade do LED verde, variando de 0 a 100.
 */
void rgb_turn_on_green(const rgb_t *pins, uint8_t intensity)
{
    //uint16_t led_intensity = calculate_led_intensity_value(intensity); // Calcula a intensidade do LED
    pwm_set_gpio_level(pins->green, intensity); // Acende o LED verde com a intensidade calculada
}

/** 
 * @brief Desliga o LED verde.
 * 
 * Define o valor de PWM do LED verde como 0, apagando-o.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 */
void rgb_turn_off_green(const rgb_t *pins)
{
    pwm_set_gpio_level(pins->green, 0); // Desliga o LED verde
}

/** 
 * @brief Acende o LED azul com a intensidade especificada.
 * 
 * A intensidade é convertida para um valor PWM adequado e então o LED azul é aceso 
 * na intensidade desejada.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 * @param intensity Intensidade do LED azul, variando de 0 a 100.
 */
void rgb_turn_on_blue(const rgb_t *pins, uint8_t intensity)
{
    //uint16_t led_intensity = calculate_led_intensity_value(intensity); // Calcula a intensidade do LED
    pwm_set_gpio_level(pins->blue, intensity); // Acende o LED azul
}

/** 
 * @brief Desliga o LED azul.
 * 
 * Define o valor de PWM do LED azul como 0, apagando-o.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 */
void rgb_turn_off_blue(const rgb_t *pins)
{
    pwm_set_gpio_level(pins->blue, 0); // Desliga o LED azul
}

/** 
 * @brief Acende o LED branco (combinando as 3 cores RGB) com a intensidade especificada.
 * 
 * A intensidade para o LED branco é calculada como a média de 1/3 da intensidade
 * de cada cor (R, G e B). O LED branco é composto por uma combinação igual das 3 cores RGB.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 * @param intensity Intensidade do LED branco, variando de 0 a 100.
 */
void rgb_turn_on_white(const rgb_t *pins, uint8_t intensity) {
    uint16_t led_intensity = calculate_led_intensity_value(intensity / 3u); // Calcula a intensidade para branco
    pwm_set_gpio_level(pins->red, led_intensity);   // Acende o LED vermelho para o branco
    pwm_set_gpio_level(pins->green, led_intensity); // Acende o LED verde para o branco
    pwm_set_gpio_level(pins->blue, led_intensity);  // Acende o LED azul para o branco
}

/** 
 * @brief Desliga o LED branco.
 * 
 * Desliga todos os LEDs (vermelho, verde e azul) que compõem o LED branco.
 * 
 * @param pins Ponteiro para a estrutura contendo os pinos GPIO dos LEDs.
 */
void rgb_turn_off_white(const rgb_t *pins) {
    pwm_set_gpio_level(pins->red, 0);   // Desliga o LED vermelho
    pwm_set_gpio_level(pins->green, 0); // Desliga o LED verde
    pwm_set_gpio_level(pins->blue, 0);  // Desliga o LED azul
}

void turn_off_led_by_gpio(uint8_t pin)
{
    pwm_set_gpio_level(pin, 0);
}

void rgb_turn_on_by_gpio(uint8_t pin, uint8_t intensity)
{
    uint16_t led_intensity = calculate_led_intensity_value(intensity);
    pwm_set_gpio_level(pin, led_intensity);
}