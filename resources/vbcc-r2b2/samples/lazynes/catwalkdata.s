; data

	section "rodata"
	
	; --- sprites
	global _roboOff
	global _roboTab
	global _roboPal
_roboOff:
	incbin "robos1-nes.off"
	incbin "robos2-nes.off"
_roboTab:
	incbin "robos1-nes.tab"
	incbin "robos2-nes.tab"
_roboPal:
	incbin "robos1-nes.pal"

	; --- background
	global _backPal
	global _backCol
_backPal:
	incbin "catwalkchr.pal"
_backCol:
	incbin "catwalkchr.col"

	; --- audio
	global _musicData
	global _soundData
_musicData=powerglove5b_music_data
	include "powerglove5b.s"
_soundData=sounds
	include "sounds.s"

	; --- CHRROM
	section "chars"

	incbin "catwalkchr.chr"
	incbin "robos1-nes.spr"
	incbin "robos2-nes.spr"
