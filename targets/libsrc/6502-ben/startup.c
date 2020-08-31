#include "lcdio/lcdio.h"

#define NULL ((void *)0)

// Linker defined symbols
extern unsigned char* _SE;
extern unsigned char* _DS;
extern unsigned char* _DE;
extern unsigned char* _DC;
extern unsigned char* _BB;
extern unsigned char* _BE;

// Used by compiler. Needs to be an inline assembly function because it calls
// sp, and we don't want compiler to prepend an underscore to it.
void set_user_sp_address(__reg("a/x") unsigned char** se) =
    "\tsta sp\n"
    "\tstx sp+1\n";

// Hardware stack pointer
void set_hardware_sp_address(__reg("a") char val) =
    "\ttax\n"
    "\ttxs";

// Initialize ram
void init_ram()
{
    unsigned char **src = &_DC;
    unsigned char **dst = &_DS;

    /* Copy ROM data to RAM */
    while (dst < &_DE)
        *dst++ = *src++;

    /* Initialize RAM bss */
    for (dst = &_BB; dst < &_BE; dst++)
        *dst = 0;
}

extern int main(int argc, char **argv);

// Is called once main completes
void exit()
{
    // jmp to exit indefinitely
    exit();
}

// After init is called, this handles the call to main and then exit.
void start()
{
    main(0, NULL);
    exit();
}

#pragma section init
void init()
{
    set_user_sp_address(&_SE);
    set_hardware_sp_address(0xff);
    init_ram();
    lcd_reset();
    start();
}
# pragma section default
