; attiny45 register and bit definitions
ADCSRB:	.equ 0x03
	ADTS0:	.equ 0
	ADTS1:	.equ 1
	ADTS2:	.equ 2
	IPR:	.equ 5
	ACME:	.equ 6
	BIN:	.equ 7
ADCL:	.equ 0x04
ADCH:	.equ 0x05
ADCSRA:	.equ 0x06
	ADPS0:	.equ 0
	ADPS1:	.equ 1
	ADPS2:	.equ 2
	ADIE:	.equ 3
	ADIF:	.equ 4
	ADATE:	.equ 5
	ADSC:	.equ 6
	ADEN:	.equ 7
ADMUX:	.equ 0x07
	MUX0:	.equ 0
	MUX1:	.equ 1
	MUX2:	.equ 2
	MUX3:	.equ 3
	REFS2:	.equ 4
	ADLAR:	.equ 5
	REFS0:	.equ 6
	REFS1:	.equ 7
ACSR:	.equ 0x08
	ACIS0:	.equ 0
	ACIS1:	.equ 1
	ACIE:	.equ 3
	ACI:	.equ 4
	ACO:	.equ 5
	ACBG:	.equ 6
	ACD:	.equ 7
USICR:	.equ 0x0d
	USITC:	.equ 0
	USICLK:	.equ 1
	USICS0:	.equ 2
	USICS1:	.equ 3
	USIWM0:	.equ 4
	USIWM1:	.equ 5
	USIOIE:	.equ 6
	USISIE:	.equ 7
USISR:	.equ 0x0e
	USICNT0:.equ 0
	USICNT1:.equ 1
	USICNT2:.equ 2
	USICNT3:.equ 3
	USIDC:	.equ 4
	USIPF:	.equ 5
	USIOIF:	.equ 6
	USISIF:	.equ 7
USIDR:	.equ 0x0f
USIBR:	.equ 0x10
GPIOR0:	.equ 0x11
GPIOR1:	.equ 0x12
GPIOR2:	.equ 0x13
DIDR0:	.equ 0x14
	AIN0D:	.equ 0
	AIN1D:	.equ 1
	ADC1D:	.equ 2
	ADC3D:	.equ 3
	ADC2D:	.equ 4
	ADC0D:	.equ 5
PCMSK:	.equ 0x15
	PCINT0:	.equ 0
	PCINT1:	.equ 1
	PCINT2:	.equ 2
	PCINT3:	.equ 3
	PCINT4:	.equ 4
	PCINT5:	.equ 5
PINB:	.equ 0x16
	PINB0:	.equ 0
	PINB1:	.equ 1
	PINB2:	.equ 2
	PINB3:	.equ 3
	PINB4:	.equ 4
	PINB5:	.equ 5
DDRB:	.equ 0x17
	DDB0:	.equ 0
	DDB1:	.equ 1
	DDB2:	.equ 2
	DDB3:	.equ 3
	DDB4:	.equ 4
	DDB5:	.equ 5
PORTB:	.equ 0x18
	PORTB0:	.equ 0
	PORTB1:	.equ 1
	PORTB2:	.equ 2
	PORTB3:	.equ 3
	PORTB4:	.equ 4
	PORTB5:	.equ 5
EECR:	.equ 0x1c
	EERE:	.equ 0
	EEPE:	.equ 1
	EEMPE:	.equ 2
	EERIE:	.equ 3
	EEPM0:	.equ 4
	EEPM1:	.equ 5
EEDR:	.equ 0x1d
EEARL:	.equ 0x1e
	EEAR0:	.equ 0
	EEAR1:	.equ 1
	EEAR2:	.equ 2
	EEAR3:	.equ 3
	EEAR4:	.equ 4
	EEAR5:	.equ 5
	EEAR6:	.equ 6
	EEAR7:	.equ 7
EEARH:	.equ 0x1f
	EEAR8:	.equ 0
PRR:	.equ 0x20
	PRADC:	.equ 0
	PRUSI:	.equ 1
	PRTIM0:	.equ 2
	PRTIM1:	.equ 3
WDTCR:	.equ 0x21
	WDP0:	.equ 0
	WDP1:	.equ 1
	WDP2:	.equ 2
	WDE:	.equ 3
	WDCE:	.equ 4
	WDP3:	.equ 5
	WDIE:	.equ 6
	WDIF:	.equ 7
DWDR:	.equ 0x22
DTPS1:	.equ 0x23
	DTPS10:	.equ 0
	DTPS11:	.equ 1
DT1B:	.equ 0x24
	DT1BL0:	.equ 0
	DT1BL1:	.equ 1
	DT1BL2:	.equ 2
	DT1BL3:	.equ 3
	DT1BH0:	.equ 4
	DT1BH1:	.equ 5
	DT1BH2:	.equ 6
	DT1BH3:	.equ 7
DT1A:	.equ 0x25
	DT1AL0:	.equ 0
	DT1AL1:	.equ 1
	DT1AL2:	.equ 2
	DT1AL3:	.equ 3
	DT1AH0:	.equ 4
	DT1AH1:	.equ 5
	DT1AH2:	.equ 6
	DT1AH3:	.equ 7
CLKPR:	.equ 0x26
	CLKPS0:	.equ 0
	CLKPS1:	.equ 1
	CLKPS2:	.equ 2
	CLKPS3:	.equ 3
	CLKPCE:	.equ 7
PLLCSR:	.equ 0x27
	PLOCK:	.equ 0
	PLLE:	.equ 1
	PCKE:	.equ 2
	LSM:	.equ 7
OCR0B:	.equ 0x28
OCR0A:	.equ 0x29
TCCR0A:	.equ 0x2a
	WGM00:	.equ 0
	WGM01:	.equ 1
	COM0B0:	.equ 4
	COM0B1:	.equ 5
	COM0A0:	.equ 6
	COM0A1:	.equ 7
OCR1B:	.equ 0x2b
GTCCR:	.equ 0x2c
	PSR0:	.equ 0
	PSR1:	.equ 1
	FOC1A:	.equ 2
	FOC1B:	.equ 3
	COM1B0:	.equ 4
	COM1B1:	.equ 5
	PWM1B:	.equ 6
	TSM:	.equ 7
OCR1C:	.equ 0x2d
OCR1A:	.equ 0x2e
TCNT1:	.equ 0x2f
TCCR1:	.equ 0x30
	CS10:	.equ 0
	CS11:	.equ 1
	CS12:	.equ 2
	CS13:	.equ 3
	COM1A0:	.equ 4
	COM1A1:	.equ 5
	PWM1A:	.equ 6
	CTC1:	.equ 7
OSCCAL:	.equ 0x31
TCNT0:	.equ 0x32
TCCR0B:	.equ 0x33
	CS00:	.equ 0
	CS01:	.equ 1
	CS02:	.equ 2
	WGM02:	.equ 3
	FOC0B:	.equ 6
	FOC0A:	.equ 7
MCUSR:	.equ 0x34
	PORF:	.equ 0
	EXTRF:	.equ 1
	BORF:	.equ 2
	WDRF:	.equ 3
MCUCR:	.equ 0x35
	ISC00:	.equ 0
	ISC01:	.equ 1
	BODSE:	.equ 2
	SM0:	.equ 3
	SM1:	.equ 4
	SE:	.equ 5
	PUD:	.equ 6
	BODS:	.equ 7
SPMCSR:	.equ 0x37
	SPMEN:	.equ 0
	PGERS:	.equ 1
	PGWRT:	.equ 2
	RFLB:	.equ 3
	CTPB:	.equ 4
	RSIG:	.equ 5
TIFR:	.equ 0x38
	TOV0:	.equ 1
	TOV1:	.equ 2
	OCF0B:	.equ 3
	OCF0A:	.equ 4
	OCF1B:	.equ 5
	OCF1A:	.equ 6
TIMSK:	.equ 0x39
	TOIE0:	.equ 1
	TOIE1:	.equ 2
	OCIE0B:	.equ 3
	OCIE0A:	.equ 4
	OCIE1B:	.equ 5
	OCIE1A:	.equ 6
GIFR:	.equ 0x3a
	PCIF:	.equ 5
	INTF0:	.equ 6
GIMSK:	.equ 0x3b
	PCIE:	.equ 5
	INT0:	.equ 6
SPL:	.equ 0x3d
	SP0:	.equ 0
	SP1:	.equ 1
	SP2:	.equ 2
	SP3:	.equ 3
	SP4:	.equ 4
	SP5:	.equ 5
	SP6:	.equ 6
	SP7:	.equ 7
SPH:	.equ 0x3e
	SP8:	.equ 0
	SP9:	.equ 1
SREG:	.equ 0x3f
	C:	.equ 0
	Z:	.equ 1
	N:	.equ 2
	V:	.equ 3
	S:	.equ 4
	H:	.equ 5
	T:	.equ 6
	I:	.equ 7

; lock and fuse byte locations
FLB:	.equ 0x7000
	CKSEL0:		.equ 0
	CKSEL1:		.equ 1
	CKSEL2:		.equ 2
	CKSEL3:		.equ 3
	SUT0:		.equ 4
	SUT1:		.equ 5
	CKOUT:		.equ 6
	CKDIV8:		.equ 7
LOCKBITS:	.equ 0x7001
	LB0:		.equ 0
	LB1:		.equ 1
FEB:	.equ 0x7002
	SELFPRGEN:	.equ 0
FHB:	.equ 0x7003
	BODLEVEL0:	.equ 0
	BODLEVEL1:	.equ 1
	BODLEVEL2:	.equ 2
	EESAVE:		.equ 3
	WDTON:		.equ 4
	SPIEN:		.equ 5
	DWEN:		.equ 6
	RSTDISBL:	.equ 7

EEPROM:	.equ 0x4000

	.org 0
	rjmp RESET
	rjmp ?INT0 ? INT0 : .HANG
	rjmp ?PCINT0 ? PCINT0 : .HANG
	rjmp ?TIMER1_COMP_A ? TIMER1_COMP_A : .HANG
	rjmp ?TIMER1_OVF ? TIMER1_OVF : .HANG
	rjmp ?TIMER0_OVF ? TIMER0_OVF : .HANG
	rjmp ?EE_RDY ? EE_RDY : .HANG
	rjmp ?ANA_COMP ? ANA_COMP : .HANG
	rjmp ?ADC ? ADC : .HANG
	rjmp ?TIMER1_COMP_B ? TIMER1_COMP_B : .HANG
	rjmp ?TIMER0_COMP_A ? TIMER0_COMP_A : .HANG
	rjmp ?TIMER0_COMP_B ? TIMER0_COMP_B : .HANG
	rjmp ?WDT ? WDT : .HANG
	rjmp ?USI_START ? USI_START : .HANG
	rjmp ?USI_OVF ? USI_OVF : .HANG

.HANG:
	cli
	sleep
	rjmp .HANG