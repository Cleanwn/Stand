.DATA
	fpLogCaughtExp		dq	0
	fpLogUncaughtExp	dq	0
	ignore_exceptions	db	0

	exp_ptr				dq	0
	exp_handler			dq	0

	exp_ret				dd	1

.CODE
	exceptional_init PROC
		mov fpLogCaughtExp, rcx
		mov fpLogUncaughtExp, rdx
		ret
	exceptional_init ENDP

	disable_exception_handling PROC
		mov ignore_exceptions, 1
		ret
	disable_exception_handling ENDP

	exceptional_has_exp PROC
		xor eax, eax
		cmp exp_handler, 0
		setnz al
		ret
	exceptional_has_exp ENDP

	exceptional_get_exp PROC
		mov rax, exp_ptr
		mov [rcx], rax
		mov rax, exp_handler
		mov [rdx], rax
		ret
	exceptional_get_exp ENDP

	exceptional_clear_exp PROC
		mov exp_ptr, 0
		mov exp_handler, 0
		ret
	exceptional_clear_exp ENDP

	ignore_exception PROC
		; return EXCEPTION_CONTINUE_SEARCH;
		xor eax, eax
		ret
	ignore_exception ENDP

	exceptional_on_exception PROC
		cmp ignore_exceptions, 0
		jne ignore_exception

	_wait_for_availablity:
		cmp exp_ptr, 0
		jne _wait_for_availablity

		mov exp_ptr, rcx
		mov exp_handler, rdx

	_wait_for_completion:
		cmp exp_ptr, 0
		jne _wait_for_completion

		mov eax, exp_ret
		ret
	exceptional_on_exception ENDP

	exceptional_on_caught_exception PROC
		mov rdx, fpLogCaughtExp
		jmp exceptional_on_exception
	exceptional_on_caught_exception ENDP

	exceptional_on_uncaught_exception PROC
		mov rdx, fpLogUncaughtExp
		jmp exceptional_on_exception
	exceptional_on_uncaught_exception ENDP

	exceptional_set_retval PROC
		mov exp_ret, ecx
		ret
	exceptional_set_retval ENDP
END
