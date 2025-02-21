# Disk size is in kilobytes
# 2880 = 2x 1.44mb floppy sized
# 2.88mb
DISK_SIZE=2880

# Target file output definitions
KERNEL_FILE := src/baos/bin/baos_kernel.elf
BOOTLOADER_FILE := src/baboot/bin/bootx64.efi

# Final goal of compilation is a bootable ISO
all: baos.iso

# The ISO depends on the kernel and the bootloader
# This is the section that requires mtools
baos.iso: output ${KERNEL_FILE} ${BOOTLOADER_FILE}
	dd if=/dev/zero of=./output/efi_part.img bs=1024 count=${DISK_SIZE}
	mformat -i ./output/efi_part.img -f ${DISK_SIZE} ::
	mmd -i ./output/efi_part.img ::EFI
	mmd -i ./output/efi_part.img ::EFI/BOOT
	mcopy -i ./output/efi_part.img ${BOOTLOADER_FILE} ::EFI/BOOT
	mcopy -i ./output/efi_part.img ${KERNEL_FILE} ::
	mkdir ./output/iso
	mv ./output/efi_part.img ./output/iso
	xorriso -as mkisofs -R -f -e efi_part.img -no-emul-boot -o baos.iso ./output/iso

# Output directory
output:
	mkdir output

# Kernel
${KERNEL_FILE}:
	make -C src/kernel

# Bootloader
${BOOTLOADER_FILE}:
	make -C src/bootloader

# Clean the project directory and subdirectories
clean:
	rm -rf ./output \
		     ./*.iso
	make -C src/bootloader clean
	make -C src/kernel clean

# Run the system in qemu
qemu: all
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,unit=0,file=bios64.bin \
		-net none \
		-cdrom baos.iso \
		-d cpu_reset \
		-d int \
		--no-reboot \
		 -no-shutdown

# Output total line count
wc:
	wc -l `find src` 2>/dev/null

# Run the script that flashes the system to a usb
usb: all
	./scripts/usb.sh
