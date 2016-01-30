;This program shuffles the bytes appropriately then writes the shuffled bytes to the output locations.
		.ORIG x3000
		AND R0, R0, #0      ; clear R0
		LD  R0, MASK        ; Get the mask
		; Byte 0x4005
		AND R1, R1, #0      ; clear R1
		AND R1, R0, #3      ; AND mask with 0000 0011 to get the bit[0:1]
		ADD R1, R1, OLD     ; The value got will be added to 0x4000 (OLD)
		MOV  R1, DEST        ; R1 is old byte according to mask and DEST is destination
        ; Byte 0x4006
        AND R1, R1, #0      ; clear R1
		AND R1, R0, #12     ; AND mask with 0000 1100 to get the bit[2:3]
		RSHFL R1, R1, #2    ; right shift logical
		ADD R1, R1, OLD     ; The value got will be added to 0x4000 (OLD)
		AND R2, R2, #0      ; clear R2
	    ADD R2, DEST, #1    ; Get location of the second Destination x4006
		MOV  R1, R2          ; R1 is old byte according to mask and R2 is destination
		; Byte 0x4007
		AND R1, R1, #0      ; clear R1
		AND R1, R0, #48     ; AND mask with 0011 0000 to get the bit[4:5]
		RSHFL R1, R1, #4    ; right shift logical
		ADD R1, R1, OLD     ; The value got will be added to 0x4000 (OLD)
		AND R2, R2, #0      ; clear R1
	    ADD R2, DEST, #2    ; Get location of the third Destination x4007
		MOV  R1, R2          ; R1 is old byte according to mask and R2 is destination
		; Byte 0x4008
		AND R1, R1, #0      ; clear R1
		AND R1, R0, #192    ; AND mask with 1100 0000 to get the bit[6:7]
		RSHFL R1, R1, #6    ; right shift logical
		ADD R1, R1, OLD     ; The value got will be added to 0x4000 (OLD)
		AND R2, R2, #0      ; clear R2
	    ADD R2, DEST, #3    ; Get location of the second Destination x4008
		MOV  R1, R2          ; R1 is old byte according to mask and R2 is destination
DONE	TRAP x25		    ;The last executable instruction

MASK    .FILL x4004
OLD     .FILL x4000
DEST    .FILL x4005

	    .END  ;The pseudo-op, delimiting the source program
	    
	    
	    
	    .ORIG X3000
	   
	   ; initialize any vars here. If you run out of registers you can us ".FILL" to allocate memory to load and
	   ;	store from. So "VAR1" can be a place holder to store a variable.
	    MV R0, #4 ; let some register be your counter, and put 4 in there somehow
	    
LOOPSTART
	    ; loop code here
	    
	    ; increment/decrement/change any vars here
	    ADD R0, R0, #-1 ; decrement counter
	    ; the nzp bits need to be set somehow, if "ADD" doesn't do it then look for a compare operation
	    BRp LOOPSTART ; branch if positive bit is set, to beginning of loop
	    
	    
	    HALT
	    
SOURCE	    .FILL X4000
MASK	    .FILL X4004
DESTNTN	    .FILL X4005
VAR1	    .FILL X0000
	    
	    .END
