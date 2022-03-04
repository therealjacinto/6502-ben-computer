;this file for FamiTone2 library generated by text2data tool

cavern2_music_data:
	byte 1
	word .instruments
	word .samples-3
	word .song0ch0,.song0ch1,.song0ch2,.song0ch3,.song0ch4,256,213

.instruments:
	byte $30 ;instrument $00
	word .env5,.env13,.env0
	byte $00
	byte $30 ;instrument $0e
	word .env2,.env14,.env0
	byte $00
	byte $30 ;instrument $14
	word .env2,.env14,.env0
	byte $00
	byte $30 ;instrument $15
	word .env6,.env15,.env0
	byte $00
	byte $30 ;instrument $18
	word .env7,.env13,.env0
	byte $00
	byte $30 ;instrument $22
	word .env9,.env16,.env0
	byte $00
	byte $30 ;instrument $23
	word .env10,.env16,.env0
	byte $00
	byte $b0 ;instrument $24
	word .env4,.env0,.env0
	byte $00
	byte $b0 ;instrument $25
	word .env3,.env0,.env0
	byte $00
	byte $b0 ;instrument $29
	word .env8,.env0,.env0
	byte $00
	byte $f0 ;instrument $2b
	word .env1,.env11,.env0
	byte $00
	byte $f0 ;instrument $2c
	word .env1,.env12,.env0
	byte $00

.samples:
.env0:
	byte $c0,$00,$00
.env1:
	byte $c8,$c8,$c9,$c9,$ca,$cb,$02,$cc,$08,$cb,$02,$ca,$05,$c9,$02,$c8
	byte $02,$c7,$05,$c6,$03,$c5,$04,$c4,$03,$c3,$03,$c2,$04,$c1,$03,$c0
	byte $00,$1f
.env2:
	byte $cf,$cb,$c8,$c7,$c6,$c6,$c3,$00,$06
.env3:
	byte $c4,$03,$c2,$15,$c1,$09,$c1,$00,$06
.env4:
	byte $c8,$02,$c7,$03,$c6,$03,$c5,$c5,$c4,$02,$c3,$04,$c2,$04,$c1,$09
	byte $c1,$00,$10
.env5:
	byte $cf,$ce,$cd,$cc,$cb,$ca,$c9,$c8,$c7,$c6,$c5,$c4,$c3,$c2,$c1,$c0
	byte $00,$0f
.env6:
	byte $cf,$00,$00
.env7:
	byte $cb,$06,$c8,$c7,$c6,$c5,$c4,$c3,$c2,$c1,$c0,$00,$0a
.env8:
	byte $cf,$cb,$c9,$c8,$c6,$c5,$c8,$c7,$c6,$c5,$c4,$c3,$c2,$00,$06
.env9:
	byte $c4,$c5,$c2,$c1,$c0,$00,$04
.env10:
	byte $c5,$c5,$c4,$c3,$c2,$04,$c1,$09,$c0,$00,$08
.env11:
	byte $c5,$c5,$c9,$c9,$c0,$c0,$c0,$00,$00
.env12:
	byte $c5,$c5,$c8,$c8,$c0,$c0,$c0,$00,$00
.env13:
	byte $d0,$cd,$c8,$c5,$c3,$c0,$00,$05
.env14:
	byte $c0,$02,$bd,$bb,$bb,$b8,$b8,$b4,$b4,$00,$08
.env15:
	byte $cc,$c0,$00,$01
.env16:
	byte $be,$c0,$00,$01


.song0ch0:
	byte $fb,$06
.ref0:
	byte $92,$2d,$2f,$35,$01,$35,$33,$3d,$3f,$3d,$25,$0d,$00,$91
.ref1:
	byte $33,$35,$39,$01,$39,$35,$33,$35,$3c,$85,$01,$3f,$3c,$85,$2b,$00
	byte $81
.ref2:
	byte $2d,$2f,$35,$01,$35,$33,$3d,$3f,$3d,$25,$0d,$00,$91
	byte $ff,$11
	word .ref1
.song0ch0loop:
.ref4:
	byte $92,$2d,$2f,$35,$96,$3d,$92,$35,$33,$3d,$3f,$3d,$25,$0d,$01,$96
	byte $32,$8d
.ref5:
	byte $92,$33,$35,$39,$94,$35,$92,$39,$35,$33,$35,$3c,$85,$94,$33,$92
	byte $3f,$3c,$85,$2b,$00,$81
	byte $fd
	word .song0ch0loop

.song0ch1:
.ref6:
	byte $8e,$46,$85,$90,$47,$8e,$54,$85,$90,$55,$8e,$55,$57,$55,$90,$57
	byte $55,$00,$91
.ref7:
	byte $8e,$50,$85,$90,$51,$8e,$4c,$85,$90,$4d,$8e,$4a,$85,$44,$85,$90
	byte $45,$8e,$4a,$85,$90,$4b,$8e,$4c,$85
.ref8:
	byte $46,$85,$90,$47,$8e,$54,$85,$90,$55,$8e,$55,$57,$55,$90,$57,$55
	byte $00,$91
	byte $ff,$10
	word .ref7
.song0ch1loop:
	byte $ff,$0d
	word .ref6
	byte $ff,$10
	word .ref7
	byte $fd
	word .song0ch1loop

.song0ch2:
.ref12:
	byte $86,$16,$9d,$24,$9d
.ref13:
	byte $20,$9d,$1c,$9d
.ref14:
	byte $16,$9d,$24,$9d
.ref15:
	byte $20,$9d,$1c,$8d,$80,$40,$40,$40,$00,$3a,$00,$2c,$00
.song0ch2loop:
.ref16:
	byte $82,$47,$86,$16,$85,$00,$89,$2e,$85,$82,$3c,$85,$00,$89,$86,$25
	byte $21,$1c,$81
.ref17:
	byte $82,$39,$86,$21,$38,$89,$01,$1c,$85,$82,$34,$85,$86,$2c,$85,$26
	byte $85,$24,$85
	byte $fd
	word .song0ch2loop

.song0ch3:
.ref18:
	byte $bf
.ref19:
	byte $bf
.ref20:
	byte $8a,$08,$85,$88,$09,$8a,$08,$85,$8c,$09,$84,$08,$85,$8a,$08,$85
	byte $88,$09,$8a,$08,$85,$8c,$09,$84,$09,$1a,$81
.ref21:
	byte $8a,$08,$85,$88,$09,$8a,$08,$85,$8c,$09,$84,$08,$85,$8a,$08,$85
	byte $88,$09,$8a,$08,$85,$8c,$09,$84,$09,$88,$08,$81
.song0ch3loop:
	byte $ff,$11
	word .ref20
	byte $ff,$11
	word .ref21
	byte $fd
	word .song0ch3loop

.song0ch4:
.ref24:
	byte $bf
.ref25:
	byte $bf
.ref26:
	byte $bf
.ref27:
	byte $bf
.song0ch4loop:
.ref28:
	byte $bf
.ref29:
	byte $bf
	byte $fd
	word .song0ch4loop