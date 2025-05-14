C_SOURCES = $(wildcard kernel/*.c drivers/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h libc/*.h)

OBJ = ${C_SOURCES:.c=.o}

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
LD = /usr/local/i386elfgcc/bin/i386-elf-ld

CFLAGS = -g

all: os-image.bin

run: all
	qemu-system-i386 -fda os-image.bin

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin & \
	${GDB} kernel.elf \
		-ex "target remote localhost:1234" \
		-ex "add-symbol-file kernel.elf 0x1000"

os-image.bin: boot/boot.bin kernel.bin
	cat $^ > os-image.bin

kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^

kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^ --oformat binary

%.o : %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

%.bin : %.asm
	nasm $< -f bin -o $@

clean:
	rm -fr *.bin *.dis *.o os-image kernel.elf
	rm -fr kernel/*.o drivers/*.o libc/*.o boot/*.bin
