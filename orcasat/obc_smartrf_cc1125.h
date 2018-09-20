/*
 * sfu_smartrf_cc1125.h
 *
 *  Created on: Apr 30, 2017
 *      Author: steven
 *
 *      DEPRECATED
 *
 *      Our radio was initially the CC1125, but we've now moved to the CC1101.
 *      TODO: Delete this.
 */

#ifndef SFUSAT_OBC_SMARTRF_CC1125_H_
#define SFUSAT_OBC_SMARTRF_CC1125_H_


// RF settings for CC1125
typedef struct {
    uint8 iocfg3;           // GPIO3 IO Pin Configuration
    uint8 iocfg2;           // GPIO2 IO Pin Configuration
    uint8 iocfg1;           // GPIO1 IO Pin Configuration
    uint8 iocfg0;           // GPIO0 IO Pin Configuration
    uint8 sync_cfg1;        // Sync Word Detection Configuration Reg. 1
    uint8 deviation_m;      // Frequency Deviation Configuration
    uint8 modcfg_dev_e;     // Modulation Format and Frequency Deviation Configur..
    uint8 dcfilt_cfg;       // Digital DC Removal Configuration
    uint8 freq_if_cfg;      // RX Mixer Frequency Configuration
    uint8 iqic;             // Digital Image Channel Compensation Configuration
    uint8 chan_bw;          // Channel Filter Configuration
    uint8 mdmcfg0;          // General Modem Parameter Configuration Reg. 0
    uint8 symbol_rate2;     // Symbol Rate Configuration Exponent and Mantissa [1..
    uint8 symbol_rate1;     // Symbol Rate Configuration Mantissa [15:8]
    uint8 symbol_rate0;     // Symbol Rate Configuration Mantissa [7:0]
    uint8 agc_ref;          // AGC Reference Level Configuration
    uint8 agc_cs_thr;       // Carrier Sense Threshold Configuration
    uint8 agc_cfg1;         // Automatic Gain Control Configuration Reg. 1
    uint8 agc_cfg0;         // Automatic Gain Control Configuration Reg. 0
    uint8 fifo_cfg;         // FIFO Configuration
    uint8 fs_cfg;           // Frequency Synthesizer Configuration
    uint8 pkt_cfg0;         // Packet Configuration Reg. 0
    uint8 pkt_len;          // Packet Length Configuration
    uint8 if_mix_cfg;       // IF Mix Configuration
    uint8 freqoff_cfg;      // Frequency Offset Correction Configuration
    uint8 freq2;            // Frequency Configuration [23:16]
    uint8 freq1;            // Frequency Configuration [15:8]
    uint8 freq0;            // Frequency Configuration [7:0]
    uint8 if_adc0;          // Analog to Digital Converter Configuration Reg. 0
    uint8 fs_dig1;          // Frequency Synthesizer Digital Reg. 1
    uint8 fs_dig0;          // Frequency Synthesizer Digital Reg. 0
    uint8 fs_cal0;          // Frequency Synthesizer Calibration Reg. 0
    uint8 fs_divtwo;        // Frequency Synthesizer Divide by 2
    uint8 fs_dsm0;          // FS Digital Synthesizer Module Configuration Reg. 0
    uint8 fs_dvc0;          // Frequency Synthesizer Divider Chain Configuration ..
    uint8 fs_pfd;           // Frequency Synthesizer Phase Frequency Detector Con..
    uint8 fs_pre;           // Frequency Synthesizer Prescaler Configuration
    uint8 fs_reg_div_cml;   // Frequency Synthesizer Divider Regulator Configurat..
    uint8 fs_spare;         // Frequency Synthesizer Spare
    uint8 xosc5;            // Crystal Oscillator Configuration Reg. 5
    uint8 xosc3;            // Crystal Oscillator Configuration Reg. 3
    uint8 xosc1;            // Crystal Oscillator Configuration Reg. 1
} RF_SETTINGS;

#define SMARTRF_RADIO_CC1125
#define SMARTRF_SETTING_IOCFG3           0xB0
#define SMARTRF_SETTING_IOCFG2           0x06
#define SMARTRF_SETTING_IOCFG1           0xB0
#define SMARTRF_SETTING_IOCFG0           0x40
#define SMARTRF_SETTING_SYNC_CFG1        0x08
#define SMARTRF_SETTING_DEVIATION_M      0xA3
#define SMARTRF_SETTING_MODCFG_DEV_E     0x0A
#define SMARTRF_SETTING_DCFILT_CFG       0x1C
#define SMARTRF_SETTING_FREQ_IF_CFG      0x33
#define SMARTRF_SETTING_IQIC             0xC6
#define SMARTRF_SETTING_CHAN_BW          0x10
#define SMARTRF_SETTING_MDMCFG0          0x05
#define SMARTRF_SETTING_SYMBOL_RATE2     0x3F
#define SMARTRF_SETTING_SYMBOL_RATE1     0x75
#define SMARTRF_SETTING_SYMBOL_RATE0     0x10
#define SMARTRF_SETTING_AGC_REF          0x20
#define SMARTRF_SETTING_AGC_CS_THR       0x19
#define SMARTRF_SETTING_AGC_CFG1         0xA9
#define SMARTRF_SETTING_AGC_CFG0         0xCF
#define SMARTRF_SETTING_FIFO_CFG         0x00
#define SMARTRF_SETTING_FS_CFG           0x12
#define SMARTRF_SETTING_PKT_CFG0         0x20
#define SMARTRF_SETTING_PKT_LEN          0xFF
#define SMARTRF_SETTING_IF_MIX_CFG       0x00
#define SMARTRF_SETTING_FREQOFF_CFG      0x22
#define SMARTRF_SETTING_FREQ2            0x56
#define SMARTRF_SETTING_FREQ1            0xEC
#define SMARTRF_SETTING_FREQ0            0x28
#define SMARTRF_SETTING_IF_ADC0          0x05
#define SMARTRF_SETTING_FS_DIG1          0x00
#define SMARTRF_SETTING_FS_DIG0          0x5F
#define SMARTRF_SETTING_FS_CAL0          0x0E
#define SMARTRF_SETTING_FS_DIVTWO        0x03
#define SMARTRF_SETTING_FS_DSM0          0x33
#define SMARTRF_SETTING_FS_DVC0          0x17
#define SMARTRF_SETTING_FS_PFD           0x50
#define SMARTRF_SETTING_FS_PRE           0x6E
#define SMARTRF_SETTING_FS_REG_DIV_CML   0x14
#define SMARTRF_SETTING_FS_SPARE         0xAC
#define SMARTRF_SETTING_XOSC5            0x0E
#define SMARTRF_SETTING_XOSC3            0xC7
#define SMARTRF_SETTING_XOSC1            0x07

#define SMARTRF_SETTING_IOCFG3_ADDR           0x0000
#define SMARTRF_SETTING_IOCFG2_ADDR           0x0001
#define SMARTRF_SETTING_IOCFG1_ADDR           0x0002
#define SMARTRF_SETTING_IOCFG0_ADDR           0x0003
#define SMARTRF_SETTING_SYNC_CFG1_ADDR        0x0008
#define SMARTRF_SETTING_DEVIATION_M_ADDR      0x000A
#define SMARTRF_SETTING_MODCFG_DEV_E_ADDR     0x000B
#define SMARTRF_SETTING_DCFILT_CFG_ADDR       0x000C
#define SMARTRF_SETTING_FREQ_IF_CFG_ADDR      0x000F
#define SMARTRF_SETTING_IQIC_ADDR             0x0010
#define SMARTRF_SETTING_CHAN_BW_ADDR          0x0011
#define SMARTRF_SETTING_MDMCFG0_ADDR          0x0013
#define SMARTRF_SETTING_SYMBOL_RATE2_ADDR     0x0014
#define SMARTRF_SETTING_SYMBOL_RATE1_ADDR     0x0015
#define SMARTRF_SETTING_SYMBOL_RATE0_ADDR     0x0016
#define SMARTRF_SETTING_AGC_REF_ADDR          0x0017
#define SMARTRF_SETTING_AGC_CS_THR_ADDR       0x0018
#define SMARTRF_SETTING_AGC_CFG1_ADDR         0x001C
#define SMARTRF_SETTING_AGC_CFG0_ADDR         0x001D
#define SMARTRF_SETTING_FIFO_CFG_ADDR         0x001E
#define SMARTRF_SETTING_FS_CFG_ADDR           0x0021
#define SMARTRF_SETTING_PKT_CFG0_ADDR         0x0028
#define SMARTRF_SETTING_PKT_LEN_ADDR          0x002E
#define SMARTRF_SETTING_IF_MIX_CFG_ADDR       0x2F00
#define SMARTRF_SETTING_FREQOFF_CFG_ADDR      0x2F01
#define SMARTRF_SETTING_FREQ2_ADDR            0x2F0C
#define SMARTRF_SETTING_FREQ1_ADDR            0x2F0D
#define SMARTRF_SETTING_FREQ0_ADDR            0x2F0E
#define SMARTRF_SETTING_IF_ADC0_ADDR          0x2F11
#define SMARTRF_SETTING_FS_DIG1_ADDR          0x2F12
#define SMARTRF_SETTING_FS_DIG0_ADDR          0x2F13
#define SMARTRF_SETTING_FS_CAL0_ADDR          0x2F17
#define SMARTRF_SETTING_FS_DIVTWO_ADDR        0x2F19
#define SMARTRF_SETTING_FS_DSM0_ADDR          0x2F1B
#define SMARTRF_SETTING_FS_DVC0_ADDR          0x2F1D
#define SMARTRF_SETTING_FS_PFD_ADDR           0x2F1F
#define SMARTRF_SETTING_FS_PRE_ADDR           0x2F20
#define SMARTRF_SETTING_FS_REG_DIV_CML_ADDR   0x2F21
#define SMARTRF_SETTING_FS_SPARE_ADDR         0x2F22
#define SMARTRF_SETTING_XOSC5_ADDR            0x2F32
#define SMARTRF_SETTING_XOSC3_ADDR            0x2F34
#define SMARTRF_SETTING_XOSC1_ADDR            0x2F36

#endif /* SFUSAT_OBC_SMARTRF_CC1125_H_ */
