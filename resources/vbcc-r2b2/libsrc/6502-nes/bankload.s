tmp     equ     ___bankv+23


        global  ___bankload
	zpage	r30
	zpage	r31

	section text
___bankload:
	tya
	sta	switchTab,y
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
	sta	switchTab,y
	rts

