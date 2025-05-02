BUILD = build
SRC = kernel
ISO = iso
GRUB = $(ISO)/boot/grub

BOOT_SRC = $(SRC)/boot.s
KERNEL_SRC = $(SRC)/kernel.c
FILESYSTEM_SRC = $(SRC)/filesystem.c
LINKER_SCRIPT = link.ld

BOOT_OBJ = $(BUILD)/boot.o
KERNEL_OBJ = $(BUILD)/kernel.o
FILESYSTEM_OBJ = $(BUILD)/filesystem.o
KERNEL_ELF = $(BUILD)/kernel.elf
ISO_FILE = myos.iso

all: $(ISO_FILE)

$(BUILD):
	mkdir -p $(BUILD)

$(BOOT_OBJ): $(BOOT_SRC) | $(BUILD)
	nasm -f elf32 $(BOOT_SRC) -o $(BOOT_OBJ)

$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD)
	gcc -m32 -ffreestanding -fno-pie -fno-pic -I$(SRC) -c $(KERNEL_SRC) -o $(KERNEL_OBJ)

$(FILESYSTEM_OBJ): $(FILESYSTEM_SRC) | $(BUILD)
	gcc -m32 -ffreestanding -fno-pie -fno-pic -I$(SRC) -c $(FILESYSTEM_SRC) -o $(FILESYSTEM_OBJ)

$(KERNEL_ELF): $(BOOT_OBJ) $(KERNEL_OBJ) $(FILESYSTEM_OBJ) $(LINKER_SCRIPT)
	ld -m elf_i386 -T $(LINKER_SCRIPT) -o $(KERNEL_ELF) $(BOOT_OBJ) $(KERNEL_OBJ) $(FILESYSTEM_OBJ)

$(ISO_FILE): $(KERNEL_ELF)
	mkdir -p $(GRUB)
	cp $(KERNEL_ELF) $(ISO)/boot/kernel.elf
	echo 'set timeout=0' > $(GRUB)/grub.cfg
	echo 'menuentry "My Custom OS" {' >> $(GRUB)/grub.cfg
	echo '  multiboot /boot/kernel.elf' >> $(GRUB)/grub.cfg
	echo '  boot' >> $(GRUB)/grub.cfg
	echo '}' >> $(GRUB)/grub.cfg
	grub-mkrescue -o $(ISO_FILE) $(ISO)

run: all
	qemu-system-i386 -cdrom $(ISO_FILE)

clean:
	rm -rf $(BUILD) $(ISO)/boot $(ISO_FILE)
