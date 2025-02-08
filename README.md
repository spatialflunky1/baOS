# baOS
## Description
The BAsic Operating System (baOS) is [will be] an operating system built for simple tasks such as:
* Playing a video
* Listening to audio
* Or writing a file
## Compiling
Compilation is as simple as:
```
make
```
A UEFI bootable iso `baos.iso` will be located in the root of the source tree following compilation
## Testing
To run a test VM with qemu run:
```
make qemu
```
## Note:
* When reading the source run the command below so the editor knows where the headers are located
```
bear -- make
```
## Note 2:
* Until the OS is back at a functional state, all commits will be to the main branch
## Build Dependencies
- clang
- lld
- mtools
- xorriso
- OPTIONAL: qemu-system-x86_64
- OPTIONAL: bear
