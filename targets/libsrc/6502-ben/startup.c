#include "lcdio/lcdio.h"

#define NULL ((void *)0)

// Stack End (__SE)is defined in linker
extern int* _SE;

// Used by compiler
void set_user_sp_address(__reg("a/x") int** se) =
  "\tsta sp\n"
  "\tstx sp+1\n";

// Hardware stack pointer
void set_hardware_sp_address(__reg("a") char val) =
  "\ttax\n"
  "\ttxs";

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
    // Call exit
    exit();
}

#pragma section init
void init1()
{
    set_user_sp_address(&_SE);
    set_hardware_sp_address(0xff);
    lcd_reset();
    // Call start() without returning
    start();
}

#pragma section default