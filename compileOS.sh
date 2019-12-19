dd if=/dev/zero of=diskc.img bs=512 count=1000
nasm bootload.asm
dd if=bootload of=diskc.img bs=512 bs=512 count=1 conv=notrunc


bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o

as86 userlib.asm -o userlib.o

bcc -ansi -c -o shell_c.o shell.c
ld86 -o shell -d shell_c.o userlib.o

bcc -ansi -c -o tstpr1.o tstpr1.c
ld86 -d -o tstpr1 tstpr1.o userlib.o

bcc -ansi -c -o tstpr2.o tstpr2.c
ld86 -d -o tstpr2 tstpr2.o userlib.o



gcc -o loadFile loadFile.c
./loadFile kernel
./loadFile shell
./loadFile message.txt
./loadFile tstpr1
./loadFile tstpr2
