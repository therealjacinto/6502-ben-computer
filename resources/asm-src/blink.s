    .org $8000
reset:
    lda #$ff
    sta $6002

; Adding Improvements from python ROM
    lda #$50
    sta $60000

loop:
    ;lda #$55
    ;sta $6000

    ;lda #$aa
    ;sta $6000

; Adding improvements from python ROM
    ror
    sta $6000

    jmp loop

    .org $fffc
    .word reset
    .word $0000