.DATA
	ins_count				dd	0
	last_opcode				db	0
	last_ins_ptr			dd	0
	last_stack_ptr			dq	0
	last_frame_ptr			dq	0

	jump_table				dq	0
	on_error				dq	0
	on_invalid_op			dq	0
	vm_return				dq	0

	native_op_count			dq	0
	og_native_op			dq	0
	op_native_executing		dq	0

.CODE
	script_vm_helper_init_1 PROC
		mov jump_table, rcx
		mov on_invalid_op, rdx
		ret
	script_vm_helper_init_1 ENDP

	script_vm_helper_init_2 PROC
		mov on_error, rcx
		mov vm_return, rdx
		ret
	script_vm_helper_init_2 ENDP

	script_vm_helper_get_last_opcode PROC
		mov al, last_opcode
		ret
	script_vm_helper_get_last_opcode ENDP

	script_vm_helper_get_last_ins_ptr PROC
		mov eax, last_ins_ptr
		ret
	script_vm_helper_get_last_ins_ptr ENDP

	script_vm_helper_get_last_stack_ptr PROC
		mov rax, last_stack_ptr
		ret
	script_vm_helper_get_last_stack_ptr ENDP

	script_vm_helper_get_last_frame_ptr PROC
		mov rax, last_frame_ptr
		ret
	script_vm_helper_get_last_frame_ptr ENDP

	script_vm_helper_reset_ins_count PROC
		xor eax, eax
		mov ins_count, eax
		ret
	script_vm_helper_reset_ins_count ENDP

	script_vm_switch_detour PROC
		; Avoid freeze
		mov eax, ins_count
		inc eax
		mov ins_count, eax
		cmp eax, 100000000
		je script_vm_freezing

		; Rewritten switch
		inc rdi
		movzx eax, byte ptr [rdi]
		cmp eax, 131
		ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r14
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx

	script_vm_invalid_op:
		mov rcx, rax
		mov rax, on_invalid_op
		call rax
		mov eax, 2
		mov rcx, vm_return
		jmp rcx

	script_vm_freezing:
		mov eax, 69
		mov rcx, vm_return
		jmp rcx
	script_vm_switch_detour ENDP

	script_vm_error_detour PROC
		mov rax, on_error
		call rax
		mov rcx, vm_return
		jmp rcx
	script_vm_error_detour ENDP

	script_vm_op_native_init PROC
		mov rax, [rcx]
		add rax, 37 ; offset to after `mov r8, ...` instruction
		mov og_native_op, rax
		lea rax, script_vm_op_native_detour
		mov [rcx], rax
		ret
	script_vm_op_native_init ENDP

	script_vm_op_native_tick PROC
		mov rax, native_op_count
		xor native_op_count, rax
		ret
	script_vm_op_native_tick ENDP

	script_vm_op_native_detour PROC
		; Original game instructions
		movzx   eax, byte ptr [rdi+2]
		movzx   r12d, byte ptr [rdi+1]
		add     rdi, 3
		movzx   ecx, byte ptr [rdi]
		shl     eax, 8
		mov     r14d, r12d
		or      rcx, rax
		mov     rax, [r9+40h]
		sar     r14d, 2
		mov     r8, [rax+rcx*8]

		; Do our stuff
		mov op_native_executing, r8 ; entrypoint
		inc native_op_count

		; Continue the game stuff
		mov rax, og_native_op
		jmp rax
	script_vm_op_native_detour ENDP

	script_vm_get_op_native_executing PROC
		mov rax, op_native_executing
		ret
	script_vm_get_op_native_executing ENDP

	; BP Optim Madness

	script_vm_bpoptim_op_0 PROC
		sub rdi, rsi
		mov rdx, rdi
		mov rax, rdi
		mov rcx, rdi
		sar rax, 0Eh
		and ecx, 3FFFh
		mov rax, [r13+rax*8+0]
		lea rdi, [rcx-1]
		add rdi, rax
		mov rsi, rdi
		sub rsi, rdx
		; continue
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_bpoptim_op_0 ENDP

	script_vm_bpoptim_op_2 PROC
		sub rbx, r12
		mov eax, [rbx+8]
		sub [rbx], eax
		; continue
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_bpoptim_op_2 ENDP

	; 41 **Almost** Identical Continue Blocks

	script_vm_continue_1 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_1 ENDP

	script_vm_continue_2 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_2 ENDP

	script_vm_continue_3 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_3 ENDP

	script_vm_continue_4 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_4 ENDP

	script_vm_continue_5 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_5 ENDP

	script_vm_continue_6 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_6 ENDP

	script_vm_continue_7 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_7 ENDP

	script_vm_continue_8 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_8 ENDP

	script_vm_continue_9 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_9 ENDP

	script_vm_continue_10 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_10 ENDP

	script_vm_continue_11 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_11 ENDP

	script_vm_continue_12 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_12 ENDP

	script_vm_continue_13 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_13 ENDP

	script_vm_continue_14 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_14 ENDP

	script_vm_continue_15 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_15 ENDP

	script_vm_continue_16 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_16 ENDP

	script_vm_continue_17 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_17 ENDP

	script_vm_continue_18 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_18 ENDP

	script_vm_continue_19 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_19 ENDP

	script_vm_continue_20 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_20 ENDP

	script_vm_continue_21 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_21 ENDP

	script_vm_continue_22 PROC
		mov r8, [rbp+6Fh]

		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_22 ENDP

	script_vm_continue_23 PROC
		mov r8, [rbp+6Fh]

		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_23 ENDP

	script_vm_continue_24 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_24 ENDP

	script_vm_continue_25 PROC
		mov r8, [rbp+6Fh]

		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_25 ENDP

	script_vm_continue_26 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_26 ENDP

	script_vm_continue_27 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_27 ENDP

	script_vm_continue_28 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_28 ENDP

	script_vm_continue_29 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_29 ENDP

	script_vm_continue_30 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_30 ENDP

	script_vm_continue_31 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_31 ENDP

	script_vm_continue_32 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_32 ENDP

	script_vm_continue_33 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_33 ENDP

	script_vm_continue_34 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_34 ENDP

	script_vm_continue_35 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_35 ENDP

	script_vm_continue_36 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_36 ENDP

	script_vm_continue_37 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_37 ENDP

	script_vm_continue_38 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_38 ENDP

	script_vm_continue_39 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_39 ENDP

	script_vm_continue_40 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_40 ENDP

	script_vm_continue_41 PROC
		inc rdi
		movzx eax, byte ptr [rdi]
		;cmp eax, 126
		;ja script_vm_invalid_op
		mov last_opcode, al
		mov edx, edi
		sub edx, esi
		mov last_ins_ptr, edx
		mov last_stack_ptr, rbx
		mov last_frame_ptr, r11
		mov rdx, jump_table
		mov rcx, [rdx+rax*8]
		jmp rcx
	script_vm_continue_41 ENDP

END
