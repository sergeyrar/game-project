AL=arm-elf-ld
AS=arm-elf-as
AR=arm-elf-ar
NM=arm-elf-nm
OB=arm-elf-objcopy
CC=arm-elf-gcc

OFLAGS  = -O binary
TOOLCHAIN 	=	/usr/cross/arm-elf
TCHAIN_DIRS	=	$(TOOLCHAIN)/arm-elf/lib/ \
				$(TOOLCHAIN)/lib/gcc/arm-elf/4.1.1/


all: 
	$(MAKE) DM9000.o
	$(MAKE) Demo.bin
	
clean:
	rm -f libclass.a net.o _Uart.o ConsoleIf.o main.o TestDM9000.o  TestGPIO.o Timer.o *.elf *.bin  *.map
	
main.o: main.c tests.h 
	$(CC) -g -march=armv4 -c $< -o $@ 

net.o: net.c net.h
	$(CC) -g -march=armv4 -c $< -o $@ 

DM9000.o: DM9000.c DM9000.h net.h
	$(CC) -g -march=armv4 -c $< -o $@ 

	
TestDM9000.o: TestDM9000.c DM9000Regs.h
	$(CC) -g -march=armv4 -c $< -o $@ 

TestGPIO.o: TestGPIO.c GPIORegs.h
	$(CC) -g -march=armv4 -c $< -o $@ 
	
Timer.o: Timer.c PWMRegs.h INTRRegs.h INTRDefs.h
	$(CC) -g -march=armv4 -c $< -o $@ 

_Uart.o: _Uart.c _Uart.h
	$(CC) -g -march=armv4 -c $< -o $@ 
	
ConsoleIf.o: ConsoleIf.c Uart.h _Uart.h
	$(CC) -g -march=armv4 -c $< -o $@ 
	
	
libclass.a: ConsoleIf.o _Uart.o 
	arm-elf-ar crv  libclass.a  _Uart.o ConsoleIf.o 
	
Demo.elf: Demo.lds main.o libclass.a TestDM9000.o TestGPIO.o Timer.o net.o makefile
	$(AL) -T $< -L$(TOOLCHAIN)/arm-elf/lib/ -L$(TOOLCHAIN)/lib/gcc/arm-elf/4.1.1/   -Map Demo.map -o $@
	
Demo.bin: Demo.elf
	$(OB) $< $(OFLAGS) $@
	@cp $@ u-boot.bin
	
