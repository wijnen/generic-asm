	org 0x1000
	call lab
	jp z, lab
	jr nz, lab
	ld a,3
lab:	ld b,e
lab3:
	halt
	jr lab3
	jp 0x1234
