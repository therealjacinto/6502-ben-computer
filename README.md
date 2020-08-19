# 6502-ben-computer
Ben Eater's 6502 Computer - C Implementation

## Background
The kit and tutorials created by Ben Eater are carried out using the assembly language. This project attempts to follow the tutorials using C instead. I am using the vbcc 6502 compiler in this repo and attempting to write that (as closely as I can) resembles the code created by Ben.

## Building
In order to build this, first make a `bin` folder and place all the executable files necessary for vbcc cross compilation (`vasm6502_oldstyle`, `vbcc6502`, `vc`, `vlink`, `vcpr6502`). You can download them here: http://www.compilers.de/vbcc.html in the "6502 related Download/link area" section. You can then run the `buildall.sh` script to build all the programs into the build directory.

## TODO
There is much optimization that can be done as I learn more about how to use `vbcc`. I can utilize more inline functions, avoid saving register states when entering certain functions, etc.

Also, I need to understand more about how to use certain libraries and include header files. In general, this is a big work in progress.
