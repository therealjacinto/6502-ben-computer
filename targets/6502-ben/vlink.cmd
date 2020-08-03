STACKSTART = 0x0100;
STACKLEN   = 0x00ff;

MEMORY
{
    zero : org = 0x0000, len = 0x00ff
    stack : org = STACKSTART, len = STACKLEN
    ram : org = 0x0200, len = 0x3dff
    rom : org = 0x8000, len = 0x8000
}

SECTIONS
{
    zpage  (NOLOAD) : {*(zpage)} >zero
    init            : {*(init)} >rom
    text            : {*(text)} >rom
    rodata          : {*(rodata)} >rom
    
    . = 0xfffc;
    other : {
        SHORT(ADDR(init));
        SHORT(0x0000);
    }
    
    __SE = STACKSTART+STACKLEN;
}
