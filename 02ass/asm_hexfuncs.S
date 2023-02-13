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

		cmpq $32, %rdi /*?? why is it q 8 bytes - thought hex was 4 bytes?? can you explain bits and bytes pls*/
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
		/* TODO */
		movl $0, %r10d /* move 0 into r10d, which is index of array*/
		movl $8, %r9 /*number of elements- store elsewhere like r9. if wanted to save, use caller saved (vals will stay the same across function calls*/ 

	.LarrayInitLoop: /*initializing array of 8 char to 0s*/
		cmpl %esi, %r10d
		jae .LloopDone /*if r10d >= esi, done with loop*/
		movb $0, (%rsi, %r10, 1)  /*move 0 into current array index*/
		incl %r10d /*increment array index*/
		jmp .LarrayInitLoop
		
	.LloopDone:
		movq $7, %rsi /*move int i = 7 into rsi*/
		movl $0, %esi /*esi is while loop counter - */
		
		jmp .LcheckCond
/*rdi is 1st parameter*/
	.Ltop:
	/*move offset to another var*/
		andl $0xF, %rdi  /*?? bitwise and: 1111 & offset (stored where??), andb may also work*/
		/*compare result with 10, change result accordingly*/
		cmpl 
		/*decrement r10d=*/
	.LcheckCond:
		cmp %rsi, esi
		jle .Ltop /* while (esi <= rsi), continue loop*/

		

		ret

		.globl hex_format_byte_as_hex
	hex_format_byte_as_hex:
		/* TODO */
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