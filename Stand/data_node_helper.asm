.DATA
	data_node_write_cpp		dq 0
	data_node_write_return	dq 0

.CODE
	data_node_helper_init PROC
		mov data_node_write_cpp, rcx
		mov data_node_write_return, rdx
		ret
	data_node_helper_init ENDP

	data_node_write_detour PROC
		mov r11, data_node_write_cpp
		call r11
		mov r11, data_node_write_return
		jmp r11
	data_node_write_detour ENDP

END
