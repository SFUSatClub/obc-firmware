/*
 * sfu_flags.h
 *
 *  Created on: Jun 23, 2018
 *      Author: Richard & Lana
 *
 *      - the primary aim of this is reusable flags that don't run into each other, not storage savings :D
 *
 *      Organization of the flag file:
 *      Data						Start Addr		End Addr		Notes
 *      "created +  timestamp"			0				19 max		added when created
 *      "PBIST_RESET:1"					24				?
 *      "FSYS_PREFIX:a"					48				?
 */

#ifndef SFUSAT_SFU_FLAGS_H_
#define SFUSAT_SFU_FLAGS_H_

#define RESET_FLAG_START 	14
#define RESET_FLAG_MSG		"PBIST_RESET:1"		/* when we reset once we log this */
#define RESET_CLEAR_MSG		"PBIST_RESET:0"
#define RESET_FLAG_LEN		strlen(RESET_FLAG_MSG) - 1

#define PREFIX_FLAG_START 	0
#define PREFIX_FLAG_BASE	"FSYS_PREFIX:a"		/* when we reset we record this */
#define PREFIX_FLAG_LEN		strlen(PREFIX_FLAG_BASE) - 1

/* ----- New flag file methodology
 *
 * Structs are written into flash, and all structs must be written once (default values) at startup
 *
 * 	- a struct is defined with the data + timestamp
 * 	- a union is defined with the struct and a byte array to allow single-byte access for read/write to flash.
 * 	- the union has the 'Wrap' suffix
 * 	- the timestamp is updated every time the flag is updated
 */

#include "stdtelem.h"

typedef struct flag_char{
	char flag;
	uint32_t timestamp;
} flagChar_t;

typedef union{
	flagChar_t payload;
	uint8_t byteAddr[sizeof(flagChar_t)];
} flagCharWrap_t;

flagChar_t myFlag;
flagChar_t myFlag2;
telemConfig_t myCfg;

//eeprom_memory_layout_t * ee;

   /* eeprom_read(offset, size, buf) */
//   eeprom_read(offsetof(eeprom_memory_layout_t, param3), sizeof(ee->param3), &buf);


void writeFlag(uint32_t offset, uint32_t size, uint8_t * bytes);

#define EXPAND_AS_STRUCT(wrap_type, struct_name, variable) wrap_type struct_name;
// wrapper type, name to use during accesses,
#define FLAG_TABLE(ENTRY)     \
        ENTRY(flagCharWrap_t, reset_flag, myFlag) \
        ENTRY(flagCharWrap_t, prefix_flag, myFlag2) \
        ENTRY(telemConfigWrap_t, gen_telem_config, myCfg)

/* Create the flag table used to determine offset from name
 * 	- members accessed by 'struct_name', are of wrapper type
 * */
typedef struct{
    FLAG_TABLE(EXPAND_AS_STRUCT)
} flag_memory_table_t;

flag_memory_table_t flag_memory_table;

#define UPDATE_TIMESTAMP(wrap_type, struct_name, variable) flag_memory_table.struct_name.payload.timestamp = timestamp;
#define WRITEFLAG_CALL(wrap_type, struct_name, variable) writeFlag(offsetof(flag_memory_table_t, struct_name), sizeof(wrap_type), flag_memory_table.struct_name.byteAddr);
void initFlagTable();





//
//#define EXPAND_AS_INITIALIZER(a,b,c) a = c;
//#define EXPAND_AS_DECLARATION(a,b,c) volatile uint8_t a _at_ b;
//#define
//#define FLAG_TABLE(ENTRY)                   \
//    ENTRY(RESET_FLAG, flagCharWrap_t) \
//    ENTRY(PREFIX_FLAG, FPGA_ADDRESS_OFFSET + 1, 0x55) \
//    ENTRY(GENERAL_TELEM, FPGA_ADDRESS_OFFSET + 2, 0x1b) \
//    ENTRY(GENERAL_TELEM, FPGA_ADDRESS_OFFSET + X, 0x33)
//
///* declare the registers */
//REGISTER_TABLE(EXPAND_AS_DECLARATION)

#endif /* SFUSAT_SFU_FLAGS_H_ */
