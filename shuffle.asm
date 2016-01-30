;This program counts from 10 to 0
		.ORIG x3000
		LOAD x4004 $ecx     ; Get the mask
		AND  $le $ecx x03   ;  AND mask with 0000 0011 to ger the bit[0:1]
		ADD R0, $le, x4000  ; The value got will be added to 0x4000
		; example, we got 0000 0010 then the address will be 0x4002
		MOV R0, x4005       ;  Move or copy the value in 0x4002 to 0x4005
		AND  $le $ecx x0C   ;  AND mask with 0000 1100 to get the bit[2:3]
		RSHFT $le, 2        ; Right shift twice to get the value in first two bits
		ADD R0, $le, x4000  ; The value got will be added to 0x4000
		MOV R0, x4005       ; Move or copy the value in 0x4000+le to 0x4006
		AND  $le $ecx x30   ; AND mask with 0011 0000 to get the bit[2:3]
		RSHFT $le, 4        ;  Right shift 4 times to get the value in first two bits
		ADD R0, $le, x4000  ;  The value got will be added to 0x4000
		MOV R0, x4005       ;  Move or copy the value in 0x4000+le to 0x4007
		AND  $le $ecx xC0   ; AND mask with 1100 0000 to get the bit[2:3]
		RSHFT $le, 6        ;  Right shift 6 times to get the value in first two bits
		ADD R0, $le, x4000  ;  The value got will be added to 0x4000
		MOV R0, x4005       ;  Move or copy the value in 0x4000+le to 0x4008
DONE	TRAP x25		    ;The last executable instruction
	    .END  ;The pseudo-op, delimiting the source program
	    
	    
	    
	    .ORIG X3000
	    
	    
	    ; code here

	    
SOURCE	    .FILL X4000
MASK	    .FILL X4004
DESTNTN	    .FILL X4005
	    
	    .END
