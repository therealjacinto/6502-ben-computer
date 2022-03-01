tmp     equ     ___bankv+23


        global  ___bankstore
	zpage	r30
	zpage	r31

	section text
___bankstore:
	phx
	jsr	___bankswitch
	plx
	ldy	#0
loop:
	lda	___bankv,x
	sta	(r30),y
	inx
	iny
	cpy	tmp
	bne	loop
	pla
	tay
	jmp	___bankswitch

