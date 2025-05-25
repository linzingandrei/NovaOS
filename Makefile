C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h)

OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
LD = /usr/local/i386elfgcc/bin/i386-elf-ld

CFLAGS = -g

all: os-image.bin

run: all
	qemu-system-i386 -drive format=raw,file=os-image.bin

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

os-image.bin: boot/boot.bin kernel.bin
	# cat $^ > os-image.bin
	dd if=/dev/zero of=os-image.bin bs=512 count=64 status=none
	dd if=boot/boot.bin of=os-image.bin bs=512 seek=0 conv=notrunc status=none
	dd if=kernel.bin   of=os-image.bin bs=512 seek=1 conv=notrunc status=none

kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x7E00 $^

kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x7E00 $^ --oformat binary

%.o : %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

%.bin : %.asm
	nasm $< -f bin -o $@

clean:
	rm -fr *.bin *.dis *.o os-image kernel.elf
	rm -fr kernel/*.o drivers/*.o cpu/*.o boot/*.bin boot/*.o
