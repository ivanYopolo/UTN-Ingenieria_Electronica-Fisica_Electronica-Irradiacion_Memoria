/*********************************************
 *
 * ### ram_management.h ###
 * 
 * @brief           Manejo de memoria RAM.
 * @date            15/11/2026
 * @author          iyopolo
 *
 *********************************************/


#ifndef         RAM_MANAGEMENT_H_
    #define     RAM_MANAGEMENT_H_

//	#include "Aplicacion/ram_management.h"


    /* ###########################################
     * ### INCLUDES GLOBALES ###
     * ########################################### */
	#include "Drivers/tipos.h"
	#include "Modulos/includeModulos.h"
	#include "Drivers/Systick/systick.h"
	#include "Drivers/LPC845.h"
	#include "Drivers/C-Timer/c_timer.h"


    /* ###########################################
     * ### MACROS & TIPOS DE DATOS GLOBALES ###
     * ########################################### */
    //


    /* ###########################################
     * ### VARIABLES GLOBALES PÚBLICAS ###
     * ########################################### */
	//


    /* ###########################################
     * ### PROTOTIPOS DE FUNCIONES PÚBLICAS ###
     * ########################################### */
	void Check_Timer_Events();
    void Init_RAM_Write();
//    void Clear_RAM_Bytes( void *inputptr, uint32_t size );
//    void Tx_Memory_Data();
//    void Write_Bytes_in_Memory( const void *dataInput, uint32_t nBytes,
//								void *writtenData, const uint32_t sizeWrite );
//    void Read_Bytes_in_Memory( void *dataRead, void *changedData,
//    						   const uint32_t size );
//    bool CheckBitFlips_RAM( const uint32_t * const inputReadData, uint32_t * const changedData );
#endif          /* RAM_MANAGEMENT_H_ */
