# TOOLCHAINS
ARCH ?= x86_64
AS = nasm
CC = $(ARCH)-elf-gcc
LD = $(ARCH)-elf-ld
OBJCOPY = $(ARCH)-elf-objcopy
QEMU = qemu-system-$(ARCH)

# FLAGS
NASMFLAGS = -f elf64
NASMFLAGS_BIN = -f bin
CFLAGS = -ffreestanding -nostdlib -mno-red-zone -Wall -Wextra -O3 -mcmodel=kernel -Ilibk
LDFLAGS = -T arch/x86/linker.ld -nostdlib

# DIRECTORIES
BUILD_DIR = build
ARCH_DIR = arch/x86
KERNEL_DIR = kernel
LIBK_DIR = libk

# SOURCE FILES
BOOT_SRC = $(ARCH_DIR)/boot/first.asm $(ARCH_DIR)/boot/second.asm
KERNEL_ENTRY = $(ARCH_DIR)/entry.asm
KERNEL_SRC = $(KERNEL_DIR)/kernel.c
LIBK_SRC = $(wildcard $(LIBK_DIR)/impl/*.c)

# OBJECT FILES
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
LIBK_OBJ = $(patsubst $(LIBK_DIR)/impl/%.c, $(BUILD_DIR)/%.o, $(LIBK_SRC))
ENTRY_OBJ = $(BUILD_DIR)/entry.o

# OUTPUT FILES
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
BOOTLOADER_IMG = $(BUILD_DIR)/disk.img

all: $(BOOTLOADER_IMG)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/first.bin: $(ARCH_DIR)/boot/first.asm | $(BUILD_DIR)
	$(AS) $(NASMFLAGS_BIN) $< -o $@

$(BUILD_DIR)/second.bin: $(ARCH_DIR)/boot/second.asm | $(BUILD_DIR)
	$(AS) $(NASMFLAGS_BIN) $< -o $@

$(BUILD_DIR)/entry.o: $(ARCH_DIR)/entry.asm | $(BUILD_DIR)
	$(AS) $(NASMFLAGS) $< -o $@

$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIBK_DIR)/impl/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(ENTRY_OBJ) $(KERNEL_OBJ) $(LIBK_OBJ) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

$(BOOTLOADER_IMG): $(BUILD_DIR)/first.bin $(BUILD_DIR)/second.bin $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BUILD_DIR)/first.bin of=$@ bs=512 seek=0 conv=notrunc
	dd if=$(BUILD_DIR)/second.bin of=$@ bs=512 seek=1 conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=4 conv=notrunc

clean:
	rm -rf $(BUILD_DIR)

run: $(BOOTLOADER_IMG)
	$(QEMU) -drive file=$(BOOTLOADER_IMG),format=raw -serial stdio

debug: $(BOOTLOADER_IMG)
	$(QEMU) -drive file=$(BOOTLOADER_IMG),format=raw -serial stdio -S -gdb tcp::1234

.PHONY: all clean run debug