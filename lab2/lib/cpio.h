#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define QEMU_CPIO_ADDRESS (volatile unsigned int*)0x8000000
#define RASPI_CPIO_ADDRESS (volatile unsigned int*)0x20000000

typedef struct cpio_newc_header {
		   char	   c_magic[6];
		   char	   c_ino[8];
		   char	   c_mode[8];
		   char	   c_uid[8];
		   char	   c_gid[8];
		   char	   c_nlink[8];
		   char	   c_mtime[8];
		   char	   c_filesize[8];
		   char	   c_devmajor[8];
		   char	   c_devminor[8];
		   char	   c_rdevmajor[8];
		   char	   c_rdevminor[8];
		   char	   c_namesize[8];
		   char	   c_check[8];
} Header;


int parse_hex_str(char*, unsigned int);
int cpio_strlen(char *);
int align(int, int);
void readdata(char **, char *, int );
void cpio_parse_header(char **, char *, char *);
void cpio_ls();
void cpio_print_file_content(char *,unsigned long);