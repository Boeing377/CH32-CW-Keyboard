ENTRY( _start )

__stack_size = 2048;

PROVIDE( _stack_size = __stack_size );


MEMORY
{
/* CH32V20x_D6 - CH32V203F6-CH32V203G6-CH32V203K6-CH32V203C6 */
/*
	FLASH (rx) : ORIGIN = 0x00005000, LENGTH = 32K
	RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 10K
*/

/* CH32V20x_D6 - CH32V203K8-CH32V203C8-CH32V203G8-CH32V203F8                       */
/*                                                                              */
/*   Physical flash: 64KB total (0x00000000 - 0x0000FFFF)                       */
/*   Bootloader     : 0x0000 - 0x4FFF (20KB reserved, NOT in this region)       */
/*   App code       : 0x5000 - 0xE3FF (37KB)                                    */
/*                                                                              */
/*   ═══════════ STORAGE RESERVED (NOT for code) ═══════════                    */
/*   Config+MsgDir  : 0xE700 - 0xE7FF (256B Fast page, see storage_backend.h)   */
/*   Msg Slots 0-11 : 0xE800 - 0xFFFF (12×512B, see storage_backend.h)          */
/*                                                                              */
/*   If code exceeds 0xE3FF the linker will FAIL — do NOT increase LENGTH       */
/*   without relocating the storage area first.                                 */
	FLASH (rx) : ORIGIN = 0x00005000, LENGTH = 37K
	RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 20K

/* Do not pirate. */

/* CH32V20x_D8 - CH32V203RB
   CH32V20x_D8W - CH32V208x
   FLASH + RAM supports the following configuration
   FLASH-128K + RAM-64K
   FLASH-144K + RAM-48K
   FLASH-160K + RAM-32K
*/
/*
	FLASH (rx) : ORIGIN = 0x00005000, LENGTH = 160K
	RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 32K
*/

}


SECTIONS
{

	.init :
	{
		_sinit = .;
		. = ALIGN(4);
		KEEP(*(SORT_NONE(.init)))
		. = ALIGN(4);
		_einit = .;
	} >FLASH AT>FLASH

  .vector :
  {
      *(.vector);
	  . = ALIGN(64);
  } >FLASH AT>FLASH

	.text :
	{
		. = ALIGN(4);
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata*)
		*(.gnu.linkonce.t.*)
		. = ALIGN(4);
	} >FLASH AT>FLASH

	.fini :
	{
		KEEP(*(SORT_NONE(.fini)))
		. = ALIGN(4);
	} >FLASH AT>FLASH

	PROVIDE( _etext = . );
	PROVIDE( _eitcm = . );

	.preinit_array  :
	{
	  PROVIDE_HIDDEN (__preinit_array_start = .);
	  KEEP (*(.preinit_array))
	  PROVIDE_HIDDEN (__preinit_array_end = .);
	} >FLASH AT>FLASH

	.init_array     :
	{
	  PROVIDE_HIDDEN (__init_array_start = .);
	  KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
	  KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
	  PROVIDE_HIDDEN (__init_array_end = .);
	} >FLASH AT>FLASH

	.fini_array     :
	{
	  PROVIDE_HIDDEN (__fini_array_start = .);
	  KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
	  KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
	  PROVIDE_HIDDEN (__fini_array_end = .);
	} >FLASH AT>FLASH

	.ctors          :
	{
	  KEEP (*crtbegin.o(.ctors))
	  KEEP (*crtbegin?.o(.ctors))
	  KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
	  KEEP (*(SORT(.ctors.*)))
	  KEEP (*(.ctors))
	} >FLASH AT>FLASH

	.dtors          :
	{
	  KEEP (*crtbegin.o(.dtors))
	  KEEP (*crtbegin?.o(.dtors))
	  KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
	  KEEP (*(SORT(.dtors.*)))
	  KEEP (*(.dtors))
	} >FLASH AT>FLASH

	.dalign :
	{
		. = ALIGN(4);
		PROVIDE(_data_vma = .);
	} >RAM AT>FLASH

	.dlalign :
	{
		. = ALIGN(4);
		PROVIDE(_data_lma = .);
	} >FLASH AT>FLASH

	.data :
	{
	    *(.gnu.linkonce.r.*)
	    *(.data .data.*)
	    *(.gnu.linkonce.d.*)
		. = ALIGN(8);
	    PROVIDE( __global_pointer$ = . + 0x800 );
		*(.sdata .sdata.*)
		*(.sdata2.*)
	    *( .gnu.linkonce.s.*)
	    . = ALIGN(8);
	    *(.srodata.cst16)
	    *(.srodata.cst8)
	    *(.srodata.cst4)
	    *(.srodata.cst2)
	    *(.srodata .srodata.*)
	    . = ALIGN(4);
		PROVIDE( _edata = .);
	} >RAM AT>FLASH

	.bss :
	{
		. = ALIGN(4);
		PROVIDE( _sbss = .);
	    *(.sbss*)
        *(.gnu.linkonce.sb.*)
		*(.bss*)
	    *( .gnu.linkonce.b.*)
		*(COMMON*)
		. = ALIGN(4);
		PROVIDE( _ebss = .);
	} >RAM AT>FLASH

	PROVIDE( _end = _ebss);
	PROVIDE( end = . );

    .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
    {
        PROVIDE( _heap_end = . );
        . = ALIGN(4);
        PROVIDE(_susrstack = . );
        . = . + __stack_size;
        PROVIDE( _eusrstack = .);
    } >RAM

    /* ── Legacy upgrade detection guard ───────────────────────────── */
    /* Old firmware (no-EEPROM flash version) stored user Config at   */
    /* 0x0800CF00.  This section MUST NOT be loaded so the ISP        */
    /* programmer leaves the old data intact.                         */
    /*                                                                */
    /* If this assertion fires your firmware has grown past 32 KiB    */
    /* and the old-settings detection will break.                     */
    /*                                                                */
    /* Threshold: 0xCF00 – 0x5000 = 0x7F00 (32 512 bytes)            */
    /* ────────────────────────────────────────────────────────────── */
    _flash_app_end = _data_lma + SIZEOF(.data);
}

ASSERT(
    _flash_app_end <= 0x0000CF00,
    "ERROR: Firmware code+data exceeds 0xCF00! Old-settings upgrade detection will fail. Reduce code size."
)