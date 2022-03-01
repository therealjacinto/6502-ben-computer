lazyNES

As lazy as possible NES hardware support library for vbcc6502

The lazyNES library includes all crucial functionality to write NES games in C,
except audio. For this, lazyNES happily cooperates with the included famitone2
Famitracker replay library from Shiru.


Usage:

To use lazyNES, you have to #include the lazynes.h file and to link the lazynes
library with -llazynes (see buildexamples.sh) - further informations can be
found in the lazynes.h file itself and in the included examples.


Examples:

lazyhello example:
- writes text to the NES nametable

balloon example:
- moves a balloon metasprite, build of of six 8x8 sprites

bubbles example:
- installs a SPR0HIT split and scrolls the lower part only
- implements an interactive list of drawable objects
- all objects use shifted coordinates to enable sub pixel movments
- checks how many sprites can be moved with 60 fps (or 50 fps in PAL)

descend example:
- writes blocks to nametables with lnList() while the screen is visible
- scrolls the screen vertically with lnScroll()
- includes graphics and color infos from files generated with png2chr
- includes graphics and metasprite infos from files generated with png2spr
- links famitone2 to play music

catwalk example:
- writes blocks to nametables, read from a tilemap build out of 2x2 tiles
- scrolls the screen horizontally with lnScroll()
- implements an interactive list of drawable objects
- all objects use shifted coordinates to enable sub pixel movments
- checks for collision between sprites and between tilemap and sprites
- includes graphics and color infos from files generated with png2chr
- includes graphics and metasprite infos from files generated with png2spr
- links famitone2 to play music and a sound effect


Notes:

The lazyNES library installs its own NMI handler and because of this you
cannot use vbcc's stdio.h and timer.h. (both rely on vbcc's default NMI handler)
All other standard libraries should work.

If you don't want to use the included tools png2chr and png2spr, take a look
at "YY-CHR" or "NES Screen Tool".

As the famitone2 tools text2data and nsf2data currently do not support vasm
output, you can use asm6 output instead and replace all '@' with '.' with a
text editor. (or with sed)

examples:
text2data mysong.txt -asm6
sed -e's/@/\./g' mysong.asm >mysong-vasm.s
nsf2data mysounds.nsf -asm6
sed -e's/@/\./g' mysounds.asm >mysounds-vasm.s


History:
V1.0   [20.06.2020] first public release for vbcc6502
V1.0.1 [04.07.2020] restructured NMI to fix race condition


Do you have the latest version? Take a look!
Any feedback? Please tell me!

homepage: www.lazycow.de/lazynes
 contact: moo@lazycow.de
