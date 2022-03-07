#include "string.h"

int strlen(char *s){
	int i = 0;
	while(*(s) != '\0'){
		s = s+1;
		i++;
	}
	return i;
}

int strcmp(char *s1, char *s2){
	if(strlen(s1) != strlen(s2)) return 0;
	for(int i = 0; i< strlen(s1); i++){
		if(s1[i] != s2[i])	return 0;
	}
	return 1;
}

int strncmp(char* s1, char* s2, int n)
{
	int count = 0;

	if (*s1 == '\0' && *s2 != 0)
	{
		return 0;
	}

	if (*s2 == '\0' && *s1 != 0)
	{
		return 0;
	}

	for (; *s1 != '\0' && *s2 != '\0' &&count<n; s1++, s2++,count++)
	{
		if ((*s1 - *s2) > 0)
		{
			return 0;
		}

		else if ((*s1 - *s2) <0)
		{
			return 0;
		}
	}

	return 1;
}

void stritohex ( unsigned int  d, int size, char * s ){
    int i = 0;
    unsigned int n;
    int c;

    c = size * 8;
    s[0] = '0';
    s[1] = 'x';

    for( c = c - 4, i = 2; c >= 0; c -= 4, i++)
    {
        // get highest tetrad
        n = ( d >> c ) & 0xF;

        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        if ( n > 9 && n < 16 )
            n = ('A' + n - 10);
        else
            n += '0';
       
        s[i] = n;
    }

    s[i] = '\0';
}

int little_endian_to_big_endian(int address){
    address = ((address>>24)&0xff) | // move byte 3 to byte 0
               ((address>>8)&0xff00) | // move byte 2 to byte 1
               ((address<<8)&0xff0000) | // move byte 1 to byte 2
               ((address<<24)&0xff000000); // byte 0 to byte 3
    return address;
}