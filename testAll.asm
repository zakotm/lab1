	.ORIG #4096
        ADD R1, R1, R0
	AND R1, R1, R2
	BR Y
	BRn A
	BRnz A
	BRnp A
	BRp A
	BRpz A
	BRz A
	BRnzp A
	HALT
	JMP R1
	JSR Y
	JSSR R1
	LDB R1, R4, #5
	LEA R0, A
	NOP 
	NOT R1, R0
	RET 
	LSHF R2, R3, #3
	RSHFL R2, R3, #3
	RSHFA R2, R3, #3
	RTI
	STB R4, R2, #10
	STW R4, R2, #10
	TRAP x23
	XOR R3, R1, R2
 	XOR R3, R1, #12
A	LEA R1, Y
	LDW R1, R1, #0
	LDW R1, R1, #0
	ADD R1, R1, R1
	ADD R1, R1, x-10	;x-10 is the negative of x10
	BRN A
	HALT
Y	.FILL #263
	.FILL #13
	.FILL #6
	.END