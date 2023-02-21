/* Esther Kwon (kkwon16@jhu.edu) and Emily Guan (eguan2@jhu.edu) */

/* Assembly language implementation of hexdump functions */

	.section .rodata

	/* TODO: add read-only data such as strings here */
	byteval:
		.string ""
	.section .text

		.globl hex_to_printable
	hex_to_printable:
		/* byteval stored in rdi*/

		cmpb $32, %dil 		/* compare 32 and dil (4 bits...?) as bindary */
		jl .LnotPrintable	/* conditional jump if rdi < 32 */
		cmpq $127, %rdi
		jge .LnotPrintable 	/* conditional jump if rdi >= 127 */
		
		movq %rdi, %rax 	/* move value to return val*/
		jmp .LprintByte		/* jump to return the value */

	.LnotPrintable: 		/* rdi < 32 or rdi >= 127 */
		movq $46, %rax 		/* replace rdi with ascii for '.' */
		jmp .LprintByte		/* jump to return the value */
	
	.LprintByte: /* 32<=rdi<127, so print whatever is stored in rdi */
		ret /* return %rax */



		.globl hex_format_offset
	hex_format_offset: 
	    movl $8, %r10d 			/* set array index r10d = 8 -> need 9 spaces with null terminator */
	    movb $0, (%rsi, %r10, 1)  /* move null term into last elem of array */ 
        decl %r10d 				/* decrement array index to 7 */
        
    .LloopHexFormat:
		cmpl $0, %r10d 			/* check if we should continue (index >= 0) */
		jl .LloopFinal			/* if r10d < 0, done with formatting loop */

		; movb %dil, %r9b 		/* store 4 bits of rdi */
		; andb $0b1111, %r9b		/* bitwise and with the 4 bits */
		movb $0b1111, %r9b
		andb %dil, %r9b

		cmpb $10, %r9b 			/* Compare result with 10 */
		jl .LhexFormatNum 		/* Conditional jump if r9d < 10 */

		addb $87, %r9b 			/* Else, add 87 to the result */
		jmp .LhexFormatDigit

	.LhexFormatNum:
		addb $48, %r9b			/* add 48 to result */
		jmp .LhexFormatDigit
	
	.LhexFormatDigit:
		movb %r9b, (%rsi, %r10, 1) 	/*add result to place in array*/
		sarl $4, %edi 				/*shift argument right 4 bits ??why shift dil (4 bits)? can you do shrq with rdi??*/ 
		decl %r10d					/*decrement index of array / loop counter */
		
		jmp .LloopHexFormat  		/* jump to top of loop */

	.LloopFinal:
		ret




		.globl hex_format_byte_as_hex
	hex_format_byte_as_hex:
		movl $2, %r10d 				/* move 2 into %r10d as index of array */
		movb $0, (%rsi, %r10, 1) 	/* move null terminator into LAST element (index 2) of char array rsi*/
		decl %r10d 				/* decrement index of array / loop counter */
		
	.LcompareLoop:
		cmpl $0, %r10d 			/* check if we should continue*/
		jl .LformatFinal		/* jump to return once we are done with the loop */

		movb %dil, %r9b 		/* 4 bits of rdi into dil as binary*/
		andb $0b1111, %r9b		/* bitmask the last 4 bits of %r9b and store in %r9b */

		cmpb $10, %r9b 			/* Compare result with 10 */
		jl .LhexNumber 			/* Conditional jump if r9d < 10 */

		addb $87, %r9b 			/* Else, add 87 to the result = alphabet */
		jmp .LformatDigit		/* jump to store this value in the array */
	
	.LhexNumber:
		addb $48, %r9b 			/* add 48 to result = number */
		jmp .LformatDigit		/* jump to store this value in the array */
	
	.LformatDigit:
		movb %r9b, (%rsi, %r10, 1) 	/* add result to the array */
		sarb $4, %dil 				/* shift dil right by 4 bits  to get the next 4*/ 
		decl %r10d					/* decrement index of array / loop counter */
				
		jmp .LcompareLoop 			/* keep going to compare next 4 bits */

	.LformatFinal:
		ret		/* return the byte as hex */
		


		.globl hex_read
	hex_read:
		movl $0, %r10d
		call read

		ret

		.globl hex_write_string
	hex_write_string:
		/* TODO */
		ret

	

/* vim:ft=gas:
 */
