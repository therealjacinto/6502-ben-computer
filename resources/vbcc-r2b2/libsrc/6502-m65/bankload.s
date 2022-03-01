tmp     equ     ___bankv+23


        global  ___bankload
	zpage	r30
	zpage	r31

	section text
___bankload:
	phx
	jsr	___bankswitch
	plx
	ldy	#0
loop:
	lda	(r30),y
	sta	___bankv,x
	inx
	iny
	cpy	tmp
	bne	loop
	pla
	tay
	jmp	___bankswitch

