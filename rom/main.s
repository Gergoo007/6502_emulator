	.org $8000

irq:
	INC $00
	RTI

nmi:
	INC $01
	RTI

test:
	INC $10
	LDA $10
	INX
	STA $2000,X
	JMP test

	.org $fffa
	.word nmi
	.word test
	.word irq