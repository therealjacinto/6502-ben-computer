tmp     equ     ___bankv+23


	global	___bankstore2
	section text
___bankstore2:
	pha
	lda	#2
	sta	tmp
	jmp	___bankstore

