	include	"reu.i"

	zpage	r28
	zpage	r29
	zpage	r30
	zpage	r31
	zpage	btmp0
	zpage	btmp1

bufsize	equ	24

	global	___bankcopy
	global	___bankcopy_len
	global	__fmemcpy

	section	text
__fmemcpy:
	lda	btmp1
	sta	r28
	lda	btmp1+1
	sta	r29
	ldy	btmp1+2
	lda	btmp0
	sta	r30
	lda	btmp0+1
	sta	r31
	ldx	btmp0+2
	lda	r0
	sta	___bankcopy_len
	lda	r1
	sta	___bankcopy_len+1

___bankcopy:
	cpx	#255
	bne	destbanked
	cpy	#255
	bne	bnktoun
untoun:
	ldy	#0
	ldx	___bankcopy_len+1
	beq	rest
loophi:
        lda     (r28),y
        sta     (r30),y
        iny
        bne     loophi
        inc     r29
        inc     r31
        dex
        bne     loophi
rest:
        ldx     ___bankcopy_len
        beq     done
looplo:
        lda     (r28),y
        sta     (r30),y
        iny
        dex
        bne     looplo
done:
	rts
bnktoun:
	asl	r29
	asl	r29
	tya
	lsr
	ror	r29
	lsr
	ror	r29
	sta	reu_bank
	lda	r28
	sta	reu_lo
	lda	r29
	sta	reu_hi
	lda	r30
	sta	reu_c64lo
	lda	r31
	sta	reu_c64hi
	lda	___bankcopy_len
	sta	reu_len
	lda	___bankcopy_len+1
	sta	reu_len+1
	lda	#129+16
	sta	reu_cmd
	rts
destbanked:
	cpy	#255
	bne	bnktobnk
untobnk:
	asl	r31
	asl	r31
	txa
	lsr
	ror	r31
	lsr
	ror	r31
        sta     reu_bank
        lda     r30
        sta     reu_lo
        lda     r31
        sta     reu_hi
        lda     r28
        sta     reu_c64lo
        lda     r29
        sta     reu_c64hi
        lda     ___bankcopy_len
        sta     reu_len
        lda     ___bankcopy_len+1
        sta     reu_len+1
        lda     #128+16
        sta     reu_cmd
        rts

bnktobnk:
	lda	#<___bankv
	sta	reu_c64lo
	lda	#>___bankv
	sta	reu_c64hi
	lda	#0
	sta	reu_len+1
loop:
	lda	___bankcopy_len+1
	bne	full
	lda	___bankcopy_len
	cmp	#bufsize
	bcc	do
full:
	asl	r29
	asl	r29
	tya
	lsr
	ror	r29
	lsr
	ror	r29
	tay
	asl	r31
	asl	r31
	txa
	lsr
	ror	r31
	lsr
	ror	r31
	tax
	lda	#bufsize
do:
	sta	reu_len
	sta	tmp
	lda	r28
	sta	reu_lo
	lda	r29
	sta	reu_hi
	sty	reu_bank
	lda	#129+16+32
	sta	reu_cmd
	lda	r30
	sta	reu_lo
	lda	r31
	sta	reu_hi
	stx	reu_bank
	lda	#128+16+32
	sta	reu_cmd

	lda	r28
	clc
	adc	tmp
	sta	r28
	lda	r29
	adc	#0
	sta	r29
	bcc	l1
	iny
l1:
	lda	r30
	clc
	adc	tmp
	sta	r30
	lda	r31
	adc	#0
	sta	r31
	bcc	l2
	inx
l2:
        lda     ___bankcopy_len
        sec
        sbc     tmp
        sta     ___bankcopy_len
        lda     ___bankcopy_len+1
        sbc     #0
        sta     ___bankcopy_len+1
	bne	loop
	lda	___bankcopy_len
	bne	loop

exit:
	rts

	section	bss
___bankcopy_len:
	reserve	2
tmp:
	reserve 1
