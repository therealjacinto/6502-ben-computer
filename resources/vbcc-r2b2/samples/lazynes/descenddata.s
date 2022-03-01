;
	; --- sprites
	section "rodata"

	global _sprTab
	global _sprOff
	global _sprPal
_sprTab: incbin "descendspr.tab"
_sprOff: incbin "descendspr.off"
_sprPal: incbin "descendspr.pal"

	; --- background
	global _chrPal
_chrPal: incbin "descendchr.pal"

	; --- audio
	global _musicData
_musicData=cavern2_music_data
	include "cavern2.s"

	; --- CHRROM
	section "chars"

	incbin "descendchr.chr"
	incbin "descendspr.spr"
