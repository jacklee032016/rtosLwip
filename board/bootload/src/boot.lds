
/*------------------------------------------------------------------------------
 *      Linker script for running in internal SRAM on the SAM4E16
 *----------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* Memory Spaces Definitions */
MEMORY
{
/*
    rom (W!RX) : ORIGIN = 0x20400000, LENGTH = 0x1000
    sram (W!RX) : ORIGIN = 0x20401000, LENGTH = 0x0005E000 
*/    
		rom (rx)  : ORIGIN = 0x00400000, LENGTH = 0x00100000
		ram (rwx) : ORIGIN = 0x20400000, LENGTH = 0x00060000
}

SECTIONS
{
    /* startup code in the .isr_vector */
    .text :
    {
        . = ALIGN(4);
        _stext = .;
        KEEP(*(.isr_vector .isr_vector.*))
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.ramfunc .ramfunc.*)
        *(.glue_7)
        *(.glue_7t)
        *(.gcc_except_table)
        *(.rodata .rodata*)
        *(.gnu.linkonce.r.*)
        . = ALIGN(4);
        _etext = .;
    } > rom

    /* data */
    .data :
    {
        . = ALIGN(4);
        _sidata = .;
        _sdata = .;

        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } > ram

    .bss (NOLOAD) : {
        _szero = .;
        *(.bss)
        . = ALIGN(4);
        _ezero = .;
    } > ram

    /* Stack in SRAM */
    _sstack = 0x2045FFF0;
}
. = ALIGN(4);
end = .;
