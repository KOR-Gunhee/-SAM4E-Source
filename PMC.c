#include ".\cpu.h"

void INITPMC(uint8_t ul_type)
{
	PMC->PMC_PCER0=(1<<ul_type);
}