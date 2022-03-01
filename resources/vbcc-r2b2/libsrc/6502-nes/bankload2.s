tmp     equ     ___bankv+23


	global	___bankload2
	section text
___bankload2:
	pha
	lda	#2
	sta	tmp
	jmp	___bankload

