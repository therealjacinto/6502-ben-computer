#include "lcdio/lcdio.h"

#define NULL ((void *)0)

// Stack End (__SE) is defined in linker
extern int* _SE;
extern int _BB;
extern int _BE;
extern unsigned char* _DS;
extern int _DE;
extern volatile unsigned char* _DC;

// Used by compiler
void set_user_sp_address(__reg("a/x") int** se) =
  "\tsta sp\n"
  "\tstx sp+1\n";

// Hardware stack pointer
void set_hardware_sp_address(__reg("a") char val) =
  "\ttax\n"
  "\ttxs";

// Initialize ram
// void init_ram(__reg("a") char val) = 
//   "\tsta $2000\n"
//   "\tlda #0\n"
//   "\tsta $2001";
// {
//     //char *src = (char *)_DC;
//     *_DS = 'H';
//     *(_DS + 1) = 0;

//     /* ROM has data; copy it to RAM */
//     //while (dst < (char *)_DE) {
//     //    *dst++ = *src++;
//     //}

//     /* Zero bss */
//     //for (dst = &_BB; dst< &_BE; dst++)
//     //    *dst = 0;
// }

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
    lcd_reset();
    start();
}
# pragma section default
