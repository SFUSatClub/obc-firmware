/*
 * sfu_fs_structure.h
 *
 *  Created on: Mar 13, 2018
 *      Author: Richard
 *
 * OVERVIEW
 *
 * File name format: <index><subsystem suffix>
 * ex: aA = first system log
 * ex: bA = second system log
 * ex: eB = 5th current log
 *
 * We maintain the current index prefix inside the spiffs struct. This is updated every FSYS_LOOP_INTERVAL and rolls over after letter PREFIX_QUANTITY
 * for flight this will be 'g' (7 days).
 * We start back up at 'a,' having deleted all previous files with the 'a' index prefix.
 *
 * SPIFFS doesn't have directories, which is why we need to do this. We bulk-delete files based on the index prefix, making it most
 * efficient to have this indicator first. A well-defined name structure makes it easy to construct the file name, and it can be compact
 * since we don't have a particularly large number of files at any one time.
 */

#ifndef SPIFFS_SFU_FS_STRUCTURE_H_
#define SPIFFS_SFU_FS_STRUCTURE_H_

#include "spiffs.h"
#include "sfusat_spiffs.h"
#include "FreeRTOS.h"
#include "rtos_task.h"

extern TaskHandle_t xSPIFFSHandle;
extern TaskHandle_t xSPIFFSRead;

/* commands */
void dumpFile(char prefix, char suffix);
char currentPrefix(void);

/* SFUSat Configs */
#define SFU_MAX_DATA_WRITE 21  							/* bytes or chars. The max amount of data we can write to a file at once that is GUARANTEED not to be chopped off. The actual max depends on the time stamp. */
#define SFU_WRITE_DATA_BUF (SFU_MAX_DATA_WRITE + 12) 	/* DON'T TOUCH: to size the file write buffer */
#define FSYS_OFFSET 65 									/* the first char of file names is 'A' */
#define FSYS_NUM_SUBSYS 4 								/* number of subsystem logs */

/* ASCII codes for the subsystem log suffix
 * 	- These are passed to read, write so that we can grab the correct file
 * 	- Stick with ASCII codes so we can easily iterate based on FSYS_OFFSET
 * 	- Not semantic, but go in alphabetical order (makes it easy to create the entire set of files)
 */
#define FSYS_SYS 		65 		/* A, system log */
#define FSYS_ERROR 		66 		/* B, error log */
#define OBC_CURRENT 	67 		/* C, current log */
#define TEMPS 			68 		/* D, temperature log */
#define FSYS_FLAGS		69		/* E, flags */

#define FSYS_LOOP_INTERVAL pdMS_TO_TICKS(90000) /* we create new file sets on this interval */
/* Prefix stuff */
#define PREFIX_START 97 				/* a, start of prefixes */
#define PREFIX_QUANTITY 3 				/* number of unique prefixes to loop through */

/* variables */
extern uint32_t fs_num_increments;

/* Tasks */
void vFilesystemLifecycleTask(void *pvParameters);
void fs_rando_write(void *pvParameters);
void fs_read_task(void *pvParameters);
void fs_test_tasks();

/* Functions */
void sfu_fs_init();
void delete_oldest(); 									/* handles deletion and creation. Note: takes about a second to run */
void sfu_create_files(); 								/* creates files w/ current prefix and records creation time */
void sfu_create_files_wrapped(); 						/* creates files w/ current prefix and records creation time, wrapped in mutex */
void sfu_create_persistent_files();
void create_filename(char* namebuf, char file_suffix); /* creates filename with appropriate prefix and suffix */
void sfu_write_fname(char f_suffix, char *fmt, ...); 	/* write printf style data to a file name */
void sfu_write_fname_offset(char f_suffix, uint32_t offset, char *fmt, ...);
void sfu_read_fname(char f_suffix, uint8_t* outbuf, uint32_t size);
void format_entry(char* buf, char *fmt, va_list argptr); /* formats our file entries with timestamp and data */
void write_fd(spiffs_file fd, char *fmt, ...); 			/* printf style write to an already open file */
void sfu_delete_prefix(const char prefix); 				/* deletes the files with the specified prefix */
void increment_prefix();
#endif /* SPIFFS_SFU_FS_STRUCTURE_H_ */




