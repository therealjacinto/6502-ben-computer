 global ___exit
 global _start
 section text
_start:
 tsx
 stx  oldsp
 lda  #<HIMEM
 sta  sp
 lda  #>HIMEM
 sta  sp+1
; erase bss
 lda #<__BB
 sta r0
 lda #>__BB
 sta r1
 lda #0
 tay
 jmp l3
l2:
 sta (r0),y
 inc r0
 bne l3
 inc r1
l3:
 ldx r0
 cpx #<__BE
 bne l2
 ldx r1
 cpx #>__BE
 bne l2
 jsr ___main
___exit:
 ldx oldsp
 txs
 lda r0
 rts

 section data
oldsp: byte 0

 section zpage
r0: reserve 1
r1: reserve 1
r2: reserve 1
r3: reserve 1
r4: reserve 1
r5: reserve 1
r6: reserve 1
r7: reserve 1
r8: reserve 1
r9: reserve 1
r10: reserve 1
r11: reserve 1
r12: reserve 1
r13: reserve 1
r14: reserve 1
r15: reserve 1
r16: reserve 1
r17: reserve 1
r18: reserve 1
r19: reserve 1
r20: reserve 1
r21: reserve 1
r22: reserve 1
r23: reserve 1
r24: reserve 1
r25: reserve 1
r26: reserve 1
r27: reserve 1
r28: reserve 1
r29: reserve 1
r30: reserve 1
r31: reserve 1
sp: reserve 2

 global r0
 global r1
 global r2
 global r3
 global r4
 global r5
 global r6
 global r7
 global r8
 global r9
 global r10
 global r11
 global r12
 global r13
 global r14
 global r15
 global r16
 global r17
 global r18
 global r19
 global r20
 global r21
 global r22
 global r23
 global r24
 global r25
 global r26
 global r27
 global r28
 global r29
 global r30
 global r31
 global sp





