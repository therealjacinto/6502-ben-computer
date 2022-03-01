	include	"reu.i"

	global	___bankstore
	section	text
___bankstore:
	sta	reu_len
	stx	___bankvoff
	ldx	#128+16
	jmp	___bankdo


