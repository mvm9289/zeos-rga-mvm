################################
##############ZeOS #############
################################
########## Makefile ############
################################

# package dev86 is required
AS86	= as86 -0 -a
LD86	= ld86 -0

HOSTCFLAGS = -Wall -Wstrict-prototypes
HOSTCC 	= gcc
CC      = gcc 
AS      = as
LD      = ld
OBJCOPY = objcopy -O binary -R .note -R .comment -S

INCLUDEDIR = include

# Define here flags to compile the tests if needed
JP = 

CFLAGS = -O2  -g $(JP) -ffreestanding -Wall -I$(INCLUDEDIR) 
ASMFLAGS = -I$(INCLUDEDIR)
SYSLDFLAGS = -T system.lds
USRLDFLAGS = -T user.lds
LINKFLAGS = -g 

SYSOBJ = interrupt.o entry.o io.o sys.o sched.o mm.o devices.o utils.o hardware.o string.o

#add to USROBJ the object files required to complete the user program
USROBJ = libc.o string.o # libjp1.a

all:zeos.bin

zeos.bin: bootsect system build user
	$(OBJCOPY) system system.out
	$(OBJCOPY) user user.out
	./build bootsect system.out user.out > zeos.bin

build: build.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

bootsect: bootsect.o
	$(LD86) -s -o $@ $<

bootsect.o: bootsect.s
	$(AS86) -o $@ $<

bootsect.s: bootsect.S Makefile
	$(CPP) $(ASMFLAGS) -traditional $< -o $@

entry.s: entry.S $(INCLUDEDIR)/asm.h $(INCLUDEDIR)/segment.h
	$(CPP) $(ASMFLAGS) -o $@ $<

user.o:user.c $(INCLUDEDIR)/libc.h

interrupt.o:interrupt.c $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/types.h $(INCLUDEDIR)/hardware.h $(INCLUDEDIR)/entry.h $(INCLUDEDIR)/io.h $(INCLUDEDIR)/string.h

string.o:string.c $(INCLUDEDIR)/string.h

io.o:io.c $(INCLUDEDIR)/io.h $(INCLUDEDIR)/utils.h

libc.o:libc.c $(INCLUDEDIR)/libc.h $(INCLUDEDIR)/string.h

mm.o:mm.c $(INCLUDEDIR)/types.h $(INCLUDEDIR)/mm.h $(INCLUDEDIR)/hardware.h $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/sched.h

sched.o:sched.c $(INCLUDEDIR)/sched.h $(INCLUDEDIR)/mm.h

sys.o:sys.c $(INCLUDEDIR)/devices.h $(INCLUDEDIR)/utils.h $(INCLUDEDIR)/errno.h

utils.o:utils.c $(INCLUDEDIR)/utils.h $(INCLUDEDIR)/types.h

devices.o:devices.c $(INCLUDEDIR)/devices.h $(INCLUDEDIR)/io.h

hardware.o:hardware.c $(INCLUDEDIR)/hardware.h $(INCLUDEDIR)/types.h


system.o:system.c $(INCLUDEDIR)/hardware.h system.lds $(SYSOBJ) $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/types.h $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/system.h $(INCLUDEDIR)/sched.h $(INCLUDEDIR)/mm.h $(INCLUDEDIR)/io.h $(INCLUDEDIR)/mm_address.h 


system: system.o system.lds $(SYSOBJ)
	$(LD) $(LINKFLAGS) $(SYSLDFLAGS) -o $@ $< $(SYSOBJ) 

user: user.o user.lds $(USROBJ) 
	$(LD) $(LINKFLAGS) $(USRLDFLAGS) -o $@ $< $(USROBJ)


clean:
	rm -f *.o *.s bochsout.txt parport.out system.out system bootsect zeos.bin user user.out *~ build include/*~

disk: zeos.bin
	dd if=zeos.bin of=/dev/fd0

emul: zeos.bin
	bochs -q -f .bochsrc

emuldbg: zeos.bin
	bochs_nogdb -q -f .bochsrc~
