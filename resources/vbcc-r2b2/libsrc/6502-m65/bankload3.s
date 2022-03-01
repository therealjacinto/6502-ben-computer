tmp     equ     ___bankv+23


	global	___bankload3
	section text
___bankload3:
	pha
	lda	#3
	sta	tmp
	jmp	___bankload

