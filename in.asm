	org 0x1000
	jp z, lab
	call lab
	jr nz, lab3
	ld a,3
	jp nc, lab4
lab:	ld b,e
lab3:
	halt
	jr lab3
lab4:
	jp 0x1234
