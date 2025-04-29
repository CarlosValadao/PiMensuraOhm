/*
 * Por: Wilton Lacerda Silva
 *    Ohmímetro utilizando o ADC da BitDogLab
 *
 * 
 * Neste exemplo, utilizamos o ADC do RP2040 para medir a resistência de um resistor
 * desconhecido, utilizando um divisor de tensão com dois resistores.
 * O resistor conhecido é de 10k ohm e o desconhecido é o que queremos medir.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

// #include "lib/ssd1306.h"
// #include "lib/font.h"
#include "lib/push_button.h"
#include "lib/oledgfx.h"
#include "lib/rgb.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

#define SSD1306_ADDR 0x3C
#define ADC_PIN 28 // GPIO para o voltímetro

#define BUTTON_A 5  // GPIO para botão A
#define BUTTON_B 6
#define PB_JOYSTICK 22

#define FIRST_MENU_COLORS  1
#define SECOND_MENU_COLORS 2
#define THIRD_MENU_COLORS  3
#define UNDEFINED_MENU     0

/// @brief RGB LED pin configuration
#define RED_PIN   13     ///< Red LED PWM pin
#define BLUE_PIN  12     ///< Blue LED PWM pin
#define GREEN_PIN 11     ///< Green LED PWM pin

#define JOYSTICK_PB_PRESSED (gpio == 22)
#define BUTTON_A_PRESSED (gpio == BUTTON_A)
#define BUTTON_B_PRESSED (gpio == BUTTON_B)

static volatile uint8_t control_submenus = UNDEFINED_MENU;
static volatile bool    showing_color_menu = false;

static ssd1306_t *ssd_global = NULL;

uint16_t R_conhecido = 10000;   // Resistor de 10k ohm
uint32_t R_x = 0;          // Resistor desconhecido
uint32_t resistance_sum = 0;
uint32_t resistence_avg = 0;
float ADC_VREF = 3.31;     // Tensão de referência do ADC
uint16_t ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)

// Trecho para modo BOOTSEL com botão B
void gpio_irq_handler(uint gpio, uint32_t events);
void draw_menu_colors(ssd1306_t *ssd);
void draw_menu_colors_scaffold(ssd1306_t *ssd);

int main()
{

  // Para ser utilizado o modo BOOTSEL com botão B
  pb_config_btn_b();
  // gpio_init(BUTTON_B);
  // gpio_set_dir(BUTTON_B, GPIO_IN);
  // gpio_pull_up(BUTTON_B);
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  // Aqui termina o trecho para modo BOOTSEL com botão B

  pb_config_btn_a();
  // gpio_init(BUTTON_A);
  // gpio_set_dir(BUTTON_A, GPIO_IN);
  // gpio_pull_up(BUTTON_A);

  pb_config(PB_JOYSTICK, true);

  pb_set_irq_callback(&gpio_irq_handler);

  pb_enable_irq(BUTTON_A);
  pb_enable_irq(BUTTON_B);
  pb_enable_irq(PB_JOYSTICK);

  ssd1306_t ssd;
  rgb_t rgb;
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA);                                        // Pull up the data line
  gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
                                                  // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd);                                         // Configura o display
  ssd1306_send_data(&ssd);                                      // Envia os dados para o display

  ssd_global = &ssd;

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  adc_init();
  adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica

  rgb_init_all(&rgb, RED_PIN, GREEN_PIN, BLUE_PIN, 1.0, 2048);

  char str_x[5]; // Buffer para armazenar a string
  char str_y[5]; // Buffer para armazenar a string

  bool cor = true;
  while (true)
  {
    resistance_sum = 0;
    adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica
    for (int i = 0; i < 500; i++)
    {
      resistance_sum += adc_read();
      sleep_ms(1);
    }
    resistence_avg = resistance_sum / 500;

      // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
    R_x = (R_conhecido * resistence_avg) / (ADC_RESOLUTION - resistence_avg);

    sprintf(str_x, "%u", resistence_avg); // Converte o inteiro em string
    sprintf(str_y, "%u", R_x);   // Converte o float em string

    //  Atualiza o conteúdo do display com animações
    if(!showing_color_menu)
    {
    oledgfx_clear_screen(&ssd);
    //   ssd1306_fill(&ssd, !cor);                          // Limpa o display
    oledgfx_draw_border(&ssd, BORDER_LIGHT);
    //   ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
      oledgfx_draw_hline(&ssd, 14, BORDER_LIGHT);        //   ssd1306_line(&ssd, 3, 25, 123, 25, cor);           // Desenha uma linha
    //   ssd1306_line(&ssd, 3, 37, 123, 37, cor);           // Desenha uma linha
      ssd1306_draw_string(&ssd, "PiMensuraOhm", 16, 2); // Desenha uma string
      oledgfx_draw_resistor(&ssd, 24, 18);
      ssd1306_draw_string(&ssd, "Bk", 28, 27);
      ssd1306_draw_string(&ssd, "Bk", 48, 27);
      ssd1306_draw_string(&ssd, "Bk", 68, 27);
      ssd1306_draw_string(&ssd, "Gd", 87, 27);
    //   ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16);  // Desenha uma string
    //   ssd1306_draw_string(&ssd, "  Ohmimetro", 10, 28);  // Desenha uma string
      ssd1306_draw_string(&ssd, "ADC", 13, 41);          // Desenha uma string
      ssd1306_draw_string(&ssd, "Resisten.", 50, 41);    // Desenha uma string
      ssd1306_line(&ssd, 44, 37, 44, 60, cor);           // Desenha uma linha vertical
      ssd1306_draw_string(&ssd, str_x, 8, 52);           // Desenha uma string
      ssd1306_draw_string(&ssd, str_y, 59, 52);          // Desenha uma string
      ssd1306_send_data(&ssd);
    //   oledgfx_draw_resistor(ssd_global, 16, 32);
    //   oledgfx_render(ssd_global);
      sleep_ms(700);
    }
    else draw_menu_colors(ssd_global);
  }
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
  if(pb_is_debounce_delay_over())
  {
    
    if(JOYSTICK_PB_PRESSED)
    {
      if(showing_color_menu)
      {
        showing_color_menu = false;
        control_submenus = UNDEFINED_MENU;
      }
      else
      {
        showing_color_menu = true;
        control_submenus = FIRST_MENU_COLORS;
      }
    }
    else if (BUTTON_B_PRESSED)
    {
      if(control_submenus == UNDEFINED_MENU) reset_usb_boot(0, 0);
      control_submenus++;
      if(control_submenus > THIRD_MENU_COLORS)
      {
        control_submenus = UNDEFINED_MENU;
        showing_color_menu = false;
      }
    }
    else if(BUTTON_A_PRESSED)
    {
      if(control_submenus != UNDEFINED_MENU) control_submenus--;
      if(control_submenus == 0) control_submenus = THIRD_MENU_COLORS;
    }
  }
}
  
void draw_menu_colors_scaffold(ssd1306_t *ssd)
{
  oledgfx_draw_string(ssd, "Colors", 39, 2);
  oledgfx_draw_border(ssd, BORDER_LIGHT);
  oledgfx_draw_hline(ssd, 10, BORDER_LIGHT);
}

void draw_menu_colors(ssd1306_t *ssd)
{
  oledgfx_clear_screen(ssd);
  oledgfx_render(ssd);
  draw_menu_colors_scaffold(ssd);
  if(control_submenus == FIRST_MENU_COLORS)
  {
    oledgfx_draw_string(ssd, "(1) Bk - Black", 1, 16);
    oledgfx_draw_string(ssd, "(2) Bn - Marron", 1, 24);
    oledgfx_draw_string(ssd, "(3) R  - Red", 1, 32);
    oledgfx_draw_string(ssd, "(4) Og - Orange", 1, 40);
    oledgfx_draw_string(ssd, " < A        B >", 2, 52);
  }
  else if(control_submenus == SECOND_MENU_COLORS)
  {
    oledgfx_draw_string(ssd, "(5) Y  - Yellow", 1, 16);
    oledgfx_draw_string(ssd, "(6) G  - Green", 1, 24);
    oledgfx_draw_string(ssd, "(7) B  - Blue", 1, 32);
    oledgfx_draw_string(ssd, "(8) Vt - Violet", 1, 40);
    oledgfx_draw_string(ssd, " < A        B >", 2, 52);
  }
  else if(control_submenus == THIRD_MENU_COLORS)
  {
    oledgfx_draw_string(ssd, "(9)  Gy - Gray", 1, 16);
    oledgfx_draw_string(ssd, "(10) Wt - White", 1, 24);
    oledgfx_draw_string(ssd, "(11) Gd - Gold", 1, 32);
    oledgfx_draw_string(ssd, "(12) Vt - Silve", 1, 40);
    oledgfx_draw_string(ssd, " < A B (exit) >", 2, 52);
  }
  oledgfx_render(ssd);
}