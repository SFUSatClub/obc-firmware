/*
 * bq25703.h
 *
 *  Created on: May 9, 2018
 *      Author: Eric Kwok
 */

#ifndef SFUSAT_BQ25703_H_
#define SFUSAT_BQ25703_H_

#define BATT_CHRG 0x6B // i2C address of the BMS chip

/* battery_status_bits struct
 * - gives easy to use names to the fields we read in
 * - combined with the union, we can easily update the struct with the current register values
 * - makes it really easy to figure out what's happening
 */
typedef struct battery_status_bits{
	uint16_t fault_otg_ocp: 1;
	uint16_t fault_otg_ovp: 1;
	uint16_t fault_latchoff: 1;
	uint16_t res1: 1;
	uint16_t sysovp_stat: 1;
	uint16_t fault_acoc: 1;
	uint16_t fault_batoc: 1;
	uint16_t fault_acov: 1;
	uint16_t in_otg: 1;
	uint16_t in_pchrg: 1;
	uint16_t in_fchrg: 1;
	uint16_t in_iindpm: 1;
	uint16_t in_vindpm: 1;
	uint16_t res: 1;
	uint16_t ico_done: 1;
	uint16_t ac_stat: 1;
} BATTERY_STATUS_BITS_t ;

/* battery_status
 * 	- by reading the two halves of the register, shifting the MSB and assigning to the 'struct_assign' member, we update the battery status bits struct
 */
typedef union battery_status{
	BATTERY_STATUS_BITS_t bits_struct;
	uint16_t struct_assign;
} BATTERY_STATUS_t;

// register addresses (LSByte)
#define CHARGERSTATUS_REG 	0x20
#define ADCOPTION_REG_MSB	0x3B
#define ADCOPTION_REG_LSB	0x3A
#define ADCVSYSVBAT_REG 	0x2C
#define CHARGEOPT0_REG		0x01
#define CHARGEOPT1_REG		0x31
#define ADCIBAT_REG			0x28

/* Status reg bitmask */
/* MSByte of status register */
#define STAT_AC_STAT_BIT 	0b10000000
#define STAT_IN_VINDPM_BIT	0b00010000
#define STAT_IN_IINDPM_BIT	0b00001000
#define STAT_IN_FCHRG_BIT	0b00000100
#define STAT_IN_PCHRG_BIT	0b00000010

/* ADC Option Bitmask */
/* MSByte of option register */
#define ADCOPT_ADC_CONV_BIT		0b10000000
#define ADCOPT_ADC_START_BIT	0b01000000

/* MSByte of Charge Option 0 register */
#define CHARGEOPT0_EN_LWPWR		0b10000000

/* MSByte of Charge Option 1 register */
#define CHARGEOPT1_EN_IBAT 		0b10000000

/* LSByte of option register */
#define ADCOPT_EN_ADC_CMPIN_BIT	0b10000000
#define ADCOPT_EN_ADC_VBUS_BIT	0b01000000
#define ADCOPT_EN_ADC_PSYS_BIT	0b00100000
#define ADCOPT_EN_ADC_IIN		0b00010000
#define ADCOPT_EN_ADC_IDCHG		0b00001000
#define ADCOPT_EN_ADC_ICHG		0b00000100
#define ADCOPT_EN_ADC_VSYS		0b00000010
#define ADCOPT_EN_ADC_VBAT		0b00000001

// TODO: rename these
//chip addresses
#define OBC_BATT 		0x2C
#define OBC_CHRGR 		0x20
#define OBC_ADC 		0x3A
#define OBC_ILIM_MSB	0x03
#define OBC_ILIM_LSB	0x02
#define OBC_OPT0 		0x00
#define OBC_OPT1		0x30
#define OBC_IBAT 		0x28

uint8_t read_batt_volt_raw(uint8_t addr);
uint8_t read_volt(uint8_t addr);
uint16_t read_curr_raw(uint8_t addr);
uint16_t read_curr(uint8_t addr);
void set_chrg(uint8_t addr);
int16_t get_chrg_stat(uint8_t addr);

/* These functions read/write a single byte to a register
 * 	- to read/write the MSByte, add 1 to the base address defined in the "register addresses" section above
 */
int16_t read_register(uint8_t addr, uint8_t *reg_return);
int16_t write_register(uint8_t addr, uint8_t *to_write);

/* helpers/testing functions */
BATTERY_STATUS_BITS_t get_bms_status(void);
void bms_test();


#endif /* SFUSAT_BQ25703_H_ */
