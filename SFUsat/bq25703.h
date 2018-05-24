/*
 * bq25703.h
 *
 *  Created on: May 9, 2018
 *      Author: Eric Kwok
 */

#ifndef SFUSAT_BQ25703_H_
#define SFUSAT_BQ25703_H_

//chip addresses
#define OBC_BATT 0x2C
#define OBC_CHRGR 0x20
#define OBC_ADC 0x3A
#define OBC_ILIM 0x02
#define OBC_OPT0 0x00
#define OBC_OPT1 0x30
#define OBC_IBAT 0x28

int16_t read_batt_raw(uint8_t addr);
int16_t read_batt(uint8_t addr);
void set_chrg(uint8_t addr);

#endif /* SFUSAT_BQ25703_H_ */
