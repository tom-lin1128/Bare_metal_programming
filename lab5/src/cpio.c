#include "uart.h"
#include "string.h"
#include "cpio.h"

static int to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    return 0;
}

int hex_to_int(char *s, int len) {
    int sum = 0;
    for(int i = 0; i < len; ++i) {
        sum <<= 4;
        sum += to_int(*(s+i));
    }
    return sum;
}

unsigned long align(unsigned long addr, unsigned long align_size) {
    unsigned long tmp = addr % align_size;
    if(tmp == 0) return addr;
    else return addr + (align_size - tmp);
}

void cpio_ls(unsigned long addr) {
    struct cpio_newc_header* header = (struct cpio_newc_header*)(addr == 0 ? CPIO_ADDR : addr);
    while(1) {
        unsigned long file_size = hex_to_int(header->c_filesize, 8);
        unsigned long name_size = hex_to_int(header->c_namesize, 8);
        char *pathname = (char *)((char *)header + CPIO_SIZE);
        if(strcmp(pathname, "TRAILER!!!")) break;
        printf_s(pathname);
        printf_c('\n');
        header = (struct cpio_newc_header*)((char *)header + align(CPIO_SIZE+name_size, 4) + align(file_size, 4));
    }
}

void cpio_print_file_content(char *file, unsigned long addr) {
    struct cpio_newc_header* header = (struct cpio_newc_header*)(addr == 0 ? CPIO_ADDR : addr);
    int flag = 0;
    unsigned long file_size, name_size;
    while(1) {
        file_size = hex_to_int(header->c_filesize, 8);
        name_size = hex_to_int(header->c_namesize, 8);
        char *pathname = (char *)((char *)header + CPIO_SIZE);
        if(strcmp(pathname, "TRAILER!!!")) break;
        else if(strcmp(pathname, file)) {
            flag = 1;
            break;
        }
        header = (struct cpio_newc_header*)((char *)header + align(CPIO_SIZE+name_size, 4) + align(file_size, 4));
    }
    if(flag) {
        char *file_content = (char *)((char *)header + align(CPIO_SIZE + name_size, 4));
        printf_c('\n');
        for(int i = 0; i < file_size; i++) printf_c(*(file_content + i));
        printf_c('\n');
    }
    else{
        printf_s("No such file found\n");
    }
}

char* cpio_exec(const char *file, unsigned long addr){
    struct cpio_newc_header* header = (struct cpio_newc_header*)(addr == 0 ? CPIO_ADDR : addr);
    int flag = 0;
    unsigned long file_size, name_size;
    while(1) {
        file_size = hex_to_int(header->c_filesize, 8);
        name_size = hex_to_int(header->c_namesize, 8);
        char *pathname = (char *)((char *)header + CPIO_SIZE);
        if(strcmp(pathname, "TRAILER!!!")) break;
        else if(strcmp(pathname, file)) {
            flag = 1;
            break;
        }
        header = (struct cpio_newc_header*)((char *)header + align(CPIO_SIZE+name_size, 4) + align(file_size, 4));
    }
    if(flag){
        return (char *)((char *)header + align(CPIO_SIZE + name_size, 4));
    }
    else{
       return (char*)0;
    }
}