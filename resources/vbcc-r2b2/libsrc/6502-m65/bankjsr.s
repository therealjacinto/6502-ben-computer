	global	___bankjsr
	zpage	r29
	zpage	r30
	zpage	r31

	section text
___bankjsr:
	pha
	jsr	___bankswitch
	jsr	l1
	pla
	tay
	jsr	___bankswitch
	rts
l1:
	jmp	(r30)


