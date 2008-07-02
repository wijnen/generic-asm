	org 0x1000
	jp z, lab
	ld a,3
lab:	ld b,e
	halt
	jp 0x1234
