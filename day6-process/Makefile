SHELL = /bin/sh
ARCH = riscv64-linux-gnu
CC = $(ARCH)-gcc
LD = $(ARCH)-ld
OBJCOPY = $(ARCH)-objcopy
IDIR = inc
SDIR = src
BDIR = build
CFLAGS = -g -fno-builtin -nostdlib -Wall -mcmodel=medany -g -I $(IDIR) -O0
SFLAGS = -g -I $(IDIR)
OBJCOPY = $(ARM)-objcopy
S_SRCS = $(wildcard $(SDIR)/*.s)
C_SRCS = $(wildcard $(SDIR)/*.c)
S_OBJS = $(S_SRCS:$(SDIR)/%.s=$(BDIR)/%_asm.o)
C_OBJS = $(C_SRCS:$(SDIR)/%.c=$(BDIR)/%.o)


all: clean kernel.img

kernel.img: kernel.elf
	$(OBJCOPY) kernel.elf -I binary kernel.img

kernel.elf: $(S_OBJS) link.ld $(C_OBJS)
	$(LD) -T link.ld -o kernel.elf $(S_OBJS) $(C_OBJS)

$(BDIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%_asm.o: $(SDIR)/%.s
	$(CC) $(SFLAGS) -c $< -o $@

mkfs/mkfs: $(SDIR)/fs.c mkfs/mkfs.c
	gcc -Werror -Wall -I$(IDIR) -o mkfs/mkfs mkfs/mkfs.c $(SDIR)/fs.c

disk: mkfs/mkfs user
	dd of=disk.img if=/dev/zero bs=M count=2
	mkfs/mkfs user/*
clean:
	rm -f $(BDIR)/*_asm.o $(BDIR)/*.o kernel.elf kernel.img

user/user2: $(BDIR)/uart.o $(BDIR)/usys_asm.o
	$(CC) $(CFLAGS) -Werror -Wall -I$(IDIR) -c user/user2.c -o $(BDIR)/user2.o 
	$(LD) -T user/user.ld -o user/user2 $(BDIR)/user2.o $(BDIR)/printf.o $(BDIR)/uart.o $(BDIR)/usys_asm.o

user/user1: $(BDIR)/uart.o $(BDIR)/usys_asm.o
	$(CC) $(CFLAGS) -Werror -Wall -I$(IDIR) -c user/user1.c -o $(BDIR)/user1.o 
	$(LD) -T user/user.ld -o user/user1 $(BDIR)/user1.o $(BDIR)/printf.o $(BDIR)/uart.o $(BDIR)/usys_asm.o

QEMUARG = -gdb tcp::1234 -m 128M -M virt -serial stdio -display none -bios none -kernel kernel.img -global virtio-mmio.force-legacy=false -drive file=disk.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

run: all
	qemu-system-riscv64 $(QEMUARG)

debug: all
	qemu-system-riscv64 $(QEMUARG) -S
