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
#define SMARTRF_SETTING_IOCFG2_ADDR           0x0000
#define SMARTRF_SETTING_IOCFG1_ADDR           0x0001
#define SMARTRF_SETTING_IOCFG0_ADDR           0x0002
#define SMARTRF_SETTING_FIFOTHR_ADDR          0x0003
#define SMARTRF_SETTING_SYNC1_ADDR            0x0004
#define SMARTRF_SETTING_SYNC0_ADDR            0x0005
#define SMARTRF_SETTING_PKTLEN_ADDR           0x0006
#define SMARTRF_SETTING_PKTCTRL1_ADDR         0x0007
#define SMARTRF_SETTING_PKTCTRL0_ADDR         0x0008
#define SMARTRF_SETTING_ADDR_ADDR             0x0009
#define SMARTRF_SETTING_CHANNR_ADDR           0x000A
#define SMARTRF_SETTING_FSCTRL1_ADDR          0x000B
#define SMARTRF_SETTING_FSCTRL0_ADDR          0x000C
#define SMARTRF_SETTING_FREQ2_ADDR            0x000D
#define SMARTRF_SETTING_FREQ1_ADDR            0x000E
#define SMARTRF_SETTING_FREQ0_ADDR            0x000F
#define SMARTRF_SETTING_MDMCFG4_ADDR          0x0010
#define SMARTRF_SETTING_MDMCFG3_ADDR          0x0011
#define SMARTRF_SETTING_MDMCFG2_ADDR          0x0012
#define SMARTRF_SETTING_MDMCFG1_ADDR          0x0013
#define SMARTRF_SETTING_MDMCFG0_ADDR          0x0014
#define SMARTRF_SETTING_DEVIATN_ADDR          0x0015
#define SMARTRF_SETTING_MCSM2_ADDR            0x0016
#define SMARTRF_SETTING_MCSM1_ADDR            0x0017
#define SMARTRF_SETTING_MCSM0_ADDR            0x0018
#define SMARTRF_SETTING_FOCCFG_ADDR           0x0019
#define SMARTRF_SETTING_BSCFG_ADDR            0x001A
#define SMARTRF_SETTING_AGCCTRL2_ADDR         0x001B
#define SMARTRF_SETTING_AGCCTRL1_ADDR         0x001C
#define SMARTRF_SETTING_AGCCTRL0_ADDR         0x001D
#define SMARTRF_SETTING_WOREVT1_ADDR          0x001E
#define SMARTRF_SETTING_WOREVT0_ADDR          0x001F
#define SMARTRF_SETTING_WORCTRL_ADDR          0x0020
#define SMARTRF_SETTING_FREND1_ADDR           0x0021
#define SMARTRF_SETTING_FREND0_ADDR           0x0022
#define SMARTRF_SETTING_FSCAL3_ADDR           0x0023
#define SMARTRF_SETTING_FSCAL2_ADDR           0x0024
#define SMARTRF_SETTING_FSCAL1_ADDR           0x0025
#define SMARTRF_SETTING_FSCAL0_ADDR           0x0026
#define SMARTRF_SETTING_RCCTRL1_ADDR          0x0027
#define SMARTRF_SETTING_RCCTRL0_ADDR          0x0028
#define SMARTRF_SETTING_FSTEST_ADDR           0x0029
#define SMARTRF_SETTING_PTEST_ADDR            0x002A
#define SMARTRF_SETTING_AGCTEST_ADDR          0x002B
#define SMARTRF_SETTING_TEST2_ADDR            0x002C
#define SMARTRF_SETTING_TEST1_ADDR            0x002D
#define SMARTRF_SETTING_TEST0_ADDR            0x002E
#define SMARTRF_SETTING_PARTNUM_ADDR          0x0030
#define SMARTRF_SETTING_VERSION_ADDR          0x0031
#define SMARTRF_SETTING_FREQEST_ADDR          0x0032
#define SMARTRF_SETTING_LQI_ADDR              0x0033
#define SMARTRF_SETTING_RSSI_ADDR             0x0034
#define SMARTRF_SETTING_MARCSTATE_ADDR        0x0035
#define SMARTRF_SETTING_WORTIME1_ADDR         0x0036
#define SMARTRF_SETTING_WORTIME0_ADDR         0x0037
#define SMARTRF_SETTING_PKTSTATUS_ADDR        0x0038
#define SMARTRF_SETTING_VCO_VC_DAC_ADDR       0x0039
#define SMARTRF_SETTING_TXBYTES_ADDR          0x003A
#define SMARTRF_SETTING_RXBYTES_ADDR          0x003B
#define SMARTRF_SETTING_RCCTRL1_STATUS_ADDR   0x003C
#define SMARTRF_SETTING_RCCTRL0_STATUS_ADDR   0x003D


// TX
#define SMARTRF_SETTING_IOCFG2_VAL_TX           0x5C //on for 3.58us, set HW 1. GDO2 is stuck, no matter the setting a single 35us pulse occures per task cycle
#define SMARTRF_SETTING_IOCFG1_VAL_TX           0x2e
#define SMARTRF_SETTING_IOCFG0_VAL_TX           0x06 //01 RX 06 TX//Asserts when sync word has been sent / received, and de-asserts at the end of the packet.
#define SMARTRF_SETTING_FIFOTHR_VAL_TX          0x47
#define SMARTRF_SETTING_SYNC1_VAL_TX            0xD3 //sync word: 1101 0011 1001 0001
#define SMARTRF_SETTING_SYNC0_VAL_TX            0x91
#define SMARTRF_SETTING_PKTLEN_VAL_TX           0x1E //62 bytes fixed length packet
#define SMARTRF_SETTING_PKTCTRL1_VAL_TX         0x04 //status byte enabled, no address check
#define SMARTRF_SETTING_PKTCTRL0_VAL_TX         0x04 //fixed packet length, CRC enabled, use FIFO, no whitening
#define SMARTRF_SETTING_ADDR_VAL_TX             0x00
#define SMARTRF_SETTING_CHANNR_VAL_TX           0x00
#define SMARTRF_SETTING_FSCTRL1_VAL_TX          0x06
#define SMARTRF_SETTING_FSCTRL0_VAL_TX          0x00
#define SMARTRF_SETTING_FREQ2_VAL_TX            0x10
#define SMARTRF_SETTING_FREQ1_VAL_TX            0xBB
#define SMARTRF_SETTING_FREQ0_VAL_TX            0x13
#define SMARTRF_SETTING_MDMCFG4_VAL_TX          0xF5
#define SMARTRF_SETTING_MDMCFG3_VAL_TX          0x83
#define SMARTRF_SETTING_MDMCFG2_VAL_TX          0x03
#define SMARTRF_SETTING_MDMCFG1_VAL_TX          0x22
#define SMARTRF_SETTING_MDMCFG0_VAL_TX          0xF8
#define SMARTRF_SETTING_DEVIATN_VAL_TX          0x13
#define SMARTRF_SETTING_MCSM2_VAL_TX            0x07 //RX timeout
#define SMARTRF_SETTING_MCSM1_VAL_TX            0x30 //idle after sending packet
#define SMARTRF_SETTING_MCSM0_VAL_TX            0x18 //autocal when going from IDLE to TX
#define SMARTRF_SETTING_FOCCFG_VAL_TX           0x16
#define SMARTRF_SETTING_BSCFG_VAL_TX            0x6C
#define SMARTRF_SETTING_AGCCTRL2_VAL_TX         0x03
#define SMARTRF_SETTING_AGCCTRL1_VAL_TX         0x40
#define SMARTRF_SETTING_AGCCTRL0_VAL_TX         0x91
#define SMARTRF_SETTING_WOREVT1_VAL_TX          0x87
#define SMARTRF_SETTING_WOREVT0_VAL_TX          0x6B
#define SMARTRF_SETTING_WORCTRL_VAL_TX          0xFB
#define SMARTRF_SETTING_FREND1_VAL_TX           0x56
#define SMARTRF_SETTING_FREND0_VAL_TX           0x10 //using PA_TABLE index 0
#define SMARTRF_SETTING_FSCAL3_VAL_TX           0xE9
#define SMARTRF_SETTING_FSCAL2_VAL_TX           0x2A
#define SMARTRF_SETTING_FSCAL1_VAL_TX           0x00
#define SMARTRF_SETTING_FSCAL0_VAL_TX           0x1F
#define SMARTRF_SETTING_RCCTRL1_VAL_TX          0x41
#define SMARTRF_SETTING_RCCTRL0_VAL_TX          0x00
#define SMARTRF_SETTING_FSTEST_VAL_TX           0x59
#define SMARTRF_SETTING_PTEST_VAL_TX            0x7F
#define SMARTRF_SETTING_AGCTEST_VAL_TX          0x3F
#define SMARTRF_SETTING_TEST2_VAL_TX            0x81
#define SMARTRF_SETTING_TEST1_VAL_TX            0x35
#define SMARTRF_SETTING_TEST0_VAL_TX            0x09
#define SMARTRF_SETTING_PARTNUM_VAL_TX          0x00
#define SMARTRF_SETTING_VERSION_VAL_TX          0x04
#define SMARTRF_SETTING_FREQEST_VAL_TX          0x00
#define SMARTRF_SETTING_LQI_VAL_TX              0x00
#define SMARTRF_SETTING_RSSI_VAL_TX             0x00
#define SMARTRF_SETTING_MARCSTATE_VAL_TX        0x00
#define SMARTRF_SETTING_WORTIME1_VAL_TX         0x00
#define SMARTRF_SETTING_WORTIME0_VAL_TX         0x00
#define SMARTRF_SETTING_PKTSTATUS_VAL_TX        0x00
#define SMARTRF_SETTING_VCO_VC_DAC_VAL_TX       0x00
#define SMARTRF_SETTING_TXBYTES_VAL_TX          0x00
#define SMARTRF_SETTING_RXBYTES_VAL_TX          0x00
#define SMARTRF_SETTING_RCCTRL1_STATUS_VAL_TX   0x00
#define SMARTRF_SETTING_RCCTRL0_STATUS_VAL_TX   0x00

#endif /* SFUSAT_SFU_SMARTRF_CC1101_H_ */
