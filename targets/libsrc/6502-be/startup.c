#define NULL ((void *)0)

extern int main(int argc, char **argv);
void set_user_sp() = "\tlda  #<__SE\n\tsta  sp\n\tlda  #>__SE\n\tsta  sp+1";
void set_system_sp() = "\tldx  #0xff\n\ttxs";
void start();

#pragma section init
void init1()
{
    set_user_sp();
    set_system_sp();
    start();
}

#pragma section text
void start()
{
    //init1();
    main(0, NULL);
}