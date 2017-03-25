OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__start)


MEMORY
{
	EXE_BOOT (RX) : ORIGIN = 0x30000000, LENGTH = 0xF00
	MEM_BOOT (R) : ORIGIN = 0x30000F00, LENGTH = 0x100
	EXE (RX) : ORIGIN = 0x30002000, LENGTH = 128K
	RAM (RW) : ORIGIN = 0x30022000, LENGTH = 64K
}
/*

MEMORY
{
	EXE_BOOT (RX) : ORIGIN = 0x33f80000, LENGTH = 0xF00
	MEM_BOOT (R) : ORIGIN = 0x33f80F00, LENGTH = 0x100
	EXE (RX) : ORIGIN = 0x33f82000, LENGTH = 128K
	RAM (RW) : ORIGIN = 0x33fa2000, LENGTH = 64K
}
*/ 
 

SECTIONS
{
	. = 0x00000000;	

	. = ALIGN(4);
	.2440text_init :
	{
	  _stext_init = . ;	
 	  init.o (.text)
	  Nand_Minimal.o (.text) 
	  _etext_init = . ;
	} >EXE_BOOT

	. = ALIGN(4); 
   	.2440data_init      :
   	{
	  init.o(.data)
	  Nand_Minimal.o (.data)	  
	}  >MEM_BOOT
	
	. = ALIGN(4); 
   	.2440bss_init      :
   	{
    _sbss_init = .	   	;
	  init.o(.bss)
	  Nand_Minimal.o (.bss)	  	  
	_ebss_init = .		;	  
	}  >MEM_BOOT

	_rom_init_area_size = _ebss_init - _stext_init;
		
	. = ALIGN(4);	
	.text      :
	{
	  main.o (.text)	
	  MMU.o (.text)
	  HAL.o (.text)
	  TestDM9000.o (.text)	
	  TestGPIO.o (.text)		  
	  Timer.o (.text)		  
	  net.o (.text)
	  DM9000.o (.text)	  
	  libclass.a (.text)
      libc.a(.text) 	    	  	  	  
      libgcc.a(.text)
	} >EXE 
	
	. = ALIGN(4);	
   	.bss      :
   	{
   	  _bbss1 = .;
	  main.o (.bss)
	  MMU.o (.bss)	
	  HAL.o (.bss)	    	   	  
	  TestDM9000.o (.bss)	  
	  TestGPIO.o (.bss)		  	  
	  Timer.o (.bss)		  	   	  
	  net.o (.bss)
	  DM9000.o (.bss)	  
	  libclass.a (.bss)	    	  
   	  _ebss1 = .;	  
	} >RAM  	  	     	
	
	. = ALIGN(4);	
   	.data      :
   	{
	  main.o (.data)	   	     	
	  MMU.o (.data)	  
	  HAL.o (.data)	  
	  TestDM9000.o (.data)	   	     	
	  TestGPIO.o (.data)		  	  
	  Timer.o (.data)		  	  
	  net.o (.data)
	  DM9000.o (.data)	  
	  libclass.a (.data)	    	     	
	} >RAM 	  	     	

	. = ALIGN(4);
   	.rodata      :
   	{
	  main.o (.rodata)	   	     	   	
	  TestDM9000.o (.rodata)	   	     	   	
	  Timer.o (.rodata)		  	  
	  libclass.a (.rodata)	    	     	   	
	} >RAM 	  	     	


	. = ALIGN(4);
   	.rodata.str1.4      :
   	{
	  main.o (.rodata.strl.4)	   	     	   	   	
	  TestDM9000.o (.rodata.strl.4)	   	     	   	   	
	  Timer.o (.rodata.strl.4)		  	  	  
	  libclass.a (.rodata.str1.4)	    	     	   	   	
	} >RAM 	 
	
	. = ALIGN(4);	
	__RTED_usr_menu_start = .;
	.RTED_usr_menu_cmd :
	{
	  *(.RTED_usr_menu_cmd)
	} >RAM 	 
	__RTED_usr_menu_end = .;
	 	     	
 
}

 

