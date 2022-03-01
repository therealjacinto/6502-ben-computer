	include	"reu.i"
	zpage	r29

	global	___bankswitch
	section	text
___bankswitch:
	lda	#0
	sta	reu_len
	sta	reu_c64lo
	sta	reu_lo
	sta	r29
	lda	#$80
	sta	reu_c64hi
	lda	#$40
	sta	reu_len+1
	lda	curbank
	cmp	#255
	beq	nostore
	lsr
	ror	r29
	lsr
	ror	r29
	sta	reu_bank
	lda	r29
	sta	reu_hi
	lda	#128+32+16
	sta	reu_cmd
nostore:
	lda	#0
	sta	r29
	sty	curbank
	tya
	lsr
	ror	r29
	lsr
	ror	r29
	sta	reu_bank
	lda	r29
	sta	reu_hi
	lda	#129+16
	sta	reu_cmd
	rts
	section	data
curbank:
	byte	255
