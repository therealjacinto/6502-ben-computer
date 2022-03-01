	global	___bankjsr
	zpage	r29
	zpage	r30
	zpage	r31

	section text
___bankjsr:
	pha
	tya
	sta	switchTab,y
	jsr	l1
	sta	r31
	pla
	tay
	sta	switchTab,y
	lda	r31
	rts
l1:
	jmp	(r30)


