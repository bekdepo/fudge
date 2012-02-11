modules-arm: GCCFLAGS+=-Iinclude
modules-arm: LDFLAGS+=-r lib/memory.o lib/string.o
modules-x86: GCCFLAGS+=-Iinclude
modules-x86: LDFLAGS+=-r lib/memory.o lib/string.o

modules: modules-${ARCH}

modules-x86:
	@${GCC} ${GCCFLAGS} modules/acpi/driver.c -o modules/acpi/driver.o
	@${GCC} ${GCCFLAGS} modules/acpi/main.c -o modules/acpi/main.o
	@${GCC} ${GCCFLAGS} modules/ata/bus.c -o modules/ata/bus.o
	@${GCC} ${GCCFLAGS} modules/ata/device.c -o modules/ata/device.o
	@${GCC} ${GCCFLAGS} modules/ata/main.c -o modules/ata/main.o
	@${GCC} ${GCCFLAGS} modules/bga/driver.c -o modules/bga/driver.o
	@${GCC} ${GCCFLAGS} modules/bga/main.c -o modules/bga/main.o
	@${ASM} ${ASMFLAGS} modules/cpuid/cpuid.s -o modules/cpuid/cpuid.o
	@${GCC} ${GCCFLAGS} modules/cpuid/driver.c -o modules/cpuid/driver.o
	@${GCC} ${GCCFLAGS} modules/cpuid/main.c -o modules/cpuid/main.o
	@${GCC} ${GCCFLAGS} modules/ext2/driver.c -o modules/ext2/driver.o
	@${GCC} ${GCCFLAGS} modules/ext2/filesystem.c -o modules/ext2/filesystem.o
	@${GCC} ${GCCFLAGS} modules/ext2/main.c -o modules/ext2/main.o
	@${GCC} ${GCCFLAGS} modules/i915/driver.c -o modules/i915/driver.o
	@${GCC} ${GCCFLAGS} modules/i915/main.c -o modules/i915/main.o
	@${GCC} ${GCCFLAGS} modules/mbr/driver.c -o modules/mbr/driver.o
	@${GCC} ${GCCFLAGS} modules/mbr/main.c -o modules/mbr/main.o
	@${GCC} ${GCCFLAGS} modules/nodefs/driver.c -o modules/nodefs/driver.o
	@${GCC} ${GCCFLAGS} modules/nodefs/main.c -o modules/nodefs/main.o
	@${GCC} ${GCCFLAGS} modules/pci/bus.c -o modules/pci/bus.o
	@${GCC} ${GCCFLAGS} modules/pci/device.c -o modules/pci/device.o
	@${GCC} ${GCCFLAGS} modules/pci/main.c -o modules/pci/main.o
	@${GCC} ${GCCFLAGS} modules/pit/device.c -o modules/pit/device.o
	@${GCC} ${GCCFLAGS} modules/pit/main.c -o modules/pit/main.o
	@${GCC} ${GCCFLAGS} modules/ps2/kbd_device.c -o modules/ps2/kbd_device.o
	@${GCC} ${GCCFLAGS} modules/ps2/kbd_driver.c -o modules/ps2/kbd_driver.o
	@${GCC} ${GCCFLAGS} modules/ps2/main.c -o modules/ps2/main.o
	@${GCC} ${GCCFLAGS} modules/ps2/mouse_device.c -o modules/ps2/mouse_device.o
	@${GCC} ${GCCFLAGS} modules/ps2/mouse_driver.c -o modules/ps2/mouse_driver.o
	@${GCC} ${GCCFLAGS} modules/rtc/device.c -o modules/rtc/device.o
	@${GCC} ${GCCFLAGS} modules/rtc/main.c -o modules/rtc/main.o
	@${GCC} ${GCCFLAGS} modules/rtl8139/driver.c -o modules/rtl8139/driver.o
	@${GCC} ${GCCFLAGS} modules/rtl8139/main.c -o modules/rtl8139/main.o
	@${GCC} ${GCCFLAGS} modules/tty/driver.c -o modules/tty/driver.o
	@${GCC} ${GCCFLAGS} modules/tty/main.c -o modules/tty/main.o
	@${GCC} ${GCCFLAGS} modules/uart/device.c -o modules/uart/device.o
	@${GCC} ${GCCFLAGS} modules/uart/driver.c -o modules/uart/driver.o
	@${GCC} ${GCCFLAGS} modules/uart/main.c -o modules/uart/main.o
	@${GCC} ${GCCFLAGS} modules/vga/driver.c -o modules/vga/driver.o
	@${GCC} ${GCCFLAGS} modules/vga/main.c -o modules/vga/main.o
	@${LD} ${LDFLAGS} modules/acpi/driver.o modules/acpi/main.o -o modules/acpi/acpi.ko
	@${LD} ${LDFLAGS} modules/ata/bus.o modules/ata/device.o modules/ata/main.o -o modules/ata/ata.ko
	@${LD} ${LDFLAGS} modules/bga/driver.o modules/bga/main.o -o modules/bga/bga.ko
	@${LD} ${LDFLAGS} modules/cpuid/cpuid.o modules/cpuid/driver.o modules/cpuid/main.o -o modules/cpuid/cpuid.ko
	@${LD} ${LDFLAGS} modules/ext2/driver.o modules/ext2/filesystem.o modules/ext2/main.o -o modules/ext2/ext2.ko
	@${LD} ${LDFLAGS} modules/i915/driver.o modules/i915/main.o -o modules/i915/i915.ko
	@${LD} ${LDFLAGS} modules/mbr/driver.o modules/mbr/main.o -o modules/mbr/mbr.ko
	@${LD} ${LDFLAGS} modules/nodefs/driver.o modules/nodefs/main.o -o modules/nodefs/nodefs.ko
	@${LD} ${LDFLAGS} modules/pci/bus.o modules/pci/device.o modules/pci/main.o -o modules/pci/pci.ko
	@${LD} ${LDFLAGS} modules/pit/device.o modules/pit/main.o -o modules/pit/pit.ko
	@${LD} ${LDFLAGS} modules/ps2/kbd_device.o modules/ps2/kbd_driver.o modules/ps2/main.o modules/ps2/mouse_device.o modules/ps2/mouse_driver.o -o modules/ps2/ps2.ko
	@${LD} ${LDFLAGS} modules/rtc/device.o modules/rtc/main.o -o modules/rtc/rtc.ko
	@${LD} ${LDFLAGS} modules/rtl8139/driver.o modules/rtl8139/main.o -o modules/rtl8139/rtl8139.ko
	@${LD} ${LDFLAGS} modules/tty/driver.o modules/tty/main.o -o modules/tty/tty.ko
	@${LD} ${LDFLAGS} modules/uart/driver.o modules/uart/device.o modules/uart/main.o -o modules/uart/uart.ko
	@${LD} ${LDFLAGS} modules/vga/driver.o modules/vga/main.o -o modules/vga/vga.ko

modules-clean:
	@rm -f modules/*/*.o
	@rm -f modules/*/*.ko

