	include	"reu.i"

	zpage	r30
	zpage	r31

	global	___bankvoff
	global	___bankdo
	section	text

; could do some optimizations, e.g. with bankv-offset
___bankdo:
	asl	r31
	asl	r31
	tya
	lsr
	ror	r31
	lsr
	ror	r31
	tay
	lda	#0
	sta	reu_len+1
	lda	#<___bankv
	clc
	adc	___bankvoff
	sta	reu_c64lo
	lda	#>___bankv
	adc	#0
	sta	reu_c64hi
	lda	r30
	sta	reu_lo
	lda	r31
	sta	reu_hi
	sty	reu_bank
	stx	reu_cmd
	rts

	section	bss
___bankvoff:
	reserve	1
