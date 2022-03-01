tmp     equ     ___bankv+23


	global	___bankload1
	section text
___bankload1:
	pha
	lda	#1
	sta	tmp
	jmp	___bankload

