	.org $8000

subrout:
	LDY #$64
	RTS

test:
	LDA #$39
	RTS

hello:
	LDA #$20
	JSR test
	LDX #$54
	JSR subrout

	.org $fffc
	.word hello
	.word $0000