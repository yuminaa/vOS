# Toolchain
ARCH ?= x86_64
AS = nasm
CC = $(ARCH)-elf-gcc
LD = $(ARCH)-elf-ld
OBJCOPY = $(ARCH)-elf-objcopy
QEMU = qemu-system-$(ARCH)

# Build configuration
BUILD_TYPE ?= debug
BUILD_DIR = build/$(BUILD_TYPE)

# Directories
ARCH_DIR = arch/x86
KERNEL_DIR = kernel
LIBK_DIR = libk

# Common flags
NASMFLAGS = -f elf64
NASMFLAGS_BIN = -f bin

# Debug/Release specific flags
ifeq ($(BUILD_TYPE),debug)
    CFLAGS_BUILD = -g -O0 -DDEBUG
    QEMU_FLAGS = -s -S
else
    CFLAGS_BUILD = -O2 -DNDEBUG
    QEMU_FLAGS =
endif

# Compiler flags
CFLAGS = -ffreestanding \
         -nostdlib \
         -mno-red-zone \
         -Wall \
         -Wextra \
         -mcmodel=kernel \
         -I$(LIBK_DIR) \
         $(CFLAGS_BUILD)

LDFLAGS = -T $(ARCH_DIR)/linker.ld -nostdlib

# Source files
BOOT_SRC = $(ARCH_DIR)/boot.asm
KERNEL_SRC = $(KERNEL_DIR)/kernel.c
LIBK_SRC = $(wildcard $(LIBK_DIR)/impl/*.c)

# Object files
BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
LIBK_OBJ = $(patsubst $(LIBK_DIR)/impl/%.c,$(BUILD_DIR)/%.o,$(LIBK_SRC))

# Output files
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
BOOT_BIN = $(BUILD_DIR)/boot.bin
DISK_IMG = $(BUILD_DIR)/disk.img

all: $(DISK_IMG)

# Create build directory structure
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile bootloader to raw binary
$(BOOT_BIN): $(BOOT_SRC) | $(BUILD_DIR)
	$(AS) $(NASMFLAGS_BIN) $< -o $@

# Compile kernel source
$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile libk sources
$(BUILD_DIR)/%.o: $(LIBK_DIR)/impl/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL_ELF): $(KERNEL_OBJ) $(LIBK_OBJ) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

# Convert kernel to binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# Create disk image
$(DISK_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOT_BIN) of=$@ bs=512 seek=0 conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

# Clean specific build type
clean:
	rm -rf $(BUILD_DIR)

# Clean all build types
clean-all:
	rm -rf build

# Run with specific build type
run: $(DISK_IMG)
	$(QEMU) -drive format=raw,file=$(DISK_IMG) $(QEMU_FLAGS)

# Explicitly build and run debug version
debug:
	$(MAKE) BUILD_TYPE=debug run

# Explicitly build and run release version
release:
	$(MAKE) BUILD_TYPE=release run

.PHONY: all clean clean-all run debug release