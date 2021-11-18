OUTPUT_ARCH(mips)
ENTRY(main)
SECTIONS
{
   //. = 0x00400000;
   . = 0x00000000;

  _text = .;			/* Text and read-only data */
  .text : {
	*(.text)
	*(.fixup)
	*(.gnu.warning)
	}

  _etext = .;			/* End of text section */

  .data : {			/* Data */
	*(.data)
	*(.rodata)
	*(.rodata.*)
	*(.eh_frame)
	CONSTRUCTORS
	}

  _edata = .;			/* End of data section */



  __bss_start = .;		/* BSS */
  .bss : {
	*(.bss)
	}
  /DISCARD/ : {
	*(.comment)
	*(.debug_*)
	}
  end = . ;

}
