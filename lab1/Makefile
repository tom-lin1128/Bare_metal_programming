SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy


all: clean kernel8.img

a.o: a.S
	$(CC) $(CFLAGS) -c a.S -o a.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: a.o $(OBJS)
	$(LINKER) -nostdlib -nostartfiles a.o $(OBJS) -T linker.ld -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf kernel8.img *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio