; Second compiler pass errors, 

; Valid command 
mov #1, r3

; Error, VAR has has never been declared in the file, this error should appear in the second pass 
.entry VAR
