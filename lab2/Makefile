SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS))
LIBRARY_DIR = lib
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy


all: clean kernel8.img

a.o: a.S
	$(CC) $(CFLAGS) -c a.S -o a.o

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -I$(LIBRARY_DIR) -c $< -o $@

kernel8.img: a.o $(OBJS)
	$(LINKER) -nostdlib -nostartfiles a.o $(OBJS) -T linker.ld -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf kernel8.img src/*.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -display none -serial null -serial stdio

serial:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty
