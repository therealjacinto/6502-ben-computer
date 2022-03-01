MEMORY
{
    /* zero page is made up of: 32 1-byte
     * registers, 1 2-byte sp, and 4 4-byte
     * btmps */
    zero  : org = 0x0000, len = 0x00ff

    /* Empty space: 0x002b - 0x00ff */

    /* 6502 hard-coded stack */
    stack : org = 0x0100, len = 0x00ff

    /* RAM chip can be used up to 0x3fff */
    ram   : org = 0x0200, len = 0x3dff

    /* 32KB read only memory */
    rom   : org = 0x8000, len = 0x8000
}

SECTIONS
{
    zpage   (NOLOAD) : {*(zpage)}  >zero

    data             : {*(data)}   >ram  AT>rom
    bss     (NOLOAD) : {*(bss)}    >ram

    text             : {*(text)}   >rom
    rodata           : {*(rodata)} >rom
    /* Unused space of rom */
    fill             : {
        . = . + 0x10000 - 6 - ADDR(text) - SIZEOF(text) - SIZEOF(rodata) - SIZEOF(data);
    } >rom
    vectors          : {*(vectors)} >rom

    __DS = ADDR(data);
    __DE = ADDR(data) + SIZEOF(data);
    __DC = LOADADDR(data);

    __BB = ADDR(bss);
    __BE = ADDR(bss) + SIZEOF(bss);

    ___heap = ADDR(bss) + SIZEOF(bss);

    __SE = 0x3fff; /* Top of RAM address */
}
