;vcprmin=10000
	section	text
	global	_GetMapData
_GetMapData:
	sec
	lda	sp
	sbc	#15
	sta	sp
	bcs	l47
	dec	sp+1
l47:
	ldy	#14
	jsr	___rsave12
	lda	r1
	sta	r9
	lda	r0
	sta	r8
	lda	r2
	cmp	l15
	beq	l43
	lda	r2
	sta	l15
	lda	r2
	and	#1
	sta	r0
	ldx	#0
	lda	r0
	cpx	#0
	bne	l48
	cmp	#0
	beq	l19
l48:
	ldx	#16
	jmp	l28
l19:
	ldx	#0
	inc	l11
	lda	l11
	cmp	#24
	bcc	l22
	lda	#0
	sta	l11
l22:
	inc	l13
	lda	l13
	cmp	#20
	bcc	l24
	lda	#0
	sta	l13
l24:
	inc	l12
	lda	l12
	cmp	#28
	bcc	l26
	lda	#0
	sta	l12
l26:
	inc	l14
	lda	l14
	cmp	#30
	bcc	l28
	lda	#0
	sta	l14
l28:
	lda	#0
	sta	r0
	lda	#<(l9)
	clc
	adc	l12
	sta	r10
	lda	#>(l9)
	adc	#0
	sta	r11
	lda	#<(l10)
	clc
	adc	l14
	sta	r14
	lda	#>(l10)
	adc	#0
	sta	r15
	lda	r8
	clc
	adc	#24
	sta	r26
	lda	r9
	adc	#0
	sta	r27
	lda	#<(l7)
	clc
	adc	l11
	sta	r16
	lda	#>(l7)
	adc	#0
	sta	r17
	lda	r8
	clc
	adc	#8
	sta	r24
	lda	r9
	adc	#0
	sta	r25
	lda	#<(l8)
	clc
	adc	l13
	sta	r12
	lda	#>(l8)
	adc	#0
	sta	r13
	lda	r8
	clc
	adc	#20
	ldy	#1
	sta	(sp),y
	lda	r9
	adc	#0
	iny
	sta	(sp),y
	lda	r2
	and	#3
	sta	r31
	ldx	#0
	sta	r22
	stx	r23
	lda	r8
	clc
	adc	#12
	sta	r20
	lda	r9
	adc	#0
	sta	r21
	lda	r8
	clc
	adc	#12
	sta	r18
	lda	r9
	adc	#0
	sta	r19
	lda	#32
	sta	r4
	lda	#48
	sta	r1
	lda	r4
	ldy	#0
	sta	(sp),y
	sta	r6
l41:
	lda	#<(l5)
	clc
	ldy	#0
	adc	(r10),y
	sta	r2
	lda	#>(l5)
	adc	#0
	sta	r3
	txa
	clc
	adc	(r2),y
	clc
	adc	r0
	ldy	r0
	sta	(r8),y ;am(r0)
	lda	#<(l6)
	clc
	ldy	#0
	adc	(r14),y
	sta	r2
	lda	#>(l6)
	adc	#0
	sta	r3
	txa
	clc
	adc	(r2),y
	clc
	adc	r0
	sta	r31
	lda	#0
	sta	r5
	lda	r0
	sta	r4
	lda	r31
	ldy	r0
	sta	(r26),y ;am(r0)
	lda	r0
	cmp	#4
	bcs	l34
	lda	#<(l3)
	clc
	ldy	#0
	adc	(r16),y
	sta	r2
	lda	#>(l3)
	adc	#0
	sta	r3
	txa
	clc
	adc	(r2),y
	clc
	adc	r0
	sta	r31
	lda	r24
	clc
	adc	r4
	sta	r2
	lda	r25
	adc	r5
	sta	r3
	lda	r31
	sta	(r2),y
	lda	#<(l4)
	clc
	adc	(r12),y
	sta	r2
	lda	#>(l4)
	adc	#0
	sta	r3
	txa
	clc
	adc	(r2),y
	clc
	adc	r0
	sta	r31
	iny
	lda	(sp),y
	clc
	adc	r4
	sta	r2
	iny
	lda	(sp),y
	adc	r5
	sta	r3
	lda	r31
	ldy	#0
	sta	(r2),y
l34:
	lda	r23
	bne	l36
	lda	r22
	bne	l36
	lda	r20
	clc
	adc	r4
	sta	r2
	lda	r21
	adc	r5
	sta	r3
	lda	r1
	ldy	#0
	sta	(r2),y
	jmp	l37
l36:
	lda	r6
	ldy	r0
	sta	(r18),y ;am(r0)
l37:
	inc	r0
	inc	r1
	inc	r6
	lda	r0
	cmp	#8
	bcc	l41
l43:
	ldy	#14
	jsr	___rload12
	clc
	lda	sp
	adc	#15
	sta	sp
	bcc	l49
	inc	sp+1
l49:
	rts
; stacksize=0+??
	section	rodata
l3:
	byte	64
	byte	68
	byte	96
	byte	100
	section	rodata
l4:
	byte	76
	byte	72
	byte	108
	byte	104
	section	rodata
l5:
	byte	128
	byte	160
	byte	192
	byte	224
	section	rodata
l6:
	byte	136
	byte	168
	byte	200
	byte	232
	section	rodata
l7:
	byte	0
	byte	2
	byte	1
	byte	2
	byte	3
	byte	1
	byte	3
	byte	2
	byte	0
	byte	3
	byte	2
	byte	0
	byte	1
	byte	0
	byte	1
	byte	0
	byte	3
	byte	0
	byte	1
	byte	2
	byte	0
	byte	1
	byte	0
	byte	3
	section	rodata
l8:
	byte	1
	byte	2
	byte	3
	byte	2
	byte	0
	byte	1
	byte	2
	byte	0
	byte	1
	byte	2
	byte	3
	byte	0
	byte	1
	byte	3
	byte	2
	byte	0
	byte	1
	byte	2
	byte	0
	byte	3
	section	rodata
l9:
	byte	0
	byte	1
	byte	2
	byte	3
	byte	3
	byte	2
	byte	0
	byte	1
	byte	1
	byte	0
	byte	0
	byte	0
	byte	2
	byte	1
	byte	1
	byte	0
	byte	0
	byte	2
	byte	3
	byte	3
	byte	2
	byte	0
	byte	0
	byte	0
	byte	2
	byte	1
	byte	2
	byte	1
	section	rodata
l10:
	byte	1
	byte	0
	byte	1
	byte	2
	byte	3
	byte	2
	byte	1
	byte	0
	byte	0
	byte	0
	byte	1
	byte	2
	byte	3
	byte	3
	byte	2
	byte	1
	byte	0
	byte	0
	byte	1
	byte	0
	byte	1
	byte	0
	byte	0
	byte	0
	byte	2
	byte	3
	byte	3
	byte	2
	byte	1
	byte	0
	section	data
l11:
	byte	0
	section	data
l12:
	byte	0
	section	data
l13:
	byte	0
	section	data
l14:
	byte	0
	section	data
l15:
	byte	255
;vcprmin=10000
	section	text
	global	_main
_main:
	sec
	lda	sp
	sbc	#32
	sta	sp
	bcs	l247
	dec	sp+1
l247:
	ldy	#31
	jsr	___rsave8
	ldy	#13
	lda	(sp),y
	sta	r23
	dey
	lda	(sp),y
	sta	r22
	ldy	#14
	lda	(sp),y
	sta	r21
	lda	#0
	ldy	#3
	sta	(sp),y
	dey
	sta	(sp),y
	lda	#1
	jsr	_lnSync
	sta	r31
	and	#128
	sta	r16
	ldx	#63
	lda	#0
	sta	r31
	lda	#8
	sta	r0
	lda	#>(_chrPal)
	sta	r3
	lda	#<(_chrPal)
	sta	r2
	lda	r31
	jsr	_lnPush
	ldx	#63
	lda	#17
	sta	r31
	lda	#3
	sta	r0
	lda	#>(1+_sprPal)
	sta	r3
	lda	#<(1+_sprPal)
	sta	r2
	lda	r31
	jsr	_lnPush
	ldx	#63
	lda	#21
	sta	r31
	lda	#3
	sta	r0
	lda	#>(5+_sprPal)
	sta	r3
	lda	#<(5+_sprPal)
	sta	r2
	lda	r31
	jsr	_lnPush
	ldx	#63
	lda	#25
	sta	r31
	lda	#3
	sta	r0
	lda	#>(9+_sprPal)
	sta	r3
	lda	#<(9+_sprPal)
	sta	r2
	lda	r31
	jsr	_lnPush
	lda	#>(_musicData)
	sta	r1
	lda	#<(_musicData)
	sta	r0
	lda	r16
	jsr	_ftInit
	lda	#0
	sta	r20
l201:
	lda	#0
	sta	r7
	lda	r20
	sta	r6
	asl	r6
	rol	r7
	asl	r6
	rol	r7
	asl	r6
	rol	r7
	lda	r6
	clc
	adc	#192
	pha
	lda	r7
	adc	#35
	tax
	pla
	sta	r31
	lda	#8
	sta	r0
	lda	#>(l52)
	sta	r3
	lda	#<(l52)
	sta	r2
	lda	r31
	jsr	_lnPush
	inc	r20
	lda	r20
	cmp	#8
	bcc	l201
l208:
	lda	#0
	ldy	#5
	sta	(sp),y
	dey
	sta	(sp),y
	lda	#>(l61)
	sta	r17
	lda	#<(l61)
	sta	r16
	dey
	lda	(sp),y
	sta	r19
	dey
	lda	(sp),y
	sta	r18
	lda	r19
	bne	l64
	lda	r18
	bne	l64
	lda	#0
	ldy	#9
	sta	(sp),y
	lda	#16
	dey
	sta	(sp),y
	lda	#0
	ldy	#11
	sta	(sp),y
	lda	#4
	dey
	sta	(sp),y
	lda	#0
	sta	r22
	sta	r23
	sta	r21
	ldy	#16
	sta	(sp),y
	dey
	sta	(sp),y
	lda	#255
	ldy	#17
	sta	(sp),y
	lda	#0
	jsr	_ftMusicPlay
l64:
	ldy	#2
	lda	(sp),y
	sta	r12
	and	#3
	sta	r31
	ldx	#0
	sta	r14
	stx	r15
	txa
	bne	l66
	lda	r14
	bne	l66
	lda	r18
	cmp	#216
	lda	r19
	sbc	#9
	bvc	l248
	eor	#128
l248:
	bmi	l68
	lda	r23
	cmp	#30
	bcc	l79
	lda	#0
	sta	r23
	jmp	l79
l68:
	lda	r18
	cmp	#96
	lda	r19
	sbc	#9
	bvc	l249
	eor	#128
l249:
	bpl	l79
	lda	r18
	cmp	#64
	lda	r19
	sbc	#1
	bvc	l250
	eor	#128
l250:
	bmi	l75
	lda	r12
	and	#7
	sta	r31
	ldx	#0
	sta	r6
	stx	r7
	txa
	bne	l251
	lda	r6
	beq	l72
l251:
l75:
	lda	r18
	cmp	#8
	lda	r19
	sbc	#2
	bvc	l252
	eor	#128
l252:
	bmi	l79
	lda	r15
	bne	l79
	lda	r14
	bne	l79
l72:
	ldy	#15
	lda	(sp),y
	clc
	adc	#1
	sta	(sp),y
	iny
	lda	(sp),y
	adc	#0
	sta	(sp),y
	inc	r21
	lda	r21
	cmp	#240
	bcc	l79
	lda	r21
	sec
	sbc	#240
	sta	r21
l79:
	lda	r12
	and	#7
	sta	r31
	ldx	#0
	sta	r6
	stx	r7
	txa
	bne	l83
	lda	r6
	bne	l83
	ldy	#10
	lda	(sp),y
	clc
	adc	#1
	sta	(sp),y
	iny
	lda	(sp),y
	adc	#0
	sta	(sp),y
	sta	r7
	dey
	lda	(sp),y
	sta	r6
	lda	r7
	cmp	#0
	bcc	l83
	bne	l253
	lda	r6
	cmp	#16
	bcc	l83
	beq	l83
l253:
	lda	#0
	ldy	#11
	sta	(sp),y
	lda	#16
	dey
	sta	(sp),y
l83:
	ldy	#11
	lda	(sp),y
	sta	r7
	dey
	lda	(sp),y
	sta	r6
	ldy	#8
	lda	(sp),y
	clc
	adc	r6
	sta	(sp),y
	iny
	lda	(sp),y
	adc	r7
	sta	(sp),y
l66:
	lda	r23
	cmp	#30
	bcs	l85
	lda	r12
	and	#1
	sta	r31
	ldx	#0
	sta	r6
	stx	r7
	txa
	bne	l214
	lda	r6
	bne	l214
	ldx	#1
	lda	r22
	bne	l89
	lda	r18
	cmp	#96
	lda	r19
	sbc	#9
	bvc	l254
	eor	#128
l254:
	bpl	l89
	ldx	#2
l89:
	lda	#0
	sta	r10
	cpx	#1
	bcc	l210
	lda	r23
	ldy	#20
	sta	(sp),y
	lda	r21
	clc
	ror
	clc
	ror
	clc
	ror
	iny
	sta	(sp),y
	lda	r22
	sta	r31
	asl
	iny
	sta	(sp),y
l203:
	lda	r10
	sta	r31
	ldy	#20
	lda	(sp),y
	sec
	sbc	r31
	sta	r8
	lda	r10
	bne	l96
	lda	r8
	sta	r31
	lda	#>(l62)
	sta	r1
	lda	#<(l62)
	sta	r0
	lda	r31
	sta	r2
	lda	r19
	ldy	#7
	sta	(sp),y
	lda	r18
	dey
	sta	(sp),y
	lda	r17
	ldy	#1
	sta	(sp),y
	lda	r16
	dey
	sta	(sp),y
	lda	r23
	ldy	#13
	sta	(sp),y
	lda	r22
	dey
	sta	(sp),y
	lda	r21
	ldy	#14
	sta	(sp),y
	lda	r10
	ldy	#19
	sta	(sp),y
	lda	r8
	ldy	#23
	sta	(sp),y
	txa
	ldy	#18
	sta	(sp),y
	jsr	_GetMapData
	ldy	#7
	lda	(sp),y
	sta	r19
	dey
	lda	(sp),y
	sta	r18
	ldy	#1
	lda	(sp),y
	sta	r17
	dey
	lda	(sp),y
	sta	r16
	ldy	#13
	lda	(sp),y
	sta	r23
	dey
	lda	(sp),y
	sta	r22
	ldy	#14
	lda	(sp),y
	sta	r21
	ldy	#19
	lda	(sp),y
	sta	r10
	ldy	#23
	lda	(sp),y
	sta	r8
	ldy	#18
	lda	(sp),y
	tax
l96:
	lda	r8
	clc
	ldy	#21
	adc	(sp),y
	sta	r8
	sec
	sbc	#30
	bvc	l255
	eor	#128
l255:
	bmi	l98
	lda	r8
	sec
	sbc	#30
	sta	r8
l98:
	lda	r8
	sec
	sbc	#0
	bvc	l256
	eor	#128
l256:
	bmi	l212
	lda	r8
	sec
	sbc	#30
	bvc	l257
	eor	#128
l257:
	bpl	l212
	stx	r31
	ldx	#0
	lda	r8
	bpl	l258
	dex
l258:
	sta	r6
	stx	r7
	ldx	r31
	stx	r30
	ldx	r7
	lda	r6
	stx	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	ldx	r31
	sta	r6
	stx	r7
	ldx	r30
	lda	r6
	clc
	adc	#0
	ldy	#4
	sta	(sp),y
	lda	r7
	adc	#32
	iny
	sta	(sp),y
	stx	r30
	lda	(sp),y
	ldx	#0
	stx	r31
	sta	r6
	stx	r7
	ldx	r30
	lda	r7
	ora	#0
	sta	r7
	lda	r6
	ora	#64
	sta	r6
	ldy	#0
	sta	(r16),y
	inc	r16
	bne	l259
	inc	r17
l259:
	ldy	#5
	lda	(sp),y
	sta	r7
	dey
	lda	(sp),y
	sta	r6
	lda	#0
	sta	r7
	lda	r6
	and	#255
	sta	r6
	ldy	#0
	sta	(r16),y
	inc	r16
	bne	l260
	inc	r17
l260:
	lda	#32
	sta	(r16),y
	inc	r16
	bne	l261
	inc	r17
l261:
	lda	#0
	sta	r1
	txa
	ldy	#18
	sta	(sp),y
	lda	r1
	ldy	#0
	sta	(sp),y
	lda	r10
	beq	l205
	ldy	#0
	lda	(sp),y
	tax
l197:
	lda	0+l62,x ;am(x)
	ldy	#0
	sta	(r16),y
	inc	r16
	bne	l262
	inc	r17
l262:
	inx
	cpx	#32
	bcc	l197
	ldy	#18
	lda	(sp),y
	tax
	jmp	l212
l205:
	lda	#0
	sta	r7
	lda	r1
	sta	r6
	asl	r6
	rol	r7
	lda	r6
	clc
	adc	#<(l62)
	sta	r6
	lda	r7
	adc	#>(l62)
	sta	r7
	lda	r6
	sta	r30
	lda	r7
	sta	r31
	lda	#0
	sta	r7
	ldy	#0
	lda	(r30),y
	sta	r6
	lda	r7
	cmp	#128
	ror	r7
	ror	r6
	lda	r6
	clc
	adc	#<(_fTab)
	sta	r6
	lda	r7
	adc	#>(_fTab)
	sta	r7
	lda	(r6),y
	sta	r3
	lda	r22
	sec
	sbc	#0
	bvc	l263
	eor	#128
l263:
	bpl	l114
	lda	#0
	sta	r3
	jmp	l117
l114:
	lda	r3
	beq	l117
	lda	r3
	clc
	ldy	#22
	adc	(sp),y
	sta	r3
l117:
	lda	r3
	ldy	#0
	sta	(r16),y
	inc	r16
	bne	l264
	inc	r17
l264:
	lda	r3
	clc
	adc	#1
	sta	(r16),y
	inc	r16
	bne	l265
	inc	r17
l265:
	inc	r1
	lda	r1
	cmp	#16
	bcc	l205
l212:
	inc	r10
	cpx	r10
;
	beq	l266
	bcs	l203
l266:
l210:
	lda	r18
	cmp	#96
	lda	r19
	sbc	#9
	bvc	l267
	eor	#128
l267:
	bpl	l119
	inc	r22
	lda	r22
	sec
	sbc	#3
	bvc	l268
	eor	#128
l268:
	bmi	l214
	inc	r23
	lda	#0
	sta	r22
	jmp	l214
l119:
	dec	r22
	lda	r22
	sec
	sbc	#255
	bvc	l269
	eor	#128
l269:
	bpl	l124
	inc	r23
	lda	#2
	sta	r22
l124:
	lda	r23
	cmp	#30
	bcc	l214
	lda	#255
	ldy	#3
	sta	(sp),y
	dey
	sta	(sp),y
	jmp	l214
l85:
	lda	r21
	and	#7
	sta	r31
	ldx	#0
	sta	r6
	stx	r7
	txa
	bne	l214
	lda	r6
	bne	l214
	ldy	#17
	lda	(sp),y
	cmp	r21
	beq	l214
	lda	r18
	cmp	#216
	lda	r19
	sbc	#9
	bvc	l270
	eor	#128
l270:
	bpl	l214
	lda	r21
	clc
	ror
	clc
	ror
	clc
	ror
	sta	r11
	lda	r21
	ldy	#17
	sta	(sp),y
	lda	#0
	sta	r7
	lda	r11
	sta	r6
	stx	r30
	ldx	r7
	lda	r6
	stx	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	ldx	r31
	sta	r6
	stx	r7
	ldx	r30
	lda	r6
	clc
	adc	#0
	sta	r6
	lda	r7
	adc	#32
	sta	r7
	stx	r30
	lda	r7
	ldx	#0
	stx	r31
	sta	r8
	stx	r9
	ldx	r30
	lda	r9
	ora	#0
	sta	r9
	lda	r8
	ora	#64
	sta	r8
	lda	#>(1+l61)
	sta	r17
	lda	#<(1+l61)
	sta	r16
	lda	r8
	sta	l61
	lda	#0
	sta	r7
	lda	r6
	and	#255
	sta	r6
	ldy	#0
	sta	(r16),y
	lda	#32
	sta	2+l61
	lda	#>(3+l61)
	sta	r17
	lda	#<(3+l61)
	sta	r16
	lda	#0
	sta	r10
	ldy	#19
	sta	(sp),y
	tax
l199:
	lda	#0
	tay
	sta	(r16),y
	inc	r16
	bne	l271
	inc	r17
l271:
	inx
	cpx	#32
	bcc	l199
	lda	r11
	bne	l137
	lda	#29
	sta	r11
	jmp	l138
l137:
	dec	r11
l138:
	lda	#0
	sta	r7
	lda	r11
	sta	r6
	stx	r30
	ldx	r7
	lda	r6
	stx	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	asl
	rol	r31
	ldx	r31
	sta	r6
	stx	r7
	ldx	r30
	lda	r6
	clc
	adc	#0
	ldy	#4
	sta	(sp),y
	lda	r7
	adc	#32
	iny
	sta	(sp),y
	stx	r30
	lda	(sp),y
	ldx	#0
	stx	r31
	sta	r6
	stx	r7
	ldx	r30
	lda	r7
	ora	#0
	sta	r7
	lda	r6
	ora	#64
	sta	r6
	ldy	#0
	sta	(r16),y
	inc	r16
	bne	l272
	inc	r17
l272:
	ldy	#5
	lda	(sp),y
	sta	r7
	dey
	lda	(sp),y
	sta	r6
	lda	#0
	sta	r7
	lda	r6
	and	#255
	sta	r6
	ldy	#0
	sta	(r16),y
	inc	r16
	bne	l273
	inc	r17
l273:
	lda	#32
	sta	(r16),y
	inc	r16
	bne	l274
	inc	r17
l274:
	stx	r30
	ldy	#16
	lda	(sp),y
	tax
	dey
	lda	(sp),y
	stx	r31
	clc
	ror	r31
	ror
	clc
	ror	r31
	ror
	clc
	ror	r31
	ror
	ldx	r31
	sta	r6
	stx	r7
	ldx	r30
	lda	r6
	clc
	adc	#30
	sta	r6
	bcc	l275
	inc	r6+1
l275:
	lda	r6
	sta	r31
	lda	#>(l62)
	sta	r1
	lda	#<(l62)
	sta	r0
	lda	r31
	sta	r2
	lda	r17
	ldy	#1
	sta	(sp),y
	lda	r16
	dey
	sta	(sp),y
	lda	r23
	ldy	#13
	sta	(sp),y
	lda	r22
	dey
	sta	(sp),y
	lda	r21
	ldy	#14
	sta	(sp),y
	jsr	_GetMapData
	ldy	#1
	lda	(sp),y
	sta	r17
	dey
	lda	(sp),y
	sta	r16
	ldy	#13
	lda	(sp),y
	sta	r23
	dey
	lda	(sp),y
	sta	r22
	ldy	#14
	lda	(sp),y
	sta	r21
	lda	#0
	sta	r10
	ldy	#19
	sta	(sp),y
	tax
l200:
	lda	0+l62,x ;am(x)
	ldy	#0
	sta	(r16),y
	inc	r16
	bne	l276
	inc	r17
l276:
	inx
	cpx	#32
	bcc	l200
l214:
	ldy	#5
	lda	(sp),y
	bne	l277
	dey
	lda	(sp),y
	beq	l144
l277:
	lda	#255
	ldy	#0
	sta	(r16),y
	ldx	#>(l61)
	lda	#<(l61)
	jsr	_lnList
l144:
	stx	r30
	ldy	#9
	lda	(sp),y
	tax
	dey
	lda	(sp),y
	stx	r31
	cpx	#128
	ror	r31
	ror
	cpx	#128
	ror	r31
	ror
	cpx	#128
	ror	r31
	ror
	cpx	#128
	ror	r31
	ror
	ldx	r31
	sta	r6
	stx	r7
	ldx	r30
	ldy	#16
	lda	(sp),y
	sta	r5
	dey
	lda	(sp),y
	sta	r4
	lda	r6
	sec
	sbc	r4
	sta	r4
	lda	r7
	sbc	r5
	sta	r5
	lda	#>(_sprTab)
	sta	r1
	lda	#<(_sprTab)
	sta	r0
	lda	#0
	sta	r3
	lda	#96
	sta	r2
	jsr	_lnAddSpr
	ldy	#2
	lda	(sp),y
	clc
	adc	#1
	sta	(sp),y
	iny
	lda	(sp),y
	adc	#0
	sta	(sp),y
	lda	#0
	sta	r3
	lda	r21
	sta	r2
	lda	#0
	sta	r1
	sta	r0
	jsr	_lnScroll
	lda	#0
	jsr	_lnSync
	jmp	l208
	sta	r31
	ldy	#31
	jsr	___rload8
	clc
	lda	sp
	adc	#32
	sta	sp
	bcc	l278
	inc	sp+1
l278:
	lda	r31
	rts
; stacksize=0+??
	section	rodata
l52:
	byte	0
	byte	0
	byte	0
	byte	85
	byte	85
	byte	0
	byte	0
	byte	0
	section	bss
l61:
	reserve	71
	section	bss
l62:
	reserve	32
	global	_fTab
	section	rodata
_fTab:
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	8
	byte	14
	byte	14
	byte	14
	byte	14
	byte	14
	byte	14
	byte	8
	byte	8
	byte	14
	byte	14
	byte	14
	byte	14
	byte	14
	byte	14
	byte	8
	byte	8
	byte	14
	byte	8
	byte	14
	byte	14
	byte	8
	byte	14
	byte	8
	byte	8
	byte	14
	byte	8
	byte	14
	byte	14
	byte	8
	byte	14
	byte	8
	byte	0
	byte	0
	byte	2
	byte	8
	byte	8
	byte	2
	byte	0
	byte	0
	byte	0
	byte	0
	byte	0
	byte	2
	byte	2
	byte	0
	byte	0
	byte	0
	byte	0
	byte	2
	byte	8
	byte	8
	byte	8
	byte	8
	byte	2
	byte	0
	byte	0
	byte	2
	byte	2
	byte	8
	byte	8
	byte	2
	byte	2
	byte	0
	byte	2
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	2
	byte	2
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	2
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	byte	8
	global	_lnSync
	global	_lnPush
	global	_lnList
	global	_lnScroll
	global	_lnAddSpr
	global	_ftInit
	global	_ftMusicPlay
	global	_chrPal
	global	_sprPal
	global	_sprTab
	global	_musicData
	zpage	sp
	zpage	r0
	zpage	r1
	zpage	r2
	zpage	r3
	zpage	r4
	zpage	r5
	zpage	r6
	zpage	r7
	zpage	r8
	zpage	r9
	zpage	r10
	zpage	r11
	zpage	r12
	zpage	r13
	zpage	r14
	zpage	r15
	zpage	r16
	zpage	r17
	zpage	r18
	zpage	r19
	zpage	r20
	zpage	r21
	zpage	r22
	zpage	r23
	zpage	r24
	zpage	r25
	zpage	r26
	zpage	r27
	zpage	r28
	zpage	r29
	zpage	r30
	zpage	r31
	zpage	btmp0
	zpage	btmp1
	zpage	btmp2
	zpage	btmp3
