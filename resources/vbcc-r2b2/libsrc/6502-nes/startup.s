	include "nes.i"

	zpage r0
	zpage r1
	zpage r2
	zpage r3
	zpage sp

	global	___exit

	section "vectors"

	word ___nmi   ; $fffa vblank nmi
	word start ; $fffc reset
	word ___irq   ; $fffe irq / brk

	section "text"

	; --- system init code ---
start:
	sei
	cld ; disable decimal mode at famiclones
	ldx #$ff
	txs
	inx
	stx PPUMASK	
	stx DMCFREQ
	stx PPUCTRL		;no NMI
	bit PPUSTATUS ; init PPU
.1:
	bit PPUSTATUS
	bpl .1
.2:
	bit PPUSTATUS
	bpl .2
	; --- clear palette
	lda #$3f
	sta PPUADDR
	stx PPUADDR
	lda #$0f
	ldx #$20
.3:
	sta PPUDATA
	dex
	bne .3
	; --- clear vram
	txa
	ldy #$20
	sty PPUADDR
	sta PPUADDR
	ldy #$10
.4:
	sta PPUDATA
	inx
	bne .4
	dey
	bne .4
	; --- clear ram
	txa
.5:
	sta $00,x
	sta $0100,x
	sta $0200,x
	sta $0300,x
	sta $0400,x
	sta $0500,x
	sta $0600,x
	sta $0700,x
	inx
	bne .5

; copy initialized data from ROM to RAM
	lda #<__DS
	sta r0
	lda #>__DS
	sta r1
	lda #<__DC
	sta r2
	lda #>__DC
	sta r3
	ldy #0
	beq .6
.7:
	lda (r2),y
	sta (r0),y
	inc r0
	bne .8
	inc r1
.8:
	inc r2
	bne .6
	inc r3
.6:
	lda r0
	cmp #<__DE
	bne .7
	lda r1
	cmp #>__DE
	bne .7

; set software stack pointer
	lda #<__STACK
	sta sp
	lda #>__STACK
	sta sp+1

	; --- end of system init code, let's go


	jsr ___main

___exit:
	jmp ___exit

	;section "SAMPLES"
	;section "chars"
	;incbin "tileset.chr"

 section zpage
r0: reserve 1
r1: reserve 1
r2: reserve 1
r3: reserve 1
r4: reserve 1
r5: reserve 1
r6: reserve 1
r7: reserve 1
r8: reserve 1
r9: reserve 1
r10: reserve 1
r11: reserve 1
r12: reserve 1
r13: reserve 1
r14: reserve 1
r15: reserve 1
r16: reserve 1
r17: reserve 1
r18: reserve 1
r19: reserve 1
r20: reserve 1
r21: reserve 1
r22: reserve 1
r23: reserve 1
r24: reserve 1
r25: reserve 1
r26: reserve 1
r27: reserve 1
r28: reserve 1
r29: reserve 1
r30: reserve 1
r31: reserve 1
sp: reserve 2

 global r0
 global r1
 global r2
 global r3
 global r4
 global r5
 global r6
 global r7
 global r8
 global r9
 global r10
 global r11
 global r12
 global r13
 global r14
 global r15
 global r16
 global r17
 global r18
 global r19
 global r20
 global r21
 global r22
 global r23
 global r24
 global r25
 global r26
 global r27
 global r28
 global r29
 global r30
 global r31
 global sp

