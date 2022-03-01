tmp     equ     ___bankv+23


	global	___bankstore4
	section text
___bankstore4:
	pha
	lda	#4
	sta	tmp
	jmp	___bankstore

