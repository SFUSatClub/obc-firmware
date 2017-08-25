#include "sfu_utils.h"

void busyWait(uint32_t ticksToWait){
	// DON'T USE THIS - only for writing quick tests
	uint32_t i = 0;
	while(i <= ticksToWait){
		i++;
	}
}
