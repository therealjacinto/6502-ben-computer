tmp     equ     ___bankv+23


	global	___bankstore1
	section text
___bankstore1:
	pha
	lda	#1
	sta	tmp
	jmp	___bankstore

