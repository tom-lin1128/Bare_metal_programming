SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS))
ARMS = $(wildcard *.S)
ARMO = $(ARMS:.S=.o)
LIBRARY_DIR = lib
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -g
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy


all: clean kernel8.img

%.o: %.S 
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -I$(LIBRARY_DIR) -c $< -o $@

kernel8.img: $(ARMO) $(OBJS)
	$(LINKER) -nostdlib -nostartfiles $(ARMO) $(OBJS) -T linker.ld -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf kernel8.img src/*.o >/dev/null 2>/dev/null || true
dis:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -serial null -serial stdio
run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -display none -serial null -serial stdio

serial:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -display none -S -s -serial null -serial stdio

int:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -display none -serial null -serial stdio -d int
