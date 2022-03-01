; NMI handler for vbcc6502 stdin/stdout/clock() support, 'Lazycow 2020

	include "nes.i"

OAMBUF=$0200

	global ___nmi
	global ___nmiPPUCTRL, ___nmiPPUMASK, ___nmiSCROLLY, ___nmiCOUNTER
	global ___nmiJOYPAD1, ___vruCON, ___vruSRC

	section "text"

FlushVramUpdateNMI: ; sub from shiru's neslib
	ldy #0
.updName:
	lda (___vruSRC),y
	iny
	cmp #$40 ;is it a non-sequental write?
	bcs .updNotSeq
	sta PPUADDR
	lda (___vruSRC),y
	iny
	sta PPUADDR
	lda (___vruSRC),y
	iny
	sta PPUDATA
	jmp .updName
.updNotSeq:
	tax
	lda ___nmiPPUCTRL
	cpx #$80 ;is it a horizontal or vertical sequence?
	bcc .updHorzSeq
	cpx #$ff ;is it end of the update?
	beq .updDone
.updVertSeq:
	ora #$04
	bne .updNameSeq ;bra
.updHorzSeq:
	and #$fb
.updNameSeq:
	sta PPUCTRL
	txa
	and #$3f
	sta PPUADDR
	lda (___vruSRC),y
	iny
	sta PPUADDR
	lda (___vruSRC),y
	iny
	tax
.updNameLoop:
	lda (___vruSRC),y
	iny
	sta PPUDATA
	dex
	bne .updNameLoop
	lda ___nmiPPUCTRL
	sta PPUCTRL
	jmp .updName
.updDone:
	rts

InitNMI:
	lda #8+128
	sta ___nmiPPUCTRL
	lda #2+4+8+16 ; chr/spr are visible
	sta ___nmiPPUMASK

	; --- load color palettes
	lda #<$3f00
	ldx #>$3f00
	stx PPUADDR
	sta PPUADDR
	ldx #0
.c1:
	lda bgPal,x
	sta PPUDATA
	inx
	cpx #32
	bne .c1
	; --- disable sprites, maybe except cursor
	lda #255
	ldx #0
	ldy #252
	sty nmiTemp
	ldy OAMBUF+252
	bne .c2
	ldy OAMBUF+255
	bne .c2
	stx nmiTemp
.c2:
	sta OAMBUF,x
	inx
	cpx nmiTemp
	bne .c2
	rts

	; --- --- ---
___nmi:
	pha
	txa
	pha
	tya
	pha

	; --- init sprites/palettes/display?
	lda nmiFlags
	bne .skipInit ; not 1st call? -> skip init
	jsr InitNMI
	lda #1
	sta nmiFlags
	bne .skipUpd ; always
.skipInit

	; --- flash cursor (sprite #63)
	ldx #0
	stx OAMBUF+254
	lda ___nmiCOUNTER
	and #31
	cmp #8
	bcc .sc
	ldx #255
.sc
	stx OAMBUF+253

	; --- display sprites
	lda #>OAMBUF
	sta OAMDMA

	; --- handle vram updates
	lda ___vruCON
	beq .skipUpd
	jsr FlushVramUpdateNMI
	lda #0
	sta ___vruCON
.skipUpd:

	; --- scrolling
	lda ___nmiPPUMASK
	sta PPUMASK
	lda #0
	sta PPUADDR
	sta PPUADDR
	lda #0 ; scroll-x
	sta PPUSCROLL
	lda ___nmiSCROLLY
	sta PPUSCROLL
	lda ___nmiPPUCTRL
	sta PPUCTRL

	; --- joypad
	ldy #0 ; port
	ldx #3
.j1:
	lda #1
	sta CTRLPORT1
	sta nmiTemp-1,x
	lda #0
	sta CTRLPORT1
.j2:
	lda CTRLPORT1,y
	lsr a
	rol nmiTemp-1,x
	bcc .j2
	dex
	bne .j1
	lda nmiTemp
	cmp nmiTemp+1
	beq .j3
	cmp nmiTemp+2
	beq .j3
	lda nmiTemp+1
.j3:
	sta ___nmiJOYPAD1

	; --- counter
	inc ___nmiCOUNTER
	bne .z
	inc ___nmiCOUNTER+1
.z
	pla
	tay
	pla
	tax
	pla
	rti

	section "zpage"

	zpage nmiFlags
	zpage nmiTemp
	zpage ___nmiPPUCTRL
	zpage ___nmiPPUMASK
	zpage ___nmiSCROLLY
	zpage ___nmiCOUNTER
	zpage ___nmiJOYPAD1
	zpage ___vruSRC
	zpage ___vruCON

nmiTemp: byte 0,0,0 ; temp vars for NMI only
nmiFlags: byte 0 ; init flag
___nmiPPUCTRL: byte 0
___nmiPPUMASK: byte 0
___nmiSCROLLY: byte 0
___nmiCOUNTER: word 0
___nmiJOYPAD1: byte 0
___vruSRC: word 0 ; lo/hi
___vruCON: byte 0 ; semaphore

	section "rodata"

bgPal:
	byte 2,33,32,32, 15,32,32,32, 15,32,32,32, 15,32,32,32
	byte 2,33,32,32, 15,32,32,32, 15,32,32,32, 15,32,32,32
