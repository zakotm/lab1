	.ORIG #4096
        ADD R1, R1, R0
	AND R1, R1, R2
	ADD R0, R0, xA
	BR Y
	BRN A
	BRNZ A
	BRNP A
	BRP A
	BRZP A
	BRZ A
	BRNZP A
	HALT
	JMP R1
	JSR Y
	JSRR R1
	LDB R1, R4, #5
	LEA R0, A
	NOP 
	NOT R1, R0
	RET 
	LSHF R2, R3, #3
	RSHFL R2, R3, #3
R5	RSHFA R2, R3, #3
	RTI
	STB R4, R2, #10
A	STW R4, R2, #10
	TRAP x23
	XOR R3, R1, R2
 	XOR R3, R1, #12
	LEA R1, Y
	LDW R1, R1, #0
	LDW R1, R1, #0
	ADD R1, R1, R1
	ADD R1, R1, x-10	;x-10 is the negative of x10
	BRN A
	HALT
Y	.FILL #263
	.FILL #13
	.FILL #6
	.FILL xffff
	.END
