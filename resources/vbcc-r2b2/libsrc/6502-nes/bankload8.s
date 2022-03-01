tmp     equ     ___bankv+23


	global	___bankload8
	section text
___bankload8:
	pha
	lda	#8
	sta	tmp
	jmp	___bankload

