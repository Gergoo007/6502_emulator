	.org $8000

hello:
	LDX #$10
	BEQ test

test:
	LDA #$20
	BEQ hello

	.org $fffc
	.word test
	.word $0000