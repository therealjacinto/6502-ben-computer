	include	"reu.i"

	global	___bankload
	section	text
___bankload:
	sta	reu_len
	stx	___bankvoff
	ldx	#129+16
	jmp	___bankdo


