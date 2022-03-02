#include "uart.h"

extern volatile unsigned int _bss_end;
extern volatile unsigned int _end;

void* simple_malloc(int size); 