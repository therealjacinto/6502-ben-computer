	include	"reu.i"
	zpage	r30

	global	___bankstore
	section	text
___bankstore:
	sta	reu_len
	cpy	#255
	beq	nonbanked
	stx	___bankvoff
	ldx	#128+16
	jmp	___bankdo

nonbanked:
        iny
loop:
        lda     ___bankv,x
	sta	(r30),y
        iny
        inx
        dec     reu_len
        bne     loop
	rts
