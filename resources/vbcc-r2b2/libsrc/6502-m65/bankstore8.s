tmp     equ     ___bankv+23


	global	___bankstore8
	section text
___bankstore8:
	pha
	lda	#8
	sta	tmp
	jmp	___bankstore

