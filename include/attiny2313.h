; attiny2313 register and bit definitions
DIDR:	.equ 0x01
	AIN0D:	.equ 0
	AIN1D:	.equ 0
UBRRH:	.equ 0x02
UCSRC:	.equ 0x03
	UCPOL:	.equ 0
	UCSZ0:	.equ 1
	UCSZ1:	.equ 2
	USBS:	.equ 3
	UPM0:	.equ 4
	UPM1:	.equ 5
	UMSEL:	.equ 6
ACSR:	.equ 0x08
	ACIS0:	.equ 0
	ACIS1:	.equ 1
	ACIE:	.equ 3
	ACI:	.equ 4
	ACO:	.equ 5
	ACBG:	.equ 6
	ACD:	.equ 7
UBRRL:	.equ 0x09
UCSRB:	.equ 0x0a
	TXB8:	.equ 0
	RXB8:	.equ 1
	UCSZ2:	.equ 2
	TXEN:	.equ 3
	RXEN:	.equ 4
	UDRIE:	.equ 5
	TXCIE:	.equ 6
	RXCIE:	.equ 7
UCSRA:	.equ 0x0b
	MPCM:	.equ 0
	U2X:	.equ 1
	PE:	.equ 2
	DOR:	.equ 3
	FE:	.equ 4
	UDRE:	.equ 5
	TXC:	.equ 6
	RXC:	.equ 7
; These are three names for the same register.
UDR:	.equ 0x0c
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
PIND:	.equ 0x10
	PIND0:	.equ 0
	PIND1:	.equ 1
	PIND2:	.equ 2
	PIND3:	.equ 3
	PIND4:	.equ 4
	PIND5:	.equ 5
	PIND6:	.equ 6
DDRD:	.equ 0x11
	DDRD0:	.equ 0
	DDRD1:	.equ 1
	DDRD2:	.equ 2
	DDRD3:	.equ 3
	DDRD4:	.equ 4
	DDRD5:	.equ 5
	DDRD6:	.equ 6
PORTD:	.equ 0x12
	PORTD0:	.equ 0
	PORTD1:	.equ 1
	PORTD2:	.equ 2
	PORTD3:	.equ 3
	PORTD4:	.equ 4
	PORTD5:	.equ 5
	PORTD6:	.equ 6
GPIOR0:	.equ 0x13
GPIOR1:	.equ 0x14
GPIOR2:	.equ 0x15
PINB:	.equ 0x16
	PINB0:	.equ 0
	PINB1:	.equ 1
	PINB2:	.equ 2
	PINB3:	.equ 3
	PINB4:	.equ 4
	PINB5:	.equ 5
	PINB6:	.equ 6
	PINB7:	.equ 7
DDRB:	.equ 0x17
	DDRB0:	.equ 0
	DDRB1:	.equ 1
	DDRB2:	.equ 2
	DDRB3:	.equ 3
	DDRB4:	.equ 4
	DDRB5:	.equ 5
	DDRB6:	.equ 6
	DDRB7:	.equ 7
PORTB:	.equ 0x18
	PORTB0:	.equ 0
	PORTB1:	.equ 1
	PORTB2:	.equ 2
	PORTB3:	.equ 3
	PORTB4:	.equ 4
	PORTB5:	.equ 5
	PORTB6:	.equ 6
	PORTB7:	.equ 7
PINA:	.equ 0x19
	PINA0:	.equ 0
	PINA1:	.equ 1
	PINA2:	.equ 2
DDRA:	.equ 0x1a
	DDRA0:	.equ 0
	DDRA1:	.equ 1
	DDRA2:	.equ 2
PORTA:	.equ 0x1b
	PORTA0:	.equ 0
	PORTA1:	.equ 1
	PORTA2:	.equ 2
EECR:	.equ 0x1c
	EERE:	.equ 0
	EEPE:	.equ 1
	EEMPE:	.equ 2
	EERIE:	.equ 3
	EEPM0:	.equ 4
	EEPM1:	.equ 5
EEDR:	.equ 0x1d
EEAR:	.equ 0x1e
PCMSK:	.equ 0x20
	PCINT0:	.equ 0
	PCINT1:	.equ 1
	PCINT2:	.equ 2
	PCINT3:	.equ 3
	PCINT4:	.equ 4
	PCINT5:	.equ 5
	PCINT6:	.equ 6
	PCINT7:	.equ 7
WDTCR:	.equ 0x21
	WDP0:	.equ 0
	WDP1:	.equ 1
	WDP2:	.equ 2
	WDE:	.equ 3
	WDCE:	.equ 4
	WDP3:	.equ 5
	WDIE:	.equ 6
	WDIF:	.equ 7
TCCR1C:	.equ 0x22
	FOC1B:	.equ 6
	FOC1A:	.equ 7
GTCCR:	.equ 0x23
	PSR10:	.equ 0
ICR1L:	.equ 0x24
ICR1H:	.equ 0x25
CLKPR:	.equ 0x26
	CLKPS0:	.equ 0
	CLKPS1:	.equ 1
	CLKPS2:	.equ 2
	CLKPS3:	.equ 3
	CLKPCE:	.equ 7
OCR1BL:	.equ 0x28
OCR1BH:	.equ 0x29
OCR1AL:	.equ 0x2a
OCR1AH:	.equ 0x2b
TCNT1L:	.equ 0x2c
TCNT1H:	.equ 0x2d
TCCR1B:	.equ 0x2e
	CS10:	.equ 0
	CS11:	.equ 1
	CS12:	.equ 2
	WGM12:	.equ 3
	WGM13:	.equ 4
	ICES1:	.equ 6
	ICNC1:	.equ 7
TCCR1A:	.equ 0x2f
	WGM10:	.equ 0
	WGM11:	.equ 1
	COM1B0:	.equ 4
	COM1B1:	.equ 5
	COM1A0:	.equ 6
	COM1A1:	.equ 7
TCCR0A:	.equ 0x30
	WGM00:	.equ 0
	WGM01:	.equ 1
	COM0B0:	.equ 4
	COM0B1:	.equ 5
	COM0A0:	.equ 6
	COM0A1:	.equ 7
OSCCAL:	.equ 0x31
TCNT0:	.equ 0x32
TCCR0B:	.equ 0x33
	CS00:	.equ 0
	CS01:	.equ 1
	CS02:	.equ 2
	WGM02:	.equ 3
	FOC0B:	.equ 6
	FOC0A:	.equ 7
MCUCSR:	.equ 0x34
	PORF:	.equ 0
	EXTRF:	.equ 1
	BORF:	.equ 2
	WDRF:	.equ 3
MCUCR:	.equ 0x35
	ISC00:	.equ 0
	ISC01:	.equ 1
	ISC10:	.equ 2
	ISC11:	.equ 3
	SM0:	.equ 4
	SE:	.equ 5
	SM1:	.equ 6
	PUD:	.equ 7
OCR0A:	.equ 0x36
SPMCSR:	.equ 0x37
	SELFPRGEN:	.equ 0
	PGERS:	.equ 1
	PGWRT:	.equ 2
	RFLB:	.equ 3
	CTPB:	.equ 4
TIFR:	.equ 0x38
	OCF0A:	.equ 0
	TOV0:	.equ 1
	OCF0B:	.equ 2
	ICF1:	.equ 3
	OCF1B:	.equ 5
	OCF1A:	.equ 6
	TOV1:	.equ 7
TIMSK:	.equ 0x39
	OCIE0A:	.equ 0
	TOIE0:	.equ 1
	OCIE0B:	.equ 2
	ICIE1:	.equ 3
	OCIE1B:	.equ 5
	OCIE1A:	.equ 6
	TOIE1:	.equ 7
EIFR:	.equ 0x3a
	PCIF:	.equ 5
	INTF0:	.equ 6
	INTF1:	.equ 7
GIMSK:	.equ 0x3b
	PCIE:	.equ 5
	INT0:	.equ 6
	INT1:	.equ 7
OCR0B:	.equ 0x3c
SPL:	.equ 0x3d
	SP0:	.equ 0
	SP1:	.equ 1
	SP2:	.equ 2
	SP3:	.equ 3
	SP4:	.equ 4
	SP5:	.equ 5
	SP6:	.equ 6
	SP7:	.equ 7
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
	BODEN:		.equ 6
	BODLEVEL:	.equ 7
LOCKBITS:	.equ 0x7001
	LB1:		.equ 0
	LB2:		.equ 1
	BLB01:		.equ 2
	BLB02:		.equ 3
	BLB11:		.equ 4
	BLB12:		.equ 5
FHB:	.equ 0x7003
	BOOTRST:	.equ 0
	BOOTSZ0:	.equ 1
	BOOTSZ1:	.equ 2
	EESAVE:		.equ 3
	CKOPT:		.equ 4
	SPIEN:		.equ 5
	WDTON:		.equ 6
	RSTDISBL:	.equ 7

EEPROM:	.equ 0x4000

; 0x20 registers; 0x40 i/o registers; 0x80 sram bytes.
RAMEND:	.equ 0xdf

	.org 0
	rjmp INT_RESET
	rjmp ?INT_INT0 ? INT_INT0 : HANG
	rjmp ?INT_INT1 ? INT_INT1 : HANG
	rjmp ?INT_TIMER1_CAPT ? INT_TIMER1_CAPT : HANG
	rjmp ?INT_TIMER1_COMPA ? INT_TIMER1_COMPA : HANG
	rjmp ?INT_TIMER1_OVF ? INT_TIMER1_OVF : HANG
	rjmp ?INT_TIMER0_OVF ? INT_TIMER0_OVF : HANG
	rjmp ?INT_USART0_RX ? INT_USART0_RX : HANG
	rjmp ?INT_USART0_UDRE ? INT_USART0_UDRE : HANG
	rjmp ?INT_USART0_TX ? INT_USART0_TX : HANG
	rjmp ?INT_ANALOG_COMP ? INT_ANALOG_COMP : HANG
	rjmp ?INT_PCINT ? INT_PCINT : HANG
	rjmp ?INT_TIMER1_COMPB ? INT_TIMER1_COMPB : HANG
	rjmp ?INT_TIMER0_COMPA ? INT_TIMER0_COMPA : HANG
	rjmp ?INT_TIMER0_COMPB ? INT_TIMER0_COMPB : HANG
	rjmp ?INT_USI_START ? INT_USI_START : HANG
	rjmp ?INT_USI_OVERFLOW ? INT_USI_OVERFLOW : HANG
	rjmp ?INT_EE_READY ? INT_EE_READY : HANG
	rjmp ?INT_WDT_OVERFLOW ? INT_WDT_OVERFLOW : HANG

HANG:
	cli
	ldi $16, #'!'
.HANG:
	out @UDR, $16
	rjmp .HANG
