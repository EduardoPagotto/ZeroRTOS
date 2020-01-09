/**
 * @file disp7seg.cpp
 * @author Eduardo Pagotto (edupagotto@gmail.com)
 * @brief Controlador do display de 7 segmentos
 * @version 0.1
 * @date 2020-01-09
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "disp7seg.h"

uint8_t DDRA, PORTA;

/**
 * @brief  Inicializa os pinos utilizados para o controle do display
 */
void vDisp7SegInit() {
    // Configura os pinos de controle dos displays 7 segmentos com saída digital
    pinMode(DISPLAY_1, OUTPUT);
    pinMode(DISPLAY_2, OUTPUT);
    pinMode(DISPLAY_3, OUTPUT);
    pinMode(DISPLAY_4, OUTPUT);

    DDRA = 0xFF;  // Todos os pinos do PORTA como saída
    PORTA = 0xFF; // Inicializa todos os pinos do PORTA em nível alto
}

/**
 * @brief Encontra o códico correspondente ao digito a ser enviado para o Display
 * @param num Digito a ser enviado para o Display
 * @return uint8_t Código correspondente ao digito desejado em hexadecimal
 */
uint8_t ucDisplay(uint8_t num) {
    // o vetor segmentos representa em hexadecimal os digitos de 0 a 9 correspondentes ao Display de 7 segmentos
    uint8_t segmentos[] = {
        0x3F, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66, // 4
        0x6D, // 5
        0x7D, // 6
        0x07, // 7
        0x7F, // 8
        0x67  // 9
    };

// No caso dos displays andodo comum, o valor é retornado com negação
#ifdef CATODO_COMUM
    return segmentos[num];
#else
    return ~segmentos[num];
#endif
}

/**
 * @brief Calcula o digito a ser mostrado em cada display
 *
 * @param valor Valor inteiro a ser mostrado
 * @param disp qual dos 4 displays
 * @return uint8_t digito a ser enviado para o display
 */
uint8_t ucObtemValorDisplay(int16_t valor, char disp) {
    uint8_t digito;

    switch (disp) {
        case 1:
            digito = valor / 1000;
            break;
        case 2:
            digito = (valor % 1000) / 100;
            break;
        case 3:
            digito = (valor % 100) / 10;
            break;
        case 4:
            digito = valor % 10;
            break;
    }
    return digito;
}

/**
 * @brief Faz a multiplexação dos displays
 *
 * @param valor Valor a ser mostrado
 * @param disp qual display
 */
void vEscreveNoDisplay(uint8_t valor, char disp) {
    // Maquina de Estados para a atualização do Display
    switch (disp) {
        case ESCREVE_DISPLAY_1: {
            digitalWrite(DISPLAY_1, LIGA);
            digitalWrite(DISPLAY_2, DESLIGA);
            digitalWrite(DISPLAY_3, DESLIGA);
            digitalWrite(DISPLAY_4, DESLIGA);
            PORTA = ucDisplay(valor);
            break;
        }
        case ESCREVE_DISPLAY_2: {
            digitalWrite(DISPLAY_1, DESLIGA);
            digitalWrite(DISPLAY_2, LIGA);
            digitalWrite(DISPLAY_3, DESLIGA);
            digitalWrite(DISPLAY_4, DESLIGA);
            PORTA = ucDisplay(valor);
            break;
        }
        case ESCREVE_DISPLAY_3: {
            digitalWrite(DISPLAY_1, DESLIGA);
            digitalWrite(DISPLAY_2, DESLIGA);
            digitalWrite(DISPLAY_3, LIGA);
            digitalWrite(DISPLAY_4, DESLIGA);
            PORTA = ucDisplay(valor);
            break;
        }
        case ESCREVE_DISPLAY_4: {
            digitalWrite(DISPLAY_1, DESLIGA);
            digitalWrite(DISPLAY_2, DESLIGA);
            digitalWrite(DISPLAY_3, DESLIGA);
            digitalWrite(DISPLAY_4, LIGA);
            PORTA = ucDisplay(valor);
            break;
        }
    }
} // end EscreveNoDisplay
