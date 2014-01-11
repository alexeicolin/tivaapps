/******************************************************************************
 *
 * Default Linker Command file for the Texas Instruments TM4E129BNCZAD
 *
 *****************************************************************************/

MEMORY
{
    FLASH (RX) : ORIGIN = 0x00000000,   LENGTH = 0x00040000
    SRAM (WX)  : ORIGIN = 0x20000000,   LENGTH = 0x00008000
}

REGION_ALIAS("REGION_TEXT", FLASH);
REGION_ALIAS("REGION_BSS", SRAM);
REGION_ALIAS("REGION_DATA", SRAM);
REGION_ALIAS("REGION_STACK", SRAM);
REGION_ALIAS("REGION_HEAP", SRAM);
REGION_ALIAS("REGION_ARM_EXIDX", FLASH);

SECTIONS {

    .text : {
        CREATE_OBJECT_SYMBOLS
        KEEP (*(.resetVecs))
        KEEP (*(.text))
        *(.text.*)
        . = ALIGN(0x4);
        KEEP (*(.ctors))
        . = ALIGN(0x4);
        __init_array_start = .;
        KEEP (*(.init_array*))
        __init_array_end = .;
        *(.init)
        *(.fini*)
    } > REGION_TEXT

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } > REGION_TEXT

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > REGION_ARM_EXIDX

    /* leave space for .vecs written by linker.cmd */
    .vecs_dummy : {
        . = . + 0x1000;
    } > SRAM

    .data : {
        __data_load__ = LOADADDR (.data);
        __data_start__ = .;
        KEEP (*(.data))
        KEEP (*(.data*))
        . = ALIGN (4);
        __data_end__ = .;
    } > REGION_DATA AT>REGION_TEXT

    .bss : {
        __bss_start__ = .;
        *(.shbss)
        KEEP (*(.bss))
        *(.bss.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end__ = .;
    } > REGION_BSS
}
