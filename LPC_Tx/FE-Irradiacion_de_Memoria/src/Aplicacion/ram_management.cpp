/*********************************************
 *
 * ### ram_management.cpp ###
 * 
 * @brief           Manejo de memoria RAM.
 * @date            15/11/2026
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
#define RAM_MEMORY_SIZE		((uint32_t) (0x100))

// # APLICACIÓN #
#define MAX_UINT32_VALUE	((uint32_t) -1)
#define INT_PI				3141592653
#define OK_MSG_SIZE			5
#define FLIP_MSG_SIZE		7


/* ###########################################
 * ### PROTOTIPOS DE FUNCIONES PRIVADAS ###
 * ########################################### */
uint32_t Calculate_uint32_t_Max_Value_Digits();
void Clear_RAM_Bytes( void *inputptr, uint32_t arraySizeBytes );
void Tx_Memory_Data();
void Write_Bytes_in_Memory( const void *dataInput, uint32_t nBytes,
							void *writtenData, const uint32_t sizeWrite );
void Read_Bytes_in_Memory( const void * const inputData, void *copyData,
						   const uint32_t size );
bool CheckBitFlips_RAM( const uint32_t * const inputData, uint32_t * const irradiatedData );
void Serial_Armar_Trama_Data( char * const inputArray, uint32_t arraySizeBytes,
							  const char incioDeTrama, const char finDeTrama );
void uint32_tToASCII( uint32_t intData, char * const asciiArray, const uint32_t arraySizeBytes );
void Reverse_Array_Data( char * const inputArray, const uint32_t arraySizeBytes );


/* ###########################################
 * ### VARIABLES GLOBALES PRIVADAS ###
 * ########################################### */
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

/*********************************************
 * Calculate_uint32_t_Max_Value_Digits
 *********************************************
 * Calcula cuántos dígitos tiene el máximo valor permitido
 * por el tipo de dato uint32_t.
 */
uint32_t Calculate_uint32_t_Max_Value_Digits() {
	uint32_t nDigits = 0;


	for ( uint32_t tmp = MAX_UINT32_VALUE; tmp != 0; tmp = tmp / 10 ) {
		++nDigits;
	}


	return nDigits;
}



/*********************************************
 * Clear_RAM_Bytes
 *********************************************
 * Borra "size" Bytes de la RAM.
 */
void Clear_RAM_Bytes( void *inputptr, uint32_t arraySizeBytes ) {

	uint8_t *ptr = (uint8_t *) inputptr;


	if ( (!inputptr) || (arraySizeBytes == 0) )
		return;


	for ( uint32_t i = 0; i < arraySizeBytes; ++i ) {
		ptr[i] = 0;
	}
}


/*********************************************
 * Write_Bytes_in_Memory
 *********************************************
 * Escribe en toda la RAM el valor asignado.
 * Hace un for cíclico para ir copiando en secuencia
 * los Bytes de "dataInput", uno pegado al otro.
 */
void Write_Bytes_in_Memory( const void * const dataInput, const uint32_t nBytes,
							void * const writtenData, const uint32_t sizeWrite ) {

	const uint8_t * const 	intDataInput	= (const uint8_t * const) dataInput;
	uint8_t	* const 		writtenRAM 		= (uint8_t * const) writtenData;

	// < ERRORS >
	if ( (!dataInput) || (!writtenData) )
		return;

	if ( (nBytes == 0) || (sizeWrite == 0) )
		return;


	for ( uint32_t i = 0; i < (sizeWrite * nBytes); ++i ) {
		writtenRAM[i] = intDataInput[i % nBytes];
	}
}


/*********************************************
 * Read_Bytes_in_Memory
 *********************************************
 * Lee toda la zona de memoria RAM disponible y guarda todo el
 * bloque en "copyData".
 */
void Read_Bytes_in_Memory( const void * const inputData, void * const copyData,
						   const uint32_t size ) {

	uint8_t * const intDataRead 		= (uint8_t * const) copyData;
	const uint8_t * const dataInRAM		= (const uint8_t * const) inputData;

	// < ERRORS >
	if ( (!copyData) || (!inputData) )
		return;
	if ( size == 0 )
		return;


	for ( uint32_t i = 0; i < size; ++i ) {
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
	uint32_t	tempChangedData[RAM_MEMORY_SIZE];
	char 		okMsg[OK_MSG_SIZE] = "(OK)";
	char 		flipMsg[FLIP_MSG_SIZE] = "(FLIP)";
	char		inicioDeTrama = '[';
	char		finDeTrama = ']';
//	char		txData[MAX_UINT32_VALUE];	// Calcular dígitos en valor máximo de uint32_t
	char		*txData = nullptr;
	uint32_t	uint32_tTmpMaxDigits = 0;
	uint32_t	txSize;


	uint32_tTmpMaxDigits = Calculate_uint32_t_Max_Value_Digits();
	txSize = uint32_tTmpMaxDigits + 2;
	txData = new char[txSize];	// Se le suma caracter de inicio y de fin de trama.

	if ( !txData )
		return;

	for ( uint32_t i = 0; i < uint32_tTmpMaxDigits; ++i ) {
		txData[i] = 0;
	}

//	Clear_RAM_Bytes( valueRead, RAM_MEMORY_SIZE );

//	Read_Bytes_in_Memory( (void *) memoryIrradiated, (void *) valueRead,
//						  RAM_MEMORY_SIZE );

	if ( CheckBitFlips_RAM( memoryIrradiated, tempChangedData ) == true ) {

		serialCOMS_LPC.Transmit( (void *) flipMsg, FLIP_MSG_SIZE - 1 );

		// Mete los valores cambiados en la lista circular.
		for ( uint32_t i = 0; i < RAM_MEMORY_SIZE; ++i ) {

			if ( tempChangedData[i] != 0 ) {
				// Preparar mensaje en ASCII.
				Serial_Armar_Trama_Data( txData, txSize, inicioDeTrama, finDeTrama );
				uint32_tToASCII( tempChangedData[i], txData + 1, uint32_tTmpMaxDigits );
				serialCOMS_LPC.Transmit( (void *) txData, txSize );
			}
		}

//		serialCOMS_LPC.Transmit( (void *) & valueRead, sizeof(valueRead) );

	} else {

		serialCOMS_LPC.Transmit( (void *) okMsg, OK_MSG_SIZE - 1 );
	}


	delete[] txData;

	timer_Tx.TimerStart( 1, Timer::bases_t::MIN, Tx_Memory_Data );
//	timer_Tx.TimerStart( 5, Timer::bases_t::SEG, Tx_Memory_Data );
}


/*********************************************
 * CheckBitFlips_RAM
 *********************************************
 * Checkea bitflips.
 * Devuelve TRUE y guarda el valor cambiado en "changedData" en caso de cambio,
 * sinó guarda 0 en el array y devuelve FALSE.
 */
bool CheckBitFlips_RAM( const uint32_t * const inputData, uint32_t * const irradiatedData ) {
	bool 					bitFliped 		= false;
	uint32_t 				tempData 		= INT_PI;
	const uint32_t * const 	inputReadData 	= (const uint32_t * const) inputData;
	uint32_t * const 		changedData 	= (uint32_t * const) irradiatedData;


	if ( (!inputData) || (!irradiatedData) )
		return false;


	for ( uint32_t i = 0; i < RAM_MEMORY_SIZE; ++i ) {

		if ( inputReadData[i] ^ tempData ) {

			changedData[i] = inputReadData[i];
			bitFliped = true;
//			break;
		} else {

			changedData[i] = 0;
//			bitFliped = false;
		}
	}


	return bitFliped;
}


/*********************************************
 * Serial_Armar_Trama_Data
 *********************************************
 * Arma
 */
void Serial_Armar_Trama_Data( char * const inputArray, uint32_t arraySizeBytes,
							  const char inicioDeTrama, const char finDeTrama ) {

	if ( (!inputArray) || (arraySizeBytes == 0) )
		return;

	inputArray[0] = inicioDeTrama;
	inputArray[arraySizeBytes - 1] = finDeTrama;
}


/*********************************************
 * uint32_tToASCII
 *********************************************
 * Convierte uint32_t a ASCII.
 *
 * ### Lógica de conversión uint32_t -> ASCII (string) ###
 * Resto con 10 (x % 10) devuelve el último dígito (menos significativo).
 * División por 10 (x / 10) elimina el último dígito para quedarse con el resto.
 */
void uint32_tToASCII( uint32_t intData, char * const asciiArray, const uint32_t arraySizeBytes ) {

	uint32_t uintMaxValue = (uint32_t) -1;


	if ( (!asciiArray) || (arraySizeBytes == 0) )
		return;

//	char tmpArray[arraySizeBytes];

	for ( uint32_t index = 0; (index <= uintMaxValue) && (intData > 0); ++index ) {
//		tmpArray[index] = intData % 10;
		asciiArray[index] = intData % 10;
		intData = intData / 10;
	}

	Reverse_Array_Data( asciiArray, arraySizeBytes );
}


/*********************************************
 * Reverse_Array_Data
 *********************************************
 * Invierte datos de un array.
 */
void Reverse_Array_Data( char * const inputArray, const uint32_t arraySizeBytes ) {

	char tmpData;


	if ( (!inputArray) || (arraySizeBytes == 0) )
		return;


	for ( uint32_t i = 0; (i < 1 + (arraySizeBytes / 2)); ++i ) {
		tmpData = inputArray[arraySizeBytes - i];
		inputArray[arraySizeBytes - i] = inputArray[i];
		inputArray[i] = tmpData;
	}
}


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
 * Init_RAM_Write
 *********************************************
 * Escritura inicial de la RAM + inicio de transmisión
 * serie (UART).
 */
void Init_RAM_Write() {

	uint32_t valueStored = INT_PI;
	memoryIrradiated = new uint32_t[RAM_MEMORY_SIZE];


	if ( !memoryIrradiated )	// ERROR por falta de memoria.
		return;

	Clear_RAM_Bytes( memoryIrradiated, RAM_MEMORY_SIZE );

	Write_Bytes_in_Memory( (const void *) & valueStored, sizeof(uint32_t),
						   (void *) memoryIrradiated, RAM_MEMORY_SIZE );

	Tx_Memory_Data();
}


