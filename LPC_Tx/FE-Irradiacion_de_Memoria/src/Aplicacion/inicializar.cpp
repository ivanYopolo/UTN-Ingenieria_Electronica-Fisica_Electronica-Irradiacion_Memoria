/*********************************************
 *
 * ### inicializar.cpp ###
 * 
 * @brief           Inicializa el SysTick a 1 ms e inicia las secuencias.
 * 					Configura el modo de funcionamiento de la interrupción externa.
 * @date            Jun 10, 2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */
#include "Aplicacion/inicializar.h"


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
#define	__CTIMER_PRESCALER_FREQ		((uint32_t) 1e6)

/***************************
 *** PINOUT ***
 ***************************/

// >> LED BLUE (GPIO)
#define __LED_BLUE_PORT				1
#define __LED_BLUE_PIN				1

// >> UART: comunicación asíncrona
#define __UART_TX_PORT				0
#define __UART_TX_PIN				25
#define __UART_RX_PORT				0
#define __UART_RX_PIN				24
#define __UART_USART_x				0
#define __UART_BAUDRATE				9600
#define __UART_MAX_RX				(uint32_t) (0x1000)
#define __UART_MAX_TX				(uint32_t) (0x1000)


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
Uart serialCOMS_LPC( __UART_TX_PORT, __UART_TX_PIN, __UART_RX_PORT, __UART_RX_PIN, __UART_USART_x,
					 __UART_BAUDRATE, Uart::bits_de_datos::ocho_bits, Uart::paridad_t::NoParidad,
					 __UART_MAX_RX, __UART_MAX_TX );

GPIO ledBlue( __LED_BLUE_PORT, __LED_BLUE_PIN,
			  GPIO::direccion_e::SALIDA, GPIO::actividad_e::BAJO );


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
//


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */

/*********************************************
 * Inicializar
 *********************************************
 * Inicializa el SysTick a 1 ms e inicia las secuencias.
 * Configura el modo de funcionamiento de la interrupción externa.
 */
void Inicializar() {
//	 ### Systick ###
	SysTick_Config( 1 );

	ledBlue.SetPin();

	// ### RAM ###
	Init_RAM_Write();
}


