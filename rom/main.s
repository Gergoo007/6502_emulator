	.org $8000

irq:
	LDA #$35
	RTI

nmi:
	LDA #$32
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