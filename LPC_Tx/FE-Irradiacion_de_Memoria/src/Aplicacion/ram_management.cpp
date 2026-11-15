/*********************************************
 *
 * ### flash_management.cpp ###
 * 
 * @brief           Manejo de memoria FLASH.
 * 					La escritura mínima es de 1 página.
 * 					En el caso del LPC845, una página son 64 B.
 * @date            31/10/2026
 * @author          iyopolo
 *
 *********************************************/


/* ###########################################
 * ### INCLUDES ###
 * ########################################### */
#include "ram_management.h"


/* ###########################################
 * ### MACROS & TIPOS DE DATOS PRIVADOS ###
 * ########################################### */
// # RAM #
#define	FLASH_BEGIN_ADDR	(0x00000000u)
#define	FLASH_END_ADDR		(0x00010000u)
#define FLASH_MEMORY_SIZE	(FLASH_END_ADDR - FLASH_BEGIN_ADDR)

// # APLICACIÓN #
#define INT_PI				3141592653
#define OK_MSG_SIZE			3
#define FLIP_MSG_SIZE		5


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
//


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
//volatile uint32_t valueRead = 0;
//volatile uint32_t valueStored = 0;
Timer 		timer_Tx;
uint32_t 	*memoryIrradiated = nullptr;


/* ###########################################
 * ### VARIABLES GLOBALES PÚBLICAS ###
 * ########################################### */
extern Uart serialCOMS_LPC;



// ---------------------------------------------------------------------------------


/* ###########################################
 * ### FUNCIONES PRIVADAS ###
 * ########################################### */
//



// ---------------------------------------------------------------------------------


/* ###########################################
 * ### FUNCIONES PÚBLICAS ###
 * ########################################### */

/*********************************************
 * Check_Timer_Events
 *********************************************
 * Función para ejecutar el callback del timer.
 */
void Check_Timer_Events() {
	timer_Tx.TmrEvent();
}


/*********************************************
 * Init_Flash_Write
 *********************************************
 * Escritura inicial de la flash + inicio de transmisión
 * serie (UART).
 */
void Init_Flash_Write() {

	uint32_t valueStored = INT_PI;
	memoryIrradiated = new uint32_t[FLASH_MEMORY_SIZE];


	if ( !memoryIrradiated )
		return;

	ClearRAM( memoryIrradiated, FLASH_MEMORY_SIZE );

	Write_Bytes_in_Memory( (const void *) & valueStored, sizeof(uint32_t),
						   (void *) memoryIrradiated, FLASH_MEMORY_SIZE );

	Tx_Memory_Data();
}


/*********************************************
 * ClearRAM
 *********************************************
 * Borra la RAM.
 */
void ClearRAM( void *inputptr, uint32_t size ) {
	uint8_t *ptr = inputptr;


	for ( uint8_t i = 0; i < size; ++i ) {
		ptr[i] = 0;
	}
}


/*********************************************
 * Write_Bytes_in_Memory
 *********************************************
 * Escribe en toda la RAM el valor asignado.
 */
void Write_Bytes_in_Memory( const void *dataInput, const uint16_t nBytes,
							void *writtenData, const uint16_t sizeWrite ) {

	const uint8_t 	intDataInput = *((const uint8_t *) dataInput);
	uint32_t		*writtenRAM = writtenData;


	for ( uint8_t i = 0; i < sizeWrite; ++i ) {
		writtenRAM[i] = intDataInput[i % nBytes];
	}
}


/*********************************************
 * Read_Bytes_in_Memory
 *********************************************
 * Lee toda la zona de memoria RAM disponible y guarda todo el
 * bloque en "dataRead".
 */
void Read_Bytes_in_Memory( void *dataRead, void *changedData,
						   const uint16_t size ) {

	uint8_t *intDataRead 	= (uint8_t *) dataRead;
	uint8_t *dataInRAM		= (uint8_t *) changedData;


	for ( uint8_t i = 0; i < size; ++i ) {
		intDataRead[i] = dataInRAM[i];
	}
}


/*********************************************
 * Tx_Memory_Data
 *********************************************
 * Cada un tiempo (1 minuto) envía datos de si cambió el dígito guardado:
 * \--> "OK" 	= sin cambios.
 * \--> "FLIP" 	= hubo bit-flip. Se prosigue a enviar el dato cambiado.
 *
 * Se aloja el dato leído en el stack así se renueva constantemente.
 */
void Tx_Memory_Data() {
//	uint32_t valueRead = CheckBitFlips();
	uint32_t 	valueRead[FLASH_MEMORY_SIZE];
	uint32_t	tempChangedData[FLASH_MEMORY_SIZE];
	char 		okMsg[OK_MSG_SIZE] = "OK";
	char 		flipMsg[FLIP_MSG_SIZE] = "FLIP";


	Read_Bytes_in_Memory( (void *) valueRead, (void *) memoryIrradiated,
						  FLASH_MEMORY_SIZE );

	if ( CheckBitFlips_RAM( valueRead, tempChangedData ) == true ) {

		serialCOMS_LPC.Transmit( (void *) flipMsg, FLIP_MSG_SIZE );
		serialCOMS_LPC.Transmit( (void *) & valueRead, sizeof(valueRead) );
	} else {

		serialCOMS_LPC.Transmit( (void *) okMsg, OK_MSG_SIZE );
	}


	timer_Tx.TimerStart( 1, Timer::bases_t::MIN, Tx_Memory_Data );
}


/*********************************************
 * CheckBitFlips_RAM
 *********************************************
 * Checkea bitflips.
 * Devuelve TRUE y guarda el valor cambiado en "changedData" en caso de cambio,
 * sinó guarda 0 y devuelve FALSE.
 */
bool CheckBitFlips_RAM( const uint32_t * const inputReadData, uint32_t * const changedData ) {
	bool bitFliped = false;
	uint32_t tempData = INT_PI;


	for ( uint8_t i = 0; i < FLASH_MEMORY_SIZE; ++i ) {

		if ( inputReadData[i] ^ tempData ) {

			changedData[i] = inputReadData[i];
			bitFliped = true;
			break;
		} else {

			changedData[i] = 0;
			bitFliped = false;
		}
	}


	return bitFliped;
}

