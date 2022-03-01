	include	"reu.i"

	zpage	btmp0
	zpage	btmp1
	zpage	r0
	zpage	r1
	zpage	r2

bufsize	equ	24

	global	__fmemcpy

	section	text

__fmemcpy:
	lda	#<___bankv
	sta	reu_c64lo
	lda	#>___bankv
	sta	reu_c64hi
	lda	#0
	ldy	btmp1+2
	ldx	btmp0+2
	sta	reu_len+1
loop:
	lda	r1
	bne	full
	lda	r0
	cmp	#bufsize
	bcc	do
full:
	lda	#bufsize
do:
	sta	reu_len
	sta	r2
	lda	btmp1
	sta	reu_lo
	lda	btmp1+1
	sta	reu_hi
	sty	reu_bank
	lda	#129+16+32
	sta	reu_cmd
	lda	btmp0
	sta	reu_lo
	lda	btmp0+1
	sta	reu_hi
	stx	reu_bank
	lda	#128+16+32
	sta	reu_cmd

	lda	btmp0
	clc
	adc	r2
	sta	btmp0
	lda	btmp0+1
	adc	#0
	sta	btmp0+1
	bcc	l1
	inx
l1:
	lda	btmp1
	clc
	adc	r2
	sta	btmp1
	lda	btmp1+1
	adc	#0
	sta	btmp1+1
	bcc	l2
	iny
l2:
        lda     r0
        sec
        sbc     r2
        sta     r0
        lda     r1
        sbc     #0
        sta     r1
	bne	loop
	lda	r0
	bne	loop

exit:
	rts
