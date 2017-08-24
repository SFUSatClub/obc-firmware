/*
 * sfu_smartrf_cc1101.h
 *
 *  Created on: Jul 20, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_SMARTRF_CC1101_H_
#define SFUSAT_SFU_SMARTRF_CC1101_H_


// RF settings for CC1101
typedef struct {
    uint8 iocfg0;     // GDO0 Output Pin Configuration
    uint8 fifothr;    // RX FIFO and TX FIFO Thresholds
    uint8 sync1;      // Sync Word, High Byte
    uint8 sync0;      // Sync Word, Low Byte
    uint8 pktlen;     // Packet Length
    uint8 pktctrl0;   // Packet Automation Control
    uint8 fsctrl1;    // Frequency Synthesizer Control
    uint8 freq2;      // Frequency Control Word, High Byte
    uint8 freq1;      // Frequency Control Word, Middle Byte
    uint8 freq0;      // Frequency Control Word, Low Byte
    uint8 mdmcfg4;    // Modem Configuration
    uint8 mdmcfg3;    // Modem Configuration
    uint8 mdmcfg2;    // Modem Configuration
    uint8 mdmcfg1;    // Modem Configuration
    uint8 deviatn;    // Modem Deviation Setting
    uint8 mcsm1;      // Main Radio Control State Machine Configuration
    uint8 mcsm0;      // Main Radio Control State Machine Configuration
    uint8 foccfg;     // Frequency Offset Compensation Configuration
    uint8 agcctrl2;   // AGC Control
    uint8 agcctrl1;   // AGC Control
    uint8 worctrl;    // Wake On Radio Control
    uint8 fscal3;     // Frequency Synthesizer Calibration
    uint8 fscal2;     // Frequency Synthesizer Calibration
    uint8 fscal1;     // Frequency Synthesizer Calibration
    uint8 fscal0;     // Frequency Synthesizer Calibration
    uint8 test2;      // Various Test Settings
    uint8 test1;      // Various Test Settings
    uint8 test0;      // Various Test Settings
} RF_SETTINGS;

// ADDR
#define SMARTRF_SETTING_IOCFG0_ADDR     0x0002
#define SMARTRF_SETTING_FIFOTHR_ADDR    0x0003
#define SMARTRF_SETTING_SYNC1_ADDR      0x0004
#define SMARTRF_SETTING_SYNC0_ADDR      0x0005
#define SMARTRF_SETTING_PKTLEN_ADDR     0x0006
#define SMARTRF_SETTING_PKTCTRL0_ADDR   0x0008
#define SMARTRF_SETTING_FSCTRL1_ADDR    0x000B
#define SMARTRF_SETTING_FREQ2_ADDR      0x000D
#define SMARTRF_SETTING_FREQ1_ADDR      0x000E
#define SMARTRF_SETTING_FREQ0_ADDR      0x000F
#define SMARTRF_SETTING_MDMCFG4_ADDR    0x0010
#define SMARTRF_SETTING_MDMCFG3_ADDR    0x0011
#define SMARTRF_SETTING_MDMCFG2_ADDR    0x0012
#define SMARTRF_SETTING_MDMCFG1_ADDR    0x0013
#define SMARTRF_SETTING_DEVIATN_ADDR    0x0015
#define SMARTRF_SETTING_MCSM1_ADDR      0x0017
#define SMARTRF_SETTING_MCSM0_ADDR      0x0018
#define SMARTRF_SETTING_FOCCFG_ADDR     0x0019
#define SMARTRF_SETTING_AGCCTRL2_ADDR   0x001B
#define SMARTRF_SETTING_AGCCTRL1_ADDR   0x001C
#define SMARTRF_SETTING_WORCTRL_ADDR    0x0020
#define SMARTRF_SETTING_FSCAL3_ADDR     0x0023
#define SMARTRF_SETTING_FSCAL2_ADDR     0x0024
#define SMARTRF_SETTING_FSCAL1_ADDR     0x0025
#define SMARTRF_SETTING_FSCAL0_ADDR     0x0026
#define SMARTRF_SETTING_TEST2_ADDR      0x002C
#define SMARTRF_SETTING_TEST1_ADDR      0x002D
#define SMARTRF_SETTING_TEST0_ADDR      0x002E

// RX
#define SMARTRF_SETTING_IOCFG0_VAL_RX     0x06
#define SMARTRF_SETTING_FIFOTHR_VAL_RX    0x4F
#define SMARTRF_SETTING_SYNC1_VAL_RX      0x7A
#define SMARTRF_SETTING_SYNC0_VAL_RX      0x0E
#define SMARTRF_SETTING_PKTLEN_VAL_RX     0x14
#define SMARTRF_SETTING_PKTCTRL1_VAL_RX   0x0A
#define SMARTRF_SETTING_PKTCTRL0_VAL_RX   0x04
#define SMARTRF_SETTING_FSCTRL1_VAL_RX    0x06
#define SMARTRF_SETTING_FREQ2_VAL_RX      0x21
#define SMARTRF_SETTING_FREQ1_VAL_RX      0x62
#define SMARTRF_SETTING_FREQ0_VAL_RX      0x76
#define SMARTRF_SETTING_MDMCFG4_VAL_RX    0xCA
#define SMARTRF_SETTING_MDMCFG3_VAL_RX    0xF8
#define SMARTRF_SETTING_MDMCFG2_VAL_RX    0x16
#define SMARTRF_SETTING_MDMCFG1_VAL_RX    0xA2
#define SMARTRF_SETTING_DEVIATN_VAL_RX    0x40
#define SMARTRF_SETTING_MCSM1_VAL_RX      0x3F
#define SMARTRF_SETTING_MCSM0_VAL_RX      0x1A
#define SMARTRF_SETTING_FOCCFG_VAL_RX     0x16
#define SMARTRF_SETTING_AGCCTRL2_VAL_RX   0x43
#define SMARTRF_SETTING_AGCCTRL1_VAL_RX   0x49
#define SMARTRF_SETTING_WORCTRL_VAL_RX    0xFB
#define SMARTRF_SETTING_FSCAL3_VAL_RX     0xE9
#define SMARTRF_SETTING_FSCAL2_VAL_RX     0x2A
#define SMARTRF_SETTING_FSCAL1_VAL_RX     0x00
#define SMARTRF_SETTING_FSCAL0_VAL_RX     0x1F
#define SMARTRF_SETTING_TEST2_VAL_RX      0x81
#define SMARTRF_SETTING_TEST1_VAL_RX      0x35
#define SMARTRF_SETTING_TEST0_VAL_RX      0x09

// TX
#define SMARTRF_SETTING_IOCFG0_VAL_TX     0x06
#define SMARTRF_SETTING_FIFOTHR_VAL_TX    0x40
#define SMARTRF_SETTING_SYNC1_VAL_TX      0x7A
#define SMARTRF_SETTING_SYNC0_VAL_TX      0x0E
#define SMARTRF_SETTING_PKTLEN_VAL_TX     0x14
#define SMARTRF_SETTING_PKTCTRL0_VAL_TX   0x04
#define SMARTRF_SETTING_FSCTRL1_VAL_TX    0x06
#define SMARTRF_SETTING_FREQ2_VAL_TX      0x21
#define SMARTRF_SETTING_FREQ1_VAL_TX      0x62
#define SMARTRF_SETTING_FREQ0_VAL_TX      0x76
#define SMARTRF_SETTING_MDMCFG4_VAL_TX    0xCA
#define SMARTRF_SETTING_MDMCFG3_VAL_TX    0xF8
#define SMARTRF_SETTING_MDMCFG2_VAL_TX    0x16
#define SMARTRF_SETTING_MDMCFG1_VAL_TX    0xA2
#define SMARTRF_SETTING_DEVIATN_VAL_TX    0x40
#define SMARTRF_SETTING_MCSM1_VAL_TX      0x1A
#define SMARTRF_SETTING_MCSM0_VAL_TX      0x1A
#define SMARTRF_SETTING_FOCCFG_VAL_TX     0x16
#define SMARTRF_SETTING_AGCCTRL2_VAL_TX   0x43
#define SMARTRF_SETTING_AGCCTRL1_VAL_TX   0x49
#define SMARTRF_SETTING_WORCTRL_VAL_TX    0xFB
#define SMARTRF_SETTING_FSCAL3_VAL_TX     0xE9
#define SMARTRF_SETTING_FSCAL2_VAL_TX     0x2A
#define SMARTRF_SETTING_FSCAL1_VAL_TX     0x00
#define SMARTRF_SETTING_FSCAL0_VAL_TX     0x1F
#define SMARTRF_SETTING_TEST2_VAL_TX      0x81
#define SMARTRF_SETTING_TEST1_VAL_TX      0x35
#define SMARTRF_SETTING_TEST0_VAL_TX      0x09

#endif /* SFUSAT_SFU_SMARTRF_CC1101_H_ */
