	section	text
	global	___copycharset
___copycharset:
	lda	#0
	sta	8192
	sta	8193
	sta	8198
	sta	8198
	lda	#>___stdchr
	sta	r1
	lda	#<___stdchr
	sta	r0
	ldx	#32
	ldy	#0
l7:
	lda	(r0),y
	sta	8199
	iny
	bne	l7
	inc	r1
	dex
	bne	l7

	lda	___nmiPPUMASK
	sta	8193
	lda	___nmiPPUCTRL
	sta	8192
	rts

