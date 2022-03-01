#!/bin/sh

# convert example graphics for lazyhello, balloon, bubbles
#png2chr -n -c 29,33,2 example.png
#png2chr -n -c 29,33,18,3 sprites.png

# lazyhello
vc +nrom256v -+ -O3 lazyhello.c lazydata.s -llazynes -o lazyhello.nes

# balloon
vc +nrom256v -+ -O3 balloon.c lazydata.s -llazynes -o balloon.nes

# bubbles
vc +nrom256v -+ -O3 bubbles.c lazydata.s -llazynes -o bubbles.nes

# descend
#png2chr -n -u -c 29,2,33,57,5 descendchr.png
#png2spr -n -u -c 29,0,39,16,0,39,57,33,44,6 -l 1 -r 64,48 descendspr.png
vc +nrom256v -+ -O2 descend.c descenddata.s -llazynes -lfamitone2 -o descend.nes

# catwalk
#png2chr -n -u -c 29,45,28,16,45,33,49,45,8,9 catwalkchr.png
#png2spr -n -u -c 29,56,38,23,56,38,22,56,28,33,22,39,0 -l 24 robos1-nes.png
#png2spr -n -u -c 29,56,38,23,56,38,22,56,28,33,22,39,0 -l 8 -r 24,32 -b 39 -t 472 robos2-nes.png
vc +nrom256v -+ -O3 catwalk.c catwalkdata.s -llazynes -lfamitone2 -o catwalk.nes
