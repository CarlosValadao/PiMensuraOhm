/*
 * Ohmímetro utilizando o ADC da BitDogLab
 * Autor: Wilton Lacerda Silva
 * 
 * Este código implementa um ohmímetro baseado no Raspberry Pi Pico (RP2040) que:
 * 1. Mede resistências desconhecidas usando um divisor de tensão
 * 2. Exibe os resultados em um display OLED
 * 3. Oferece uma interface de menu com botões
 * 4. Mostra o código de cores correspondente ao valor comercial mais próximo
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <string.h>
 
 // Bibliotecas específicas do Raspberry Pi Pico
 #include "pico/stdlib.h"
 #include "pico/bootrom.h"    // Para função de reset para bootloader
 #include "hardware/adc.h"     // Para acesso ao conversor analógico-digital
 #include "hardware/i2c.h"     // Para comunicação I2C com o display
 
 // Bibliotecas personalizadas para componentes
 #include "lib/push_button.h"  // Para tratamento dos botões
 #include "lib/oledgfx.h"      // Para gráficos no display OLED
 #include "lib/ws2812b.h"      // Para controle de LEDs RGB (não utilizado neste código)
 
 // Configuração do hardware - I2C para o display OLED
 #define I2C_PORT i2c1         // Porta I2C utilizada
 #define I2C_SDA 14            // Pino de dados (SDA)
 #define I2C_SCL 15            // Pino de clock (SCL)
 #define SSD1306_ADDR 0x3C     // Endereço I2C do display OLED
 
 // Configuração do ADC para medição
 #define ADC_PIN 28            // Pino GPIO conectado ao divisor de tensão
 
 // Definições dos botões
 #define BUTTON_A 5            // GPIO para botão A
 #define BUTTON_B 6            // GPIO para botão B
 #define PB_JOYSTICK 22        // GPIO para botão do joystick
 
 // Definições para controle de menus
 #define FIRST_MENU_COLORS  1
 #define SECOND_MENU_COLORS 2
 #define THIRD_MENU_COLORS  3
 #define UNDEFINED_MENU     0
 
 // Definições para os LEDs RGB (não utilizados no código principal)
 #define RED_PIN   13
 #define BLUE_PIN  12
 #define GREEN_PIN 11
 
 // Macros para verificação de botões pressionados
 #define JOYSTICK_PB_PRESSED (gpio == 22)
 #define BUTTON_A_PRESSED (gpio == BUTTON_A)
 #define BUTTON_B_PRESSED (gpio == BUTTON_B)
 
 // Macro para cálculo do deslocamento do símbolo Ohm (Ω) na tela
 #define CALC_OMEGA_OFFSET(x) (size_t)(8 * strlen(x))
 
 // Número de valores na série E24 utilizada
 #define NUM_RESISTORS 49
 
 // Macro para converter caractere ASCII para valor numérico
 #define CHAR_TO_NUM(value) ((uint8_t)(value - 48))
 
 // Variáveis globais para controle de estado
 static volatile uint8_t control_submenus = UNDEFINED_MENU;  // Controle do menu atual
 static volatile bool showing_color_menu = false;            // Flag para menu de cores
 
 // Valores comerciais de resistores (série E24)
 static const uint32_t e24_resistance_values[] = {
     560, 620, 680, 750, 820, 910, 1000, 1100,
     1200, 1300, 1500, 1600, 1800, 2000,
     2200, 2400, 2700, 3000, 3300, 3600,
     3900, 4300, 4700, 5100, 5600, 6200,
     6800, 7500, 8200, 9100, 18000, 20000, 22000,
     24000, 27000, 30000, 33000, 36000, 39000,
     43000, 47000, 51000, 56000, 62000, 68000,
     75000, 82000, 91000, 100000
 };
 
 // Códigos de cores para resistores (abreviaturas em inglês)
 static const char *ring_colors[] = { 
     "Bk", "Bn", "R", "Og", "Y", "G", "B", "Vt", "Gy", "Wt", "Gd", "S" 
 };
 
 // Ponteiro global para a estrutura do display OLED
 static ssd1306_t *ssd_global = NULL;
 
 // Parâmetros para medição de resistência
 uint16_t R_conhecido = 10000;      // Resistor conhecido (10kΩ) do divisor de tensão
 uint32_t R_x = 0;                  // Resistor desconhecido (a ser medido)
 uint32_t resistance_sum = 0;       // Acumulador para média das leituras
 uint32_t resistence_avg = 0;       // Média das leituras do ADC
 uint16_t ADC_RESOLUTION = 4095;    // Resolução do ADC (12 bits)
 
 // Protótipos de funções
 void gpio_irq_handler(uint gpio, uint32_t events);
 void draw_menu_colors(ssd1306_t *ssd);
 void draw_menu_colors_scaffold(ssd1306_t *ssd);
 uint32_t find_comercial_value(uint32_t value);
 uint8_t find_resistance_power_multiplyer(const char* sresistance);
 
 /**
  * Função principal
  * Inicializa hardware, configura interrupções e executa loop de medição
  */
 int main() {
     // Configuração dos botões com tratamento de debounce
     pb_config_btn_b();
     pb_config_btn_a();
     pb_config(PB_JOYSTICK, true);
     
     // Configuração das interrupções dos botões
     gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
     pb_set_irq_callback(&gpio_irq_handler);
     pb_enable_irq(BUTTON_A);
     pb_enable_irq(BUTTON_B);
     pb_enable_irq(PB_JOYSTICK);
 
     // Inicialização do display OLED via I2C
     ssd1306_t ssd;
     i2c_init(I2C_PORT, 400000);  // I2C a 400kHz
     
     // Configuração dos pinos I2C
     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(I2C_SDA);
     gpio_pull_up(I2C_SCL);
     
     // Inicialização do display OLED
     ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT);
     ssd1306_config(&ssd);
     ssd1306_send_data(&ssd);
     ssd_global = &ssd;
     
     // Limpa o display
     ssd1306_fill(&ssd, false);
     ssd1306_send_data(&ssd);
     
     // Inicialização do ADC
     adc_init();
     adc_gpio_init(ADC_PIN);  // Configura pino 28 como entrada analógica
     
     // Buffers para strings de exibição
     char str_x[5];  // Para valor medido
     char str_y[5];  // Para valor comercial
     
     // Loop principal
     while (true) {
         resistance_sum = 0;
         adc_select_input(2);  // Seleciona o canal ADC 2 (GPIO 28)
         
         // Faz 500 leituras para calcular uma média (melhora precisão)
         for (int i = 0; i < 500; i++) {
             resistance_sum += adc_read();
             sleep_ms(1);
         }
         resistence_avg = resistance_sum / 500;
         
         // Calcula a resistência desconhecida usando fórmula do divisor de tensão:
         // R_x = R_conhecido * (ADC_value / (ADC_RESOLUTION - ADC_value))
         R_x = (R_conhecido * resistence_avg) / (ADC_RESOLUTION - resistence_avg);
         
         // Converte os valores para strings
         sprintf(str_x, "%u", R_x);  // Valor medido
         sprintf(str_y, "%u", find_comercial_value(R_x));  // Valor comercial mais próximo
         
         // Atualização da interface gráfica
         if (!showing_color_menu) {
             oledgfx_clear_screen(&ssd);
             oledgfx_draw_border(&ssd, BORDER_LIGHT);
             oledgfx_draw_hline(&ssd, 14, BORDER_LIGHT);
             
             // Exibe informações no display
             ssd1306_draw_string(&ssd, "PiMensuraOhm", 16, 2);
             oledgfx_draw_resistor(&ssd, 24, 18);
             
             // Exibe código de cores do resistor (3 primeiras faixas)
             ssd1306_draw_string(&ssd, ring_colors[CHAR_TO_NUM(str_y[0])], 28, 27);
             ssd1306_draw_string(&ssd, ring_colors[CHAR_TO_NUM(str_y[1])], 48, 27);
             ssd1306_draw_string(&ssd, ring_colors[find_resistance_power_multiplyer(str_y)], 68, 27);
             ssd1306_draw_string(&ssd, "Gd", 87, 27);  // Quarta faixa (tolerância)
             
             // Exibe valores medidos e comerciais
             ssd1306_draw_string(&ssd, "Meas.", 8, 41);
             ssd1306_draw_string(&ssd, str_x, 16, 52);
             size_t omega_offset = CALC_OMEGA_OFFSET(str_x);
             oledgfx_draw_ohm_symbol(&ssd, 16 + omega_offset, 52);
             
             ssd1306_line(&ssd, 60, 37, 60, 60, true);
             ssd1306_draw_string(&ssd, "Com.", 80, 41);
             ssd1306_draw_string(&ssd, str_y, 70, 52);
             omega_offset = CALC_OMEGA_OFFSET(str_y);
             oledgfx_draw_ohm_symbol(&ssd, 70 + omega_offset, 52);
             
             ssd1306_send_data(&ssd);
             sleep_ms(100);
         } else {
             // Mostra menu de cores se ativado
             draw_menu_colors(ssd_global);
         }
     }
 }
 
 /**
  * Manipulador de interrupção dos GPIOs (botões)
  * @param gpio Pino que gerou a interrupção
  * @param events Tipo de evento (não utilizado)
  */
 void gpio_irq_handler(uint gpio, uint32_t events) {
     if (pb_is_debounce_delay_over()) {
         if (JOYSTICK_PB_PRESSED) {
             // Botão do joystick alterna entre menu principal e menu de cores
             showing_color_menu = !showing_color_menu;
             control_submenus = showing_color_menu ? FIRST_MENU_COLORS : UNDEFINED_MENU;
         } 
         else if (BUTTON_B_PRESSED) {
             if (control_submenus == UNDEFINED_MENU) {
                 // No menu principal, botão B entra em modo BOOTSEL
                 reset_usb_boot(0, 0);
             } else {
                 // Nos submenus, navega para frente
                 control_submenus++;
                 if (control_submenus > THIRD_MENU_COLORS) {
                     control_submenus = UNDEFINED_MENU;
                     showing_color_menu = false;
                 }
             }
         } 
         else if (BUTTON_A_PRESSED) {
             // Botão A navega para trás nos menus
             if (control_submenus != UNDEFINED_MENU) {
                 control_submenus--;
                 if (control_submenus == 0) {
                     control_submenus = THIRD_MENU_COLORS;
                 }
             }
         }
     }
 }
 
 /**
  * Desenha a estrutura básica do menu de cores
  * @param ssd Ponteiro para a estrutura do display
  */
 void draw_menu_colors_scaffold(ssd1306_t *ssd) {
     oledgfx_draw_string(ssd, "Colors", 39, 2);
     oledgfx_draw_border(ssd, BORDER_LIGHT);
     oledgfx_draw_hline(ssd, 10, BORDER_LIGHT);
 }
 
 /**
  * Desenha o conteúdo do menu de cores baseado no submenu atual
  * @param ssd Ponteiro para a estrutura do display
  */
 void draw_menu_colors(ssd1306_t *ssd) {
     oledgfx_clear_screen(ssd);
     draw_menu_colors_scaffold(ssd);
     switch (control_submenus) {
         case FIRST_MENU_COLORS:
             // Primeira página do menu de cores
             oledgfx_draw_string(ssd, "(1) Bk - Black", 1, 16);
             oledgfx_draw_string(ssd, "(2) Bn - Marron", 1, 24);
             oledgfx_draw_string(ssd, "(3) R  - Red", 1, 32);
             oledgfx_draw_string(ssd, "(4) Og - Orange", 1, 40);
             oledgfx_draw_string(ssd, " < A        B >", 2, 52);
             break;
             
         case SECOND_MENU_COLORS:
             // Segunda página do menu de cores
             oledgfx_draw_string(ssd, "(5) Y  - Yellow", 1, 16);
             oledgfx_draw_string(ssd, "(6) G  - Green", 1, 24);
             oledgfx_draw_string(ssd, "(7) B  - Blue", 1, 32);
             oledgfx_draw_string(ssd, "(8) Vt - Violet", 1, 40);
             oledgfx_draw_string(ssd, " < A        B >", 2, 52);
             break;
             
         case THIRD_MENU_COLORS:
             // Terceira página do menu de cores
             oledgfx_draw_string(ssd, "(9)  Gy - Gray", 1, 16);
             oledgfx_draw_string(ssd, "(10) Wt - White", 1, 24);
             oledgfx_draw_string(ssd, "(11) Gd - Gold", 1, 32);
             oledgfx_draw_string(ssd, "(12) S - Silver", 1, 40);
             oledgfx_draw_string(ssd, " < A B (exit) >", 2, 52);
             break;
     }
     
     oledgfx_render(ssd);
 }
 
 /**
  * Encontra o valor comercial mais próximo na série E24
  * @param resistance_value Valor medido da resistência
  * @return Valor comercial mais próximo
  */
 uint32_t find_comercial_value(uint32_t resistance_value) {
     uint32_t minDifference = UINT16_MAX;
     uint32_t difference;
     uint32_t curr_resistance, commercial_resistance_value = 0;
     
     for (uint8_t i = 0; i < NUM_RESISTORS; i++) {
         curr_resistance = e24_resistance_values[i];
         
         // Calcula diferença absoluta
         difference = (curr_resistance > resistance_value) ? 
                      (curr_resistance - resistance_value) : 
                      (resistance_value - curr_resistance);
         
         // Atualiza se encontrou uma diferença menor
         if (difference < minDifference) {
             commercial_resistance_value = curr_resistance;
             minDifference = difference;
         }
     }
     return commercial_resistance_value;
 }
 
 /**
  * Determina o multiplicador baseado no tamanho do valor de resistência
  * @param sresistance String com o valor de resistência
  * @return Índice da cor correspondente ao multiplicador
  */
 uint8_t find_resistance_power_multiplyer(const char* sresistance) {
     size_t size_sresistance = strlen(sresistance);
     if (size_sresistance == 3) return 1;  // 1 dígito + 0 (ex: 56 -> 560Ω)
     else if (size_sresistance == 4) return 2;  // 2 dígitos + 0 (ex: 100 -> 1kΩ)
     else if (size_sresistance == 5) return 3;  // 3 dígitos + 0 (ex: 1000 -> 10kΩ)
     else if (size_sresistance == 6) return 4;  // 4 dígitos + 0 (ex: 10000 -> 100kΩ)
     return 0;
 }