; CHR/SPR for vbcc6502 NES stdlib support

	global	___stdchr
	section "chars"
___stdchr:
	incbin "stdchr.chr"
	incbin "stdspr.chr"
