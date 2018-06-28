/*
 * sfu_gps.c
 *
 *  Created on: Jun 6, 2018
 *      Author: Victor Leon
 */

#include "sfu_gps.h"
#include "sfu_hardwaredefs.h"
#include "sfu_task_radio.h"
#include "sfu_state.h"




unsigned char currCharGPS = '\0';
bool gps_sciSend(sciBASE_t *sci, uint32 length, uint8 * data);
bool gps_sciReceive(sciBASE_t *sci, uint32 length, uint8 * data);
bool gps_sciReceiveByte(sciBASE_t *sci, char *output);

void serialGPSInit() {
	sciInit(); //initialize the SCI driver
	sciSetBaudrate(GPS_PORT, 9600);
//	sciEnableNotification(GPS_PORT, SCI_RX_INT);  //receive buffer ready
	if(!gps_sciReceive(GPS_PORT, 1, &currCharGPS)){
		serialSendln("GPS INIT TIMEOUT");
	}
}
//
//BaseType_t serialGPSSendQ(const char * toSend) {
//	if (xQueueSendToBack(xSerialGpsRXQueue, &toSend, 0) == pdPASS) { //call returns immediately because of 0
//		return pdPASS;
//	} else {
//		return pdFAIL;
//	}
//}
//
//BaseType_t serialGPSSendQFromISR(char * toSend) {
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;														//default pdFALSE
//	BaseType_t xStatus = xQueueSendToBackFromISR(xSerialGpsRXQueue, &toSend, &xHigherPriorityTaskWoken);	//sending Priority by pointer, changes value
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);														//checks if pdFALSE or pdTRUE
//	sciReceive(GPS_PORT, 1, &currCharGPS);
//
//	return xStatus;
//}

bool gps_sciSend(sciBASE_t *sci, uint32 length, uint8 * data){
    uint32 index = (sci == sciREG) ? 0U : 1U;
    uint8 txdata;
    uint32_t timeout = 0;
//    if ((g_sciTransfer_t[index].mode & (uint32)SCI_TX_INT) != 0U){
//        /* we are in interrupt mode */
//
//        g_sciTransfer_t[index].tx_length = length;
//        g_sciTransfer_t[index].tx_data   = data;
//
//		txdata = *g_sciTransfer_t[index].tx_data;
//		sci->TD     = (uint32)(txdata);
//		g_sciTransfer_t[index].tx_data++;
//        sci->SETINT = (uint32)SCI_TX_INT;
//        return 1;
//    }
//    else{
        while (length > 0U){
            while ((sci->FLR & (uint32)SCI_TX_INT) == 0U && timeout < GPS_CHAR_TIMEOUT)
            {
            	timeout++;
            } /* Wait */
            if(timeout >= GPS_CHAR_TIMEOUT){
            	return 0;
            }
			txdata = *data;
            sci->TD = (uint32)(txdata);
			data++;
			length--;
			timeout = 0;
        }
//    }
    return 1;
}

bool gps_sciReceive(sciBASE_t *sci, uint32 length, uint8 * data){
	uint32_t timeout = 0;
//    if ((sci->SETINT & (uint32)SCI_RX_INT) == (uint32)SCI_RX_INT){
//        /* we are in interrupt mode */
//        uint32 index = (sci == sciREG) ? 0U : 1U;
//
//        /* clear error flags */
//        sci->FLR = ((uint32) SCI_FE_INT | (uint32) SCI_OE_INT | (uint32) SCI_PE_INT);
//        g_sciTransfer_t[index].rx_length = length;
//        g_sciTransfer_t[index].rx_data   = data;
//        return 1;
//    }
//    else{
        while (length > 0U){
            while (((sci->FLR & (uint32)SCI_RX_INT) == 0U ) && timeout < GPS_CHAR_TIMEOUT)
            {
            	timeout++;
            }
            if(timeout >= GPS_CHAR_TIMEOUT){
            	return 0;
            }
            *data = (uint8)(sci->RD & 0x000000FFU);
			data++;
			length--;
			timeout = 0;
        }
//    }
    return 1;
}

bool gps_sciReceiveByte(sciBASE_t *sci, char *output){
	uint32_t timeout = 0;
    while ((sci->FLR & (uint32)SCI_RX_INT) == 0U && timeout < GPS_CHAR_TIMEOUT)
    {
    	timeout++;
    } /* Wait */
    if(timeout >= GPS_CHAR_TIMEOUT){
    	return 0;
    }

    output[0] = (char)(sci->RD & (uint32)0x000000FFU);
    return 1;
}


bool serialGPSSendln(const char* stringToSend) {
	if(!	gps_sciSend(GPS_PORT, strlen(stringToSend), (unsigned char *) stringToSend)){
		serialSendln("GPS SEND FAILED");
		return 0;
	}
	if(!gps_sciSend(GPS_PORT, 2, "\r\n")){
		serialSendln("GPS SEND FAILED");
		return 0;
	}
	gps_sciReceive(GPS_PORT, 1, &currCharGPS);
	return 1;
}

void restartGPS() {
	if(!gps_sciSend(GPS_PORT, 5, "RESET")){
		serialSendln("GPS SEND FAILED");
		return;
	}
	gps_sciReceive(GPS_PORT, 1, &currCharGPS);
}

void readGPS() {
	GPSdata_t data;

	char buff[300] = {'\0'};
	uint32_t timeout = 0;
	if(serialGPSSendln("log bestposa ontime 1")){
		uint8_t hashtag = 0;
		serialSendln("GPS ");
		while (sciIsRxReady(GPS_PORT) && timeout < 3*GPS_CHAR_TIMEOUT){
			timeout++;
		}
		if(timeout >= 3*GPS_CHAR_TIMEOUT){
			serialSendln("GPS Command Failed ");
		}

		timeout = 0;
		if(gps_sciReceiveByte(GPS_PORT, &buff[0])){
			uint16_t i = 1;
			bool gps_good = 1;
			while (hashtag < 2 && gps_good){
				 if(!gps_sciReceiveByte(GPS_PORT, &buff[i])){
//					gps_good = 0;
				 }
				if(buff[i] == '#'){
					hashtag ++;
				}
				i++;
			}
			if(gps_good){
				if(strlen(buff) > 1){
					serialSendln("GPS RX DATA");
				}
				else{
					serialSendln("GPS NO DATA");
				}
				serialSend(buff);
				serialSendln("GPS DONE");
				gps_sciReceive(GPS_PORT, 1, &currCharGPS);
//				parseGPS(&buff[0], &data);
			}
//			else{
//				serialSendln("GPS Timeout");
//			}
		}
		else{
			serialSendln("Received nothing from GPS");
		}
	}
}


void parseGPS(char *gpsBuff, GPSdata_t *data){
/////////////////////////////////TEST 1/////////////////////////////////
/*
 * This was having issues with snprintf and reading past the end of memory...
 *
 */
//	struct GPSdata data;
	const char tok[] = ",";
	const char headertok[] = ";";
    char info[250] = {'\0'};
	char *tmp = gpsBuff;
	volatile int count = 0;

	int k = strcspn (tmp, headertok);
	tmp = tmp + k + 1;

	    do {
	        int l = strcspn (tmp, tok);
	        memset(info, '\0', 250);
	        memset(data->alt, '\0', 250);
	        memset(data->lat, '\0', 250);
	        memset(data->longa, '\0', 250);

	        snprintf (info, sizeof(info), "%.*s", l, tmp);
	        tmp = tmp + l + 1;

	        if (count == 2)
	        {
	          strcpy (data->lat, info);
	        }
	        if (count == 3)
	        {
	          strcpy (data->longa, info);
	        }
	        if (count == 4)
	        {
	          strcpy (data->alt, info);
	        }

	        count++;

	        } while (tmp[-1]);

	data->flag = 1;
}

/////////////////////////////////TEST 2/////////////////////////////////
//void parseGPS(char *gpsBuff){
//
//    struct GPSdata data;
//    const char tok[1] = ",";
//    const char headertok[1] = ";";
//    char info[200] = {'\0'};
//    char *tmp = gpsBuff;
//    int i = 0;
//
//    for (i = 0; i < 200 ; i++){
//        info[i] = tmp[i];
//    }
//    volatile int semicolonIndex = 0;
//    volatile int firstCommaIndex = 0;
//    volatile int secondCommaIndex = 0;
//    volatile int thirdCommaIndex = 0;
//    volatile int fourthCommaIndex = 0;
//    volatile int fifthCommaIndex = 0;
//
//    for (i = 0; i < 200 ; i++){
//        if (info[i] == ';'){
//            semicolonIndex = i;
//            break;
//        }
//    }
//
//    for (i = semicolonIndex; i < (200-semicolonIndex); i ++){
//        if (info[i] == ','){
//            firstCommaIndex = i;
//            break;
//        }
//    }
//    for (i = firstCommaIndex + 1; i < (200-firstCommaIndex); i ++){
//            if (info[i] == ','){
//                secondCommaIndex = i;
//                break;
//            }
//        }
//
//    int index1 = 0;
//    for (i = secondCommaIndex + 1; i < 200; i ++){
//        data.lat[index1] = tmp [i];
//        index1++;
//            if (info[i] == ','){
//                data.lat[index1-1] = '\0';
//                thirdCommaIndex = i;
//                break;
//            }
//        }
//
//    int index2 = 0;
//    for (i = thirdCommaIndex + 1; i < 200; i ++){
//        data.longa[index2] = tmp [i];
//        index2++;
//            if (info[i] == ','){
//                data.longa[index2-1] = '\0';
//                fourthCommaIndex = i;
//                break;
//            }
//        }
//    int index3 = 0;
//    for (i = fourthCommaIndex + 1; i < 200; i ++){
//        data.alt[index3] = tmp [i];
//        index3++;
//            if (info[i] == ','){
//                data.alt[index3-1] = '\0';
//                fifthCommaIndex = i;
//                break;
//            }
//        }
//    data.flag = 1;
//}
