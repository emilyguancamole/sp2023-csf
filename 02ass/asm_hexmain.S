/* Hexdump main function, assembly language version */

	.section .rodata
	sColon: .string ": "
	sSpace: .string " "
	sLongSpace: .string "   "
	sNewline: .string "\n"
	
	.section .bss
	offset: .space 9
	sbuf: .space 17
	hexbuf: .space 3
	printableWords: .space 17
	
	.section .text

	.globl main
	
main:
	subq $8, %rsp

	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp

	pushq %r12 /* push %r12 onto the stack (store sbuf) */
	pushq %r13 /* push %r13 onto the stack (store position) */
	pushq %r14 /* push %r14 onto the stack (store character read n) */
	pushq %r15 /* push %r15 onto stack (store printableWords array) */

	movq $0, %r13 /* move the position number into %r11 */

	movq $sbuf, %r12 /* move memory address of sbuf array into %r12 on stack */
	movq %r12, %rdi /* move %r12 = sbuf into %rdi as a parameter for hex_read */
	call hex_read
	movq %rax, %r14 /* store the value from hexread into %r14 */
	movq $0, %rax /* reset %rax for future use */

.Ltop:
	cmpq $0, %r14 /* compare 0 with the number of chars read from hex_read (stored in %r14) */
	je .Ldone /* if %r14 == 0, Loop is done */

	movq %r13, %rdi /* move the sbuf into %rdi */
	movq $offset, %rsi /* move offset into %rsi */
	call hex_format_offset /* set offset */

	movq $offset, %rdi /* move offset into %rdi */
	call hex_write_string /* print the offet */
	movq $sColon, %rdi /* move colon string into %rdi */
	call hex_write_string /* print colon */

.LformatAsHexStart:
	movq $0, %r10 /* initialize loop counter */

.LformatAsHexCont:
	cmpq %r14, %r10 /* compare n (number of chars read) with loop counter */
	jge .LfillSpace 

	movq (%r12, %r10, 1), %rdi /* get sbuf[i] */
	movq $hexbuf, %rsi 
	pushq %r10/* preserve value of loop counter */
	
	call hex_format_byte_as_hex
	movq $hexbuf, %rdi /* move changed hexbuf into %rdi */
	call hex_write_string /* print hexbuf */
	movq $sSpace, %rdi
	call hex_write_string /* print space */
	
	popq %r10 

	incq %r10 /* inc loop counter */
	jmp .LformatAsHexCont

.LfillSpace:
	movq %r14, %r10 /* initialize loop counter */
	cmpq $16, %r10 /* compare loop counter with 16 */
	jge .LprintWords /* %r10 >= 16, break out of loop */

	movq $sLongSpace, %rdi /* move "   into %rdi */
	call hex_write_string /* print the space to fill in */

	incq %r10 /* increment loop counter */
	jmp .LfillSpace /* jump back to top */

	movq $sSpace, %rdi 
	call hex_write_string /* print out " " */

	movq $0, %r10 /* reset loop counter to 0 for next part */

.LstoreWords:
	cmpq %r14, %r10 /* compare loop counter with n */
	jge .LprintWords /* if >=n, break out */

	movq (%r12, %r10, 1), %rdi /* get sbuf[i] */
	call hex_to_printable /* check if char at buf[i] is printable */
	movq %rax, (%r15, %r10, 1) /* store the char to printableWords[i] */
	inc %r10 /* increment loop counter */
	jmp .LstoreWords /* jump back to top */

.LprintWords:
	movq $printableWords, %rdi /* move the printableWords array into %rdi */
	call hex_write_string /* print out the printableWords array */
	movq $sNewline, %rdi
	call hex_write_string /* print out newline */

	movq $sbuf, %rdi /* move sbuf back into rdi */
	call hex_read /* read the next bunch of characters */
	movq %rax, %r14 /* store new n into %r14 */
	addq $16, %r13 /* increase position by 16 */
	jmp .Ltop /* jump all the way back to the top */

.Ldone:
	popq %r15 /* pop all the callee-saved registers */
	popq %r14
	popq %r13
	popq %r12

	addq $8, %rsp
	ret

/* vim:ft=gas:
 */
