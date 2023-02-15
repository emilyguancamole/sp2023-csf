/* Assembly language implementation of hexdump functions */

	.section .rodata

	/* TODO: add read-only data such as strings here */
	byteval:
		.string ""
	.section .text
	/* TODO: add functions here */
		.globl hex_to_printable
	hex_to_printable:
		/* byteval stored in rdi*/
		/*subq $8, %rsp ?? do we still need this??*/

		cmpb $32, %dil /* changed to dil and b*/
		jl .LnotPrintable /*conditional jump if rdi < 32*/
		cmpq $127, %rdi
		jge .LnotPrintable /*conditional jump if rdi >= 127*/
		
		movq %rdi, %rax /* move value to return val*/
		jmp .LprintByte

	.LnotPrintable: /* rdi < 32 or rdi >= 127 */
		movq $46, %rax /* replace rdi with ascii for '.' */
		jmp .LprintByte
	
	.LprintByte: /* 32<=rdi<127, so print whatever is stored in rdi */
		/*addq $8, %rsp*/
		ret



		.globl hex_format_offset
	hex_format_offset: 
		movl $0, %r10d 			/* move 0 into r10d, index of array*/

	.LarrayInitLoop: 			/* initializing array of 8 char to 0s */
        cmpl $7, %r10d 			/* compare number of elements and index r10d */
        jg .LloopHexFormat 			/*if r10 > 7, done with loop*/
        movb $0, (%rsi, %r10, 1)  /*move 0 char into current array index ?? r9?*/ 
        incl %r10d 				/*increment array index*/
        jmp .LarrayInitLoop
        
    .LloopHexFormat:
		movb $0, (%rsi, %r10, 1) /* move null terminator into last element of array (should be idx 8*/


		cmpl $0, %r10d 			/* check if we should continue*/
		jl .LformatFinal		/* if r10d < 0, done with formatting loop */

		movb %dil, %r9b 		/* store 4 bits of rdi */
		andb $0b1111, %r9b		/* bitwise and with the 4 bits */

		cmpb $10, %r9b 			/* Compare result with 10 */
		jl .LhexNumber 			/* Conditional jump if r9d < 10 */

		addb $87, %r9b 			/* Else, add 87 to the result */
		jmp .LformatDigit
	
        jmp .LcheckCond

		ret



		.globl hex_format_byte_as_hex
	hex_format_byte_as_hex:
		/* TODO */
		movl $2, %r10d /*index of array*/
		movb $0, (%rsi, %r10, 1) /* move null terminator into LAST element (index 2) of char array rsi*/
	/*?? why does 0 null term vs 0 int??*/
	/*?? why movb - would movl work*/
		decl %r10d /*decrement index of array / loop counter */
		
	.LcompareLoop:
		cmpl $0, %r10d 	/* check if we should continue*/
		jl .LformatFinal

		movb %dil, %r9b /* 4 bits of rdi ????? is that what its doing bc both dil and rdi have 72*/
		/*compare, store in rsi*/
		andb $0b1111, %r9b

		cmpb $10, %r9b 	/* Compare result with 10 */
		jl .LhexNumber 	/* Conditional jump if r9d < 10 */

		addb $87, %r9b 	/* Else, add 87 to the result */
		jmp .LformatDigit
	
	.LhexNumber:
		addb $48, %r9b /* add 48 to result */
		jmp .LformatDigit
	
	.LformatDigit:
		movb %r9b, (%rsi, %r10, 1) 	/*add result to place in array*/
		sarb $4, %dil 				/*shift argument right 4 bits ??why shift dil (4 bits)? can you do shrq with rdi??*/ 
		decl %r10d					/*decrement index of array / loop counter */
				
		jmp .LcompareLoop /*keep going to compare next 4 bits*/

	.LformatFinal:
		ret
		

		.globl hex_read
	hex_read:
		/* TODO */
		ret

		.globl hex_write_string
	hex_write_string:
		/* TODO */
		ret

	

/* vim:ft=gas:
 */
