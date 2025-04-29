#include "push_button.h"
#include "pico/stdlib.h"

/**
 * @brief Tempo de debounce do botão em milissegundos.
 */
#define DEBOUNCE_DELAY_MS 300

/**
 * @brief Flag para indicar a primeira configuração de IRQ.
 */
volatile bool FIRST_IRQ_USE = true;

/**
 * @brief Ponteiro para a função de callback de interrupção do botão.
 */
volatile gpio_irq_callback_t PB_IRQ_CALLBACK = NULL;

/**
 * @brief Armazena o tempo da última ativação do botão para controle de debounce.
 */
volatile uint32_t PB_DEBOUNCE_LAST_TIME = 0;

/**
 * @brief Configura o pino do botão como entrada e ativa o pull-up, se necessário.
 *
 * @param button_pin Pino do GPIO ao qual o botão está conectado.
 * @param pullup Define se o pull-up interno deve ser ativado (`true` para ativar).
 */
void pb_config(uint8_t button_pin, bool pullup)
{
    gpio_init(button_pin); // Inicializa o pino do botão
    if(pullup) gpio_pull_up(button_pin); // Ativa pull-up interno se necessário
}

/**
 * @brief Define a função de callback para interrupções do botão.
 *
 * @param callback Função de callback a ser chamada quando o botão for pressionado.
 */
void pb_set_irq_callback(gpio_irq_callback_t callback) { PB_IRQ_CALLBACK = callback; }

/**
 * @brief Habilita a interrupção no botão e registra a callback definida.
 *
 * @param button_pin Pino do GPIO ao qual o botão está conectado.
 */
void pb_enable_irq(uint button_pin)
{
    if(FIRST_IRQ_USE) {
        // Registra a callback e ativa a interrupção na borda de descida
        gpio_set_irq_enabled_with_callback(button_pin, GPIO_IRQ_EDGE_FALL, true, PB_IRQ_CALLBACK);
    } else {
        gpio_set_irq_enabled(button_pin, GPIO_IRQ_EDGE_FALL, true); // Habilita a interrupção sem callback
    }
}

/**
 * @brief Verifica se o botão está pressionado.
 *
 * @param button_pin Pino do GPIO ao qual o botão está conectado.
 * @return `true` se o botão estiver pressionado, `false` caso contrário.
 */
bool pb_is_button_pressed(uint8_t button_pin) { return (!gpio_get(button_pin)); }

/**
 * @brief Verifica se o tempo de debounce já passou desde o último acionamento.
 *
 * @return `true` se o tempo de debounce já passou, `false` caso contrário.
 */
bool pb_is_debounce_delay_over()
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual desde o boot
    bool debounce_time_elapsed = (current_time - PB_DEBOUNCE_LAST_TIME >= DEBOUNCE_DELAY_MS);
    
    if(debounce_time_elapsed)
    {
        PB_DEBOUNCE_LAST_TIME = current_time; // Atualiza o tempo do último acionamento
        return true;
    }
    return false;
}
