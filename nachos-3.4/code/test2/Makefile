# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt shell matmult sort loop msb msb2 bigloop matmult2 matmult3

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult
	
loop.o: loop.c
	$(CC) $(CFLAGS) -c loop.c
loop: loop.o start.o
	$(LD) $(LDFLAGS) start.o loop.o -o loop.coff
	../bin/coff2noff loop.coff loop

matmult3.o: matmult3.c
	$(CC) $(CFLAGS) -c matmult3.c
matmult3: matmult3.o start.o
	$(LD) $(LDFLAGS) start.o matmult3.o -o matmult3.coff
	../bin/coff2noff matmult3.coff matmult3
		
matmult2.o: matmult2.c
	$(CC) $(CFLAGS) -c matmult2.c
matmult2: matmult2.o start.o
	$(LD) $(LDFLAGS) start.o matmult2.o -o matmult2.coff
	../bin/coff2noff matmult2.coff matmult2
	
bigloop.o: bigloop.c
	$(CC) $(CFLAGS) -c bigloop.c
bigloop: bigloop.o start.o
	$(LD) $(LDFLAGS) start.o bigloop.o -o bigloop.coff
	../bin/coff2noff bigloop.coff bigloop
	
msb2.o: msb2.c
	$(CC) $(CFLAGS) -c msb2.c
msb2: msb2.o start.o
	$(LD) $(LDFLAGS) start.o msb2.o -o msb2.coff
	../bin/coff2noff msb2.coff msb2
	
msb.o: msb.c
	$(CC) $(CFLAGS) -c msb.c
msb: msb.o start.o
	$(LD) $(LDFLAGS) start.o msb.o -o msb.coff
	../bin/coff2noff msb.coff msb