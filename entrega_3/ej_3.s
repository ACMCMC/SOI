	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15, 6
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movl	$0, -4(%rbp)
	movl	$30, %edi
	leaq	_gestion(%rip), %rsi
	callq	_signal
	movq	$-1, %rcx
	cmpq	%rcx, %rax
	jne	LBB0_2
## %bb.1:
	leaq	L_.str(%rip), %rdi
	movb	$0, %al
	callq	_printf
LBB0_2:
	movl	$31, %edi
	leaq	_gestion(%rip), %rsi
	callq	_signal
	movq	$-1, %rcx
	cmpq	%rcx, %rax
	jne	LBB0_4
## %bb.3:
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
LBB0_4:
	callq	_getpid
	movl	%eax, -8(%rbp)
	callq	_fork
	movl	%eax, -12(%rbp)
	cmpl	$0, %eax
	jne	LBB0_7
## %bb.5:
	movl	-8(%rbp), %edi
	movl	$30, %esi
	callq	_kill
LBB0_6:                                 ## =>This Inner Loop Header: Depth=1
	jmp	LBB0_6
LBB0_7:
	movl	-12(%rbp), %edi
	movl	$31, %esi
	callq	_kill
	movl	$1, %edi
	movl	%eax, -20(%rbp)         ## 4-byte Spill
	callq	_sleep
	leaq	L_.str.2(%rip), %rdi
	movl	%eax, %esi
	movb	$0, %al
	callq	_printf
	movl	-12(%rbp), %edi
	movl	$15, %esi
	movl	%eax, -24(%rbp)         ## 4-byte Spill
	callq	_kill
	leaq	-16(%rbp), %rdi
	movl	%eax, -28(%rbp)         ## 4-byte Spill
	callq	_wait
	movl	-16(%rbp), %ecx
	andl	$127, %ecx
	cmpl	$127, %ecx
	je	LBB0_11
## %bb.8:
	movl	-16(%rbp), %eax
	andl	$127, %eax
	cmpl	$0, %eax
	je	LBB0_11
## %bb.9:
	movl	-16(%rbp), %eax
	andl	$127, %eax
	cmpl	$15, %eax
	jne	LBB0_11
## %bb.10:
	leaq	L_.str.3(%rip), %rdi
	xorl	%eax, %eax
	movb	%al, %cl
	movl	%eax, -32(%rbp)         ## 4-byte Spill
	movb	%cl, %al
	callq	_printf
	movl	-32(%rbp), %edi         ## 4-byte Reload
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	callq	_exit
LBB0_11:
	movl	-16(%rbp), %eax
	andl	$127, %eax
	cmpl	$127, %eax
	je	LBB0_14
## %bb.12:
	movl	-16(%rbp), %eax
	andl	$127, %eax
	cmpl	$0, %eax
	je	LBB0_14
## %bb.13:
	movq	___stderrp@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	movl	-16(%rbp), %ecx
	andl	$127, %ecx
	movq	%rdi, -48(%rbp)         ## 8-byte Spill
	movl	%ecx, %edi
	callq	_strsignal
	leaq	L_.str.4(%rip), %rsi
	xorl	%ecx, %ecx
                                        ## kill: def $cl killed $cl killed $ecx
	movq	-48(%rbp), %rdi         ## 8-byte Reload
	movq	%rax, %rdx
	movb	%cl, %al
	callq	_fprintf
	movl	$1, %edi
	movl	%eax, -52(%rbp)         ## 4-byte Spill
	callq	_exit
LBB0_14:
	movq	___stderrp@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	leaq	L_.str.5(%rip), %rsi
	xorl	%ecx, %ecx
                                        ## kill: def $cl killed $cl killed $ecx
	movb	%cl, %al
	callq	_fprintf
	movl	$1, %edi
	movl	%eax, -56(%rbp)         ## 4-byte Spill
	callq	_exit
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function gestion
_gestion:                               ## @gestion
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	subl	$30, %ecx
	movl	%eax, -8(%rbp)          ## 4-byte Spill
	je	LBB1_1
	jmp	LBB1_4
LBB1_4:
	movl	-8(%rbp), %eax          ## 4-byte Reload
	subl	$31, %eax
	je	LBB1_2
	jmp	LBB1_3
LBB1_1:
	callq	_getpid
	leaq	L_.str.6(%rip), %rdi
	movl	%eax, %esi
	movb	$0, %al
	callq	_printf
	jmp	LBB1_3
LBB1_2:
	callq	_getpid
	leaq	L_.str.7(%rip), %rdi
	movl	%eax, %esi
	movb	$0, %al
	callq	_printf
LBB1_3:
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Error al crear gestor 1\n"

L_.str.1:                               ## @.str.1
	.asciz	"Error al crear gestor 2\n"

L_.str.2:                               ## @.str.2
	.asciz	"%d\n"

L_.str.3:                               ## @.str.3
	.asciz	"El hijo se ha matado adecuadamente.\n"

L_.str.4:                               ## @.str.4
	.asciz	"El hijo no ha salido con la se\303\261al esperada, sino que ha salido con: %s.\n"

L_.str.5:                               ## @.str.5
	.asciz	"Se ha obtenido un valor de retorno inesperado del hijo.\n"

L_.str.6:                               ## @.str.6
	.asciz	"Sen\314\203al tipo 1 recibida. Soy %d\n"

L_.str.7:                               ## @.str.7
	.asciz	"Sen\314\203al tipo 2 recibida. Soy %d\n"

.subsections_via_symbols
