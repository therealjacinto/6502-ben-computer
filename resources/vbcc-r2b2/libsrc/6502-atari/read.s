CHID  =   $0340
ICDNO  =   $0341
ICCOM  =   $0342
ICSTA  =   $0343
ICBAL  =   $0344
ICBAH  =   $0345
ICPTL  =   $0346
ICPTH  =   $0347
ICBLL  =   $0348
ICBLH  =   $0349
ICAXl  =   $034A
ICAX2  =   $034B
CIOV   =   $E456

	global	___read
	section	text
___read:
	ldx	#0
	lda	#5
	sta	ICCOM
	lda	r2
	sta	ICBAL
	lda	r3
	sta	ICBAH
	lda	r4
	sta	ICBLL
	bne	notzero
	lda	r5
	bne	notzero
	lda	#0
	ldx	#0
	rts
notzero:
	lda	r5
	sta	ICBLH
	jsr	CIOV
	lda	ICBLL
	ldx	ICBLH
	rts

