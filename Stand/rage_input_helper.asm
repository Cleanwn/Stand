.DATA
	rage_input_helper_keyboard_array	dq	0 ; float(*)[256]
	set_value_from_keyboard_ret			dq	0
	set_value_from_mkb_axis_ret_og		dq	0
	set_value_from_mkb_axis_ret_bail	dq	0

	;FLOAT_0_1	dd	3DCCCCCDh
	;FLOAT_0_5	dd	3F000000h
	FLOAT_1_0	dd	3F800000h

.CODE
	rage_input_helper_set_override PROC
		mov rage_input_helper_keyboard_array, rcx
		ret
	rage_input_helper_set_override ENDP

	rage_input_helper_get_override PROC
		mov rax, rage_input_helper_keyboard_array
		ret
	rage_input_helper_get_override ENDP

	set_value_from_keyboard_init PROC
		mov set_value_from_keyboard_ret, rcx
		ret
	set_value_from_keyboard_init ENDP

	set_value_from_mkb_axis_init_ret_og PROC
		mov set_value_from_mkb_axis_ret_og, rcx
		ret
	set_value_from_mkb_axis_init_ret_og ENDP

	set_value_from_mkb_axis_init_ret_bail PROC
		mov set_value_from_mkb_axis_ret_bail, rcx
		ret
	set_value_from_mkb_axis_init_ret_bail ENDP

	; `p` is in edi

	set_value_from_keyboard_detour PROC
		cmp rage_input_helper_keyboard_array, 0
		jne _set_value_from_keyboard_do_override

		movss xmm6, dword ptr [FLOAT_1_0]
	_set_value_from_keyboard_ret:
		cmp edi, 9
		mov r10, set_value_from_keyboard_ret
		jmp r10

	_set_value_from_keyboard_do_override:
		mov r10, qword ptr [rage_input_helper_keyboard_array]
		movsxd r11, edi
		movss xmm6, dword ptr [r10 + 4*r11]
		jmp _set_value_from_keyboard_ret
	set_value_from_keyboard_detour ENDP

	set_value_from_mkb_axis_detour PROC
		cmp rage_input_helper_keyboard_array, 0
		jne _set_value_from_mkb_axis_do_override

		mov eax, edi
		mov r8b, r10b

		mov r12, set_value_from_mkb_axis_ret_og
		jmp r12

	_set_value_from_mkb_axis_do_override:
		mov r10, qword ptr [rage_input_helper_keyboard_array]

		mov r11d, edi
		shr r11d, 8
		movzx r11d, r11b
		movss xmm6, dword ptr [r10 + 4*r11]

		movzx r11, dil
		subss xmm6, dword ptr [r10 + 4*r11]

		mov r10, set_value_from_mkb_axis_ret_bail
		jmp r10
	set_value_from_mkb_axis_detour ENDP

END
