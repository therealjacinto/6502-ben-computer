# 6502-ben-computer
Ben Eater's 6502 Computer - C Implementation

## Background
The kit and tutorials created by Ben Eater are carried out using the assembly language. This project attempts to follow the tutorials using C instead. I am using the vbcc 6502 compiler in this repo and attempting to write that (as closely as I can) resembles the code created by Ben.

## Building
In order to build this, first make a `bin` folder and place all the executable files necessary for vbcc cross compilation (`vasm6502_oldstyle`, `vbcc6502`, `vc`, `vlink`, `vcpr6502`). You can download them here: http://www.compilers.de/vbcc.html in the "6502 related Download/link area" section. You can then run the `buildall.sh` script to build all the programs into the build directory.

## Debugging
This project utilizes the `py65mon` monitor in order to debug the binary output that is built. Documentation for this program can be found here: https://py65.readthedocs.io. 

In order to use the debugger, first verify you have the conda environment installed:

`./scripts/install_conda_env.sh`

Once the environment is installed, then you can build the binary with debugging tools:

`./scripts/buildmanually.sh --all --debug`

This will output a raw segment file that will allow us to add some labels to `py65mon`. Now that everything is build properly, you can launch the debugger:

`./scripts/launch_debugger.sh [PATH_TO_BINARY]`

### Adding function labels
When using `py65mon`, it might be useful to understand where certain functions exist in the binary, especially as you make change to it. Something useful I have found is to prefix the function with `__section("[NAME_OF_TEMPORARY_SECTION")` and add this section name to the `targets/6502-ben/vlink.cmd` file (with the same format as the text section). Once you do this, rebuild all with the `--debug` flag, and relaunch the debugger. Using the `show_labels` `py65mon` command, it will tell you the location of the function in the binary.

## TODO
There is much optimization that can be done as I learn more about how to use `vbcc`. I can utilize more inline functions, avoid saving register states when entering certain functions, etc.

Also, I need to understand more about how to use certain libraries and include header files. In general, this is a big work in progress.
