#include "sfu_utils.h"
#include "gio.h"
#include "sfu_hardwaredefs.h"

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

        if(num> 9)
                bsbuff = utoa2(num/base, (buffer+1), base, itr+1);

        else
        {
                bsbuff = buffer;
                *(buffer+1) = '\0';
        }

        *(bsbuff - itr) = num%(base) < 10 ? num%base + 48 : num%base + 55 ;

        return itr>0 ? bsbuff: num2<0 ?buffer-1: buffer;
}
//char *dec(unsigned x, char *s)
//{ // call with end of buffer, returns beginning
//    *--s = 0;
//    if (!x) *--s = '0';
//    for (; x; x/=10) *--s = '0'+x%10;
//    return s;
//}
//
//void utoa(uint32_t x, char *s){
////	4294967296
//	char buf[10]; // all we need to hold a 32-bit int
//	uint32_t count;
//	while(x > 9){
//		buf *++ = x % 10;
//	}
//}

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
