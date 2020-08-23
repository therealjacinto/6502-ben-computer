# 6502-ben-computer
Ben Eater's 6502 Computer - C Implementation

## Background
The kit and tutorials created by Ben Eater are carried out using the assembly language. This project attempts to follow the tutorials using C instead. I am using the vbcc 6502 compiler in this repo and attempting to write that (as closely as I can) resembles the code created by Ben.

## Linker
For the most part, you won't have to muck with the linker script (`targets/6502-ben/vlink.cmd`), unless you are adding custom sections that might help with debugging (see the Debugging Tools section below). I found this page useful in case you want to make changes: https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_mono/ld.html

## Building
In order to build this, first make a `bin` folder and place all the executable files necessary for vbcc cross compilation (`vasm6502_oldstyle`, `vbcc6502`, `vc`, `vlink`, `vcpr6502`). You can download them here: http://www.compilers.de/vbcc.html in the "6502 related Download/link area" section. You can then run the `buildall.sh` script to build all the programs into the build directory.

## Debugging Tools
There are many 6502 emulators out there that you can use with various different environments (http://www.6502.org/tools/emu/). For this project, my choice was primarily driven by being able to run on a linux environment, so I went with a personal forked version of `py65mon`.

### py65mon
This project utilizes the `py65mon` monitor in order to debug the binary output that is built. Documentation for this program can be found here: https://py65.readthedocs.io. 

In order to use the debugger, first verify you have the conda environment installed:

`./scripts/install_conda_env.sh`

Once the environment is installed, then you can build the binary with debugging tools:

`./scripts/buildmanually.sh --all --debug`

This will output a raw segment file that will allow us to add some labels to `py65mon`. Now that everything is build properly, you can launch the debugger:

`./scripts/launch_debugger.sh [PATH_TO_BINARY]`

### Adding function labels
When using `py65mon`, it might be useful to understand where certain functions exist in the binary, especially as you make change to it. Something useful I have found is to prefix the function with `__section("[NAME_OF_TEMPORARY_SECTION")` and add this section name to the `targets/6502-ben/vlink.cmd` file (with the same format as the text section). Once you do this, rebuild all with the `--debug` flag, and relaunch the debugger. Using the `show_labels` `py65mon` command, it will tell you the location of the function in the binary.

### Forked note
I didn't change much in the fork, I only added a sting buffer to represent the ascii characters that get sent to the LCD when a certain write command gets sent to PORTA. You can see this implementation here: https://github.com/therealjacinto/py65/commit/5d5bbed2ab60191999ea170714393f1a9f71701b

## TODO
There is much optimization that can be done as I learn more about how to use `vbcc`. I can utilize more inline functions, avoid saving register states when entering certain functions, etc.

Also, I need to understand more about how to use certain libraries and include header files. In general, this is a big work in progress.
