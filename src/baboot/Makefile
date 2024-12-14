##
## For the compilation of bootx64.efi
##

#
# Compiler/linker definitions
#
CC := clang
LD := lld

#
# File/directory definitions
#
C_SRC := $(wildcard src/*.c)
OBJ := $(C_SRC:src/%.c=obj/%.o)
INC_DIR := src/inc
OUTFILE := bootx64.efi

# Compile flags
# --------------
# -ffreestanding: Indicates a freestanding environment without access to a
#                 standard library 
#	-mno-red-zone: Disables the included red-zone next to the stack pointer as 
#	               it may create issues 
# -I${INC_DIR}: Specifies the directory of the included headers
# -target x86_64-unknown-windows: Sets the target to "windows" although it does
#                                 not actually build for the windows OS
# -Wall: Enable all warnings
# -Wextra: Enable extra warnings 
# -Wmissing-prototypes: Warn of prototypes missing from header
# -Wstrict-prototypes: Warn of prototype mismatch
# -D__DEBUG__: Define a macro to print out more detailed information on boot
CFLAGS := -ffreestanding \
	      -mno-red-zone \
	      -I${INC_DIR} \
	      -target x86_64-unknown-windows \
		  -DDEBUG \
		  -DINFO \
		  -Wall \
		  -Wextra \
		  -Wmissing-prototypes \
		  -Wstrict-prototypes \
			-D__DEBUG__

# Link flags (lld)
# -----------
# -flavor link: Self explanitory, tells lld to link
# -subsystem:efi_application: Also self explanitory, links for booting the
#                             efi program
# -entry:efi_main: Specifies the entry point function 
LDFLAGS := -flavor link \
		   -subsystem:efi_application \
		   -entry:efi_main

all: obj bin bin/${OUTFILE}

# 
# Creation of directories
#
obj:
	mkdir obj

bin:
	mkdir bin

#
# Link the bootloader
#
bin/${OUTFILE}: ${OBJ}
	${LD} ${LDFLAGS} $^ -out:bin/${OUTFILE}

#
# Compile the bootloader
#
$(OBJ): obj/%.o: src/%.c
	${CC} ${CFLAGS} -c $< -o $@

#
# Clean the source tree
#
clean:
	rm -rf \
		./bin \
		./obj
