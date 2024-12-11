##
## For the compilation of baos_kernel.elf
##

#
# Compiler definitions
#
CC := clang

#
# File/directory definitions
#
C_SRC = $(wildcard src/*/*.c)
OBJ = $(C_SRC:src/%.c=obj/%.o)
INC_DIR := src/inc
OUTFILE := baos_kernel.elf

# Compile flags
# --------------
# -ffreestanding: Indicates a freestanding environment without access to a
#                 standard library
#	-fno-stack-protector: Disables the stack protector, needed
#	-mno-red-zone: Disables the included red-zone next to the stack pointer as 
#	               it may create issues with interrupts
#	-I${INC_DIR}: Specifies the directory of included headers
#	-Wall: Enable all warnings
#	-Wextra: Enable extra warnings
#	-Wmissing-prototypes: Warn of prototypes missing from header
#	-Wstrict-prototypes: Warn of prototype mismatch
CFLAGS := \
			-ffreestanding \
		  -fno-stack-protector \
		  -mno-red-zone \
		  -I${INC_DIR} \
		  -Wall \
		  -Wextra \
		  -Wmissing-prototypes \
		  -Wstrict-prototypes

# Link flags
# -----------
# -ffreestanding: Same as above, environment without standard library
# -no-pie: Disables the creation of a position independent executable.
#          Needed as the kernel will need to be specifically placed
# -nostdlib: Similar to freestanding, explicitly builds without linking to
#            any other nonspecified files 
# -T src/kernel/kernel.ld: Specify linker script for layout
LDFLAGS := \
		   -ffreestanding \
		   -no-pie \
		   -nostdlib \
		   -T src/kernel/kernel.ld

all: obj bin bin/${OUTFILE}

#
# Creation of directories
#
obj:
	mkdir -pv $(dir ${OBJ})

bin:
	mkdir bin

#
# Kernel build target
#
bin/${OUTFILE}: ${OBJ}
	${CC} -o $@ ${LDFLAGS} $^

#
# Create object partfiles
# 
obj/%.o: src/%.c
	${CC} ${CFLAGS} -c $< -o $@

#
# Clean the source tree
#
clean:
	rm -rf \
		./obj \
		./bin
