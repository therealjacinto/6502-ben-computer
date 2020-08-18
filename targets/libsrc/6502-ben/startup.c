#include "lcdio/lcdio.h"
#define NULL ((void *)0)

extern int main(int argc, char **argv);
void set_user_sp() = "\tlda  #<__SE\n\tsta  sp\n\tlda  #>__SE\n\tsta  sp+1";
void set_system_sp() = "\tldx  #0xff\n\ttxs";
void goto_start() = "\tjmp _start";
void goto_exit() = "\tjmp _exit";

#pragma section init
void init1()
{
    set_user_sp();
    set_system_sp();
    lcd_reset();
    goto_start();
}

#pragma section text
void start()
{
    main(0, NULL);
    goto_exit();
}

void exit()
{
    goto_exit();
}