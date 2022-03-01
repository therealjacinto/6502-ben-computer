tmp     equ     ___bankv+23


	global	___bankload4
	section text
___bankload4:
	pha
	lda	#4
	sta	tmp
	jmp	___bankload

