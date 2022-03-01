	include	"reu.i"
	zpage	r30

	global	___bankload
	section	text
___bankload:
        sta     reu_len
        cpy     #255
        beq     nonbanked
	stx	___bankvoff
	ldx	#129+16
	jmp	___bankdo

nonbanked:
	iny
loop:
	lda	(r30),y
	sta	___bankv,x
	iny
	inx
	dec	reu_len
	bne	loop
	rts
