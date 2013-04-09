; attiny828 register and bit definitions
PINA:	.equ 0x00
DDRA:	.equ 0x01
PORTA:	.equ 0x02
PUEA:	.equ 0x03
PINB:	.equ 0x04
DDRB:	.equ 0x05
PORTB:	.equ 0x06
PUEB:	.equ 0x07
PINC:	.equ 0x08
DDRC:	.equ 0x09
PORTC:	.equ 0x0a
PUEC:	.equ 0x0b
PIND:	.equ 0x0c
DDRD:	.equ 0x0d
PORTD:	.equ 0x0e
PUED:	.equ 0x0f

PHDE:	.equ 0x14
	PHDEC:	.equ 2
TIFR0:	.equ 0x15
	TOV0:	.equ 0
	OCF0A:	.equ 1
	OCF0B:	.equ 2
TIFR1:	.equ 0x16
	TOV1:	.equ 0
	OCF1A:	.equ 1
	OCF1B:	.equ 2
	ICF1:	.equ 5

PCIFR:	.equ 0x1b
	PCIF0:	.equ 0
	PCIF1:	.equ 1
	PCIF2:	.equ 2
	PCIF3:	.equ 3
EIFR:	.equ 0x1c
	INTF0:	.equ 0
	INTF1:	.equ 1
EIMSK:	.equ 0x1d
	INT0:	.equ 0
	INT1:	.equ 1
GPIOR0:	.equ 0x1e
EECR:	.equ 0x1f
	EERE:	.equ 0
	EEPE:	.equ 1
	EEMPE:	.equ 2
	EERIE:	.equ 3
	EEPM0:	.equ 4
	EEPM1:	.equ 5
EEDR:	.equ 0x20
EEARL:	.equ 0x21

GTCCR:	.equ 0x23
	PSR:	.equ 0
	TSM:	.equ 7
TCCR0A:	.equ 0x24
	WGM00:	.equ 0
	WGM01:	.equ 1
	COM0B0:	.equ 4
	COM0B1:	.equ 5
	COM0A0:	.equ 6
	COM0A1:	.equ 7
TCCR0B:	.equ 0x25
	CS00:	.equ 0
	CS01:	.equ 1
	CS02:	.equ 2
	WGM02:	.equ 3
	FOC0B:	.equ 6
	FOC0A:	.equ 7
TCNT0:	.equ 0x26
OCR0A:	.equ 0x27
OCR0B:	.equ 0x28

GPIOR1:	.equ 0x2a
GPIOR2:	.equ 0x2b
SPCR:	.equ 0x2c
	SPR0:	.equ 0
	SPR1:	.equ 1
	CPHA:	.equ 2
	CPOL:	.equ 3
	MSTR:	.equ 4
	DORD:	.equ 5
	SPE:	.equ 6
	SPIE:	.equ 7
SPSR:	.equ 0x2d
	SPI2X:	.equ 0
	WCOL:	.equ 6
	SPIF:	.equ 7
SPDR:	.equ 0x2e
ACSRB:	.equ 0x2f
	ACPMUX0:	.equ 0
	ACPMUX1:	.equ 1
	ACNMUX0:	.equ 2
	ACNMUX1:	.equ 3
	ACLR:	.equ 5
	HLEV:	.equ 6
	HSEL:	.equ 7
ACSRA:	.equ 0x30
	ACIS0:	.equ 0
	ACIS1:	.equ 1
	ACIC:	.equ 2
	ACIE:	.equ 3
	ACI:	.equ 4
	ACO:	.equ 5
	ACPMUX2:	.equ 6
	ACD:	.equ 7
DWDR:	.equ 0x31

SMCR:	.equ 0x33
	SE:	.equ 0
	SM0:	.equ 1
	SM1:	.equ 2
MCUSR:	.equ 0x34
	PORF:	.equ 0
	EXTRF:	.equ 1
	BORF:	.equ 2
	WDRF:	.equ 3
MCUCR:	.equ 0x35
	IVSEL:	.equ 1
CCP:	.equ 0x36
SPMCSR:	.equ 0x37
	SPMEN:	.equ 0
	PGERS:	.equ 1
	PGWRT:	.equ 2
	RWFLB:	.equ 3
	RWWSRE:	.equ 4
	RSIG:	.equ 5
	RWWSB:	.equ 6
	SPMIE:	.equ 7

SPL:	.equ 0x3d
SPH:	.equ 0x3e
SREG:	.equ 0x3f
	C:	.equ 0
	Z:	.equ 1
	N:	.equ 2
	V:	.equ 3
	S:	.equ 4
	H:	.equ 5
	T:	.equ 6
	I:	.equ 7

; Everything below cannot be addressed as an I/O register, but only as
; memory. 0x20 must be added to the address for that.
WDTCR:	.equ 0x40
	WDP0:	.equ 0
	WDP1:	.equ 1
	WDP2:	.equ 2
	WDE:	.equ 3
	WDP3:	.equ 5
	WDIE:	.equ 6
	WDIF:	.equ 7
CLKPR:	.equ 0x41
	CLKPS0:	.equ 0
	CLKPS1:	.equ 1
	CLKPS2:	.equ 2
	CLKPS3:	.equ 3
PRR:	.equ 0x44
	PRADC:	.equ 0
	PRUSART0:	.equ 1
	PRSPI:	.equ 2
	PRTIM1:	.equ 3
	PRTIM0:	.equ 5
	PRTWI:	.equ 7
OSCCAL0:	.equ 0x46
OSCCAL1:	.equ 0x47
PCICR:	.equ 0x48
	PCIE0:	.equ 0
	PCIE1:	.equ 1
	PCIE2:	.equ 2
	PCIE3:	.equ 3
EICRA:	.equ 0x49
	ISC00:	.equ 0
	ISC01:	.equ 1
	ISC10:	.equ 2
	ISC11:	.equ 3
PCMSK0:	.equ 0x4b
	PCINT0:	.equ 0
	PCINT1:	.equ 1
	PCINT2:	.equ 2
	PCINT3:	.equ 3
	PCINT4:	.equ 4
	PCINT5:	.equ 5
	PCINT6:	.equ 6
	PCINT7:	.equ 7
PCMSK1:	.equ 0x4c
	PCINT8:	.equ 0
	PCINT9:	.equ 1
	PCINT10:	.equ 2
	PCINT11:	.equ 3
	PCINT12:	.equ 4
	PCINT13:	.equ 5
	PCINT14:	.equ 6
	PCINT15:	.equ 7
PCMSK2:	.equ 0x4d
	PCINT16:	.equ 0
	PCINT17:	.equ 1
	PCINT18:	.equ 2
	PCINT19:	.equ 3
	PCINT20:	.equ 4
	PCINT21:	.equ 5
	PCINT22:	.equ 6
	PCINT23:	.equ 7
TIMSK0:	.equ 0x4e
	TOIE0:	.equ 0
	OCIE0A:	.equ 1
	OCIE0B:	.equ 2
TIMSK1:	.equ 0x4f
	TOIE1:	.equ 0
	OCIE1A:	.equ 1
	OCIE1B:	.equ 2
	ICIE1:	.equ 5
PCMSK3:	.equ 0x53
	PCINT24:	.equ 0
	PCINT25:	.equ 1
	PCINT26:	.equ 2
	PCINT27:	.equ 3
ADCL:	.equ 0x58
ADCH:	.equ 0x59
ADCSRA:	.equ 0x5a
	ADPS0:	.equ 0
	ADPS1:	.equ 1
	ADPS2:	.equ 2
	ADIE:	.equ 3
	ADIF:	.equ 4
	ADATE:	.equ 5
	ADSC:	.equ 6
	ADEN:	.equ 7
ADCSRB:	.equ 0x5b
	ADTS0:	.equ 0
	ADTS1:	.equ 1
	ADTS2:	.equ 2
	ADLAR:	.equ 3
ADMUXA:	.equ 0x5c
	MUX0:	.equ 0
	MUX1:	.equ 1
	MUX2:	.equ 2
	MUX3:	.equ 3
	MUX4:	.equ 4
ADMUXB:	.equ 0x5d
	MUX5:	.equ 0
	REFS:	.equ 5
DIDR0:	.equ 0x5e
	ADC0D:	.equ 0
	ADC1D:	.equ 1
	ADC2D:	.equ 2
	ADC3D:	.equ 3
	ADC4D:	.equ 4
	ADC5D:	.equ 5
	ADC6D:	.equ 6
	ADC7D:	.equ 7
DIDR1:	.equ 0x5f
	ADC8D:	.equ 0
	ADC9D:	.equ 1
	ADC10D:	.equ 2
	ADC11D:	.equ 3
	ADC12D:	.equ 4
	ADC13D:	.equ 5
	ADC14D:	.equ 6
	ADC15D:	.equ 7
TCCR1A: .equ 0x60
	WGM10:	.equ 0
	WGM11:	.equ 1
	COM1B0:	.equ 4
	COM1B1:	.equ 5
	COM1A0:	.equ 6
	COM1A1:	.equ 7
TCCR1B: .equ 0x61
	CS10:	.equ 0
	CS11:	.equ 1
	CS12:	.equ 2
	WGM12:	.equ 3
	WGM13:	.equ 4
	ICES1:	.equ 6
	ICNC1:	.equ 7
TCCR1C:	.equ 0x62
	FOC1B:	.equ 6
	FOC1A:	.equ 7
TCNT1L:	.equ 0x64
TCNT1H:	.equ 0x65
ICR1L:	.equ 0x66
ICR1H:	.equ 0x67
OCR1AL:	.equ 0x68
OCR1AH:	.equ 0x69
OCR1BL:	.equ 0x6a
OCR1BH:	.equ 0x6b

TWSCRA:	.equ 0x98
	TWSME:	.equ 0
	TWPME:	.equ 1
	TWSIE:	.equ 2
	TWEN:	.equ 3
	TWASIE:	.equ 4
	TWDIE:	.equ 5
	TWSHE:	.equ 7
TWSCRB:	.equ 0x99
	TWCMD0:	.equ 0
	TWCMD1:	.equ 1
	TWAA:	.equ 2
TWSSRA:	.equ 0x9a
	TWAS:	.equ 0
	TWDIR:	.equ 1
	TWBE:	.equ 2
	TWC:	.equ 3
	TWRA:	.equ 4
	TWCH:	.equ 5
	TWASIF:	.equ 6
	TWDIF:	.equ 7
TWSAM:	.equ 0x9b
TWSA:	.equ 0x9c
TWSD:	.equ 0x9d
UCSRA:	.equ 0xa0
	MPCM:	.equ 0
	U2X:	.equ 1
	UPE:	.equ 2
	DOR:	.equ 3
	FE:	.equ 4
	UDRE:	.equ 5
	TXC:	.equ 6
	RXC:	.equ 7
UCSRB:	.equ 0xa1
	TXB8:	.equ 0
	RXB8:	.equ 1
	UCSZ2:	.equ 2
	TXEN:	.equ 3
	RXEN:	.equ 4
	UDRIE:	.equ 5
	TXCIE:	.equ 6
	RXCIE:	.equ 7
UCSRC:	.equ 0xa2
	UCPOL:	.equ 0
	UCSZ0:	.equ 1
	UCP:	.equ 1
	UCSZ1:	.equ 2
	UDO:	.equ 2
	USBS:	.equ 3
	UPM0:	.equ 4
	UPM1:	.equ 5
	UMSEL0:	.equ 6
	UMSEL1:	.equ 7
UCSRD:	.equ 0xa3
	SFDE:	.equ 5
	RXS:	.equ 6
	RXSIE:	.equ 7
UBRRL:	.equ 0xa4
UBRRH:	.equ 0xa5
UDR:	.equ 0xa6
DIDR2:	.equ 0xbe
DIDR3:	.equ 0xbf
TOCPMCOE:	.equ 0xc2
TOCPMSA0:	.equ 0xc8
TOCPMSA1:	.equ 0xc9
OSCTCAL0A:	.equ 0xd0
OSCTCAL0B:	.equ 0xd1
; lock and fuse byte locations
LOCKBITS:	.equ 0x7001
	LB1:		.equ 0
	LB2:		.equ 1
	BLB01:		.equ 2
	BLB02:		.equ 3
	BLB11:		.equ 4
	BLB12:		.equ 5
FLB:	.equ 0x7000
	CKSEL0:		.equ 0
	CKSEL1:		.equ 1
	SUT0:		.equ 4
	SUT1:		.equ 5
	CKOUT:		.equ 6
	CKDIV8:		.equ 7
FHB:	.equ 0x7003
	BODLEVEL0:	.equ 0
	BODLEVEL1:	.equ 1
	BODLEVEL2:	.equ 2
	EESAVE:		.equ 3
	WDTON:		.equ 4
	SPIEN:		.equ 5
	DWEN:		.equ 6
	RSTDISBL:	.equ 7
FEB:	.equ 0x7002
	BOOTRST:	.equ 0
	BOOTSZ0:	.equ 1
	BOOTSZ1:	.equ 2
	BODACT0:	.equ 4
	BODACT1:	.equ 5
	BODPD0:		.equ 6
	BODPD1:		.equ 7

EEPROM:	.equ 0x4000

; 0x20 registers; 0x40 i/o registers; 0x80 sram bytes.
RAMEND:	.equ 0x2ff

	.org 0
	rjmp INT_RESET
	rjmp ?INT_INT0 ? INT_INT0 : HANG
	rjmp ?INT_INT1 ? INT_INT1 : HANG
	rjmp ?INT_PCINT0 ? INT_PCINT0 : HANG
	rjmp ?INT_PCINT1 ? INT_PCINT1 : HANG
	rjmp ?INT_PCINT2 ? INT_PCINT2 : HANG
	rjmp ?INT_PCINT3 ? INT_PCINT3 : HANG
	rjmp ?INT_WDT ? INT_WDT : HANG
	rjmp ?INT_TIM1_CAPT ? INT_TIM1_CAPT : HANG
	rjmp ?INT_TIM1_COMPA ? INT_TIM1_COMPA : HANG
	rjmp ?INT_TIM1_COMPB ? INT_TIM1_COMPB : HANG
	rjmp ?INT_TIM1_OVF ? INT_TIM1_OVF : HANG
	rjmp ?INT_TIM0_COMPA ? INT_TIM0_COMPA : HANG
	rjmp ?INT_TIM0_COMPB ? INT_TIM0_COMPB : HANG
	rjmp ?INT_TIM0_OVF ? INT_TIM0_OVF : HANG
	rjmp ?INT_SPI ? INT_SPI : HANG
	rjmp ?INT_USART0_RXS ? INT_USART0_RXS : HANG
	rjmp ?INT_USART0_RXC ? INT_USART0_RXC : HANG
	rjmp ?INT_USART0_DRE ? INT_USART0_DRE : HANG
	rjmp ?INT_USART0_TXC ? INT_USART0_TXC : HANG
	rjmp ?INT_ADC_READY ? INT_ADC_READY : HANG
	rjmp ?INT_EE_RDY ? INT_EE_RDY : HANG
	rjmp ?INT_ANA_COMP ? INT_ANA_COMP : HANG
	rjmp ?INT_TWI ? INT_TWI : HANG
	rjmp ?INT_SPM_RDY ? INT_SPM_RDY : HANG
	rjmp ?INT_RESERVED ? INT_RESERVED : HANG

HANG:
	cli
.HANG:
	rjmp .HANG
