#include "devicetree.h"
#include "string.h"
#include "uart.h"

extern volatile unsigned char _devicetree;

void parse_devicetree(){
    HEADER** header = (HEADER **)&_devicetree;
    unsigned int totalsize,off_dt_struct,size_dt_struct,off_dt_strings,size_dt_strings;
    totalsize = little_endian_to_big_endian((*header)->totalsize);
    off_dt_struct = little_endian_to_big_endian((*header)->off_dt_struct);
    size_dt_struct = little_endian_to_big_endian((*header)->size_dt_struct);
    off_dt_strings = little_endian_to_big_endian((*header)->off_dt_strings);
    size_dt_strings =  little_endian_to_big_endian((*header)->size_dt_strings);
    printf_i(totalsize);
    printf_c('\n');
    printf_i(off_dt_struct);
    printf_c('\n');
    printf_i(size_dt_struct);
    printf_c('\n');
    printf_i(off_dt_strings);
    printf_c('\n');
    printf_i(size_dt_strings);
    printf_c('\n');
    unsigned long struct_start = (unsigned long)(*header) + off_dt_struct;
    unsigned long struct_end   = struct_start + size_dt_struct;
    unsigned long string_start = (unsigned long)(*header) + off_dt_strings;
    printf_h(struct_start);
    printf_c('\n');
    printf_h(struct_end);
    printf_c('\n');

    //parse devicetree
    unsigned char *address_now = (unsigned char *)struct_start;
    printf_h((unsigned long)address_now);
    printf_c('\n');
    while((unsigned long)address_now < struct_end){
        unsigned int token = little_endian_to_big_endian(*((unsigned int*)address_now));
        //printf_h((unsigned long)address_now);
        //printf_c('\n');
        address_now += 4;
        
        if(token == FDT_BEGIN_NODE){
            char *devicename = (char *)address_now;
            address_now += strlen(devicename);
            if(strlen(devicename) != 0){
                printf_s("----------------------------------------\n");
                printf_s(devicename);
                printf_c('\n');
                if((unsigned long)address_now % 4 != 0){
                    int offset = 4 - ((unsigned long)address_now % 4);
                    address_now += offset;
                }
                printf_s("----------------------------------------\n");
            }
        }
        else if(token == FDT_PROP){
            unsigned int len = 0, nameoff = 0;
            len     = little_endian_to_big_endian(*((unsigned int*)address_now));
            address_now += 4;
            nameoff = little_endian_to_big_endian(*((unsigned int*)address_now));
            address_now += 4;
            if(len){
                char *prop_name  = (char *)string_start + nameoff;
                char *prop_value = (char *)address_now;
                printf_s("property name : ");
                printf_s(prop_name);
                printf_s(" | value : ");
                printf_s(prop_value);
                printf_c('\n');
                address_now += len;
                if((unsigned long)address_now % 4 != 0){
                    int offset = 4 - ((unsigned long)address_now % 4);
                    address_now += offset;
                }
            }
        }
        else if(token == FDT_END_NODE)  asm volatile("nop");
        else if(token == FDT_NOP)       asm volatile("nop");
        else if(token == FDT_END)       asm volatile("nop");
        else                            asm volatile("nop");
    }
}

unsigned long get_deviceprop(char *pattern){
    HEADER** header = (HEADER **)&_devicetree;
    unsigned int off_dt_struct,size_dt_struct,off_dt_strings;
    off_dt_struct = little_endian_to_big_endian((*header)->off_dt_struct);
    size_dt_struct = little_endian_to_big_endian((*header)->size_dt_struct);
    off_dt_strings = little_endian_to_big_endian((*header)->off_dt_strings);

    unsigned long struct_start = (unsigned long)(*header) + off_dt_struct;
    unsigned long struct_end   = struct_start + size_dt_struct;
    unsigned long string_start = (unsigned long)(*header) + off_dt_strings;

    unsigned char *address_now = (unsigned char *)struct_start;

    while((unsigned long)address_now < struct_end){
        unsigned int token = little_endian_to_big_endian(*((unsigned int*)address_now));
        //printf_h((unsigned long)address_now);
        //printf_c('\n');
        address_now += 4;
        
        if(token == FDT_BEGIN_NODE){
            char *devicename = (char *)address_now;
            address_now += strlen(devicename);
            if(strlen(devicename) != 0){
                if((unsigned long)address_now % 4 != 0){
                    int offset = 4 - ((unsigned long)address_now % 4);
                    address_now += offset;
                }
            }
        }
        else if(token == FDT_PROP){
            unsigned int len = 0, nameoff = 0;
            len     = little_endian_to_big_endian(*((unsigned int*)address_now));
            address_now += 4;
            nameoff = little_endian_to_big_endian(*((unsigned int*)address_now));
            address_now += 4;
            if(len){
                char *prop_name  = (char *)string_start + nameoff;
                unsigned long prop_value = little_endian_to_big_endian(*((unsigned int*)address_now));
                if(strcmp(prop_name,pattern) == 1){
                    printf_h(prop_value);
                    printf_c('\n');
                    return prop_value;
                }
                address_now += len;
                if((unsigned long)address_now % 4 != 0){
                    int offset = 4 - ((unsigned long)address_now % 4);
                    address_now += offset;
                }
            }
        }
        else if(token == FDT_END_NODE)  asm volatile("nop");
        else if(token == FDT_NOP)       asm volatile("nop");
        else if(token == FDT_END)       asm volatile("nop");
        else                            asm volatile("nop");
    }
    return 0;
}