;	assumes that RAM is always non-banked

	zpage	btmp0
	zpage	btmp1
	zpage	r0
	zpage	r1

	global	__fmemcpy
	section	text
__fmemcpy:
	sta	retbank
	lda	btmp1+2
        tay
        sta     switchTab,y
	ldy	#0
	ldx	r1
	beq	rest
loophi:
	lda	(btmp1),y
	sta	(btmp0),y
	iny
	bne	loophi
	inc	btmp0+1
	inc	btmp1+1
	dex
	bne	loophi

rest:
	ldx	r0
	beq	done
looplo:
	lda	(btmp1),y
	sta	(btmp0),y
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
