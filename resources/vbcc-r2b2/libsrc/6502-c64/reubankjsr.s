	global	___bankjsr
	zpage	r30
	zpage	r31

	section	text
___bankjsr:
	pha
	jsr	___bankswitch
	jsr	l1
	sta	r31
	pla
	tay
	jsr	___bankswitch
	lda	r31
	rts
l1:
	jmp	(r30)

