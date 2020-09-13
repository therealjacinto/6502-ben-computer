# 6502-ben-computer
This repo is a C implementation of the Ben Eater 6502 computer. The kit and tutorials created by Ben Eater are originally carried out using the assembly language, but this project attempts to follow the tutorials using C instead. I am using the vbcc 6502 compiler in this repo and attempting to write C code that compiles as closely as possible to the original assembly code.

# Development Environment
Note: I develop using Ubuntu 18, and thus all the bash scripts in the scripts directory are made for for this platform. I have not tested them on Mac, however `vbcc` is not available for Macs (to my knowledge).

## Building
In order to build this, first make a `bin` folder and place all the executable files necessary for vbcc cross compilation (`vasm6502_oldstyle`, `vbcc6502`, `vc`, `vlink`, `vcpr6502`). You can download them here: http://www.compilers.de/vbcc.html in the "6502 related Download/link area" section. You can then run the `buildall.sh` script (in the script directory) to build all the programs into the build directory. This script will not use any optimizers. I have found that each file requires different optimizations and I don't know how to use `vbcc` with inidividual optimization flags (rather than telling it to use the same flag for all files). Therefore, there is another script in the script directory named `buildmanually.sh`. In this file, all the steps are laid out 1 by 1 and can be tweaked. Run the script with no arguments to have a usage printed out. This will help you build only the parts you want. The standard use-cases when using this script are the following:
* For building a production binary:
    
        buildmanually.sh --all

* For building a binary to debug:

        buildmanually.sh --all --debug --save-output-files

## Debugging
There are many 6502 emulators out there that you can use with various different environments (http://www.6502.org/tools/emu/). For this project, my choice was primarily driven by being able to run on a linux environment, so I went with a personal forked version of `py65mon`. py65mon is a monitor that allows you step into your code and saves the entire address space from 0x0000 to 0xffff. It is a powerful tool that has many capabilities, unfortunately it can sometimes take a while to use. The fork I have created adds the LCD as an additional monitor. That way you can see what the LCD will output with the binary file being debugged. You can see this implementation here: https://github.com/therealjacinto/py65/commit/5d5bbed2ab60191999ea170714393f1a9f71701b.

## Requirements
In order to use the debugger setup for this project, you need `expect` and `conda` installed on the environment. On Ubuntu, you can install expect using apt:

    sudo apt install expect

and you can install conda via their website: https://conda.io/en/latest/miniconda.html. Once installed, use the install script to install the environment:

    ./scripts/install_conda_env.sh

### How To Launch The Debugger
In the scripts directory, there is a `launch_debugger.sh` script to attempt to automate a lot of tasks that occur prior to debugging a binary file. It will automatically setup the ROM to the correct address space, add various labels useful to debugging (such as register names, hardware locations, method locations, label locations), and adds breakpoints at infinite jmp loops. It is recommended you build the binary using the recommended command for building a binary to debug above, but it is not required. You can launch the debugger on any binary file, but more information/labels will be added if you build it as recommended because of the output assembly files that are build with the `--save-output-files` flag. These create the labels for method locations. In order to launch the debugger, run the following:

    launch_debugger.sh [PATH_TO_BINARY] --find-labels

The script will point the monitor to the address speficied at 0x7ffc. Because of this action, the number of cycles displayed by py65mon will be 3 rather than 0. It is better to use cycles as a relative number rather than an absolute number.

Without the `--find-labels` flag, no additional labels will be added for method and label locations from the assembly output file. You can also use the `--verbose` flag for more information on what is happening in the background. This flag is currently under construction and works most of the time, but there are times where it will not. It will hardly error, but if it does, then don't use the flag. For the most part it will either find all labels or most of them. As the project continues, this should get better and better.

You can also use the launch_debugger script to just build the disassemble file and not launch the debugger. Use the `--disassemble-only` flag for this option.

You may also specify the directory of the assembly files in case they aren't in the same place as the binary. You can do so like the following:

    launch_debugger.sh [PATH_TO_BINARY] --find-labels --asm-directory [PATH_TO_ASM_FILES]

### Debugger Primer
If you just want to run the binary and see what is outputted to the LCD, then once the debugger is launched, pass the `goto` command into py65mon using the current address that it is displaying (under PC). This will run the binary with the current state of the machine. If it doesn't hit a breakpoint or it hangs, you can use `ctrl+c` to break and it will tell you the current state of the machine, including what was sent to the LCD. Similarly, you can use `return` to run your file and stop just before the next RTS or RTI is executed. `step` will just execute one command. 

If you are interested in memory, you can use the `mem` command. You will need to pass an address argument, or if you want to see a range, you can pass a range like [MIN_ADDR]:[MAX_ADDR]. For example, if you wanted to see the entire zpage, you can do:

    .mem 0:ff

You can also use labels in the place of addresses, this will give you a similar result (using the labels created in the launch debugger script):

    .mem r0:stack

Finally, you can use `disassemble` to show the assembly code from a given address range. This is very useful for learning where you are in the code. You can use addresses and ranges like above. For example, if you wanted to disassemble your entire ROM, you can do:

    .disassemble 8000:7fff

## TODO
The labelling process (`parseFile.py`) can use some improvements, in particular if there exists more than one match for the label. If this is the case, we shouldn't determine its location until maybe we have more information.

There is much optimization that can be done as I learn more about how to use `vbcc`. I can utilize more inline functions, avoid saving register states when entering certain functions, etc.

Also, I need to understand more about how to use certain libraries and include header files.
