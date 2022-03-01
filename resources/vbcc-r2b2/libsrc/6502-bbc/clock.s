	global	_clock
	zpage	btmp0,btmp1
	section	text
_clock:
	ldx	#<btmp0
	ldy	#>btmp0
	lda	#1
	jmp	0xfff1
