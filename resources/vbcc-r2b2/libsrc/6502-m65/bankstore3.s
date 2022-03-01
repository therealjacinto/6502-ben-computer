tmp     equ     ___bankv+23


	global	___bankstore3
	section text
___bankstore3:
	pha
	lda	#3
	sta	tmp
	jmp	___bankstore

