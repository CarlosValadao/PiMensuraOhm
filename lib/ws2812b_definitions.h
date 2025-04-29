#ifndef WS2812B_DEFINITIONS_H
#define WS2812B_DEFINITIONS_H

/**
 * @author Carlos Valadao
 * @file ws2812b_definitions.h
 * @brief Definições de padrões para números de 0 a 9 em uma matriz 5x5 de LEDs WS2812B.
 * 
 * Cada número é representado por um array unidimensional de 25 posições (5x5),
 * onde 0 significa LED apagado e 1 significa LED aceso.
 * 
 * A representação de cada número utiliza apenas uma **submatriz de 5x3** dentro da matriz 5x5.
 * Ou seja, a largura dos números é de 3 colunas, e a altura é de 5 linhas. As outras colunas 
 * da matriz 5x5 são deixadas vazias ou usadas como espaços, formando o número desejado.
 * A submatriz 5x3 é a parte visualmente ativa para exibir o número, e a matriz completa 5x5
 * é usada para maior flexibilidade na representação.
 */

static const uint8_t ZERO_GLYPH[] = { 
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t ONE_GLYPH[] =  { 
        0, 0, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t TWO_GLYPH[] =  { 
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t THREE_GLYPH[] =  { 
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t FOUR_GLYPH[] =  { 
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 1, 0
    };

static const uint8_t FIVE_GLYPH[] =  { 
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t SIX_GLYPH[] =   { 
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t SEVEN_GLYPH[] =   { 
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 
        0, 0, 0, 1, 0,
        0, 1, 0, 0, 0
    };

static const uint8_t EIGHT_GLYPH[] =   { 
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t NINE_GLYPH[] =   { 
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0, 
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    };

static const uint8_t *NUMERIC_GLYPHS[] = {
    ZERO_GLYPH,
    ONE_GLYPH,
    TWO_GLYPH,
    THREE_GLYPH,
    FOUR_GLYPH,
    FIVE_GLYPH,
    SIX_GLYPH,
    SEVEN_GLYPH,
    EIGHT_GLYPH,
    NINE_GLYPH
};

#endif // WS2812B_DEFINITIONS_H
