# Testing lda and sta via monitor
# code = bytearray([
#     0xa9, 0x42,      # lda with value 0x42
#     0x8d, 0x00, 0x60 # sta into address 0x6000
# ])

# Blink LEDs
code = bytearray([
    0xa9, 0xff,        # lda #$ff
    0x8d, 0x02, 0x60,  # sta $6002

    0xa9, 0x55,        # lda #$55
    0x8d, 0x00, 0x60,  # sta $6000

    0xa9, 0xaa,        # lda #$aa
    0x8d, 0x00, 0x60,  # sta $6000

    0x4c, 0x05, 0x80   # jmp $8005 (first write to PORTB)
])

rom = code + bytearray([0xea] * (32768 - len(code)))  # fill remaining space with no-op

rom[0x7ffc] = 0x00  # set start address to 0x8000
rom[0x7ffd] = 0x80

with open("rom.bin", "wb") as out_file:
    out_file.write(rom)
