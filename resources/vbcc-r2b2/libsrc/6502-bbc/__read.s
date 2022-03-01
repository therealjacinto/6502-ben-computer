	global	___read
	zpage	r0,r1,r2,r3,r4,r5,r6,r7
	section	text
___read:
	dec	r4
	ldy	#255
	sty	r6
	iny
	sty	r5
	tya
	ldx	#r2
	jsr	0xfff1
	bcs	nocr
	lda	#10
	sta	(r2),y
	iny
nocr:
	tya
	ldx	#0
	rts
