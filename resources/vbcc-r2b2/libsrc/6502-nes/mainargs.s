	zpage	r0
	zpage	r1
	zpage	r2
	zpage	r3

	global initmainargs

	section	rodata
pname:
	db	"n/a"
zero:
	db	0
argv:
	dw	pname
	dw	zero
	dw	0

	section text
initmainargs:
	ldx	#0
	stx	r1
	inx
	stx	r0
	lda	#<argv
	sta	r2
	lda	#>argv
	sta	r3
	rts
