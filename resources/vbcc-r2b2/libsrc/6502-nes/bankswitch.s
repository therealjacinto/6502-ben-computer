	global	___bankswitch
	zpage	r29
	zpage	r30
	zpage	r31

	section text
___bankswitch:
	tya
	sta	switchTab,y
	rts

