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
 *
 * 	How to generate offsets: https://stackoverflow.com/questions/14348601/best-way-to-define-offsets-via-c-preprocessor
 * 	X-macro tutorial: https://www.embedded.com/design/programming-languages-and-tools/4403953/C-language-coding-errors-with-X-macros-Part-1
 */

#include "stdtelem.h"
typedef struct flag_char{
	uint32_t timestamp;
	char flag;
} flagChar_t;

typedef union{
	flagChar_t payload;
	uint8_t all[sizeof(flagChar_t)];
} flagCharWrap_t;


void writeFlag(uint32_t offset, uint32_t size, uint8_t * bytes);
void initFlagTable();
bool ptrWriteFlag(uint8_t idx, uint8_t * data, uint8_t size);
bool writeFlagTest();

/* Flag initializers
 * - does not need to include timestamp
 * - these need to be separate since the initializer list itself is a macro
 *
 * 	Instructions:
 * 		- initialize any fields that you don't want set to 0
 * 		- naming convention: flag name with '_INITS' Suffix
 */
#define RESET_FLAG_INITS 	{	.flag = 'F'}
#define PREFIX_FLAG_INITS 	{	.flag = 'a'}
#define GEN_TELEM_INITS 	{	.max = 700, .min = 0, .period = 10000}

/* Flag Table
 * - wrapper type, name, payload type and initializer #define name for a flag
 *
 *	Instructions:
 *		- enter the wrapper type and payload type
 *		- enter the name for the flag
 *		- assuming you've defined an initializer macro above, enter its name in the 4th column
 *
 */
/*				flag wrap type	|	flag type	|	flag name	|	initializer handle 	*/
#define FLAG_TABLE(ENTRY)     \
        ENTRY(flagCharWrap_t, 		flagChar_t, 	RESET_FLAG,  	RESET_FLAG_INITS) \
        ENTRY(flagCharWrap_t,  		flagChar_t, 	PREFIX_FLAG, 	PREFIX_FLAG_INITS) \
        ENTRY(telemConfigWrap_t, 	telemConfig_t, 	GEN_TELEM,  	GEN_TELEM_INITS)

/* Create the flag table used to determine offset from name
 * 	- members accessed by 'struct_name', are of wrapper type
 * */
#define EXPAND_AS_STRUCT(wrap_type, payload_type, struct_name, init) wrap_type struct_name;

#pragma pack(push,1)
typedef struct{
    FLAG_TABLE(EXPAND_AS_STRUCT)
}flag_memory_table_t;
#pragma pack(pop)

#define FLAG_TABLE_SIZE sizeof(flag_memory_table_t)

typedef union{
	flag_memory_table_t flagTable;
	uint8_t flagTableBytes[sizeof(flag_memory_table_t)];
} flag_memory_table_wrap_t;

#define ENUMERATE_FLAGS(wrap_type, payload_type, struct_name, init) struct_name,
#define INIT_PAYLOAD(wrap_type, payload_type, struct_name, init) flag_memory_table.struct_name.payload = (payload_type) init;
#define UPDATE_TIMESTAMP(wrap_type, payload_type, struct_name, init) flag_memory_table.struct_name.payload.timestamp = timestamp;
#define WRITEFLAG_CALL(wrap_type, payload_type, struct_name, init) writeFlag(offsetof(flag_memory_table_t, struct_name), sizeof(wrap_type), flag_memory_table.struct_name.all);
#define FLAG_SIZE_CHECK(wrap_type, payload_type, struct_name, init) sizeof(flag_memory_table.struct_name.all),


/* --- WRITE AND READ FROM FLASH
 * 	- these functions are generated to write/read a single flag to/from flash
 * 	- their names are write_FLAG_NAME and read_FLAG_NAME
 */
extern void writeFlagRaw(uint8_t *bytes, uint8_t size, uint32_t offset);
extern void readFlagRaw(uint8_t *bytes, uint8_t size, uint32_t offset);

#define FLAG_FLASH_WRITE_DECLARE(wrap_type, payload_type, struct_name, init) void write_##struct_name(uint8_t * wrap);
#define FLAG_FLASH_WRITE_DEFINE(wrap_type, payload_type, struct_name, init) void write_##struct_name(uint8_t * wrap){ \
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 writeFlagRaw(wrap, sizeof(wrap_type), offsetof(flag_memory_table_t, struct_name)); }

#define FLAG_FLASH_READ_DECLARE(wrap_type, payload_type, struct_name, init) void read_##struct_name(uint8_t * wrap);
#define FLAG_FLASH_READ_DEFINE(wrap_type, payload_type, struct_name, init) void read_##struct_name(uint8_t * wrap){ \
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 readFlagRaw(wrap, sizeof(wrap_type), offsetof(flag_memory_table_t, struct_name)); }
/* Declare the functions to write flags to flash */
FLAG_TABLE(FLAG_FLASH_WRITE_DECLARE)

/* Declare the functions to read flags from flash */
FLAG_TABLE(FLAG_FLASH_READ_DECLARE)

/* Enum for finding number of flags or the offset by name */
typedef enum{
	FLAG_TABLE(ENUMERATE_FLAGS)
	NUM_FLAGS
} flag_memory_enum_t;

flag_memory_enum_t flagEnum;
extern flag_memory_table_t flag_memory_table;
extern void * flagPointers[NUM_FLAGS];
extern const uint8_t flagSize[NUM_FLAGS];

/* Populate the array of pointers to the byte-arrays of each flag */
#define FLAG_PTR_INIT(wrap_type, payload_type, struct_name, init) &flag_memory_table.struct_name.all,

/* Accessor macro
 * 	- use this to easily access a member of a flag
 *
 * 	Examples:
 * 		uint32_t thingy;
 *		thingy = FLAG(GEN_TELEM, min);	// get value
 *		FLAG(GEN_TELEM,min) = 23;		// set value
 */
#define FLAG(name,value) flag_memory_table.name.payload.value

#endif /* SFUSAT_SFU_FLAGS_H_ */
