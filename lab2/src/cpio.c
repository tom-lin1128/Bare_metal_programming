#include "uart.h"
#include "string.h"
#include "cpio.h"
int parse_hex_str(char *s, unsigned int max_len){
    unsigned int r = 0;
    unsigned int i;

    for (i = 0; i < max_len; i++) {
        r *= 16;
        if (s[i] >= '0' && s[i] <= '9') {
            r += s[i] - '0';
        }  else if (s[i] >= 'a' && s[i] <= 'f') {
            r += s[i] - 'a' + 10;
        }  else if (s[i] >= 'A' && s[i] <= 'F') {
            r += s[i] - 'A' + 10;
        } else {
            return r;
        }
        continue;
    }
    return r;
}

int align(int origin, int option){
    int answer = 0;

    switch (option)
    {
    case 1:
        if ((origin + 6) % 4 > 0)
            answer = ((origin + 6) / 4 + 1) * 4 - 6;
        else
            answer = origin;
        break;

    case 2:
        if (origin % 4 > 0)
            answer = (origin / 4 + 1) * 4;
        else
            answer = origin;
        break;

    default:
        break;
    }

    return answer;
}

void readdata(char **address, char *target, int count){
    while (count--)
    {
        *target = **address;
        (*address)++;
        target++;
    }
}

void cpio_parse_header(char **ramdk, char *file_name, char *file_content)
{
    Header header;
    int file_size = 0, name_size = 0;

    readdata(ramdk, header.c_magic, 6);
    (*ramdk) += 48;
    readdata(ramdk, header.c_filesize, 8);
    (*ramdk) += 32;
    readdata(ramdk, header.c_namesize, 8);
    (*ramdk) += 8;

    name_size = align(parse_hex_str(header.c_namesize,8), 1);
    file_size = align(parse_hex_str(header.c_filesize,8), 2);


    readdata(ramdk, file_name, name_size);
    readdata(ramdk, file_content, file_size);

    file_name[name_size] = '\0';
    file_content[file_size] = '\0';
}

void cpio_ls(unsigned long address)
{
    char file_name[100];
    char file_content[1000];

    //char *ramdk = (char *)QEMU_CPIO_ADDRESS;
    char *ramdk = (address == 0) ? (char *)RASPI_CPIO_ADDRESS : (char *)address;
    printf_h(ramdk);
    printf_c('\n');

    while (1)
    {
        cpio_parse_header(&ramdk, file_name, file_content);

        if ((strcmp(file_name,CPIO_FOOTER_MAGIC) == 1)){
            break;}

        uart_puts(file_name);
        uart_send('\n');
    }
}

void cpio_print_file_content(char *file_name_to_find,unsigned long address)
{
    char file_name[100];
    char file_content[1000];

    char *ramdk = (address == 0) ? (char *)RASPI_CPIO_ADDRESS : (char *)address;
    printf_h(ramdk);
    printf_c('\n');
    int f  = 0;
    while(1)
    {
        cpio_parse_header(&ramdk, file_name, file_content);
        if ((strcmp(file_name, file_name_to_find) == 1))
        {
            f = 1;
            break;
        }
        else if ((strcmp(file_name, CPIO_FOOTER_MAGIC) == 1))
            break;
    }

    if (f)
        printf_s(file_content);
    else
        printf_s("not found!");

    printf_c('\n');
}