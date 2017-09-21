#include "sfu_utils.h"

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



//itoa2(localEpoch, writeBuffer[7], 10, 0){
//	uint32_t
//}
//
//thing(uint32_t input, uint16_t * destination){
//
//}
