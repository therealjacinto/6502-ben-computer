	global	___bankswitch
	zpage	r29
	zpage	r30
	zpage	r31

	section text
___bankswitch:
	phz
	tya
	ldy	#0
	clc
	ror
	bcs	skip
	ldy	#$80
	sec
	sbc	#1
	and	#$0f
skip:
	ora	#$f0
	taz
	lda	#0
	tay
	ldx	#$f0
	map
	plz
	rts

