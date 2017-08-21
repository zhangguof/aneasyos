/*OUTPUT_FORMAT(binary)*/
/*OUTPUT_FORMAT(elf32-i386)*/
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0;
    .text16 :{
        *(.s16)
    }
    . = (0x90000+SIZEOF(.text16));
    .text32 . : /* 注意对齐问题，不要由ld控制-_-!!!-*/
    AT(SIZEOF(.text16))
    {   
        *(.s32) *(.text)
    }
    /*.data (0x90000 + SIZEOF (text16)+SIZEOF(text32)):*/
    .data . : 
    AT(SIZEOF(.text16)+SIZEOF(.text32) )
    {
        *(.data) *(.bss) *(.rodata) *(.stack)
    }
    /DISCARD/ :{
        *(.eh_frame) *(.comment) *(.symtab) *(.strtab) *(.shstrtab)
    }
}