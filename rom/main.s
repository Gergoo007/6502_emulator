	.org $8000

irq:
	LDA #$35
	RTI

nmi:
	LDA #$32
	RTI

test:
	;LDA #$20
	JMP test

	.org $fffa
	.word nmi
	.word test
	.word irq