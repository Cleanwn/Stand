.DATA
	net_event_error_continue	dq	0
	prevented_overrun			db	0

.CODE
	net_event_error_init PROC
		mov net_event_error_continue, rcx
		ret
	net_event_error_init ENDP

	overrun_helper_tick PROC
		mov al, prevented_overrun
		xor prevented_overrun, al
		ret
	overrun_helper_tick ENDP

	net_event_error_pre_memmove_detour PROC
		cmp eax, 128 ; max data size
		jle _ok ; eax <= 128
		mov eax, 128 ; eax is greater than 128, needs to be capped
		mov prevented_overrun, 1
	_ok:
		lea rcx, [r12+60h] ; our detour hook overwrote this instruction
		; continue at the next instruction
		mov r10, net_event_error_continue
		jmp r10
	net_event_error_pre_memmove_detour ENDP

END
