.ORIG x3000
JSR ADD	; JSR is parsed as an opcode, and ADD is an
			; illegal label. While illegal labels
			; should return error code 4, we accept
			; error code 1 for this case, too.
.END
