;	assumes that RAM is always non-banked

	zpage	r28
	zpage	r29
	zpage	r30
	zpage	r31

	global	___bankcopy
	global	___bankcopy_len
	section	text
___bankcopy:
	sta	retbank
        tya
        sta     switchTab,y
	ldy	#0
	ldx	___bankcopy_len+1
	beq	rest
loophi:
	lda	(r28),y
	sta	(r30),y
	iny
	bne	loophi
	inc	r29
	inc	r31
	dex
	bne	loophi

rest:
	ldx	___bankcopy_len
	beq	done
looplo:
	lda	(r28),y
	sta	(r30),y
	iny
	dex
	bne	looplo
done:
	lda	retbank
	tay
	sta     switchTab,y
	rts

	section	bss
retbank:
	reserve	1
___bankcopy_len:
	reserve	2

