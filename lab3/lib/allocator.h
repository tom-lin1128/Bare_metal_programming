#include "uart.h"

extern volatile unsigned int _bss_end;
extern volatile unsigned int _end;
#define nullptr ((void*)0)

void* simple_malloc(int size); 