	include	"reu.i"

	zpage	r28
	zpage	r29
	zpage	r30
	zpage	r31

bufsize	equ	24

	global	___bankcopy
	global	___bankcopy_len
	section	text

___bankcopy:
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
