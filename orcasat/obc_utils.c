#include "obc_hardwaredefs.h"
#include "obc_utils.h"
#include "gio.h"
#include "reg_system.h"

void busyWait(uint32_t ticksToWait){
	// DON'T USE THIS - only for writing quick tests
	uint32_t i = 0;
	while(i <= ticksToWait){
		i++;
	}
}

char* itoa2(int num, char *buffer, int base, int itr)
{
        char *bsbuff;
        int num2;

        num2 = num;
        if(num<0)
        {
        *(buffer++) = '-';
        num = -num;
        }

        if(num> (base-1))
                bsbuff = itoa2(num/base, (buffer+1), base, itr+1);

        if(num<= (base-1))
        {
                bsbuff = buffer;
                *(buffer+1) = '\0';
        }

        *(bsbuff - itr) = num%(base) < 10 ? num%base + 48 : num%base + 55 ;

        return itr>0 ? bsbuff: num2<0 ?buffer-1: buffer;
}

char* utoa2(uint32_t num, char *buffer, int base, int itr)
{
        char *bsbuff;
        int num2;

        num2 = num;

        if(num> (base-1))
                bsbuff = utoa2(num/base, (buffer+1), base, itr+1);

        if(num<= (base-1))
        {
                bsbuff = buffer;
                *(buffer+1) = '\0';
        }

        *(bsbuff - itr) = num%(base) < 10 ? num%base + 48 : num%base + 55 ;

        return itr>0 ? bsbuff: num2<0 ?buffer-1: buffer;
}


void simpleWatchdog(){
	gioSetBit(WATCHDOG_TICKLE_PORT, WATCHDOG_TICKLE_PIN, 1);
	busyWait(5000);
	gioSetBit(WATCHDOG_TICKLE_PORT, WATCHDOG_TICKLE_PIN, 0);
}

void watchdog_busywait(int32_t numtickles){
	while(numtickles > 0){
		simpleWatchdog();
		numtickles--;
	}
}

void clearBuf(char *buf,uint32_t length){
	// use memset to fill empty chars into the buffer
	memset(buf, '\0', length);
}

uint32_t adc_to_mA(uint32_t adcval){
	/* Based on the following data:
	 * Current Draw   |   ADC
	 * -----------------------------
	 *   133 mA       |   600
	 *   86 mA        |   590
	 *
	 *   Linear fit: current = 4.7 * ADC - 2687
	 *   Converting to something we can do fixed point: 47 * ADC - 26870
	 */

	return (47*adcval-26870)/10;
}

void restart_software() {
	systemREG1->SYSECR = systemREG1->SYSECR | (1U << 15);
}
