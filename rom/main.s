	.org $8000

hello:
	LDX #$10

test:
	LDA #$20
	CMP #$20
	BEQ hello
	.byte 0x00

	.org $fffc
	.word test
	.word $0000