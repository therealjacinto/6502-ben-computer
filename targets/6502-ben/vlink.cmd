STACKSTART = 0x0100;
STACKLEN   = 0x00ff;

MEMORY
{
    zero : org = 0x0000, len = 0x00ff
    stack : org = STACKSTART, len = STACKLEN
    ram : org = 0x0200, len = 0x3dff
    rom : org = 0x8000, len = 0x8000
}

PHDRS
{
    exit PT_LOAD;
    handler PT_LOAD;
    entry PT_LOAD;
    text PT_LOAD;
    lcdio PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{
    zpage   (NOLOAD) : {*(zpage)}   >zero
    bss     (NOLOAD) : {*(bss)}     >ram
    exit             : {*(exit)}    >rom         :exit
    handler          : {*(handler)} >rom         :handler
    entry            : {*(entry)}   >rom         :entry
    text             : {*(text)}    >rom         :text
    lcdio            : {*(lcdio)}   >rom         :lcdio
    data    (NOLOAD) : {*(data)}    >ram  AT>rom :data
    rodata           : {*(rodata)}  >rom         :data
    
    
    . = 0xfffc;
    other : {
        SHORT(ADDR(entry));
        SHORT(0x0000);
    }
    
    __SE = STACKSTART+STACKLEN;

    __BB = ADDR(bss);
    __BE = ADDR(bss)+SIZEOF(bss);
    
    __DS = ADDR(data);
    __DE = ADDR(data)+SIZEOF(data);
    __DC = LOADADDR(data);

    ___heap = ADDR(bss)+SIZEOF(bss)+SIZEOF(data);
    ___heapend = STACKSTART-1;
}
