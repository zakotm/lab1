;This program shuffles the bytes appropriately then writes the shuffled bytes to the output locations.

	.ORIG x3000

	; R0 will hold the mask
	; R3 will hold the base address (x4000) of the old data
	; R4 will hold the base address (x4005) of the destination

    AND R0, R0, #0      ; clear R0 
    LEA R0, MASK        ; get the address of the  mask address
    LDW R0, R0, #0      ; load the address of the mask
    LDW R0, R0, #0      ; load the mask into R0 

	AND R3, R3, #0	    ; clear
	LEA R3, OLD 		; get address
	LDW R3, R3, #0	    ; load x4000 into R3

	AND R4, R4, #0	    ; clear
	LEA R4, DEST	    ; get address
	LDW R4, R4, #0	    ; load x4005 into R4

	; Byte 0x4005
	AND R1, R1, #0      ; clear R1
	AND R1, R0, #3      ; AND mask with 0000 0011 to get the bit[0:1]
	ADD R1, R1, R3      ; The value got will be added to 0x4000 (OLD), R1 now holds the address to read from
	LDB R1, R1, #0	    ; load the value at address into R1
	STB R1, R4, #0 	    ; store R1 into x4005

	; Byte 0x4006
	AND R1, R1, #0      ; clear R1
	RSHFL R1, R0, #2	; shift bits to least significant positions
	AND R1, R1, #3      ; AND mask with 0000 0011 to get the bit[0:1]
	ADD R1, R1, R3		; The value got will be added to 0x4000 (OLD), R1 now holds the address to read from
	LDB R1, R1, #0		; load the value at address into R1
	STB R1, R4, #1		; store R1 into x4006

	; Byte 0x4007
	AND R1, R1, #0      ; clear R1
	RSHFL R1, R0, #4	; shift bits to least significant positions
	AND R1, R1, #3      ; AND mask with 0000 0011 to get the bit[0:1]
	ADD R1, R1, R3		; The value got will be added to 0x4000 (OLD), R1 now holds the address to read from
	LDB R1, R1, #0		; load the value at address into R1
	STB R1, R4, #2		; store R1 into x4007

	; Byte 0x4008
	AND R1, R1, #0      ; clear R1
	RSHFL R1, R0, #6	; shift bits to least significant positions
	AND R1, R1, #3      ; AND mask with 0000 0011 to get the bit[0:1]
	ADD R1, R1, R3		; The value got will be added to 0x4000 (OLD), R1 now holds the address to read from
	LDB R1, R1, #0		; load the value at address into R1
	STB R1, R4, #3		; store R1 into x4008

DONE	TRAP x25	    ;The last executable instruction


MASK    .FILL x4004
OLD     .FILL x4000
DEST    .FILL x4005


        .END  ;The pseudo-op, delimiting the source program




