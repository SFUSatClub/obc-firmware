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
 * We maintain the current index prefix inside the spiffs struct. This is updated every 24h and rolls over after 'g' (7 days).
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

// SFUSat Configs
#define SFU_MAX_DATA_WRITE 21 // bytes or chars. The max amount of data we can write to a file at once that is GUARANTEED not to be chopped off. The actual max depends on the time stamp.
#define SFU_WRITE_DATA_BUF (SFU_MAX_DATA_WRITE + 12) // DON'T TOUCH: to size the file write buffer

#define FSYS_OFFSET 65 // the first char of file names is 'A'
#define FSYS_NUM_SUBSYS 2 // number of subsystem logs

/* ASCII codes for the subsystem log suffix
 * These are passed to read, write so that we can grab the correct file
 * Stick with ASCII codes so we can easily iterate based on FSYS_OFFSET
 */
#define FSYS_SYS 65 // S, system log
#define FSYS_CURRENT 66 // C, current log

// Tasks
void sfu_create_fs_test(void *pvParameters);

// Functions
void write_fd(spiffs_file fd, char *fmt, ...); // printf style write to an already open file
void sfu_create_files();
void sfu_write_fname(); // write printf style data to a file name

#endif /* SPIFFS_SFU_FS_STRUCTURE_H_ */
