/*
 * sfu_config.h
 *
 *  Created on: May 26, 2018
 *      Author: Richard
 */

#ifndef SFUSAT_OBC_CONFIG_H_
#define SFUSAT_OBC_CONFIG_H_


/* Configuration item
 * 	- contains the value of the configuration item and its file location
 */
typedef struct{
	uint32_t value;
	uint32_t location;
} config_item_t;

/* Satellite config struct
 * 	- allows for system configurability
 * 	- values are stored in flash for retrieval
 */
typedef struct{
	config_item_t stdTelemDelay {.value = 10000, .location = 0},
	config_item_t fsLifeDelay {.value = 10000, .location = 8},
} sat_config_t;


#endif /* SFUSAT_OBC_CONFIG_H_ */
